#pragma once

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

int ele_print(const char *format, ...);

int ele_printerr(const char *format, ...);

typedef int (*ele_print_handler_t)(const char *format, ...);
typedef int (*ele_printerr_handler_t)(const char *format, ...);

void ele_set_printable(bool enable);
bool ele_is_printable(void);

void ele_set_print_handler(ele_print_handler_t handler);
void ele_set_printerr_handler(ele_printerr_handler_t handler);

#ifdef __cplusplus
}
#endif
