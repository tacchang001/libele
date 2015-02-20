#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <PCUnit/PCUnit.h>

#include "pcunit_unittests.h"

#include "ele_task.h"
#include "ele_error.h"
#include "ele_itc.h"

#include <stdio.h>
#include <unistd.h>

static const int TASK_ID = 100;

static void * anything(void * arg) {
	int value = *(int*)arg;
	fprintf(stderr, "value = %d\n", value);

	time_t start, curr;
	curr = start = time(NULL);
	while (1) {
		pthread_testcancel(); /* A cancellation point */
		if (curr < time(NULL)) {
			curr = time(NULL);
			printf("cnt = %d\n", ++value); /* A cancellation point */
		}
	}
	printf("\ndone.\n", value);

	return arg;
}

static void test_example01(void)
{
	int a = 10;
	ele_task_init_attr_t attr = {
		.id = TASK_ID,
		.schedpolicy = SCHED_OTHER,
		.schedparam = 0,
		.entry = anything,
		.arg = &a
	};

	if (ele_task_create(attr) != ELE_SUCCESS) {
		fprintf(stderr, "ele_tsk_create error\n");
	}

	ele_task_join(TASK_ID);
}

static void test_example02(void)
{
    PCU_ASSERT_EQUAL(1+2, 3);
}

static void test_example03(void)
{
    PCU_ASSERT_EQUAL(1+2, 3);
}

PCU_Suite *ItcTest_suite(void)
{
	static PCU_Test tests[] = {
		{ "itc 01", test_example01 },
		{ "itc 02", test_example02 },
		{ "itc 03", test_example03 },
	};
	static PCU_Suite suite = {
		"Inter Thread Communication Test", tests, sizeof tests / sizeof tests[0] };
	return &suite;
}
