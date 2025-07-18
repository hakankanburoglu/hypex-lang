#ifndef DEBUG_H
#define DEBUG_H

#include "parse/lexer.h"
#include "parse/node.h"
#include "parse/parser.h"
#include "parse/token.h"

void print_token_kind(int kind);

void print_keyword(int id);

void print_base(int base);

void print_token(Token tok);

void print_lexer(Lexer lex);

void print_node_kind(int kind);

void print_op(int op);

void print_type_kind(int kind);

void print_type(Type *type);

void print_node(Node *node, int level, const char *tag);

void print_parser(Parser p);

#endif //DEBUG_H
