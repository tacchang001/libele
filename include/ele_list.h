#pragma once

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ELE_LIST_MEMBER(type) \
	type *prev; \
	type *next

typedef struct ele_list_t {
	ELE_LIST_MEMBER(void);
} ele_list_t;

typedef int (*ele_list_compare_t)(void *value, const void *node);

inline void ele_list_init(void* node);

inline int ele_list_empty(const void * node);

void ele_list_insert_before(void *pos, void *node);

inline void ele_list_push_back(void *list, void *node);

void ele_list_insert_nodes_before(void *lst, void *nodes);

void ele_list_insert_after(void *pos, void *node);

inline void ele_list_push_front(void *list, void *node);

void ele_list_insert_nodes_after(void *lst, void *nodes);

void ele_list_merge_first(void *list1, void *list2);

void ele_list_merge_last(void *list1, void *list2);

void ele_list_erase(void *node);

void* ele_list_find_node(void *list, void *node);

void* ele_list_search(
    void *list,
    void *value,
    ele_list_compare_t comp);

size_t ele_list_size(const void *list);

#ifdef __cplusplus
}
#endif
