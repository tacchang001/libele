#pragma once

#ifdef __cplusplus
extern "C" {
#endif

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



#ifdef __cplusplus
}
#endif
