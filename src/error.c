#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdnoreturn.h>
#include <string.h>

#include "error.h"

void error(const char *file, int line, int column, const char *format, ...) {
    va_list args;
    va_start(args, format);
    if (file) fprintf(stderr, "%s:", file);
    fprintf(stderr, "%d:%d: error: ", line, column);
    vfprintf(stderr, format, args);
    fprintf(stderr, "\n");
    va_end(args);
}

noreturn void internal_error(void) {
    fprintf(stderr, "internal error\n");
    exit(EXIT_FAILURE);
}
