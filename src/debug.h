#ifndef DEBUG_H
#define DEBUG_H

#include "token.h"
#include "lexer.h"
#include "node.h"

void print_token_kind(int kind);

void print_keyword(int id);

void print_base(int base);

void print_token(Token tok);

void print_lexer(Lexer lex);

#endif //DEBUG_H