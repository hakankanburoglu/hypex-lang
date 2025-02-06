#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#include "error.h"

void error_hypex() {
    fprintf(stderr, "hypex error\n");
}

void error_file(const char *file) {
    fprintf(stderr, "no such file or directory: %s\n", file);
    exit(EXIT_FAILURE);
}

void error_file_proc(const char *file) {
    fprintf(stderr, "file processing error: %s\n", file);
    exit(EXIT_FAILURE);
}

void error_file_read(const char *file) {
    fprintf(stderr, "file could not be read completely: %s\n", file);
    exit(EXIT_FAILURE);
}

void error_char(const char *file, int line, int col, char c) {
    if (file != NULL) fprintf(stderr, "%s:", file);
    fprintf(stderr, "%d:%d invalid character: '%c' (0x%x)\n", line, col, c, c);
    exit(EXIT_FAILURE);
}

void error_number(const char *file, int line, int col, const char *value) {
    if (file != NULL) fprintf(stderr, "%s:", file);
    fprintf(stderr, "%d:%d invalid number literal: %s\n", line, col, value);
    exit(EXIT_FAILURE);
}