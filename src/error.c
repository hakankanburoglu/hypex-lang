#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdarg.h>
#include <string.h>

#include "error.h"

static inline void print_error(const char *format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
}

void error_hypex() {
    print_error("hypex error\n");
    exit(EXIT_FAILURE);
}

void error_file(const char *file) {
    print_error("no such file or directory: %s\n", file);
    exit(EXIT_FAILURE);
}

void error_file_proc(const char *file) {
    print_error("file processing error: %s\n", file);
    exit(EXIT_FAILURE);
}

void error_file_read(const char *file) {
    print_error("file could not be read completely: %s\n", file);
    exit(EXIT_FAILURE);
}

void error_char(const char *file, int line, int column, char c) {
    if (file != NULL) print_error("%s:", file);
    print_error("%d:%d error: invalid character: '%c' (0x%x)\n", line, column, c, c);
}

void error_number(const char *file, int line, int column, const char *value) {
    if (file != NULL) print_error("%s:", file);
    print_error("%d:%d error: invalid number literal: %s\n", line, column, value);
}

void error_indent(const char *file, int line, int column) {
    if (file != NULL) print_error("%s:", file);
    print_error("%d:%d error: invalid indentation\n", line, column);
}