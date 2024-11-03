#ifndef AST_NODE_H
#define AST_NODE_H

#include <stddef.h>

#include "token.h"

enum {
    ROOT_AST_NODE,
    VAR_DEF,
    VAR_INIT,
    VAR_INFER,
    ADDITION,
    SUBTRACTION,
    MULTIPLACTION,
    DIVISION,
    ASSIGN,
    BOOL_EXPR
};

typedef struct {
    int type;
    int parent;
    size_t len;
    Token *data;
} ASTNode;

ASTNode make_ast_node(const int type, const int parent);

#endif //NODE_H