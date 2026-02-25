#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "ast.h"
#include "error.h"
#include "token.h"

hypex_node *make_node(hypex_node_kind kind, hypex_node *parent) {
    hypex_node *node = malloc(sizeof *node);
    if (!node) internal_error();
    node->kind = kind;
    node->parent = parent;
    return node; 
}

hypex_type *hypex_type_make(hypex_type_kind kind) {
    hypex_type *type = malloc(sizeof *type);
    if (!type) internal_error();
    type->kind = kind;
    type->types = NULL;
    type->cap = 1;
    type->len = 0;
    type->flags = FLAG_NONE;
    return type;
}

hypex_type *hypex_type_copy(const hypex_type *type) {
    hypex_type *r = malloc(sizeof *r);
    if (!type) internal_error();
    r->kind = type->kind;
    r->types = type->types;
    r->cap = type->cap;
    r->len = type->len;
    r->flags = type->flags;
    return r;
}

hypex_node *hypex_node_copy(const hypex_node *node) {
    hypex_node *r = malloc(sizeof *r);
    if (!r) internal_error();
    r->kind = node->kind;
    r->parent = node->parent;
    switch (node->kind) {
        case HYPEX_NODE_SOURCE: case HYPEX_NODE_BLOCK:
            r->data.block.body = malloc(sizeof *r->data.block.body * node->data.block.cap);
            if (!r->data.block.body) internal_error();
            for (int i = 0; i < node->data.block.len; i++)
                r->data.block.body[i] = node->data.block.body[i];
            r->data.block.cap = node->data.block.cap;
            r->data.block.len = node->data.block.len;
            break;
        case HYPEX_NODE_EXPR: case HYPEX_NODE_RET_STMT: case HYPEX_NODE_IF_STMT:
            r->data.expr = node->data.expr;
            break;
        case HYPEX_NODE_UNARY_OP:
            r->data.unary_op.op = node->data.unary_op.op;
            r->data.unary_op.operand = node->data.unary_op.operand;
            r->data.unary_op.prefix = node->data.unary_op.prefix;
            break;
        case HYPEX_NODE_BINARY_OP:
            r->data.binary_op.op = node->data.binary_op.op;
            r->data.binary_op.left = node->data.binary_op.left;
            r->data.binary_op.right = node->data.binary_op.right;
            break;
        case HYPEX_NODE_TERNARY_OP:
            r->data.ternary_op.cond = node->data.ternary_op.cond;
            r->data.ternary_op.if_body = node->data.ternary_op.if_body;
            r->data.ternary_op.else_body = node->data.ternary_op.else_body;
            break;
        case HYPEX_NODE_LITERAL: case HYPEX_NODE_IDENT:
            r->data.value = node->data.value;
            break;
        case HYPEX_NODE_FUNC_DECL:
            r->data.func_decl.ident = node->data.func_decl.ident;
            r->data.func_decl.type = copy_type(node->data.func_decl.type);
            r->data.func_decl.args = malloc(sizeof *r->data.func_decl.args * node->data.func_decl.args_cap);
            if (!r->data.func_decl.args) internal_error();
            for (int i = 0; i < node->data.func_decl.args_len; i++)
                r->data.func_decl.args[i] = node->data.func_decl.args[i];
            r->data.func_decl.args_cap = node->data.func_decl.args_cap;
            r->data.func_decl.args_len = node->data.func_decl.args_len;
            r->data.func_decl.body = node->data.func_decl.body;
            break;
        case HYPEX_NODE_VAR_DECL:
            r->data.var_decl.ident = node->data.var_decl.ident;
            r->data.var_decl.type = copy_type(node->data.var_decl.type);
            r->data.var_decl.ident = node->data.var_decl.ident;
            break;
        case HYPEX_NODE_CALL_EXPR: case HYPEX_NODE_ARG_DECL:
            r->data.call_expr.callee = malloc(sizeof *r->data.call_expr.callee * (node->data.call_expr.len + 1));
            if (!r->data.call_expr.callee) internal_error();
            strcpy(r->data.call_expr.callee, node->data.call_expr.callee);
            r->data.call_expr.args = malloc(sizeof *r->data.call_expr.args * node->data.call_expr.args_cap);
            if (!r->data.call_expr.args) internal_error();
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

void hypex_node_free(hypex_node *node) {
    free(node);
}
