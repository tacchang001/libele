#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <errno.h>

/*
 * gsl_error.hを参考にする。
 */

/**
 *
 */
typedef enum {
	ELE_SUCCESS = 0,
	ELE_FAILURE = -1,
	ELE_ERANGE = 1,  // output range error
} ele_result_t;

typedef void ele_error_handler_t(
    const char * reason,
    const char * file,
    int line,
    int ele_errno);

/**
 *
 */
void
ele_error(const char * reason, const char * file, int line, int ele_errno);

/**
 *
 */
const char *
ele_strerror(const int ele_errno);

/**
 *
 */
ele_error_handler_t *
ele_set_error_handler(ele_error_handler_t * new_handler);

/**
 *
 */
ele_error_handler_t *
ele_set_error_handler_off(void);

/**
 *
 */
#define ELE_ERROR(reason) \
   do { \
	   const int error_no = errno; \
	   ele_error (reason, __FILE__, __LINE__, error_no) ; \
	   return ELE_FAILURE ; \
   } while (0)

/**
 *
 */
#define ELE_ERROR_RETURN(reason, value) \
	do { \
		const int error_no = errno; \
		ele_error (reason, __FILE__, __LINE__, error_no) ; \
		return value ; \
	} while (0)

/**
 *
 */
#define ELE_ERROR_RETURN_NOTHING(reason) \
	do { \
		const int error_no = errno; \
		ele_error (reason, __FILE__, __LINE__, error_no) ; \
		return ; \
	} while (0)

/**
 *
 */
#define ELE_ERROR_CONTINUE(reason) \
	do { \
		const int error_no = errno; \
		ele_error (reason, __FILE__, __LINE__, error_no) ; \
	} while (0)

#ifdef __cplusplus
}
#endif
