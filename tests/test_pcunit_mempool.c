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
	const size_t page_size = 0x100;
	const size_t page_num = 10;
	const size_t max_size = page_size * page_num * 10;
	ele_mempool_desc_t mp =
		ele_mempool_create(max_size);
	PCU_ASSERT_PTR_NOT_NULL(mp.head);
	PCU_ASSERT_TRUE(mp.get_max_size(&mp) > max_size);

	int i;
	ele_mempool_t m[page_num];
	for (i=0; i<page_num-1; i++) {
		m[i] = ele_mempool_calloc(&mp, page_size);
		PCU_ASSERT_PTR_NOT_NULL(m[i].head);
		PCU_ASSERT_PTR_NOT_NULL(m[i].data);
		PCU_ASSERT_EQUAL(m[i].get_size(&m[i]), page_size);
		ele_mempool_desc_t mp2 = m[i].get_desc(&m[i]);
		PCU_ASSERT_PTR_EQUAL(mp.head, mp2.head);
	}
	for (i=0; i<page_num-1; i++) {
		ele_mempool_free(&m[i]);
	}

	ele_mempool_destroy(&mp);
}

static void test_example02(void)
{
    PCU_ASSERT_EQUAL(1+1, 2);
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
