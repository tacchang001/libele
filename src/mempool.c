#include "ele_mempool.h"

#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <pthread.h>

#include "mempool.h"
#include "mutex_lock.h"

static const size_t MIN_SIZE = 4095;

pthread_rwlock_t lock_rw = PTHREAD_RWLOCK_INITIALIZER;

typedef struct {
	struct mempool * head;
	struct mempool * tail;
	size_t max_size;
	void * start_in_raw_area;
	void * end_in_raw_area;
} mempool_desc_t;

static mempool_desc_t pools;

static void * mempool_get_address(void * ptr) {
	assert(ptr != NULL);
	return (void*)(((unsigned long)ptr + sizeof(int)) & ~(sizeof(int)-1));
}

static mempool_t * mempool_get_next_address(mempool_t * pool) {
	assert(pool != NULL);
	return mempool_get_address(
			(void *) ((uint8_t *)pool + sizeof(mempool_t) + pool->size)
		);
}

static void mempool_print_page(mempool_t * pool) {
	assert(pool != NULL);

	printf("prev=%p <- %p(%p,%d) -> %p\n",
		pool->prev,
		pool,
		&pool->data[0],
		pool->size,
		pool->next
	);
}

void mempool_print_pool(void) {
	int i;
	SCOPED_RDLOCK(lock_rw);
	mempool_t * ptr = pools.head;
	while (ptr != NULL) {
		mempool_print_page(ptr);
		ptr = ptr->next;
		i++;
	}
}

size_t mempool_get_max_size(void) {
	SCOPED_RDLOCK(lock_rw);
	return pools.max_size;
}

static uint32_t mempool_clp2(uint32_t x) {
	x = x - 1;
	x = x | (x >> 1);
	x = x | (x >> 2);
	x = x | (x >> 4);
	x = x | (x >> 8);
	x = x | (x >> 16);
	return x + 1;
}

static mempool_t * mempool_search_pool_for(const void * const data_ptr) {
	if (data_ptr == NULL) return NULL;

	mempool_t * pool = NULL;
	mempool_t * p = pools.head;
	while (p != NULL) {
		if (&p->data[0] == data_ptr) {
			pool = p;
			break;
		}
		p = p->next;
	}

	return pool;
}

static void mempool_append(mempool_t * point, mempool_t * element) {
	assert(point != NULL);
	assert(element != NULL);

	mempool_t * old_next = point->next;
	if (old_next != NULL) {
		old_next->prev = element;
	}
	point->next = element;
	element->prev = point;
	element->next = old_next;
}

ele_result_t
ele_mempool_create(const size_t max_size) {
	assert(max_size > MIN_SIZE);

	size_t pool_area_size = mempool_clp2(max_size);
	mempool_t * const_element = NULL;
	do {
		SCOPED_WRLOCK(lock_rw);
		pools.start_in_raw_area = malloc(pool_area_size);
		if (pools.start_in_raw_area == NULL) {
			perror("malloc");
			return ELE_FAILURE;
		}
		pools.max_size = pool_area_size;
		pools.end_in_raw_area = pools.start_in_raw_area + pool_area_size;

		const_element = (mempool_t *) mempool_get_address(
			(void *) pools.start_in_raw_area + sizeof(mempool_desc_t)
		);
		pools.head = const_element;
		pools.tail = const_element;
	} while (0);
	const_element->next = NULL;
	const_element->prev = NULL;
	const_element->size = sizeof(const_element->data);

	return ELE_SUCCESS;
}

void ele_mempool_destroy(void) {
	SCOPED_RDLOCK(lock_rw);
	free(pools.start_in_raw_area);
}

void *
ele_mempool_alloc(const size_t a_size) {
	assert(a_size > 0);

	mempool_t * pool = NULL;
	do {
		SCOPED_WRLOCK(lock_rw);
		mempool_t * const next_tail = mempool_get_next_address(pools.tail);
		const void * const new_bottom_addr = mempool_get_address(
			(void *) ((uint8_t *)next_tail
			+ sizeof(mempool_t)
			+ a_size)
		);
		if (pools.end_in_raw_area > new_bottom_addr) {
			mempool_t * old_tail = pools.tail;
			uint8_t * p = (uint8_t *)old_tail;
			pool = next_tail;
			pool->size = a_size;
			mempool_append(old_tail, pool);
			pools.tail = pool;
		} else {
			mempool_t * p = pools.head->next;
			while (p != pools.tail) {
				unsigned long start_addr = (unsigned long)mempool_get_next_address(p->prev);
				unsigned long end_addr = (unsigned long)p;
				unsigned long size = (unsigned long)mempool_get_address((void *)a_size);
				if ((end_addr - start_addr) > size) {
					pool = (mempool_t *)start_addr;
					pool->size = a_size;
					mempool_append(p->prev, pool);
					break;
				}
				p = p->next;
			}
		}
	} while (0);

	assert(pool != NULL);

	return &pool->data[0];
}

void *
ele_mempool_calloc(const size_t a_size) {
	assert(a_size > 0);

	void * pool = ele_mempool_alloc(a_size);
	memset(pool, 0, a_size);
	return pool;
}

void ele_mempool_free(void * const data_ptr) {
	assert(data_ptr != NULL);
	if (data_ptr == NULL) return;

	do {
		SCOPED_WRLOCK(lock_rw);
		mempool_t * pool = mempool_search_pool_for(data_ptr);
		if (pool != NULL) {
			if (pool == pools.tail)
				pools.tail = pool->prev;
			if (pool->next != NULL)
				pool->next->prev = pool->prev;
			if (pool->prev != NULL)
				pool->prev->next = pool->next;
		}
	} while (0);
}

