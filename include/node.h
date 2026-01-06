#ifndef NODE_H
#define NODE_H

#include <stdbool.h>
#include <stddef.h>

#include "token.h"

enum {
    NODE_SOURCE,
    NODE_EXPR,
    NODE_BLOCK,
    NODE_UNARY_OP,
    NODE_BINARY_OP,
    NODE_TERNARY_OP,
    NODE_LITERAL,
    NODE_IDENT,
    NODE_PAREN_GROUP,
    NODE_FUNC_DECL,
    NODE_ARG_DECL,
    NODE_RET_STMT,
    NODE_VAR_DECL,
    NODE_CALL_EXPR,
    NODE_IF_STMT
};

enum {
    // PRE_UNARY
    OP_NOT, // !
    OP_BIT_NOT, // ~ 
    OP_NEG, // -
    OP_AMPER, // &
    OP_PRE_INC, // ++x
    OP_PRE_DEC, // --x
    // POST_UNARY
    OP_POST_INC, // x++
    OP_POST_DEC, // x--
    // BINARY
    OP_ACCESS, // .
    OP_ASSIGN, // =
    OP_ADD, // +
    OP_SUB, // -
    OP_MUL, // *
    OP_DIV, // /
    OP_MOD, // %
    OP_A_ADD, // +=
    OP_A_SUB, // -=
    OP_A_MUL, // *=
    OP_A_DIV, // /=
    OP_A_MOD, // %=
    OP_EQ, // ==
    OP_NE, // !=
    OP_LT, // <
    OP_GT, // >
    OP_LE, // <=
    OP_GE, // >=
    OP_BIT_AND, // &
    OP_BIT_OR, // |
    OP_AND, // &&
    OP_OR, // ||
    OP_XOR, // ^
    OP_SHL, // <<
    OP_SHR, // >>
    OP_A_AND, // &=
    OP_A_OR, // |=
    OP_A_XOR, // ^=
    OP_A_SHL, // <<=
    OP_A_SHR, // >>=
    // TERNARY
    OP_COND // ? :
};

enum {
    TYPE_NOTYPE,
    TYPE_IDENT,
    TYPE_INT,
    TYPE_LONG,
    TYPE_SHORT,
    TYPE_FLOAT,
    TYPE_DOUBLE,
    TYPE_CHAR,
    TYPE_UINT,
    TYPE_ULONG,
    TYPE_USHORT,
    TYPE_BOOL,
    TYPE_STRING,
    TYPE_OBJECT,
    TYPE_FUNC,
    TYPE_ARRAY,
    TYPE_FIXED_ARRAY,
    TYPE_TUPLE,
    TYPE_STRUCT,
    TYPE_CLASS,
    TYPE_IFACE,
    TYPE_GENERIC
};

enum {
    FLAG_NONE = 0,
    FLAG_CONST = 1,
    FLAG_STAT = 2,
    FLAG_PRIV = 4,
    FLAG_PROT = 8,
    FLAG_REF = 16
};

typedef struct Type {
    int kind;
    struct Type **types;
    size_t cap;
    size_t len;
    int flags;
} Type;

typedef struct Node {
    int kind;
    struct Node *parent;
    union {
        struct {
            struct Node **body;
            size_t cap;
            size_t len;
        } block; // source, block
        struct Node *expr; // expr, logic_expr, ret_stmt
        struct {
            int op;
            struct Node *operand;
            bool prefix;
        } unary_op;
        struct {
            int op;
            struct Node *left;
            struct Node *right;
        } binary_op;
        struct {
            struct Node *cond;
            struct Node *if_body;
            struct Node *else_body;
        } ternary_op;
        Token *value; // literal, ident
        struct {
            Token *ident;
            Type *type;
            struct Node **args;
            size_t args_cap;
            size_t args_len;
            struct Node *body;
        } func_decl;
        struct {
            Token *ident;
            Type *type;
            struct Node *init;
        } var_decl;
        struct {
            char *callee;
            size_t len;
            struct Node **args;
            size_t args_cap;
            size_t args_len;
        } call_expr;
        struct {
            struct Node *if_body;
            struct Node *else_body;
        } if_stmt;
    } data;
} Node;

Node *make_node(int kind, Node *parent);

Type *make_type(int kind);

Type *copy_type(const Type *type);

Node *copy_node(const Node *node);

void free_node(Node *node);

#endif // NODE_H
