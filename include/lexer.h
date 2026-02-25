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
        hypex_token **list;
        size_t cap;
        size_t len;
    } tokens;
    struct {
        int *stack;
        size_t cap;
        size_t len;
    } indents;
    int offset;
    hypex_pos pos;
    char cur;
    int state;
} hypex_lexer;

hypex_lexer *hypex_lexer_make(void);

void hypex_lexer_tokenize(hypex_lexer *lex);

void hypex_lexer_free(hypex_lexer *lex);

#endif // LEXER_H
