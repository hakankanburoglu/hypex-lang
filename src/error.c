#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>

#include "error.h"

void error(const char *format, ...) {
    va_list args;
    va_start(args, format);
    fprintf(stderr, "%s: ", format);
    char *str;
    while ((str = va_arg(args, char *)) != NULL)
        fprintf(stderr, "%s ", str);
    va_end(args);
    fprintf(stderr, "\n");
    exit(EXIT_FAILURE);
}

void errorp(const int line, const int col, const char c, const char *format, ...) {
    va_list args;
    va_start(args, format);
    fprintf(stderr, "[%d:%d] ", (line + 1), (col + 1));
    fprintf(stderr, "%s: ", format);
    char *str;
    while ((str = va_arg(args, char *)) != NULL)
        fprintf(stderr, "%s ", str);
    va_end(args);
    if (isprint(c))
        fprintf(stderr, "'%c'\n", c);
    else
        fprintf(stderr, "0x%x\n", (unsigned char)c);
    exit(EXIT_FAILURE);
}