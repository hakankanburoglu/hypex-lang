#ifndef LEXER_H
#define LEXER_H

#include <stddef.h>
#include <stdbool.h>

#include "token.h"

typedef struct {
    char *input;
    size_t inputlen;
    char *file;
    Token **tok_list;
    size_t len;
    int *indent_stack;
    size_t stack_len;
    size_t stack_capacity;
    int indent;
    int offset;
    Pos pos;
    bool newline;
    bool potential_fstring;
    bool potential_rstring;
    bool fstring_body;
    bool fstring_expr;
} Lexer;

Lexer *init_lexer(const char *input, const char *file);

void consume_lex(Lexer *lex, Token *tok);

void tokenize(Lexer *lex);

void free_lex(Lexer *lex);

#endif //LEXER_H