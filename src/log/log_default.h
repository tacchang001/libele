#pragma

#include <stdbool.h>
#include <stdlib.h>
#include <stdarg.h>

#include "ele_log.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 *
 */
void ele_log_print(
    ele_log_level_t level,
    const char *file_name,
    const char *func_name,
    unsigned int line_no,
    const char *message);

#ifdef __cplusplus
}
#endif
