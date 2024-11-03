#include <stddef.h>

#include "ast_node.h"

ASTNode make_ast_node(const int type, const int parent) {
    ASTNode ast_node;
    ast_node.type = type;
    ast_node.parent = parent;
    ast_node.len = 0;
    ast_node.data = NULL;
    return ast_node;
}