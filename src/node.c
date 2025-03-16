#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

#include "token.h"
#include "node.h"
#include "error.h"

Node *make_node(int kind, Node *parent) {
    Node *node = (Node *)malloc(sizeof(Node));
    if (!node) error_hypex();
    node->kind = kind;
    node->parent = parent;
    node->tok_list = NULL;
    node->toklen = 0;
    return node; 
}

void consume_node(Node *node, Token *tok) {
    node->tok_list = (Token **)realloc(node->tok_list, sizeof(Token) * (node->toklen + 1));
    if (!node->tok_list) error_hypex();
    node->tok_list[node->toklen++] = tok;
}

Type *make_type(int kind) {
    Type *type = (Type *)malloc(sizeof(Type));
    type->kind = kind;
    type->types = NULL;
    type->capacity = 1;
    type->len = 0;
    type->is_const = false;
    type->is_static = false;
    type->is_private = false;
    type->is_protected = false;
    type->is_by_ref = false;
    return type;
}

void push_type(Type *type, Type *elm) {
    if (type->len == type->capacity) {
        type->capacity *= 2;
        type->types = (Type **)realloc(type->types, sizeof(Type) * type->capacity);
        if (!type->types) error_hypex();
    }
    type->types[type->len++] = elm;
}

Type *copy_type(Type *type) {
    Type *r = make_type(type->kind);
    return r;
}

Node *copy_node(Node *node) {
    Node *r = (Node *)malloc(sizeof(Node));
    if (!r) error_hypex();
    r->kind = node->kind;
    r->parent = node->parent;
    r->tok_list = (Token **)malloc(sizeof(Token) * node->toklen);
    if (!r->tok_list) error_hypex();
    for (int i = 0; i < node->toklen; i++)
        r->tok_list[i] = node->tok_list[i];
    r->toklen = node->toklen;
    switch (node->kind) {
        case NODE_SOURCE: case NODE_BLOCK:
            r->data.block.body = (Node **)malloc(sizeof(Node) * node->data.block.capacity);
            if (!r->data.block.body) error_hypex();
            for (int i = 0; i < node->data.block.len; i++)
                r->data.block.body[i] = node->data.block.body[i];
            r->data.block.capacity = node->data.block.capacity;
            r->data.block.len = node->data.block.len;
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
        case NODE_LITERAL:
            r->data.literal.value = node->data.literal.value;
            break;
        case NODE_IDENT:
            r->data.ident.value = node->data.ident.value;
            break;
        case NODE_FUNC_DECL:
            r->data.func_decl.ident = node->data.func_decl.ident;
            r->data.func_decl.type = copy_type(node->data.func_decl.type);
            r->data.func_decl.args = (Node **)malloc(sizeof(Node) * node->data.func_decl.args_capacity);
            if (!r->data.func_decl.args) error_hypex();
            for (int i = 0; i < node->data.func_decl.args_len; i++)
                r->data.func_decl.args[i] = node->data.func_decl.args[i];
            r->data.func_decl.args_capacity = node->data.func_decl.args_capacity;
            r->data.func_decl.args_len = node->data.func_decl.args_len;
            r->data.func_decl.body = node->data.func_decl.body;
            break;
        case NODE_VAR_DECL:
            r->data.var_decl.ident = node->data.var_decl.ident;
            r->data.var_decl.type = copy_type(node->data.var_decl.type);
            r->data.var_decl.ident = node->data.var_decl.ident;
            break;
        case NODE_CALL_EXPR:
            r->data.call_expr.callee = (char *)malloc(sizeof(char) * (node->data.call_expr.len + 1));
            if (!r->data.call_expr.callee) error_hypex();
            strcpy(r->data.call_expr.callee, node->data.call_expr.callee);
            r->data.call_expr.args = (Node **)malloc(sizeof(Node) * node->data.call_expr.args_capacity);
            if (!r->data.call_expr.args) error_hypex();
            for (int i = 0; i < node->data.call_expr.args_len; i++)
                r->data.call_expr.args[i] = node->data.call_expr.args[i];
            r->data.call_expr.args_capacity = node->data.call_expr.args_capacity;
            r->data.call_expr.args_len = node->data.call_expr.args_len;
            break;
        default:
            break;
    }
}

void free_node(Node *node) {
    free(node);
}
