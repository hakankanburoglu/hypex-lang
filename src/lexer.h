#ifndef LEXER_H
#define LEXER_H

#include <stdbool.h>
#include <stddef.h>

#include "token.h"

typedef struct {
    char *input;
    size_t inputlen;
    char *file;
    struct {
        Token **list;
        size_t cap;
        size_t len;
    } tokens;
    struct {
        int *stack;
        size_t cap;
        size_t len;
    } indents;
    int offset;
    Pos pos;
    char cur;
    int indent;
    bool newline;
    bool potential_fstring;
    bool potential_rstring;
    bool fstring_body;
    bool fstring_expr;
} Lexer;

Lexer *init_lexer(const char *input, const char *file);

void tokenize(Lexer *lex);

void free_lex(Lexer *lex);

#endif //LEXER_H
