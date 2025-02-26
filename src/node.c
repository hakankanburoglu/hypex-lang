#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#include "token.h"
#include "node.h"
#include "error.h"

Node *make_node(int kind, Node *parent) {
    Node *node = (Node *)malloc(sizeof(Node));
    if (!node) error_hypex();
    node->kind = kind;
    node->parent = parent;
    return node; 
}

Node *copy_node(Node *node) {
    Node *r = (Node *)malloc(sizeof(Node));
    if (!r) error_hypex();
    r->kind = node->kind;
    r->parent = node->parent;
    switch (node->kind) {
        case NODE_SOURCE:
            r->data.source.body = (Node **)malloc(sizeof(Node) * node->data.source.len);
            if (!r->data.source.body) error_hypex();
            for (size_t i = 0; i < node->data.source.len; i++)
                r->data.source.body[i] = node->data.source.body[i];
            r->data.source.len = node->data.source.len;
            break;
        case NODE_EXPR:
            r->data.expr.tokens = (Token **)malloc(sizeof(Token) * node->data.expr.toklen);
            if (!r->data.expr.tokens) error_hypex();
            for (size_t i = 0; i < node->data.expr.toklen; i++)
                r->data.expr.tokens[i] = node->data.expr.tokens[i];
            r->data.expr.toklen = node->data.expr.toklen;
            break;
        case NODE_ASSIGN_EXPR:
            r->data.assign.left = node->data.assign.left;
            r->data.assign.right = node->data.assign.right;
            break;
        case NODE_PRE_UNARY_OP:
        case NODE_POST_UNARY_OP:
            r->data.unary_op.op = node->data.unary_op.op;
            r->data.unary_op.operand = node->data.unary_op.operand;
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
            r->data.ident.name = (char *)malloc(sizeof(char) * (node->data.ident.len + 1));
            if (!r->data.ident.name) error_hypex();
            strcpy(r->data.ident.name, node->data.ident.name);
            r->data.ident.len = node->data.ident.len;
            break;
        case NODE_FUNC_DECL:
            r->data.func_decl.name = (char *)malloc(sizeof(char) * (node->data.func_decl.name_len + 1));
            if (!r->data.func_decl.name) error_hypex();
            strcpy(r->data.func_decl.name, node->data.func_decl.name);
            r->data.func_decl.name_len = node->data.func_decl.name_len;
            r->data.func_decl.attr = node->data.func_decl.attr;
            // copy args
            r->data.func_decl.args = (Node **)malloc(sizeof(Node) * node->data.func_decl.args_len);
            if (!r->data.func_decl.args) error_hypex();
            for (size_t i = 0; i < node->data.func_decl.args_len; i++)
                r->data.func_decl.args[i] = node->data.func_decl.args[i];
            r->data.func_decl.args_len = node->data.func_decl.args_len;
            // copy body
            r->data.func_decl.body = (Node **)malloc(sizeof(Node) * node->data.func_decl.len);
            if (!r->data.func_decl.body) error_hypex();
            for (size_t i = 0; i < node->data.func_decl.len; i++)
                r->data.func_decl.body[i] = node->data.func_decl.body[i];
            r->data.func_decl.len = node->data.func_decl.len;
            break;
        case NODE_VAR_DECL:
            r->data.var_decl.ident = (char *)malloc(sizeof(char) * (node->data.var_decl.len + 1));
            if (!r->data.var_decl.ident) error_hypex();
            strcpy(r->data.var_decl.ident, node->data.var_decl.ident);
            r->data.var_decl.len = node->data.var_decl.len;
            r->data.var_decl.attr = node->data.var_decl.attr;
            r->data.var_decl.init = node->data.var_decl.init;
            break;
        case NODE_CALL_EXPR:
            r->data.call_expr.callee = (char *)malloc(sizeof(char) * (node->data.call_expr.len + 1));
            if (!r->data.call_expr.callee) error_hypex();
            strcpy(r->data.call_expr.callee, node->data.call_expr.callee);
            r->data.call_expr.len = node->data.call_expr.len;
            // copy args
            r->data.call_expr.args = (Node **)malloc(sizeof(Node) * node->data.call_expr.args_len);
            if (!r->data.call_expr.args) error_hypex();
            for (size_t i = 0; i < node->data.call_expr.args_len; i++)
                r->data.call_expr.args[i] = node->data.call_expr.args[i];
            r->data.call_expr.args_len = node->data.call_expr.args_len;
            break;
        default:
            break;
    }
}

void free_node(Node *node) {
    free(node);
}