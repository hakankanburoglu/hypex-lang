#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "error.h"
#include "file_lexer.h"
#include "lexer.h"

void lex_file(Lexer *lex) {
    FILE *file = fopen(lex->file, "r");
    if (!file) fatal_error("no such file or directory: %s", lex->file);
    fseek(file, 0, SEEK_END);
    size_t filelen = ftell(file);
    if (filelen == -1L) {
        fclose(file);
        fatal_error("file processing error: %s", lex->file);
    }
    rewind(file);
    lex->input = (char *)realloc(lex->input, sizeof(char) * (filelen + 1));
    if (!lex->input) {
        fclose(file);
        error_hypex();
    }
    size_t readlen = fread(lex->input, 1, filelen, file);
    lex->cur = lex->input[0];
    if (readlen < filelen) {
        if (feof(file)) {
            lex->input[readlen] = '\0';
            lex->inputlen = readlen;
        } else {
            fclose(file);
            fatal_error("file could not be read completely: %s", lex->file);
        }
        fclose(file);
        return;
    }
    lex->input[filelen] = '\0';
    fclose(file);
    lex->inputlen = readlen;
}
