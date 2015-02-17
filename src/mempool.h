#pragma once

#ifdef __cplusplus
extern "C" {
#endif

//typedef struct ele_mempool_desc {
//	void *head;
//	size_t (*get_max_size)(struct ele_mempool_desc *);
//	void (*print_pool)(struct ele_mempool_desc *);
//} ele_mempool_desc_t;
//
//typedef struct ele_mempool {
//	void *head;
//	void *data;
//	ele_mempool_desc_t (*get_desc)(struct ele_mempool *);
//	size_t (*get_size)(struct ele_mempool *);
//} ele_mempool_t;
//
typedef struct mempool {
	struct mempool * prev;
	struct mempool * next;
	size_t size;
	char data[1];
} mempool_t;

#define ELE_MEMPOOL_VALID_DESC(ptr) (ptr.head != NULL)



#ifdef __cplusplus
}
#endif
