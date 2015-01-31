#include "log.h"

#include <stdio.h>
#include <stdarg.h>
#include <libgen.h> // basename
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h> // isprint

#include "log_default.h"

#define DEFAULT_FORM_STRING_LENGTH_MAX		64

#ifndef LOG_FUNC_MAX
#define LOG_FUNC_MAX	16
#endif

static log_func_t handlers[LOG_LEVEL_NUM][LOG_FUNC_MAX];

/**
 *
 */
__attribute__((constructor))
static void init_log(void)
{
	int i, j;
	for (i=0; i<LOG_LEVEL_NUM; i++) {
		handlers[i][0] = log_print;
	}
	for (i=0; i<LOG_LEVEL_NUM; i++) {
		for (j=1; j<LOG_FUNC_MAX; j++) {
			handlers[i][j] = NULL;
		}
	}
}

/**
 *
 */
void vlog_string(
	log_levels level,
	const char *file_name,
	const char *func_name,
	const unsigned int line_no,
	const char *fmt,
	va_list args)
{
	assert(file_name != NULL);
	if (file_name == NULL) return;
	assert(func_name != NULL);
	if (func_name == NULL) return;
	assert(fmt != NULL);
	if (fmt == NULL) return;

	char msg[LOG_STRING_LENGTH_MAX];
	vsnprintf(msg, sizeof(msg)/sizeof(msg[0]), fmt, args);
	switch (level) {
    case LOG_LEVEL_ERROR:
    case LOG_LEVEL_CRITICAL:
    case LOG_LEVEL_WARNING:
    case LOG_LEVEL_MESSAGE:
    case LOG_LEVEL_INFO:
    case LOG_LEVEL_DEBUG:
    	do {
    		int i = 0;
			for (; i<LOG_FUNC_MAX; i++) {
				if (handlers[level][i] != NULL) {
					(handlers[level][i])(level, file_name, func_name, line_no, msg);
				}
			}
    	} while (0);
    	break;
    default:
    	assert(false);
    	break;
	}
}

/**
 *
 */
void log_string(
	log_levels level,
	const char *file_name,
	const char *func_name,
	const unsigned int line,
	const char *fmt,
	...)
{
	assert(file_name != NULL);
	if (file_name == NULL) return;
	assert(func_name != NULL);
	if (func_name == NULL) return;
	assert(fmt != NULL);
	if (fmt == NULL) return;

	va_list args;
	va_start( args, fmt );
	vlog_string(level, file_name, func_name, line, fmt, args);
	va_end( args );
}

static char* replace(
	char *dest,
	unsigned int pos,
	const char *src)
{
	if (dest == NULL) return NULL;
	if (src == NULL) return NULL;
	if (pos > strlen(dest)) return NULL;
	if ((strlen(dest)-pos) <= strlen(src)) return NULL;

	memcpy(dest+pos, src, strlen(src));

	return dest;
}

/**
 *
 */
void log_dump(
	log_levels level,
	const char *file_name,
	const char *func_name,
	const unsigned int line_no,
	const void *top,
	const size_t length)
{
	assert(top != NULL);
	if (top == NULL) return;
	assert(length > 0);
	if (length <= 0) return;

	const unsigned int column = 16;
	const unsigned char *ptr = (unsigned char*)top;

	unsigned int i=0;
	unsigned int pos=0;
	while (length > pos) {
		char logmsg[128];
		memset(logmsg, 0, sizeof(logmsg));
		const unsigned int lintop = 0;
		const unsigned int linlen = 5;
		const unsigned int hextop = lintop + linlen + 1;
		const unsigned int hexlen = 24 + 1 + 24;
		const unsigned int asctop = hextop + hexlen + 1;
		const unsigned int asclen = 16;
		const unsigned int maxlen = asctop + asclen;
		memset(logmsg, ' ', maxlen + 1);

		char addr[16];
		sprintf(addr, "%04X:", i*column);
		replace(logmsg, lintop, addr);

		unsigned int j=0;
		for (j=0; j<column; j++) {
			pos = i*column + j;
			const char c = (char)*(ptr + pos);
			if (pos >= length) {
				break;
			} else {
				unsigned int offset = (j < 8) ? 0 : 1;
				char hex[16];
				sprintf(hex, "%02x ", c);
				replace(logmsg, hextop + j*3+offset, hex);
				char let[8];
				if (isprint(c) != 0) let[0] = c; else let[0] = ' ';
				let[1] = '\0';
				replace(logmsg, asctop + j, let);
			}
		}
		log_string(level, file_name, func_name, line_no, logmsg);
		i++;
		pos++;
	}
}


/**
 *
 */
int log_set_handler(
	log_levels level,
	log_func_t log_func)
{
	int id = -1;
	int j;
	for (j=0; j<LOG_FUNC_MAX; j++) {
		log_func_t fp = handlers[level][j];
		if (fp == NULL) {
			id = j;
			handlers[level][j] = log_func;
			break;
		}
	}

	return id;
}

/**
 *
 */
void log_remove_handler(
	log_levels level,
	int id)
{
	if (handlers[level][id] != NULL) {
		handlers[level][id] = NULL;
	}
}

/**
 *
 */
void log_set_default_handler(
	log_levels level)
{
	log_set_handler(level, log_print);
}
