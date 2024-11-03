#ifndef OUT_H
#define OUT_H

#include <stdio.h>
#include <stddef.h>

#include "token.h"
#include "node.h"

void print_token_type(FILE *file, const int tok_type);

void print_token(FILE *file, const Token tok);

void print_tokens(FILE *file, const Token *tokens, const size_t len);

void print_token_data(FILE *file, const Token tok);

void print_node(FILE *file, const Node node, const int index);

void print_indent(FILE *file, const int n);

void print_nodes(FILE *file, const Node *nodes, const size_t len);

#endif //OUT_H