#include "ele_error.h"

#include <stdio.h>
#include <assert.h>

/**
 *
 */
void ele_error(
	const char * reason,
	const char * file,
	int line,
	int ele_errno)
{
	// not supported yet.
	assert(1);
	return;
}

/**
 *
 */
const char * ele_strerror(
	const int ele_errno)
{
	// not supported yet.
	assert(1);
	return NULL;
}
