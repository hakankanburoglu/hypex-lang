#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

#include "lexer.h"
#include "file_lexer.h"
#include "error.h"

void lex_file(Lexer *lex) {
    FILE *file = fopen(lex->file, "r");
    if (file == NULL) error_file(lex->file);
    fseek(file, 0, SEEK_END);
    size_t filelen = ftell(file);
    if (filelen == -1L) {
        fclose(file);
        error_file_proc(lex->file);
    }
    rewind(file);
    lex->input = (char *)realloc(lex->input, sizeof(char) * (filelen + 1));
    if (lex->input == NULL) {
        fclose(file);
        error_hypex();
    }
    size_t readlen = fread(lex->input, 1, filelen, file);
    if (readlen < filelen) {
        if (feof(file)) {
            lex->input[readlen] = '\0';
            lex->inputlen = readlen;
        } else {
            error_file_read(lex->file); 
        }
        fclose(file);
        return;
    }
    lex->input[filelen] = '\0';
    fclose(file);
    lex->inputlen = readlen;
}