#ifndef LOG_H_
#define LOG_H_
/**
 * @file
 */

#include <stdbool.h>
#include <stdlib.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef LOG_STRING_LENGTH_MAX
#define LOG_STRING_LENGTH_MAX	2048
#endif

/**
 *
 */
typedef enum {
	LOG_LEVEL_MIN,
	LOG_LEVEL_ERROR = LOG_LEVEL_MIN,
	LOG_LEVEL_CRITICAL,
	LOG_LEVEL_WARNING,
	LOG_LEVEL_MESSAGE,
	LOG_LEVEL_INFO,
	LOG_LEVEL_DEBUG,
	LOG_LEVEL_MAX,

	LOG_LEVEL_NUM = (LOG_LEVEL_MAX - LOG_LEVEL_MIN)
} log_levels;

typedef void (*log_func_t)(
	log_levels,
	const char*,
	const char*,
	unsigned int,
	const char*
);

/**
 *
 */
void log_string(
	log_levels level,
	const char *file_name,
	const char *func_name,
	unsigned int line_no,
	const char *fmt,
	...
);

/**
 *
 */
void vlog_string(
	log_levels level,
	const char *file_name,
	const char *func_name,
	unsigned int line_no,
	const char *fmt,
	va_list args
);

/**
 *
 */
void log_dump(
	log_levels level,
	const char *file_name,
	const char *func_name,
	unsigned int line_no,
	const void *top,
	const size_t length
);

/**
 *
 */
int log_set_handler(
	log_levels level,
	log_func_t log_func
);

/**
 *
 */
void log_remove_handler(
	log_levels level,
	int id
);

/**
 *
 */
void log_set_default_handler(
	log_levels level
);

#define error(fmt, ...)	log_string(LOG_LEVEL_ERROR, __FILE__, __func__, __LINE__, fmt, ##__VA_ARGS__)
#define critical(fmt, ...)	log_string(LOG_LEVEL_CRITICAL, __FILE__, __func__, __LINE__, fmt, ##__VA_ARGS__)
#define warning(fmt, ...)	log_string(LOG_LEVEL_WARNING, __FILE__, __func__, __LINE__, fmt, ##__VA_ARGS__)
#define message(fmt, ...)	log_string(LOG_LEVEL_MESSAGE, __FILE__, __func__, __LINE__, fmt, ##__VA_ARGS__)
#define info(fmt, ...)		log_string(LOG_LEVEL_INFO, __FILE__, __func__,__LINE__, fmt, ##__VA_ARGS__)
#if !defined(NO_LOGLEVEL_DEBUG)
#define debug(fmt, ...)	log_string(LOG_LEVEL_DEBUG, __FILE__, __func__, __LINE__, fmt, ##__VA_ARGS__)
#else
#define debug(fmt, ...)	/* NOP */
#endif

#define error_dump(ptr, len)		log_dump(LOG_LEVEL_ERROR, __FILE__, __func__, __LINE__, ptr, len)
#define critical_dump(ptr, len)	log_dump(LOG_LEVEL_CRITICAL, __FILE__, __func__, __LINE__, ptr, len)
#define warning_dump(ptr, len)		log_dump(LOG_LEVEL_WARNING, __FILE__, __func__, __LINE__, ptr, len)
#define message_dump(ptr, len)		log_dump(LOG_LEVEL_MESSAGE, __FILE__, __func__, __LINE__, ptr, len)
#define info_dump(ptr, len)		log_dump(LOG_LEVEL_INFO, __FILE__, __func__,__LINE__, ptr, len)
#if !defined(NO_LOGLEVEL_DEBUG)
#define debug_dump(ptr, len)		log_dump(LOG_LEVEL_DEBUG, __FILE__, __func__, __LINE__, ptr, len)
#else
#define debug_dump(ptr, len)	/* NOP */
#endif

#ifdef __cplusplus
}
#endif

#endif /* LOG_H_ */
