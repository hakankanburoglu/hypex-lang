#include <stdlib.h>

#include "node.h"
#include "error.h"

Node *make_node(int kind, Node *parent) {
    Node *node = (Node *)malloc(sizeof(Node));
    if (!node) error_hypex();
    node->kind = kind;
    node->parent = parent;
    return node; 
}

void free_node(Node *node) {
    free(node);
}