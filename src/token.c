#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

#include "token.h"
#include "error.h"

Token *make_token(int type, Pos pos) {
    Token *tok = (Token *)malloc(sizeof(Token));
    if (tok == NULL) error_hypex();
    tok->type = type;
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
    if (*value == NULL) {
        *value = (char *)malloc(sizeof(char) * 2);
        if (value == NULL) error_hypex();
        (*value)[0] = c;
        (*value)[1] = '\0';
        *len = 1;
    } else {
        (*value)[(*len)++] = c;
        *value = (char *)realloc(*value, sizeof(char) * (*len + 1));
        if (value == NULL) error_hypex();
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
    Token *r = make_token(tok->type, tok->pos);
    if (tok->value != NULL) {
        r->value = (char *)malloc(sizeof(char) * (tok->len + 1));
        if (r->value == NULL) error_hypex();
        strcpy(r->value, tok->value);
    }
    r->len = tok->len;
    if (r->type == KEYWORD) {
        r->id = tok->id;
    }
    if (r->type == INTEGER) {
        r->base = tok->base;
        r->num_value = (char *)malloc(sizeof(char) * (tok->num_len + 1));
        if (r->num_value == NULL) error_hypex();
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
    if (tok->type == INTEGER || tok->type == FLOAT)
        free(tok->num_value);
    free(tok->value);
    free(tok);
}