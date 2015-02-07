#pragma once

#if 0

typedef struct {
	int count; /* actual pool count */
	int max_count; /* pool array length (2^x ceil of ct) */
	int min_size; /* minimum pool size */
	int max_size; /* maximum pool size */
	int page_size; /* page size, typically 4096 */
	void **pools; /* pools */
	int *sizes; /* chunk size for each pool */
	void *frees[1]; /* heads for pools' free lists */
} ele_pool_t;

/* Initialize a memory pool for allocations between 2^min2 and 2^max2,
 * inclusive. (Larger allocations will be directly allocated and freed
 * via mmap / munmap.) */
ele_pool_t *ele_pool_create(int min2, int max2);

/* Allocate SZ bytes. */
void *ele_pool_alloc(ele_pool_t *mp, int sz);

/* mmap a new memory pool of TOTAL_SZ bytes, then build an internal
 * freelist of SZ-byte cells, with the head at (result)[0]. */
void **ele_extend_pool(unsigned int sz, unsigned int total_sz);

/* Return pointer P (SZ bytes in size) to the appropriate pool. */
void ele_pool_free(ele_pool_t *mp, void *p, int sz);

/* Resize P from OLD_SZ to NEW_SZ, copying content. */
void *ele_pool_realloc(ele_pool_t *mp, void *p, int old_sz, int new_sz);

/* Free the pool. */
void ele_pool_destroy(ele_pool_t *mp);

#endif
