#include "ele_task.h"
#include "ele_error.h"

#include <stdio.h>
#include <unistd.h>

static int cnt = 0;

static void
ele_cleanup_tsk1(void *arg)
{
	printf("%s\n", __func__);
	return ;
}

void * anything(void * arg) {
	pthread_cleanup_push(ele_cleanup_tsk1, NULL);

	int value = *(int*)arg;
	fprintf(stderr, "anything = %d\n", value);

	time_t start, curr;
	curr = start = time(NULL);
	while (1) {
		pthread_testcancel(); /* A cancellation point */
		if (curr < time(NULL)) {
			curr = time(NULL);
			printf("cnt = %d\n", cnt); /* A cancellation point */
			cnt++;
		}
	}
	printf("\ndone.\n", value);

	pthread_cleanup_pop(0);

	return NULL;
}

const int TASK_ID = 100;

int main(int argc, char *argv[]) {
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

	sleep(2);

	ele_task_destroy(TASK_ID);

	printf("completed.\n");

	return 0;
}
