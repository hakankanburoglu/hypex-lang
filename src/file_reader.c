#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "error.h"
#include "file_reader.h"
#include "lexer.h"

void file_read(Lexer *lex) {
    FILE *file = fopen(lex->file, "r");
    if (!file) fatal_error("no such file or directory: %s", lex->file);
    fseek(file, 0, SEEK_END);
    long filelen = ftell(file);
    if (filelen < 0) {
        fclose(file);
        fatal_error("file processing error: %s", lex->file);
    }
    rewind(file);
    char *buf = malloc(sizeof *buf * (filelen + 1));
    if (!buf) internal_error();
    size_t readlen = fread(buf, 1, filelen, file);
    fclose(file);
    buf[readlen] = '\0';
    lex->input = buf;
    lex->inputlen = readlen;
    lex->cur = lex->input[0];
}
