#ifndef NODE_H
#define NODE_H

#include <stddef.h>

#include "token.h"

enum {
    NODE_SOURCE,
    NODE_EXPR,
    NODE_ASSIGN_EXPR,
    NODE_PRE_UNARY_OP, // op + operand
    NODE_POST_UNARY_OP, // operand + op
    NODE_BINARY_OP,
    NODE_TERNARY_OP,
    NODE_LITERAL,
    NODE_IDENT,
    NODE_LOGIC_EXPR,
    NODE_PAREN_GROUP,
    NODE_FUNC_DECL,
    NODE_VAR_DECL,
    NODE_CALL_EXPR
};

typedef struct {
    bool is_const;
    bool is_static;
    bool is_private;
    bool is_protected;
    bool is_by_ref;
} DeclAttr;

typedef struct Node {
    int kind;
    struct Node *parent;
    union {
        struct {
            struct Node **body;
            size_t len;
        } source;
        struct {
            struct Token **tokens;
            size_t toklen;
        } expr;
        struct {
            struct Node *left;
            struct Node *right;
        } assign;
        struct {
            Token *op;
            struct Node *operand;
        } unary_op;
        struct {
            Token *op;
            struct Node *left;
            struct Node *right;
        } binary_op;
        struct {
            struct Node *cond;
            struct Node *if_body;
            struct Node *else_body;
        } ternary_op;
        struct {
            Token *value;
        } literal;
        struct {
            char *name;
            size_t len;
        } ident;
        struct {
            char *name;
            size_t name_len;
            DeclAttr attr;
            struct Node **args;
            size_t args_len;
            struct Node **body;
            size_t len;
        } func_decl;
        struct {
            char *ident;
            size_t len;
            DeclAttr attr;
            struct Node *init;
        } var_decl;
        struct {
            char *callee;
            size_t len;
            struct Node **args;
            size_t args_len;
        } call_expr;
    } data;
} Node;

Node *make_node(int kind, Node *parent);

void free_node(Node *node);

#endif //NODE_H