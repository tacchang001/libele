#pragma once

#include "ele_error.h"

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ele_mempool_desc {
	void *head;
	size_t (*get_max_size)(struct ele_mempool_desc *);
	void (*print_pool)(struct ele_mempool_desc *);
} ele_mempool_desc_t;

typedef struct ele_mempool {
	void *head;
	void *data;
	ele_mempool_desc_t (*get_desc)(struct ele_mempool *);
	size_t (*get_size)(struct ele_mempool *);
} ele_mempool_t;

ele_mempool_desc_t
ele_mempool_create(size_t max_size);

void
ele_mempool_destroy(ele_mempool_desc_t * desc);

ele_mempool_t
ele_mempool_alloc(ele_mempool_desc_t * desc, size_t size);

ele_mempool_t
ele_mempool_calloc(ele_mempool_desc_t * desc, size_t size);

void
ele_mempool_free(ele_mempool_t * pool);

#ifdef __cplusplus
}
#endif
