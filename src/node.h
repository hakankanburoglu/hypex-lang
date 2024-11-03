#ifndef NODE_H
#define NODE_H

#include <stddef.h>

#include "token.h"

enum {
    ROOT_NODE,
    EXPR_NODE,
    INDENT_BLOCK,
    PAREN_EXPR, // ()
    SQB_EXPR, // []
    BRACE_EXPR, // {}
    COMMA_EXPR // ,
};

typedef struct {
    int type;
    int parent;
    size_t len;
    Token *data;
} Node;

Node make_node(const int type, const int parent);

#endif //NODE_H