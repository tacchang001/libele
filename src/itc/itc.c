#include "ele_itc.h"

#include <sys/eventfd.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>

#include "ele_mempool.h"

#define QUEUE_ATTRIBUTE(ptr)((queue_attribute_t*)(ptr->attribute))

typedef struct {
	eventfd_t efd;
	ele_mempool_desc_t pdesc;
	char name[64];
} queue_attribute_t ;

ele_queue_t *
ele_queue_create(const char * name)
{
	assert(name != NULL);
	assert(strlen(name) > 0);

	eventfd_t efd = eventfd(0, 0);
	if (efd == -1) {
		perror("eventfd");
		return NULL;
	}

	const size_t DATA_AREA_SIZE = 4096 * 10; // T.B.D.
	ele_mempool_desc_t pdesc = ele_mempool_create(DATA_AREA_SIZE);
	if (! ELE_MEMPOOL_VALID_DESC(pdesc)) {
		close(efd);
		return NULL;
	}

	queue_attribute_t * qattr = calloc(1, sizeof(queue_attribute_t));
	if (qattr == NULL) {
		perror("calloc");
		ele_mempool_destroy(&pdesc);
		close(efd);
		return NULL;
	}
	ele_queue_t * qdes = malloc(sizeof(ele_queue_t));
	if (qdes == NULL) {
		perror("malloc");
		ele_mempool_destroy(&pdesc);
		close(efd);
		free(qattr);
		return NULL;
	}

	qattr->efd = efd;
	qattr->pdesc = pdesc;
	strncpy(qattr->name, name, sizeof(qattr->name));
	qdes->attribute = qattr;

	return qdes;
}

void
ele_queue_destroy(ele_queue_t * qdes)
{
	assert(qdes != NULL);
	assert(qdes->attribute != NULL);
	assert(QUEUE_ATTRIBUTE(qdes)->efd != -1);
	assert(ELE_MEMPOOL_VALID_DESC(&QUEUE_ATTRIBUTE(qdes)->pdesc));

	ele_mempool_destroy(&QUEUE_ATTRIBUTE(qdes)->pdesc);
	close(QUEUE_ATTRIBUTE(qdes)->efd);
	free(qdes->attribute);
	free(qdes);
	return;
}

int
ele_queue_send(ele_queue_t * qdes, const char * msg_ptr,
		size_t msg_len, unsigned int msg_flags)
{
	assert(qdes != NULL);
	assert(qdes->attribute != NULL);
	assert(QUEUE_ATTRIBUTE(qdes)->efd != -1);
	assert(msg_ptr != NULL);
	assert(msg_len > 0);

	u_int64_t value = 1;
	int write_result = eventfd_write(QUEUE_ATTRIBUTE(qdes)->efd, value);
	if (write_result == -1) {
		perror("eventfd_write");
		return ELE_FAILURE;
	}

	return ELE_SUCCESS;
}

int
ele_queue_receive(ele_queue_t * qdes, char * msg_ptr,
		size_t msg_len)
{
	assert(qdes != NULL);
	assert(qdes->attribute != NULL);
	assert(QUEUE_ATTRIBUTE(qdes)->efd != -1);
	assert(msg_ptr != NULL);
	assert(msg_len > 0);

	u_int64_t value;
	int read_result = eventfd_read(QUEUE_ATTRIBUTE(qdes)->efd, &value);
	if (read_result == -1) {
		perror("eventfd_read");
		return ELE_FAILURE;
	}

	return ELE_SUCCESS;
}

eventfd_t
ele_queue_get_desc(ele_queue_t * qdes)
{
	assert(qdes != NULL);
	assert(qdes->attribute != NULL);

	return QUEUE_ATTRIBUTE(qdes)->efd;
}
