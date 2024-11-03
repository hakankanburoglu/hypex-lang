#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "error.h"

void error_undef_arg(const char *arg) {
    fprintf(stderr, "%s: %s\n", ERR_UNDEF_ARG_MSG, arg);
    exit(EXIT_FAILURE);
}

void error_file(const char *path) {
    fprintf(stderr, "%s: %s\n", ERR_FILE_MSG, path);
    exit(EXIT_FAILURE);
}

void error_indent(const int line, const int pos) {
    fprintf(stderr, "[%d:%d] %s\n", line, pos, ERR_INDENT_MSG);
    exit(EXIT_FAILURE);
}

void error_char(const int line, const int col, const char c) {
    fprintf(stderr, "[%d:%d] %s: ", (line + 1), (col + 1), ERR_CHR_MSG);
    if (isprint(c))
        fprintf(stderr, "'%c'\n", c);
    else
        fprintf(stderr, "0x%x\n", (unsigned char)c);
    exit(EXIT_FAILURE);
}

void error_lex(const char *code) {
    fprintf(stderr, "%s: %s\n", ERR_LEX_MSG, code);
    exit(EXIT_FAILURE);
}

void error_parse(const char *code) {
    fprintf(stderr, "%s: %s\n", ERR_PARSE_MSG, code);
    exit(EXIT_FAILURE);
}

void error_ast_parse(const char *code) {
    fprintf(stderr, "%s: %s\n", ERR_AST_PARSE_MSG, code);
    exit(EXIT_FAILURE);
}