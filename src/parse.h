#ifndef PARSE_H
#define PARSE_H

#include <stddef.h>
#include <stdbool.h>

#include "token.h"
#include "node.h"

bool has_child(const Node *nodes, const size_t len, const int parent);

int get_generation(const Node *nodes, const size_t len, const int index);

void push_node(Node **nodes, size_t *nodelen, const Node node);

void consume_node(Node *node, const Token tok);

void push_parent(int **parents, size_t *parentlen, const int parent);

void pop_parent(int **parents, size_t *parentlen);

int get_node_type(const int type);

bool is_parent_node(const int type);

bool is_parent_node_end(const int node_type, const int tok_type);

Node *parse_expr(const Token *tokens, const size_t toklen, size_t *len);

#endif //PARSE_H