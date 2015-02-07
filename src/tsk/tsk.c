#include "ele_tsk.h"

#define ELE_TSK_GLOBAL
#include "tsk.h"
#undef  ELE_TSK_GLOBAL

#include <assert.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>

#include "ele_error.h"

const int INVALID_TSK_NO = -1;

static ele_task_attr_t tab[ELE_TSK_CREATE_LIMIT];
static const size_t TAB_SIZ = (sizeof(tab)/sizeof(tab[0]));

__attribute__((constructor))
static void ele_task_tab_init(void)
{
	unsigned int i;
	for (i=0; i<TAB_SIZ; i++) {
		tab[i].init_attr.id = INVALID_TSK_NO;
		tab[i].thread_id = 0;
	}
}

/*
 *
 */
pthread_t ele_task_get_thread_id(
	const int id)
{
	unsigned int i=0;
	for (i=0; i<TAB_SIZ; i++) {
		if (tab[i].init_attr.id == id) {
			return tab[i].thread_id;
		}
	}

	return 0;
}

/*
 *
 */
static ele_task_attr_t * ele_task_get_tab_space(void)
{
	unsigned int i;
	for (i=0; i<TAB_SIZ; i++) {
		if (tab[i].init_attr.id == INVALID_TSK_NO) {
			return &tab[i];
		}
	}

	return NULL;
}

/*
 *
 */
int ele_task_create(
	const ele_task_init_attr_t attr)
{
	assert(attr.id > 0);

	ele_task_attr_t * const rec = ele_task_get_tab_space();
	if (rec == NULL) {
		return ELE_FAILURE;
	}

	pthread_attr_t a;
	memset(&a, 0, sizeof(a));
	if (pthread_attr_init(&a) != 0) {
		ELE_PERROR("pthread_attr_init");
		return ELE_FAILURE;
	}
	switch (attr.schedpolicy) {
	case SCHED_FIFO:
	case SCHED_RR:
		if (pthread_attr_setinheritsched(&a, PTHREAD_EXPLICIT_SCHED) != 0) {
			ELE_PERROR("pthread_attr_setinheritsched");
			return ELE_FAILURE;
		}
		if (pthread_attr_setschedpolicy(&a, attr.schedpolicy) != 0) {
			ELE_PERROR("pthread_attr_setschedpolicy");
			return ELE_FAILURE;
		}
		const struct sched_param sp = {
			attr.schedparam
		};
		if (pthread_attr_setschedparam(&a, &sp) != 0) {
			ELE_PERROR("pthread_attr_setschedparam");
			ELE_ERROR_OUTOFRANGE(
				sched_get_priority_min(attr.schedpolicy),
				sp.__sched_priority,
				sched_get_priority_max(attr.schedpolicy));
			return ELE_FAILURE;
		}
		break;
	default:
		break;
	}
	static ele_task_func_call_t func_call;
	func_call.entry = attr.entry;
	func_call.arg = attr.arg;
	pthread_t id = 0;
	if (pthread_create(&id, &a, ele_task_entry, &func_call) != 0) {
		ELE_PERROR("pthread_create");
		return ELE_FAILURE;
	}

	rec->init_attr = attr;
	rec->thread_id = id;

	return ELE_SUCCESS;
}

/*
 *
 */
int ele_task_destroy(int id)
{
	assert(id > 0);

	const pthread_t tid = ele_task_get_thread_id(id);

	if (pthread_cancel(tid) != 0) {
		ELE_PERROR("pthread_cancel");
	}

	void *result = NULL;
	if (pthread_join(tid, &result) != 0) {
		ELE_PERROR("pthread_join");
	}

	return 0;
}

