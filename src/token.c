#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"
#include "token.h"

Token *make_token(int kind, Pos pos) {
    Token *tok = (Token *)malloc(sizeof(Token));
    if (!tok) error_hypex();
    tok->kind = kind;
    tok->value = NULL;
    tok->cap = 0;
    tok->len = 0;
    tok->pos = pos;
    return tok;
}

Token *copy_token(const Token *tok) {
    Token *r = make_token(tok->kind, tok->pos);
    if (tok->value) {
        r->value = (char *)malloc(sizeof(char) * (tok->cap + 1));
        if (!r->value) error_hypex();
        strcpy(r->value, tok->value);
    }
    r->cap = tok->cap;
    r->len = tok->len;
    if (r->kind == T_KEYWORD) {
        r->id = tok->id;
    }
    if (r->kind == T_INTEGER || r->kind == T_FLOAT) {
        r->num.base = tok->num.base;
        r->num.is_exp = tok->num.is_exp;
        r->num.is_neg = tok->num.is_neg;
    }
    return r;
}

void free_token(Token *tok) {
    free(tok->value);
    free(tok);
}
