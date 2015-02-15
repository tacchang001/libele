#include "ele_itc.h"

#include <sys/eventfd.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#define QUEUE_ATTRIBUTE(ptr)((queue_attribute_t*)(ptr->attribute))

typedef struct {
	eventfd_t efd;
	char name[64];
} queue_attribute_t ;

ele_queue_t *
ele_queue_open(const char * name)
{

}

ele_result_t
ele_queue_close(ele_queue_t * qdes)
{

}

ele_result_t
ele_queue_send(ele_queue_t * qdes, const char * msg_ptr,
		size_t msg_len, unsigned int msg_flags)
{

}

ele_result_t
ele_queue_receive(ele_queue_t * qdes, char * msg_ptr,
		size_t msg_len)
{

}

int
ele_queue_get_desc(ele_queue_t * qdes)
{
	return QUEUE_ATTRIBUTE(qdes)->efd;
}
