#include "tsk.h"

#include <assert.h>
#include <pthread.h>
#include <stdio.h>

/*
 *
 *
 */
//#pragma GCC diagnostic ignored "-Wunused-parameter"
static void
ele_tsk_cleanup_container(void *arg)
{
	printf("%s, arg=%p\n", __func__, arg);
	return ;
}
//#pragma GCC diagnostic warning "-Wunused-parameter"

/*
 *
 *
 */
void * ele_task_entry(void * arg)
{
	assert(arg != NULL);

	int execute = 0; // avoid -Wunreachable-code warning
	do {
		pthread_cleanup_push(ele_tsk_cleanup_container, NULL);

		const ele_task_func_call_t * const func_call = arg;
		func_call->entry(func_call->arg);

		pthread_cleanup_pop(execute);
	} while (0);

	return NULL;
}

