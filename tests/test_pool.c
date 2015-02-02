/* Randomly test / stress the allocator */

#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>

#include "ele_pool.h"

#define PMAX 11

int main(int argc, char **argv)
{
	int i, sz, max_pool_sz;
	long seed;
	int *ip;

	/* Init a new ele_pool_t for values 2^4 to 2^PMAX */
	ele_pool_t *mp = ele_pool_create(4, PMAX);
	max_pool_sz = mp->max_size;
	if (argc > 1) {
		seed = atol(argv[1]);
	} else {
		struct timeval tv;
		if (gettimeofday(&tv, NULL) < 0) {
			fprintf(stderr, "gettimeofday fail\n");
			return 1;
		}
		seed = tv.tv_usec;
	}
	srandom(seed);

	for (i = 0; i < 5000000; i++) {
		sz = random() % 64;
		/* also alloc some larger chunks  */
		if (random() % 100 == 0)
			sz = random() % 10000;
		sz = sz ? sz : 1; /* no 0 allocs */

		ip = (int *) ele_pool_alloc(mp, sz);
		*ip = 7;

		/* randomly repool some of them */
		if (random() % 10 == 0) { /* repool, known size */
			ele_pool_free(mp, ip, sz);
		}
		if (i % 10000 == 0 && i > 0) {
			putchar('.');
			if (i % 700000 == 0)
				putchar('\n');
		}
	}

	ele_pool_destroy(mp);
	putchar('\n');
	return 0;
}
