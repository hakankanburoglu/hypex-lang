#ifndef COMP_H
#define COMP_H

#include <stddef.h>

#include "token.h"

void print_token(const Token tok);

void print_lex(const Token *tokens, const size_t len);

void comp(const char *path);

#endif //COMP_H