#pragma once

#include <errno.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * gsl_error.hを参考にする。
 */

/**
 *
 */
enum {
	ELE_SUCCESS = 0,
	ELE_FAILURE = -1,
	ELE_ERANGE = 1,  // output range error
};

/**
 *
 */
void ele_error(const char * reason, const char * file, int line, int ele_errno);

/**
 *
 */
const char * ele_strerror(const int ele_errno);

/**
 *
 */
#define ELE_ERROR(reason, ele_errno) \
   do { \
	   ele_error (reason, __FILE__, __LINE__, ele_errno) ; \
	   return ele_errno ; \
   } while (0)

/**
 *
 */
#define GSL_ERROR_RETURN(reason, ele_errno, value) \
	do { \
		ele_error (reason, __FILE__, __LINE__, ele_errno) ; \
		return value ; \
	} while (0)

/**
 *
 */
#define GSL_ERROR_RETURN_NOTHING(reason, ele_errno) \
	do { \
		ele_error (reason, __FILE__, __LINE__, ele_errno) ; \
		return ; \
	} while (0)

/**
 *
 */
#define GSL_ERROR_CONTINUE(reason, ele_errno) \
	do { \
		ele_error (reason, __FILE__, __LINE__, ele_errno) ; \
	} while (0)

#ifdef __cplusplus
}
#endif
