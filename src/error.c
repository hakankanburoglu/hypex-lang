#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdnoreturn.h>
#include <string.h>

#include "error.h"

noreturn void fatal_error(const char *format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    exit(EXIT_FAILURE);
}

void error(const char *file, int line, int column, const char *format, ...) {
    va_list args;
    va_start(args, format);
    if (file) fprintf(stderr, "%s:", file);
    fprintf(stderr, "%d:%d error: ", line, column);
    vfprintf(stderr, format, args);
    va_end(args);
}

void error_hypex() {
    fatal_error("hypex error\n");
}
