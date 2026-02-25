#ifndef AST_H
#define AST_H

#include <stdbool.h>
#include <stddef.h>

#include "token.h"

typedef enum {
    HYPEX_NODE_SOURCE,
    HYPEX_NODE_EXPR,
    HYPEX_NODE_BLOCK,
    HYPEX_NODE_UNARY_OP,
    HYPEX_NODE_BINARY_OP,
    HYPEX_NODE_TERNARY_OP,
    HYPEX_NODE_LITERAL,
    HYPEX_NODE_IDENT,
    HYPEX_NODE_PAREN_GROUP,
    HYPEX_NODE_FUNC_DECL,
    HYPEX_NODE_ARG_DECL,
    HYPEX_NODE_RET_STMT,
    HYPEX_NODE_VAR_DECL,
    HYPEX_NODE_CALL_EXPR,
    HYPEX_NODE_IF_STMT
} hypex_node_kind;

typedef enum {
    // PRE_UNARY
    HYPEX_OP_NOT, // !
    HYPEX_OP_BIT_NOT, // ~ 
    HYPEX_OP_NEG, // -
    HYPEX_OP_AMPER, // &
    HYPEX_OP_PRE_INC, // ++x
    HYPEX_OP_PRE_DEC, // --x
    // POST_UNARY
    HYPEX_OP_POST_INC, // x++
    HYPEX_OP_POST_DEC, // x--
    // BINARY
    HYPEX_OP_ACCESS, // .
    HYPEX_OP_ASSIGN, // =
    HYPEX_OP_ADD, // +
    HYPEX_OP_SUB, // -
    HYPEX_OP_MUL, // *
    HYPEX_OP_DIV, // /
    HYPEX_OP_MOD, // %
    HYPEX_OP_A_ADD, // +=
    HYPEX_OP_A_SUB, // -=
    HYPEX_OP_A_MUL, // *=
    HYPEX_OP_A_DIV, // /=
    HYPEX_OP_A_MOD, // %=
    HYPEX_OP_EQ, // ==
    HYPEX_OP_NE, // !=
    HYPEX_OP_LT, // <
    HYPEX_OP_GT, // >
    HYPEX_OP_LE, // <=
    HYPEX_OP_GE, // >=
    HYPEX_OP_BIT_AND, // &
    HYPEX_OP_BIT_OR, // |
    HYPEX_OP_AND, // &&
    HYPEX_OP_OR, // ||
    HYPEX_OP_XOR, // ^
    HYPEX_OP_SHL, // <<
    HYPEX_OP_SHR, // >>
    HYPEX_OP_A_AND, // &=
    HYPEX_OP_A_OR, // |=
    HYPEX_OP_A_XOR, // ^=
    HYPEX_OP_A_SHL, // <<=
    HYPEX_OP_A_SHR, // >>=
    // TERNARY
    HYPEX_OP_COND // ? :
} hypex_op;

typedef enum {
    HYPEX_TYPE_NOTYPE,
    HYPEX_TYPE_IDENT,
    HYPEX_TYPE_INT,
    HYPEX_TYPE_LONG,
    HYPEX_TYPE_SHORT,
    HYPEX_TYPE_FLOAT,
    HYPEX_TYPE_DOUBLE,
    HYPEX_TYPE_CHAR,
    HYPEX_TYPE_UINT,
    HYPEX_TYPE_ULONG,
    HYPEX_TYPE_USHORT,
    HYPEX_TYPE_BOOL,
    HYPEX_TYPE_STRING,
    HYPEX_TYPE_OBJECT,
    HYPEX_TYPE_FUNC,
    HYPEX_TYPE_ARRAY,
    HYPEX_TYPE_FIXED_ARRAY,
    HYPEX_TYPE_TUPLE,
    HYPEX_TYPE_STRUCT,
    HYPEX_TYPE_CLASS,
    HYPEX_TYPE_IFACE,
    HYPEX_TYPE_GENERIC
} hypex_type_kind;

typedef enum {
    FLAG_NONE = 0,
    FLAG_CONST = 1,
    FLAG_STAT = 2,
    FLAG_PRIV = 4,
    FLAG_PROT = 8,
    FLAG_REF = 16
} hypex_flag;

typedef struct hypex_type {
    hypex_type_kind kind;
    struct hypex_type **types;
    size_t cap;
    size_t len;
    int flags;
} hypex_type;

typedef struct hypex_node {
    hypex_node_kind kind;
    struct hypex_node *parent;
    union {
        struct {
            struct hypex_node **body;
            size_t cap;
            size_t len;
        } block; // source, block
        struct hypex_node *expr; // expr, logic_expr, ret_stmt
        struct {
            hypex_op op;
            struct hypex_node *operand;
            bool prefix;
        } unary_op;
        struct {
            hypex_op op;
            struct hypex_node *left;
            struct hypex_node *right;
        } binary_op;
        struct {
            struct hypex_node *cond;
            struct hypex_node *if_body;
            struct hypex_node *else_body;
        } ternary_op;
        hypex_token *value; // literal, ident
        struct {
            hypex_token *ident;
            hypex_type *type;
            struct hypex_node **args;
            size_t args_cap;
            size_t args_len;
            struct hypex_node *body;
        } func_decl;
        struct {
            hypex_token *ident;
            hypex_type *type;
            struct hypex_node *init;
        } var_decl;
        struct {
            char *callee;
            size_t len;
            struct hypex_node **args;
            size_t args_cap;
            size_t args_len;
        } call_expr;
        struct {
            struct hypex_node *if_body;
            struct hypex_node *else_body;
        } if_stmt;
    } data;
} hypex_node;

hypex_node *hypex_node_make(hypex_node_kind kind, hypex_node *parent);

hypex_type *hypex_type_make(hypex_type_kind kind);

hypex_type *hypex_type_copy(const hypex_type *type);

hypex_node *hypex_node_copy(const hypex_node *node);

void hypex_node_free(hypex_node *node);

#endif // AST_H
