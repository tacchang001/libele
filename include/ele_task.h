#pragma once

#include <pthread.h>

/**
 *
 */
typedef void * (*ele_task_entry_t)(void*);

/**
 *
 */
typedef struct {
	int id;
	int schedpolicy;
	int schedparam;
	ele_task_entry_t entry;
	void* arg;
} ele_task_init_attr_t;

/**
 *
 */
int ele_task_create(
	const ele_task_init_attr_t attr);

/**
 *
 */
int ele_task_destroy(int id);

/**
 *
 */
int ele_task_join(int id);

/**
 *
 */
pthread_t ele_task_get_thread_id(
	const int id
);
