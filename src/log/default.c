#include "default.h"

#include <linux/limits.h> // PATH_MAX
#include <sys/time.h> // timeval
#include <stdio.h>
#include <assert.h>
#include <time.h> // localtime
#include <libgen.h> // basename
#include <string.h> // strncpy
#include <syslog.h>

#define DEFAULT_TIME_STRING_LENGTH_MAX		64

/**
 *
 */
static const char *sig[] =
{
    "EMERG",	//
    "ALERT",	//
    "CRIT",		//
    "ERR",
    "WARNING",
    "Notice",
    "info",
    "debug" };

__attribute__((constructor))
static void ele_log_default_constructor(void)
{
	openlog(NULL, LOG_CONS | LOG_PID, LOG_USER);
}

/**
 *
 */
static void get_time_string(char * const timestr)
{
	assert(timestr != NULL);
	if (timestr == NULL)
		return;

	struct timeval tv;
	struct tm *ptm, ltr;
	gettimeofday(&tv, 0x00);
	ptm = localtime_r(&tv.tv_sec, &ltr);
	snprintf(timestr, DEFAULT_TIME_STRING_LENGTH_MAX, "%02d:%02d:%02d.%03d",
	         ptm->tm_hour, ptm->tm_min, ptm->tm_sec, (int) (tv.tv_usec / 1000));
}

/**
 *
 */
static int get_syslog_priority(ele_log_level_t level)
{
	int syslog_level = LOG_USER;

	switch (level) {
	case kLogLevelEmergency:
		syslog_level |= LOG_EMERG;
		break;
	case kLogLevelAlert:
		syslog_level |= LOG_ALERT;
		break;
	case kLogLevelCtitical:
		syslog_level |= LOG_CRIT;
		break;
	case kLogLevelError:
		syslog_level |= LOG_ERR;
		break;
	case kLogLevelWarning:
		syslog_level |= LOG_WARNING;
		break;
	case kLogLevelNotice:
		syslog_level |= LOG_NOTICE;
		break;
	case kLogLevelInfo:
		syslog_level |= LOG_INFO;
		break;
	case kLogLevelDebug:
		syslog_level |= LOG_DEBUG;
		break;
	default:
		break;
	}

	return syslog_level;
}

/**
 *
 */
void ele_log_print_handler(
    ele_log_level_t level,
    const char *file_name,
    const char *func_name,
    const unsigned int line_no,
    const char *message)
{
	assert(file_name != NULL);
	if (file_name == NULL)
		return;
	assert(func_name != NULL);
	if (func_name == NULL)
		return;
	assert(message != NULL);
	if (message == NULL)
		return;

	char tstr[DEFAULT_TIME_STRING_LENGTH_MAX];
	get_time_string(tstr);

	switch (level) {
	case kLogLevelEmergency:
	case kLogLevelAlert:
	case kLogLevelCtitical:
		syslog(get_syslog_priority(level), message);
		fprintf(stderr, "%s: ** %s **: %s\n", tstr, sig[level], message);
		break;
	case kLogLevelError:
	case kLogLevelWarning:
		fprintf(stderr, "%s: ** %s **: %s\n", tstr, sig[level], message);
		break;
	case kLogLevelNotice:
		fprintf(stdout, "%s: %s\n", tstr, message);
		break;
	case kLogLevelInfo:
		fprintf(stdout, "%s: %s: %s\n", tstr, sig[level], message);
		break;
	case kLogLevelDebug:
		do {
			char fname[PATH_MAX];
			strncpy(fname, file_name, sizeof(fname) / sizeof(fname[0]));
			char *bname = basename(fname);
			char *ptr = strchr(bname, '.');
			*ptr = '\0';
			fprintf(stdout, "%s: %s(%d): %s\n", tstr, bname, line_no, message);
		} while (0);
		break;
	default:
		assert(false);
		break;
	}
}
