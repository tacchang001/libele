#include "ele_log.h"

#include <stdio.h>
#include <stdarg.h>
#include <libgen.h> // basename
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h> // isprint
#include "default.h"

#define DEFAULT_FORM_STRING_LENGTH_MAX		64

static ele_log_handler_t handlers[ELE_LOG_LEVEL_NUM];

/**
 *
 */
__attribute__((constructor))
static void ele_log_constructor(void)
{
	handlers[kLogLevelEmergency] = ele_log_syslog_handler;
	handlers[kLogLevelAlert] = ele_log_syslog_handler;
	handlers[kLogLevelCtitical] = ele_log_syslog_handler;
	handlers[kLogLevelError] = ele_log_print_handler;
	handlers[kLogLevelWarning] = ele_log_print_handler;
	handlers[kLogLevelNotice] = ele_log_print_handler;
	handlers[kLogLevelInfo] = ele_log_print_handler;
	handlers[kLogLevelDebug] = ele_log_print_handler;
}

/**
 *
 */
void ele_vlog_string(
    ele_log_level_t level,
    const char *file_name,
    const char *func_name,
    const unsigned int line_no,
    const char *fmt,
    va_list args)
{
	assert(file_name != NULL);
	if (file_name == NULL)
		return;
	assert(func_name != NULL);
	if (func_name == NULL)
		return;
	assert(fmt != NULL);
	if (fmt == NULL)
		return;

	char msg[ELE_LOG_STRING_LENGTH_MAX];
	vsnprintf(msg, sizeof(msg) / sizeof(msg[0]), fmt, args);
	switch (level) {
	case kLogLevelEmergency:
	case kLogLevelAlert:
	case kLogLevelCtitical:
	case kLogLevelError:
	case kLogLevelWarning:
	case kLogLevelNotice:
	case kLogLevelInfo:
	case kLogLevelDebug:
		do {
			if (handlers[level] != NULL) {
				(handlers[level])(level, file_name, func_name, line_no, msg);
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
void ele_log_string(
    ele_log_level_t level,
    const char *file_name,
    const char *func_name,
    const unsigned int line,
    const char *fmt,
    ...)
{
	assert(file_name != NULL);
	if (file_name == NULL)
		return;
	assert(func_name != NULL);
	if (func_name == NULL)
		return;
	assert(fmt != NULL);
	if (fmt == NULL)
		return;

	va_list args;
	va_start( args, fmt);
	ele_vlog_string(level, file_name, func_name, line, fmt, args);
	va_end( args);
}

static char* replace(char *dest, unsigned int pos, const char *src)
{
	if (dest == NULL)
		return NULL;
	if (src == NULL)
		return NULL;
	if (pos > strlen(dest))
		return NULL;
	if ((strlen(dest) - pos) <= strlen(src))
		return NULL;

	memcpy(dest + pos, src, strlen(src));

	return dest;
}

/**
 *
 */
void ele_log_dump(
    ele_log_level_t level,
    const char *file_name,
    const char *func_name,
    const unsigned int line_no,
    const void *top,
    const size_t length)
{
	assert(top != NULL);
	if (top == NULL)
		return;
	assert(length > 0);
	if (length <= 0)
		return;

	const unsigned int column = 16;
	const unsigned char *ptr = (unsigned char*) top;

	unsigned int i = 0;
	unsigned int pos = 0;
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
		sprintf(addr, "%04X:", i * column);
		replace(logmsg, lintop, addr);

		unsigned int j = 0;
		for (j = 0; j < column; j++) {
			pos = i * column + j;
			const char c = (char) *(ptr + pos);
			if (pos >= length) {
				break;
			} else {
				unsigned int offset = (j < 8) ? 0 : 1;
				char hex[16];
				sprintf(hex, "%02x ", c);
				replace(logmsg, hextop + j * 3 + offset, hex);
				char let[8];
				if (isprint(c) != 0)
					let[0] = c;
				else
					let[0] = ' ';
				let[1] = '\0';
				replace(logmsg, asctop + j, let);
			}
		}
		ele_log_string(level, file_name, func_name, line_no, logmsg);
		i++;
		pos++;
	}
}

/**
 *
 */
int ele_log_set_handler(ele_log_level_t level, ele_log_handler_t log_func)
{
	int id = -1;
	int j;
	ele_log_handler_t fp = handlers[level];
	if (fp == NULL) {
		id = j;
		handlers[level] = log_func;
	}

	return id;
}

/**
 *
 */
void ele_log_remove_handler(ele_log_level_t level, int id)
{
	if (handlers[level] != NULL) {
		handlers[level] = NULL;
	}
}

/**
 *
 */
void ele_log_set_default_handler(ele_log_level_t level)
{
	ele_log_set_handler(level, ele_log_print_handler);
}
