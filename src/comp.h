#ifndef COMP_H
#define COMP_H

#include <stddef.h>

#include "token.h"
#include "node.h"

void push_indent(int **indents, int *indentlen, const int indent);

void pop_indent(int **indents, int *indentlen);

void concat_token(Token **tokens, size_t *toklen, const Token *buf, const size_t buflen);

Token *lex(const char *src, size_t *len);

Node *parse(const char *path, size_t *len);

void comp(const char *path);

void lex_analysis(const char *src, const char *out);

void parse_analysis(const char *src, const char *out);

#endif //COMP_H