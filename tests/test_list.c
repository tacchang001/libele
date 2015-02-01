#include "ele_list.h"

#include <unistd.h>
#include <assert.h>

typedef struct list_node {
	ELE_LIST_MEMBER(struct list_node)
	;
	int value;
} list_node;

static int compare_node(void *value, const void *nd)
{
	list_node *node = (list_node*) nd;
	return ((long) (ssize_t) value == node->value) ? 0 : -1;
}

#define ARRAY_SIZE(a)	((int)(sizeof(a)/sizeof(a[0])))

int main(int argc, char *argv[])
{
	list_node nodes[4];    // must be even number of nodes
	list_node list;
	list_node list2;
	list_node *p;
	int i;  // don't change to unsigned!

	//
	// Test insert_before().
	//
	list.value = (unsigned) -1;
	ele_list_init(&list);
	for (i = 0; i < ARRAY_SIZE(nodes); ++i) {
		nodes[i].value = i;
		ele_list_insert_before(&list, &nodes[i]);
	}
	// check.
	for (i = 0, p = list.next; i < ARRAY_SIZE(nodes);
	                ++i, p = p->next) {
		assert(p->value == i);
		if (p->value != i) {
			return -1;
		}
	}

	//
	// Test insert_after()
	//
	ele_list_init(&list);
	for (i = ARRAY_SIZE(nodes) - 1; i >= 0; --i) {
		ele_list_insert_after(&list, &nodes[i]);
	}
	// check.
	for (i = 0, p = list.next; i < ARRAY_SIZE(nodes);
	                ++i, p = p->next) {
		assert(p->value == i);
		if (p->value != i) {
			return -1;
		}
	}

	//
	// Test merge_last()
	//
	// Init lists
	ele_list_init(&list);
	ele_list_init(&list2);
	for (i = 0; i < ARRAY_SIZE(nodes) / 2; ++i) {
		ele_list_insert_before(&list, &nodes[i]);
	}
	for (i = ARRAY_SIZE(nodes) / 2; i < ARRAY_SIZE(nodes);
	                ++i) {
		ele_list_insert_before(&list2, &nodes[i]);
	}
	// merge
	ele_list_merge_last(&list, &list2);
	// check.
	for (i = 0, p = list.next; i < ARRAY_SIZE(nodes);
	                ++i, p = p->next) {
		assert(p->value == i);
		if (p->value != i) {
			return -1;
		}
	}
	// check list is empty
	assert(ele_list_empty(&list2));
	if (!ele_list_empty(&list2)) {
		return -1;
	}

	//
	// Check merge_first()
	//
	ele_list_init(&list);
	ele_list_init(&list2);
	for (i = 0; i < ARRAY_SIZE(nodes) / 2; ++i) {
		ele_list_insert_before(&list, &nodes[i]);
	}
	for (i = ARRAY_SIZE(nodes) / 2; i < ARRAY_SIZE(nodes);
	                ++i) {
		ele_list_insert_before(&list2, &nodes[i]);
	}
	// merge
	ele_list_merge_first(&list2, &list);
	// check (list2).
	for (i = 0, p = list2.next; i < ARRAY_SIZE(nodes);
	                ++i, p = p->next) {
		assert(p->value == i);
		if (p->value != i) {
			return -1;
		}
	}
	// check list is empty
	assert(ele_list_empty(&list));
	if (!ele_list_empty(&list)) {
		return -1;
	}

	//
	// Test insert_nodes_before()
	//
	// init list
	ele_list_init(&list);
	for (i = 0; i < ARRAY_SIZE(nodes) / 2; ++i) {
		ele_list_insert_before(&list, &nodes[i]);
	}
	// chain remaining nodes
	ele_list_init(&nodes[ARRAY_SIZE(nodes) / 2]);
	for (i = ARRAY_SIZE(nodes) / 2 + 1;
	                i < ARRAY_SIZE(nodes); ++i) {
		ele_list_insert_before(&nodes[ARRAY_SIZE(nodes) / 2],
		                       &nodes[i]);
	}
	// insert nodes
	ele_list_insert_nodes_before(&list,
	                             &nodes[ARRAY_SIZE(nodes) / 2]);
	// check
	for (i = 0, p = list.next; i < ARRAY_SIZE(nodes);
	                ++i, p = p->next) {
		assert(p->value == i);
		if (p->value != i) {
			return -1;
		}
	}

	// erase test.
	ele_list_init(&list);
	for (i = 0; i < ARRAY_SIZE(nodes); ++i) {
		nodes[i].value = i;
		ele_list_insert_before(&list, &nodes[i]);
	}
	for (i = ARRAY_SIZE(nodes) - 1; i >= 0; --i) {
		int j;
		ele_list_erase(&nodes[i]);
		for (j = 0, p = list.next; j < i; ++j, p = p->next) {
			assert(p->value == j);
			if (p->value != j) {
				return -1;
			}
		}
	}

	// find and search
	ele_list_init(&list);
	for (i = 0; i < ARRAY_SIZE(nodes); ++i) {
		nodes[i].value = i;
		ele_list_insert_before(&list, &nodes[i]);
	}
	for (i = 0; i < ARRAY_SIZE(nodes); ++i) {
		p = (list_node*) ele_list_find_node(&list, &nodes[i]);
		assert(p == &nodes[i]);
		if (p != &nodes[i]) {
			return -1;
		}
		p = (list_node*) ele_list_search(&list, (void*) (ssize_t) i,
		                                 compare_node);
		assert(p == &nodes[i]);
		if (p != &nodes[i]) {
			return -1;
		}
	}
	return 0;
}
