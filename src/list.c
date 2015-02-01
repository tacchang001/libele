#include "ele_list.h"

#include <stdio.h>

static void ele_link_node(void *prev, void *next)
{
	((ele_list_t*) prev)->next = next;
	((ele_list_t*) next)->prev = prev;
}

void ele_list_init(void* node)
{
	ele_list_t * n = node;
	n->next = n->prev = node;
}

void ele_list_push_back(void *list, void *node)
{
	ele_list_insert_before(list, node);
}

void ele_list_push_front(void *list, void *node)
{
	ele_list_insert_after(list, node);
}

int ele_list_empty(const void * node)
{
	return ((ele_list_t*) node)->next == node;
}

void ele_list_insert_after(void *pos, void *node)
{
	((ele_list_t*) node)->prev = pos;
	((ele_list_t*) node)->next = ((ele_list_t*) pos)->next;
	((ele_list_t*) ((ele_list_t*) pos)->next)->prev = node;
	((ele_list_t*) pos)->next = node;
}

void ele_list_insert_before(void *pos, void *node)
{
	ele_list_insert_after(((ele_list_t*) pos)->prev, node);
}

void ele_list_insert_nodes_after(void *pos, void *lst)
{
	ele_list_t *lst_last = (ele_list_t *) ((ele_list_t*) lst)->prev;
	ele_list_t *pos_next = (ele_list_t *) ((ele_list_t*) pos)->next;

	ele_link_node(pos, lst);
	ele_link_node(lst_last, pos_next);
}

void ele_list_insert_nodes_before(void *pos, void *lst)
{
	ele_list_insert_nodes_after(((ele_list_t*) pos)->prev, lst);
}

void ele_list_merge_last(void *lst1, void *lst2)
{
	if (!ele_list_empty(lst2)) {
		ele_link_node(((ele_list_t*) lst1)->prev, ((ele_list_t*) lst2)->next);
		ele_link_node(((ele_list_t*) lst2)->prev, lst1);
		ele_list_init(lst2);
	}
}

void ele_list_merge_first(void *lst1, void *lst2)
{
	if (!ele_list_empty(lst2)) {
		ele_link_node(((ele_list_t*) lst2)->prev, ((ele_list_t*) lst1)->next);
		ele_link_node(((ele_list_t*) lst1), ((ele_list_t*) lst2)->next);
		ele_list_init(lst2);
	}
}

void ele_list_erase(void *node)
{
	ele_link_node(((ele_list_t*) node)->prev, ((ele_list_t*) node)->next);

	/* It'll be safer to init the next/prev fields to itself, to
	 * prevent multiple erase() from corrupting the list.
	 */
	ele_list_init(node);
}

void* ele_list_find_node(void *list, void *node)
{
	ele_list_t *p = (ele_list_t *) ((ele_list_t*) list)->next;
	while (p != list && p != node)
		p = (ele_list_t *) p->next;

	return p == node ? p : NULL;
}

void* ele_list_search(
    void *list,
    void *value,
    ele_list_compare_t comp)
{
	ele_list_t *p = (ele_list_t *) ((ele_list_t*) list)->next;
	while (p != list && (*comp)(value, p) != 0)
		p = (ele_list_t *) p->next;

	return p == list ? NULL : p;
}

size_t ele_list_size(const void *list)
{
	const ele_list_t *node = (const ele_list_t*) ((const ele_list_t*) list)
	                ->next;
	size_t count = 0;

	while (node != list) {
		++count;
		node = (ele_list_t*) node->next;
	}

	return count;
}
