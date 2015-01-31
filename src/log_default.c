#include "log_default.h"

#include <linux/limits.h> // PATH_MAX

#include <sys/time.h> // timeval

#include <stdio.h>
#include <assert.h>
#include <time.h> // localtime
#include <libgen.h> // basename
#include <string.h> // strncpy

#define DEFAULT_TIME_STRING_LENGTH_MAX		64

/**
 *
 */
static const char *sig[] = {
	"ERROR",
	"CRITICAL",
	"WARNING",
	"message",
	"info",
	"debug"
};

/**
 *
 */
static void get_time_string(
	char * const timestr)
{
	assert(timestr != NULL);
	if (timestr == NULL) return;

	struct timeval	tv;
	struct tm *ptm, ltr;
	gettimeofday(&tv, 0x00);
	ptm = localtime_r(&tv.tv_sec, &ltr);
	snprintf(timestr, DEFAULT_TIME_STRING_LENGTH_MAX,
		"%02d:%02d:%02d.%03d",
		ptm->tm_hour,
		ptm->tm_min,
		ptm->tm_sec,
		(int)(tv.tv_usec / 1000)
	);
}

/**
 *
 */
void log_print(
	log_levels level,
	const char *file_name,
	const char *func_name,
	const unsigned int line_no,
	const char *message)
{
	assert(file_name != NULL);
	if (file_name == NULL) return;
	assert(func_name != NULL);
	if (func_name == NULL) return;
	assert(message != NULL);
	if (message == NULL) return;

	char tstr[DEFAULT_TIME_STRING_LENGTH_MAX];
	get_time_string(tstr);

    switch (level) {
    case LOG_LEVEL_ERROR:
    case LOG_LEVEL_CRITICAL:
    case LOG_LEVEL_WARNING:
		fprintf(
			stderr,
			"%s: ** %s **: %s\n",
			tstr, sig[level], message);
		break;
    case LOG_LEVEL_MESSAGE:
		fprintf(
			stdout,
			"%s: %s\n",
			tstr, message);
    	break;
    case LOG_LEVEL_INFO:
		fprintf(
			stdout,
			"%s: %s: %s\n",
			tstr, sig[level], message);
    	break;
    case LOG_LEVEL_DEBUG:
    	do {
    		char fname[PATH_MAX];
    		strncpy(fname, file_name, sizeof(fname)/sizeof(fname[0]));
    		char *bname = basename(fname);
    		char *ptr = strchr(bname, '.');
    		*ptr = '\0';
			fprintf(
				stdout,
				"%s: %s(%d): %s\n",
				tstr, bname, line_no, message);
    	} while (0);
    	break;
    default:
    	assert(false);
    	break;
    }
}
