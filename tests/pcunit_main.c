#include <PCUnit/PCUnit.h>
#include <string.h>
#include <stdio.h>

PCU_Suite *ExampleTest_suite(void);

int main(void)
{
	const PCU_SuiteMethod suites[] = {
		ExampleTest_suite,
	};

	PCU_enable_color();
	PCU_set_putchar(putchar);
	PCU_set_getchar(getchar);
	PCU_set_verbose(1);
	return PCU_run(suites, sizeof suites / sizeof *suites);
}

