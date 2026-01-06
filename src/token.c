#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"
#include "token.h"

Token *make_token(int kind, Pos pos) {
    Token *tok = malloc(sizeof *tok);
    if (!tok) internal_error();
    tok->kind = kind;
    tok->value = NULL;
    tok->cap = 0;
    tok->len = 0;
    tok->pos = pos;
    return tok;
}

Token *copy_token(const Token *tok) {
    Token *r = malloc(sizeof *r);
    if (!r) internal_error();
    r->kind = tok->kind;
    if (tok->value) {
        r->value = malloc(sizeof *r->value * (tok->cap + 1));
        if (!r->value) internal_error();
        strcpy(r->value, tok->value);
    }
    r->cap = tok->cap;
    r->len = tok->len;
    r->pos = tok->pos;
    switch (r->kind) {
        case T_KEYWORD:
            r->id = tok->id;
            break;
        case T_INTEGER: case T_FLOAT:
            r->num.base = tok->num.base;
            r->num.is_exp = tok->num.is_exp;
            r->num.is_neg = tok->num.is_neg;
            break;
        case T_INDENT: case T_DEDENT:
            r->level = tok->level;
            break;
        case T_NEWLINE:
            r->comment = tok->comment;
            break;
        default:
            break;
    }
    return r;
}

void free_token(Token *tok) {
    free(tok->value);
    free(tok);
}
