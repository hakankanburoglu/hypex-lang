#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

#include "token.h"
#include "error.h"

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

void init_number(Token *tok) {
    tok->num.base = BASE_DEC;
    tok->num.value = NULL;
    tok->num.len = 0;
    tok->num.is_exp = false;
    tok->num.is_neg = false;
}

static void consume_value(char **value, size_t *len, size_t *cap, char c) {
    if (!*value) {
        *cap = 16;
        *value = (char *)realloc(*value, sizeof(char) * 17);
        if (!*value) error_hypex();
    }
    if (*len == *cap) {
        *cap *= 2;
        *value = (char *)realloc(*value, sizeof(char) * (*cap + 1));
        if (!*value) error_hypex();
    }
    (*value)[(*len)++] = c;
    (*value)[*len] = '\0';
}

void consume_token(Token *tok, char c) {
    consume_value(&tok->value, &tok->len, &tok->cap, c);
}

void consume_number(Token *tok, char c) {
    consume_value(&tok->value, &tok->len, &tok->cap, c);
    consume_value(&tok->num.value, &tok->num.len, &tok->cap, c);
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
    if (r->kind == T_INTEGER) {
        r->num.base = tok->num.base;
        r->num.value = (char *)malloc(sizeof(char) * (tok->cap));
        if (!r->num.value) error_hypex();
        strcpy(r->num.value, tok->num.value);
        r->num.len = tok->num.len;
        r->num.is_exp = tok->num.is_exp;
        r->num.is_neg = tok->num.is_neg;
    }
    return r;
}

void free_token(Token *tok) {
    if (tok->kind == T_INTEGER || tok->kind == T_FLOAT)
        free(tok->num.value);
    free(tok->value);
    free(tok);
}