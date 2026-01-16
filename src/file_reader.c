#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "error.h"
#include "file_reader.h"
#include "lexer.h"

char *file_read(const char *path, size_t *len) {
    FILE *file = fopen(path, "r");
    if (!file) fatal_error("no such file or directory: %s", path);
    if (fseek(file, 0, SEEK_END) != 0) fatal_error("could not read file: %s", path);
    long filesize = ftell(file);
    if (filesize < 0) fatal_error("could not read file: %s", path);
    rewind(file);
    size_t filelen = (size_t)filesize;
    char *buf = malloc(filelen + 1);
    if (!buf) internal_error();
    size_t readlen = fread(buf, 1, filelen, file);
    fclose(file);
    buf[readlen] = '\0';
    if (len) *len = readlen;
    return buf;
}
