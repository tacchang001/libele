#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <string.h>

#include "ele_pool.h"

#ifndef MPOOL_MALLOC
#define MPOOL_MALLOC(sz) malloc(sz)
#define MPOOL_REALLOC(p, sz) realloc(p, sz)
#define MPOOL_FREE(p, sz) free(p)
#endif

static void *get_mmap(long sz)
{
	void *p = mmap(0, sz, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1,
	               0);
	if (p == MAP_FAILED)
		return NULL;
	return p;
}

/* Optimized base-2 integer ceiling, from _Hacker's Delight_
 * by Henry S. Warren, pg. 48. Called 'clp2' there. */
static unsigned int iceil2(unsigned int x)
{
	x = x - 1;
	x = x | (x >> 1);
	x = x | (x >> 2);
	x = x | (x >> 4);
	x = x | (x >> 8);
	x = x | (x >> 16);
	return x + 1;
}

/* mmap a new memory pool of TOTAL_SZ bytes, then build an internal
 * freelist of SZ-byte cells, with the head at (result)[0].
 * Returns NULL on error. */
void **ele_extend_pool(unsigned int sz, unsigned int total_sz)
{
	void *p = get_mmap(sz > total_sz ? sz : total_sz);
	int i, o = 0, lim; /* o=offset */
	int **pool;
	void *last = NULL;
	if (p == NULL)
		return NULL;
	pool = (int **) p;
	assert(pool);
	assert(sz > sizeof(void *));

	lim = (total_sz / sz);
#		if defined(ELE_POOL_DEBUG_PRINT)
		fprintf(stderr, "ele_extend_pool sz: %d lim: %d => %d %p\n", sz, lim,
		        lim * sz, p);
#		endif
	for (i = 0; i < lim; i++) {
		if (last)
			assert(last == pool[o]);
		o = (i * sz) / sizeof(void *);
		pool[o] = (int *) &pool[o + (sz / sizeof(void *))];
		last = pool[o];
#			if defined(ELE_POOL_DEBUG_PRINT)
			fprintf(stderr, "%d (%d / 0x%04x) -> %p = %p\n", i, o, o, &pool[o],
			        pool[o]);
#			endif
	}
	pool[o] = NULL;
	return p;
}

/* Add a new pool, resizing the pool array if necessary. */
static int ele_pool_add(ele_pool_t *mp, void *p, int sz)
{
	void **nps, *nsizes; /* new pools, new sizes */
	assert(p);
	assert(sz > 0);
#		if defined(ELE_POOL_DEBUG_PRINT)
		fprintf(stderr, "mpool_add_pool (%d / %d) @ %p, sz %d\n", mp->count,
		        mp->max_count, p, sz);
#		endif
	if (mp->count == mp->max_count) {
		mp->max_count *= 2; /* ram will exhaust before overflow... */
		nps = MPOOL_REALLOC(mp->pools, mp->max_count * sizeof(void **));
		nsizes = MPOOL_REALLOC(mp->sizes, mp->max_count * sizeof(int *));
		if (nps == NULL || nsizes == NULL)
			return -1;
		mp->sizes = nsizes;
		mp->pools = nps;
	}

	mp->pools[mp->count] = p;
	mp->sizes[mp->count] = sz;
	mp->count++;
	return 0;
}

/* Initialize a memory pool set, with pools in sizes
 * 2^min2 to 2^max2. Returns NULL on error. */
ele_pool_t *ele_pool_create(int min2, int max2)
{
	int palen; /* length of pool array */
	int count = count = max2 - min2 + 1; /* pool array count */
	long pgsz = sysconf(_SC_PAGESIZE);
	ele_pool_t *mp;
	void *pools;
	int *sizes;

	palen = iceil2(count);
#		if defined(ELE_POOL_DEBUG_PRINT)
		fprintf(stderr, "ele_pool_init for cells %d - %d bytes\n", 1 << min2,
		        1 << max2);
#		endif
	assert(count > 0);
	mp = MPOOL_MALLOC(sizeof(ele_pool_t) + (count - 1) * sizeof(void *));
	pools = MPOOL_MALLOC(palen * sizeof(void **));
	sizes = MPOOL_MALLOC(palen * sizeof(int));
	if (mp == NULL || pools == NULL || sizes == NULL)
		return NULL;
	mp->count = count;
	mp->pools = pools;
	mp->max_count = palen;
	mp->page_size = pgsz;
	mp->sizes = sizes;
	mp->min_size = 1 << min2;
	mp->max_size = 1 << max2;
	bzero(sizes, palen * sizeof(int));
	bzero(pools, palen * sizeof(void *));
	bzero(mp->frees, count * sizeof(void *));

	return mp;
}

