#include "ele_print.h"

#include <stdio.h>
#include <stdbool.h>
#include <stdarg.h>
#include <assert.h>

static __thread bool printable = true;

int ele_null_print(const char *format, ...) {
	return 0;
}

int ele_null_printerr(const char *format, ...) {
	return 0;
}

int ele_default_printerr(const char *format, ...) {
	va_list args;
	va_start(args, format);
	int result = vfprintf(stderr, format, args);
	va_end(args);
	return result;
}

static __thread ele_print_handler_t print_handler = printf;
static __thread ele_printerr_handler_t printerr_handler = ele_default_printerr;
static __thread ele_print_handler_t current_print_handler = printf;
static __thread ele_printerr_handler_t current_printerr_handler = ele_default_printerr;

int ele_print(const char *format, ...) {
	va_list args;
	va_start(args, format);
	int result = current_print_handler(format, args);
	va_end(args);
	return result;
}

int ele_printerr(const char *format, ...) {
	va_list args;
	va_start(args, format);
	int result = current_printerr_handler(format, args);
	va_end(args);
	return result;
}

void ele_printable(bool enable) {
	if (printable != enable) {
		current_print_handler =
			enable ? print_handler : ele_null_print;
		current_printerr_handler =
			enable ? printerr_handler : ele_null_printerr;
	}
}

inline bool ele_get_printable(void) {
	return printable;
}

void ele_set_print_handler(ele_print_handler_t handler) {
	assert(handler != NULL);
	print_handler = handler;
}

void ele_set_printerr_handler(ele_printerr_handler_t handler) {
	assert(handler != NULL);
	printerr_handler = handler;
}
