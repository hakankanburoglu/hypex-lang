#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"
#include "node.h"
#include "token.h"

Node *make_node(int kind, Node *parent) {
    Node *node = (Node *)malloc(sizeof(Node));
    if (!node) error_hypex();
    node->kind = kind;
    node->parent = parent;
    return node; 
}

Type *make_type(int kind) {
    Type *type = (Type *)malloc(sizeof(Type));
    if (!type) error_hypex();
    type->kind = kind;
    type->types = NULL;
    type->cap = 1;
    type->len = 0;
    type->flags = FLAG_NONE;
    return type;
}

Type *copy_type(const Type *type) {
    Type *r = (Type *)malloc(sizeof(Type));
    if (!type) error_hypex();
    r->kind = type->kind;
    r->types = type->types;
    r->cap = type->cap;
    r->len = type->len;
    r->flags = type->flags;
    return r;
}

Node *copy_node(const Node *node) {
    Node *r = (Node *)malloc(sizeof(Node));
    if (!r) error_hypex();
    r->kind = node->kind;
    r->parent = node->parent;
    switch (node->kind) {
        case NODE_SOURCE: case NODE_BLOCK:
            r->data.block.body = (Node **)malloc(sizeof(Node) * node->data.block.cap);
            if (!r->data.block.body) error_hypex();
            for (int i = 0; i < node->data.block.len; i++)
                r->data.block.body[i] = node->data.block.body[i];
            r->data.block.cap = node->data.block.cap;
            r->data.block.len = node->data.block.len;
            break;
        case NODE_EXPR: case NODE_RET_STMT: case NODE_IF_STMT:
            r->data.expr = node->data.expr;
            break;
        case NODE_UNARY_OP:
            r->data.unary_op.op = node->data.unary_op.op;
            r->data.unary_op.operand = node->data.unary_op.operand;
            r->data.unary_op.prefix = node->data.unary_op.prefix;
            break;
        case NODE_BINARY_OP:
            r->data.binary_op.op = node->data.binary_op.op;
            r->data.binary_op.left = node->data.binary_op.left;
            r->data.binary_op.right = node->data.binary_op.right;
            break;
        case NODE_TERNARY_OP:
            r->data.ternary_op.cond = node->data.ternary_op.cond;
            r->data.ternary_op.if_body = node->data.ternary_op.if_body;
            r->data.ternary_op.else_body = node->data.ternary_op.else_body;
            break;
        case NODE_LITERAL: case NODE_IDENT:
            r->data.value = node->data.value;
            break;
        case NODE_FUNC_DECL:
            r->data.func_decl.ident = node->data.func_decl.ident;
            r->data.func_decl.type = copy_type(node->data.func_decl.type);
            r->data.func_decl.args = (Node **)malloc(sizeof(Node) * node->data.func_decl.args_cap);
            if (!r->data.func_decl.args) error_hypex();
            for (int i = 0; i < node->data.func_decl.args_len; i++)
                r->data.func_decl.args[i] = node->data.func_decl.args[i];
            r->data.func_decl.args_cap = node->data.func_decl.args_cap;
            r->data.func_decl.args_len = node->data.func_decl.args_len;
            r->data.func_decl.body = node->data.func_decl.body;
            break;
        case NODE_VAR_DECL:
            r->data.var_decl.ident = node->data.var_decl.ident;
            r->data.var_decl.type = copy_type(node->data.var_decl.type);
            r->data.var_decl.ident = node->data.var_decl.ident;
            break;
        case NODE_CALL_EXPR: case NODE_ARG_DECL:
            r->data.call_expr.callee = (char *)malloc(sizeof(char) * (node->data.call_expr.len + 1));
            if (!r->data.call_expr.callee) error_hypex();
            strcpy(r->data.call_expr.callee, node->data.call_expr.callee);
            r->data.call_expr.args = (Node **)malloc(sizeof(Node) * node->data.call_expr.args_cap);
            if (!r->data.call_expr.args) error_hypex();
            for (int i = 0; i < node->data.call_expr.args_len; i++)
                r->data.call_expr.args[i] = node->data.call_expr.args[i];
            r->data.call_expr.args_cap = node->data.call_expr.args_cap;
            r->data.call_expr.args_len = node->data.call_expr.args_len;
            break;
        default:
            break;
    }
    return r;
}

void free_node(Node *node) {
    free(node);
}
