#include "ele_mempool.h"

#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

static const size_t MIN_SIZE = 4095;

#define ADDRESS(ptr) (((uint8_t *)ptr + sizeof(int)) & !sizeof(int))

typedef struct {
	struct mempool * head;
	struct mempool * tail;
	size_t max_size;
} mempool_desc_t;

typedef struct mempool {
	struct mempool * prev;
	struct mempool * next;
	mempool_desc_t * desc;
	size_t size;
	char data[1];
} mempool_t;

static void * mempool_get_address(void * ptr) {
	assert(ptr != NULL);
	return (void*)(((unsigned long)ptr + sizeof(void*)) & ~(sizeof(void*)-1));
}

static mempool_t * mempool_get_next_address(mempool_t * pool) {
	assert(pool != NULL);
	return mempool_get_address(
			(void *) ((uint8_t *)pool + sizeof(mempool_t) + pool->size)
		);
}

static void mempool_print_pool(mempool_t * pool) {
	assert(pool != NULL);

	printf("pool=%p\n", pool);
	printf(" prev=%p\n", pool->prev);
	printf(" next=%p\n", pool->next);
	printf(" size=%d\n", pool->size);
	printf(" desc=%p\n", pool->desc);
}

static unsigned int mempool_clp2(size_t x) {
	x = x - 1;
	x = x | (x >> 1);
	x = x | (x >> 2);
	x = x | (x >> 4);
	x = x | (x >> 8);
	x = x | (x >> 16);
	return x + 1;
}

static bool mempool_search_pool_for(mempool_t * pool) {
	assert(pool != NULL);
	mempool_desc_t * desc = pool->desc;
	assert(desc != NULL);
	assert(desc->head != NULL);
	assert(desc->tail != NULL);

	bool searched = false;
	mempool_t * p = desc->head;
	while (p != NULL) {
		p = p->next;
		if (p == pool) {
			searched = true;
			break;
		}
	}

	return searched;
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

static size_t mempool_get_max_size(ele_mempool_desc_t * a_desc) {
	assert(a_desc != NULL);

	mempool_desc_t * desc = a_desc->head;
	return desc->max_size;
}

static size_t mempool_get_size(ele_mempool_t * a_pool) {
	assert(a_pool != NULL);

	mempool_t * pool = a_pool->head;
	return pool->size;
}

static ele_mempool_desc_t mempool_get_desc(ele_mempool_t * a_pool) {
	assert(a_pool != NULL);

	mempool_t * pool = a_pool->head;
	ele_mempool_desc_t result = {
		.head = pool->desc
	};
	return result;
}

ele_mempool_desc_t
ele_mempool_create(const size_t max_size) {
	assert(max_size > MIN_SIZE);

	ele_mempool_desc_t result = {
		.head = NULL,
		.get_max_size = NULL
	};
	size_t pool_area_size = mempool_clp2(max_size);
	mempool_desc_t * desc = malloc(pool_area_size);
	if (desc == NULL) {
		perror("malloc");
		return result;
	}
	desc->max_size = pool_area_size;

	void * p = (void *) desc + sizeof(mempool_desc_t);
	mempool_t * const_element = (mempool_t *) mempool_get_address(p);

	desc->head = const_element;
	desc->tail = const_element;
	const_element->desc = desc;
	const_element->next = NULL;
	const_element->prev = NULL;
	const_element->size = sizeof(const_element->data);

	result.head = desc;
	result.get_max_size = mempool_get_max_size;

	return result;
}

void ele_mempool_destroy(ele_mempool_desc_t * const a_desc) {
	assert(a_desc != NULL);
	assert(a_desc->head != NULL);

	mempool_desc_t * desc = a_desc->head;
	assert(desc->head != NULL);
	assert(desc->tail != NULL);

	desc->head = NULL;
	desc->tail = NULL;
	free(desc);
}

ele_mempool_t
ele_mempool_alloc(ele_mempool_desc_t * const a_desc, const size_t a_size) {
	assert(a_desc != NULL);
	assert(a_desc->head != NULL);
	assert(a_size > 0);

	mempool_desc_t * desc = a_desc->head;
	assert(desc->head != NULL);
	assert(desc->tail != NULL);

	mempool_t * pool = NULL;
	const unsigned long term_addr =
		(unsigned long) desc
		+ desc->max_size;
	mempool_t * const next_tail = mempool_get_next_address(desc->tail);
	const unsigned long new_bottom_addr = (unsigned long)mempool_get_address(
		(void *) ((uint8_t *)next_tail
		+ sizeof(mempool_t)
		+ a_size)
	);
	if (term_addr > new_bottom_addr) {
		mempool_t * old_tail = desc->tail;
		uint8_t * p = (uint8_t *)old_tail;
		pool = next_tail;
		pool->desc = desc;
		pool->size = a_size;
		mempool_append(old_tail, pool);
		desc->tail = pool;
	} else {

	}

	ele_mempool_t result = {
		.head = pool,
		.data = &pool->data[0],
		.get_desc = mempool_get_desc,
		.get_size = mempool_get_size
	};
	return result;
}

ele_mempool_t
ele_mempool_calloc(ele_mempool_desc_t * const a_desc, const size_t a_size) {
	assert(a_desc != NULL);
	assert(a_desc->head != NULL);
	assert(a_size > 0);

	ele_mempool_t pool = ele_mempool_alloc(a_desc, a_size);
	memset(pool.data, 0, pool.get_size(&pool));
	return pool;
}

void ele_mempool_free(ele_mempool_t * const a_pool) {
	assert(a_pool != NULL);
	mempool_t * pool = a_pool->head;
	assert(mempool_search_pool_for(pool));

	if (pool == pool->desc->head)
		return;

	if (pool->next != NULL)
		pool->next->prev = pool->prev;
	if (pool->prev != NULL)
		pool->prev->next = pool->next;
}

