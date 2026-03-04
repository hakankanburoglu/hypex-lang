#ifndef DEBUG_H
#define DEBUG_H

#include "ast.h"
#include "lexer.h"
#include "parser.h"
#include "token.h"

void print_token_kind(hypex_token_kind kind);

void print_keyword(hypex_keyword keyword);

void print_base(hypex_num_base base);

void print_token(const hypex_token *tok);

void print_lexer(const hypex_lexer *lex);

void print_node_kind(hypex_node_kind kind);

void print_op(hypex_op op);

void print_type_kind(hypex_type_kind kind);

void print_type(hypex_type *type);

void print_node(hypex_node *node, int level, const char *tag);

void print_parser(hypex_parser p);

#endif // DEBUG_H
