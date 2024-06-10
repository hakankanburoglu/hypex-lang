#ifndef LEX_H
#define LEX_H

#include <stddef.h>
#include <stdbool.h>

#include "token.h"

void push_token(Token **tokens, size_t *len, const Token tok);

void consume_token(Token *tok, const char c);

void consume_wtoken(Token *tok, const wchar_t c);

bool is_keyword(const char *s, const size_t len);

bool is_ident_body(const char c);

Token *tokenize(const char *input, size_t *len, const bool is_continues, const int line, int current);

#endif //LEX_H