#pragma once
// Inter-Thread communication

#include "ele_error.h"

#include <stddef.h>
#include <sys/eventfd.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ele_queue ele_queue_t;
struct ele_queue {
	void *attribute;
};

ele_queue_t *
ele_queue_create(const char * name);

void
ele_queue_destroy(ele_queue_t * qdes);

int
ele_queue_push_back(ele_queue_t * qdes, const char * msg_ptr,
		size_t msg_len);

int
ele_queue_pop_front(ele_queue_t * qdes, char * msg_ptr,
		size_t msg_len);

eventfd_t
ele_queue_get_desc(ele_queue_t * qdes);

#ifdef __cplusplus
}
#endif
