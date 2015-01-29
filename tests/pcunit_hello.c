#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <PCUnit/PCUnit.h>

test_example01(void)
{
    PCU_ASSERT_EQUAL(1+1, 2);
}

test_example02(void)
{
    PCU_ASSERT_EQUAL(1+2, 2);
//    PCU_ASSERT_EQUAL(1+2, 3);
}

PCU_Suite *ExampleTest_suite(void)
{
	static PCU_Test tests[] = {
		PCU_TEST(test_example01),
		PCU_TEST(test_example02),
	};
	static PCU_Suite suite = { 
		"ExampleTest", tests, sizeof tests / sizeof tests[0] };
	return &suite;
}
