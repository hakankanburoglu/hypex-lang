#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdarg.h>
#include <string.h>

#include "error.h"

static inline void fatal_error(const char *format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    exit(EXIT_FAILURE);
}

static inline void print_error(const char *file, int line, int column, const char *format, ...) {
    va_list args;
    va_start(args, format);
    if (file != NULL) fprintf(stderr, "%s:", file);
    fprintf(stderr, "%d:%d error: ", line, column);
    vfprintf(stderr, format, args);
    va_end(args);
}

void error_hypex() {
    fatal_error("hypex error\n");
}

void error_file(const char *file) {
    fatal_error("no such file or directory: %s\n", file);
}

void error_file_proc(const char *file) {
    fatal_error("file processing error: %s\n", file);
}

void error_file_read(const char *file) {
    fatal_error("file could not be read completely: %s\n", file);
}

void error_char(const char *file, int line, int column, char c) {
    print_error(file, line, column, "invalid character: '%c' (0x%x)\n", c, c);
}

void error_number(const char *file, int line, int column, const char *value) {
    print_error(file, line, column, "invalid number literal: %s\n", value);
}

void error_indent(const char *file, int line, int column) {
    print_error(file, line, column, "invalid indentation\n");
}