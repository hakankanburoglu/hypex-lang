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
    tok->len = 0;
    tok->pos = pos;
    return tok;
}

void init_number(Token *tok) {
    tok->base = BASE_DEC;
    tok->num_value = NULL;
    tok->num_len = 0;
    tok->is_exponent = false;
    tok->is_negative = false;
}

static void consume_value(char **value, size_t *len, char c) {
    if (!*value) {
        *value = (char *)malloc(sizeof(char) * 2);
        if (!value) error_hypex();
        (*value)[0] = c;
        (*value)[1] = '\0';
        *len = 1;
    } else {
        (*value)[(*len)++] = c;
        *value = (char *)realloc(*value, sizeof(char) * (*len + 1));
        if (!value) error_hypex();
        (*value)[*len] = '\0';
    }
}

void consume_token(Token *tok, char c) {
    consume_value(&tok->value, &tok->len, c);
}

void consume_number(Token *tok, char c) {
    consume_value(&tok->value, &tok->len, c);
    consume_value(&tok->num_value, &tok->num_len, c);
}

Token *copy_token(const Token *tok) {
    Token *r = make_token(tok->kind, tok->pos);
    if (tok->value) {
        r->value = (char *)malloc(sizeof(char) * (tok->len + 1));
        if (!r->value) error_hypex();
        strcpy(r->value, tok->value);
    }
    r->len = tok->len;
    if (r->kind == T_KEYWORD) {
        r->id = tok->id;
    }
    if (r->kind == T_INTEGER) {
        r->base = tok->base;
        r->num_value = (char *)malloc(sizeof(char) * (tok->num_len + 1));
        if (!r->num_value) error_hypex();
        strcpy(r->num_value, tok->num_value);
        r->is_exponent = tok->is_exponent;
        r->is_negative = tok->is_negative;
    }
    return r;
}

bool is_operator_token(Token *tok) {
    return tok->type > 0 && tok->type < 51;
}

bool is_number_token(Token *tok) {
    return tok->type == INTEGER || tok->type == FLOAT;
}

void free_token(Token *tok) {
    if (tok->kind == T_INTEGER || tok->kind == T_FLOAT)
        free(tok->num_value);
    free(tok->value);
    free(tok);
}