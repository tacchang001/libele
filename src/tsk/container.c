#include "tsk.h"

#include <assert.h>
#include <pthread.h>
#include <stdio.h>

/*
 *
 */
static void
tsk_cleanup_container(void *arg)
{
	printf("container completed. arg=%p\n", arg);
	return ;
}

/*
 *
 */
//#pragma GCC diagnostic ignored "-Wunused-parameter"
static void
tsk_cleanup_handler(void *arg)
{
	printf("on popup(%s) arg=%p\n", __func__, arg);
	tsk_cleanup_container(arg);
	return ;
}
//#pragma GCC diagnostic warning "-Wunused-parameter"

/*
 *
 */
void * ele_task_container(void * arg)
{
	assert(arg != NULL);

	int execute = 0; // to avoid -Wunreachable-code warning
	do {
		pthread_cleanup_push(tsk_cleanup_handler, NULL);

		const ele_task_func_call_t * const func_call = arg;
		func_call->entry(func_call->arg);

		pthread_cleanup_pop(execute);
	} while (0);

	int a = 1;
	tsk_cleanup_container(&a);

	return NULL;
}

