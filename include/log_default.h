#ifndef LOG_DEFAULT_H_
#define LOG_DEFAULT_H_

#include <stdbool.h>
#include <stdlib.h>
#include <stdarg.h>

#include "log.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 *
 */
void log_print(
	log_levels level,
	const char *file_name,
	const char *func_name,
	unsigned int line_no,
	const char *message
);

#ifdef __cplusplus
}
#endif

#endif /* LOG_DEFAULT_H_ */
