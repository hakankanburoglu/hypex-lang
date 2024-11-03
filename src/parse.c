#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

#include "token.h"
#include "node.h"
#include "parse.h"
#include "error.h"

bool has_child(const Node *nodes, const size_t len, const int parent) {
    for (int i = 0; i < len; i++)
        if (nodes[i].parent == parent)
            return true;
    return false;
}

int get_generation(const Node *nodes, const size_t len, const int index) {
    int gen = 0;
    int parent = nodes[index].parent;
    while (parent != -1) {
        parent = nodes[parent].parent;
        gen++;
    }
    return gen;
}

void push_node(Node **nodes, size_t *nodelen, const Node node) {
    *nodes = (Node *)realloc(*nodes, (*nodelen + 1) * sizeof(Node));
    if (*nodes == NULL)
        error_parse("1");
    (*nodes)[(*nodelen)++] = node;
}

void consume_node(Node *node, const Token tok) {
    node->data = (Token *)realloc(node->data, (node->len + 1) * sizeof(Token));
    if (node->data == NULL)
        error_parse("2");
    node->data[(node->len)++] = tok;
}

void push_parent(int **parents, size_t *parentlen, const int parent) {
    *parents = (int *)realloc(*parents, (*parentlen + 1) * sizeof(int));
    if (*parents == NULL)
        error_parse("3");
    (*parents)[(*parentlen)++] = parent;
}

void pop_parent(int **parents, size_t *parentlen) {
    *parents = (int *)realloc(*parents, (*parentlen - 1) * sizeof(int));
    if (*parents == NULL)
        error_parse("4");
    (*parentlen)--;
}

int get_node_type(const int type) {
    switch (type) {
        case _INDENT: return INDENT_BLOCK;
        case LPAR: return PAREN_EXPR;
        case LSQB: return SQB_EXPR;
        case LBRACE: return BRACE_EXPR;
        default: return EXPR_NODE;
    }
}

bool is_parent_node(const int type) {
    switch (type) {
        case INDENT_BLOCK: return true;
        case PAREN_EXPR: return true;
        case SQB_EXPR: return true;
        case BRACE_EXPR: return true;
        default: return false;
    }
}

bool is_parent_node_end(const int node_type, const int tok_type) {
    switch (node_type) {
        case INDENT_BLOCK:
            return tok_type == _DEDENT;
        case PAREN_EXPR:
            return tok_type == RPAR;
        case SQB_EXPR:
            return tok_type == RSQB;
        case BRACE_EXPR:
            return tok_type == RBRACE;
        default:
            return false;
    }
}

Node *parse_expr(const Token *tokens, const size_t toklen, size_t *len) {
    Node *nodes = (Node *)malloc(2 * sizeof(Node));
    nodes[0] = make_node(ROOT_NODE, -1);
    nodes[1] = make_node(EXPR_NODE, 0);
    *len = 2;
    int *parents = (int *)malloc(sizeof(int));
    parents[0] = 0;
    size_t parentlen = 1;
    for (int i = 0; i < toklen; i++) {
        if (nodes[*len - 1].type == EXPR_NODE && get_node_type(tokens[i].type) != EXPR_NODE) {
            push_node(&nodes, len, make_node(get_node_type(tokens[i].type), parents[parentlen - 1]));
        }
        if (is_parent_node(nodes[*len - 1].type)) {
            push_parent(&parents, &parentlen, (*len) - 1);
            push_node(&nodes, len, make_node(EXPR_NODE, parents[parentlen - 1]));
        } else if (is_parent_node_end(nodes[nodes[*len - 1].parent].type, tokens[i].type)) {
            pop_parent(&parents, &parentlen);
            push_node(&nodes, len, make_node(EXPR_NODE, parents[parentlen - 1]));
        } else {
            consume_node(&nodes[*len - 1], tokens[i]);
        }
    }
    free(parents);
    return nodes;
}