#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"
#include "token.h"

hypex_token *hypex_token_make(hypex_token_kind kind, hypex_pos pos) {
    hypex_token *tok = malloc(sizeof *tok);
    if (!tok) hypex_internal_error();
    tok->kind = kind;
    tok->value = NULL;
    tok->cap = 0;
    tok->len = 0;
    tok->pos = pos;
    return tok;
}

hypex_token *copy_token(const hypex_token *tok) {
    hypex_token *r = malloc(sizeof *r);
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
        case HYPEX_TOK_KEYWORD:
            r->id = tok->id;
            break;
        case HYPEX_TOK_INTEGER: case HYPEX_TOK_FLOAT:
            r->num.base = tok->num.base;
            r->num.is_exp = tok->num.is_exp;
            r->num.is_neg = tok->num.is_neg;
            break;
        case HYPEX_TOK_INDENT: case HYPEX_TOK_DEDENT:
            r->level = tok->level;
            break;
        case HYPEX_TOK_NEWLINE:
            r->comment = tok->comment;
            break;
        default:
            break;
    }
    return r;
}

void hypex_token_free(hypex_token *tok) {
    free(tok->value);
    free(tok);
}
