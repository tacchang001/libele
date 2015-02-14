#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <PCUnit/PCUnit.h>

#include "pcunit_unittests.h"

#include "ele_mempool.h"
#include "ele_error.h"

#include <stdio.h>
#include <unistd.h>

static void test_example01(void)
{
	const size_t page_size = 0x200;
	const size_t page_num = 10;
	const size_t max_size = page_size * page_num;
	ele_mempool_desc_t mp =
		ele_mempool_create(max_size);
	PCU_ASSERT_PTR_NOT_NULL(mp.head);
	PCU_ASSERT_TRUE(mp.get_max_size(&mp) > max_size);

	int j;
	for (j=0; j<10; j++) {
		printf("--- %d ---\n", j);
		mp.print_pool(&mp);
		int i;
		ele_mempool_t m[page_num];
		for (i=0; i<page_num-1; i++) {
			printf("%d ", i);
			m[i] = ele_mempool_calloc(&mp, page_size);
			PCU_ASSERT_PTR_NOT_NULL(m[i].head);
			PCU_ASSERT_PTR_NOT_NULL(m[i].data);
			PCU_ASSERT_EQUAL(m[i].get_size(&m[i]), page_size);
			ele_mempool_desc_t mp2 = m[i].get_desc(&m[i]);
			PCU_ASSERT_PTR_EQUAL(mp.head, mp2.head);
		}
		putchar('\n');
		for (i=0; i<page_num-1; i++) {
			printf("%d ", i);
			ele_mempool_free(&m[i]);
		}
		putchar('\n');
	}

	ele_mempool_destroy(&mp);
}

static void test_example02(void)
{
	const size_t page_size = 0x200;
	const size_t page_num = 10;
	const size_t max_size = page_size * page_num;
	ele_mempool_desc_t mp =
		ele_mempool_create(max_size);
	PCU_ASSERT_PTR_NOT_NULL(mp.head);
	PCU_ASSERT_TRUE(mp.get_max_size(&mp) > max_size);

	ele_mempool_t m[page_num];
	int k;
	for (k=0; k<page_num-1; k++) {
		m[k] = ele_mempool_calloc(&mp, page_size);
		PCU_ASSERT_PTR_NOT_NULL(m[k].head);
		PCU_ASSERT_PTR_NOT_NULL(m[k].data);
		PCU_ASSERT_EQUAL(m[k].get_size(&m[k]), page_size);
		ele_mempool_desc_t mp2 = m[k].get_desc(&m[k]);
		PCU_ASSERT_PTR_EQUAL(mp.head, mp2.head);
	}
	for (k=0; k<page_num/2; k++) {
		ele_mempool_free(&m[k]);
	}
	int j;
	for (j=0; j<10; j++) {
		printf("--- %d ---\n", j);
		mp.print_pool(&mp);
		int i;
		for (i=0; i<page_num/2; i++) {
			printf("%d ", i);
			m[i] = ele_mempool_calloc(&mp, page_size);
			PCU_ASSERT_PTR_NOT_NULL(m[i].head);
			PCU_ASSERT_PTR_NOT_NULL(m[i].data);
			PCU_ASSERT_EQUAL(m[i].get_size(&m[i]), page_size);
			ele_mempool_desc_t mp2 = m[i].get_desc(&m[i]);
			PCU_ASSERT_PTR_EQUAL(mp.head, mp2.head);
		}
		putchar('\n');
		for (i=0; i<page_num/2; i++) {
			printf("%d ", i);
			ele_mempool_free(&m[i]);
		}
		putchar('\n');
	}

	ele_mempool_destroy(&mp);
}

static void test_example03(void)
{
    PCU_ASSERT_EQUAL(1+1, 2);
}

PCU_Suite *MemoryPoolTest_suite(void)
{
	static PCU_Test tests[] = {
		{ "memory pool 01", test_example01 },
		{ "memory pool 02", test_example02 },
		{ "memory pool 03", test_example03 },
	};
	static PCU_Suite suite = {
		"Memory Pool UnitTest", tests, sizeof tests / sizeof tests[0] };
	return &suite;
}