/* Free a memory pool set. */
void ele_pool_destroy(ele_pool_t *mp)
{
	long i, sz, pgsz = mp->page_size;
	assert(mp);
#		if defined(ELE_POOL_DEBUG_PRINT)
		fprintf(stderr, "%d/%d pools, freeing...\n", mp->count, mp->max_count);
#		endif
	for (i = 0; i < mp->count; i++) {
		void *p = mp->pools[i];
		if (p) {
			sz = mp->sizes[i];
			assert(sz > 0);
			sz = sz >= pgsz ? sz : pgsz;
#				if defined(ELE_POOL_DEBUG_PRINT)
				fprintf(stderr, "ele_pool_destroy %ld, sz %ld (%p)\n", i, sz,
				        mp->pools[i]);
#				endif
			if (munmap(mp->pools[i], sz) == -1) {
				fprintf(stderr,
				        "munmap error while unmapping %lu bytes at %p\n", sz,
				        mp->pools[i]);
			}
		}
	}
	MPOOL_FREE(mp->pools, mp->count * sizeof(*pools));
	MPOOL_FREE(mp, sizeof(*mp));
}

/* Allocate memory out of the relevant memory pool.
 * If larger than max_size, just mmap it. If pool is full, mmap a new one and
 * link it to the end of the current one. Returns NULL on error. */
void *ele_pool_alloc(ele_pool_t *mp, int sz)
{
	void **cur, **np; /* new pool */
	int i, p, szceil = 0;
	assert(mp);
	if (sz >= mp->max_size) {
		cur = get_mmap(sz); /* just mmap it */
		if (cur == NULL)
			return NULL;
#			if defined(ELE_POOL_DEBUG_PRINT)
			fprintf(stderr, "ele_pool_alloc mmap %d bytes @ %p\n", sz, cur);
#			endif
		return cur;
	}

	for (i = 0, p = mp->min_size;; i++, p *= 2) {
		if (p > sz) {
			szceil = p;
			break;
		}
	}
	assert(szceil > 0);
	cur = mp->frees[i]; /* get current head */
	if (cur == NULL) { /* lazily allocate & init pool */
		void **pool = ele_extend_pool(szceil, mp->page_size);
		if (pool == NULL)
			return NULL;
		mp->pools[i] = pool;
		mp->frees[i] = &pool[0];
		mp->sizes[i] = szceil;
		cur = mp->frees[i];
	}
	assert(cur);

	if (*cur == NULL) { /* if at end, attach to a new page */
#			if defined(ELE_POOL_DEBUG_PRINT)
			fprintf(stderr, "ele_pool_alloc adding pool w/ cell size %d\n",
			        szceil);
#			endif
		np = ele_extend_pool(szceil, mp->page_size);
		if (np == NULL)
			return NULL;
		*cur = &np[0];
		assert(*cur);
		if (ele_pool_add(mp, np, szceil) < 0)
			return NULL;
	}

	assert(*cur > (void *)4096);
#		if defined(ELE_POOL_DEBUG_PRINT)
		fprintf(stderr,
		        "ele_pool_alloc pool %d bytes @ %p (list %d, szceil %d )\n", sz,
		        (void*) cur, i, szceil);
#		endif
	mp->frees[i] = *cur; /* set head to next head */
	return cur;
}

/* Push an individual pointer P back on the freelist for
 * the pool with size SZ cells.
 * if SZ is > the max pool size, just munmap it. */
void ele_pool_free(ele_pool_t *mp, void *p, int sz)
{
	int i = 0, szceil, max_size = mp->max_size;
	void **ip;

	if (sz > max_size) {
#			if defined(ELE_POOL_DEBUG_PRINT)
			fprintf(stderr, "ele_pool_free munmap sz %d @ %p\n", sz, p);
#			endif
		if (munmap(p, sz) == -1) {
			fprintf(stderr, "munmap error while unmapping %d bytes at %p\n", sz,
			        p);
		}
		return;
	}

	szceil = iceil2(sz);
	szceil = szceil > mp->min_size ? szceil : mp->min_size;

	ip = (void **) p;
	*ip = mp->frees[i];
	assert(ip);
	mp->frees[i] = ip;
#		if defined(ELE_POOL_DEBUG_PRINT)
		fprintf(stderr, "ele_pool_free list %d, %d bytes (ceil %d): %p\n", i, sz,
		        szceil, ip);
#		endif
}

/* Reallocate data, growing or shrinking and copying the contents.
 * Returns NULL on reallocation error. */
void *mpool_realloc(ele_pool_t *mp, void *p, int old_sz, int new_sz)
{
	void *r = ele_pool_alloc(mp, new_sz);
	if (r == NULL)
		return NULL;
	memcpy(r, p, old_sz);
	ele_pool_free(mp, p, old_sz);
	return r;
}
