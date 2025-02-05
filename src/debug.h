#ifndef DEBUG_H
#define DEBUG_H

#include "token.h"
#include "lexer.h"

void print_token_type(int type);

void print_keyword(int id);

void print_base(int base);

void print_token(Token tok);

void print_lexer(Lexer lex);

#endif //DEBUG_H