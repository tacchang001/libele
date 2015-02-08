#include "ele_perf.h"

#include <stdio.h>

void measure_clock(
	const struct timespec lap,
	uint32_t attr[ELE_MES_DATA_BLOCKS])
{
	printf("%llu.%06llu,%u,%u,%d,%d,%d\n", lap.tv_sec, lap.tv_nsec,
		attr[0], attr[1], attr[2], attr[3], attr[4]) ;
}
