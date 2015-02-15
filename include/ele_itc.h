#pragma once
// Inter-Thread communication

#include "ele_error.h"

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ele_queue ele_queue_t;
struct ele_queue {
	void *attribute;
};

ele_queue_t *
ele_queue_open(const char * name);

ele_result_t
ele_queue_close(ele_queue_t * qdes);

ele_result_t
ele_queue_send(ele_queue_t * qdes, const char * msg_ptr,
		size_t msg_len, unsigned int msg_flags);

ele_result_t
ele_queue_receive(ele_queue_t * qdes, char * msg_ptr,
		size_t msg_len);

int
ele_queue_get_desc(ele_queue_t * qdes);

#ifdef __cplusplus
}
#endif
