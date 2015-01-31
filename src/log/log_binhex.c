#include "log_binhex.h"

#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <limits.h>

/**
 *
 */
size_t ele_log_text_to_bytes(char * const input, void *output)
{
	assert(input != NULL);
	assert(output != NULL);

	const char *delm = " \r\n";

	size_t length = 0;
	char *outptr = output;

	char *ptr = strtok(input, delm);
	while (ptr != NULL)
	{
		char *endptr = NULL;
		errno = 0;
		long n = strtol(ptr, &endptr, 16);
		if (errno == 0) {
			if (*endptr != '\0') {
				break;
			}
			if ((n >= 0) && (n <= UCHAR_MAX)) {
				*(outptr + length) = (char)n;
				++length;
			} else {
				fprintf(stderr, "out of range=%ld(<=%d)", n, UCHAR_MAX);
				length = 0;
				break;
			}
			ptr = strtok(NULL, delm);
		} else {
			perror("strtol");
			length = 0;
			break;
		}
	}

	return length;
}
