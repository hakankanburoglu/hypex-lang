#ifndef COMP_H
#define COMP_H

#include <stddef.h>

#include "token.h"

void print_token(const Token tok);

void print_lex(const Token *tokens, const size_t len);

void push_indent(int **indents, int *indentlen, const int indent);

void concat_token(Token **tokens, size_t *toklen, const Token *buf, const size_t buflen);

void comp(const char *path);

#endif //COMP_H