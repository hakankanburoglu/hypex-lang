#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

#include "token.h"
#include "node.h"
#include "parser.h"
#include "error.h"

Parser *init_parser(Lexer *lex) {
    Parser *p = (Parser *)malloc(sizeof(Parser));
    p->tok_list = lex->tokens.list;
    p->toklen = lex->tokens.len;
    p->expr = make_node(NODE_SOURCE, NULL);
    p->expr->data.block.body = NULL;
    p->expr->data.block.cap = 1;
    p->expr->data.block.len = 0;
    p->file = lex->file;
    p->offset = 0;
    return p;
}

static inline void init_block(Node *node) {
    node->data.block.body = NULL;
    node->data.block.cap = 1;
    node->data.block.len = 0;
}

static inline void init_unary_op(Node *node) {
    node->data.unary_op.op = -1;
    node->data.unary_op.operand = NULL;
    node->data.unary_op.prefix = false;
}

static inline void init_binary_op(Node *node) {
    node->data.binary_op.op = -1;
    node->data.binary_op.left = NULL;
    node->data.binary_op.right = NULL;
}

static inline void init_ternary_op(Node *node) {
    node->data.ternary_op.cond = NULL;
    node->data.ternary_op.if_body = NULL;
    node->data.ternary_op.else_body = NULL;
}

static inline void init_func_decl(Node *node) {
    node->data.func_decl.ident = NULL;
    node->data.func_decl.type = make_type(TYPE_NOTYPE);
    node->data.func_decl.args = NULL;
    node->data.func_decl.args_cap = 1;
    node->data.func_decl.args_len = 0;
    node->data.func_decl.body = NULL;
}

static inline void init_var_decl(Node *node) {
    node->data.var_decl.ident = NULL;
    node->data.var_decl.type = make_type(TYPE_NOTYPE);
    node->data.var_decl.init = NULL;
}

static inline void init_call_expr(Node *node) {
    node->data.call_expr.callee = NULL;
    node->data.call_expr.len = 0;
    node->data.call_expr.args = NULL;
    node->data.call_expr.args_cap = 1;
    node->data.call_expr.args_len = 0;
}

static inline void init_arg_decl(Node *node) {
    node->data.arg_decl.ident = NULL;
    node->data.arg_decl.type = make_type(TYPE_NOTYPE);
}

static void push_node(Node ***nodes, size_t *cap, size_t *len, Node *node) {
    if (!*nodes) {
        *nodes = (Node **)realloc(*nodes, sizeof(Node *));
        if (!*nodes) error_hypex();
    }
    if (*len == *cap) {
        *cap *= 2;
        *nodes = (Node **)realloc(*nodes, *cap * sizeof(Node *));
        if (!*nodes) error_hypex();
    }
    (*nodes)[(*len)++] = node;
}

static inline void consume_parse(Parser *p, Node *node) {
    push_node(&(p->expr->data.block.body), &(p->expr->data.block.cap), &(p->expr->data.block.len), node);
}

static inline void consume_func_decl_arg(Node *node, Node *arg) {
    push_node(&(node->data.func_decl.args), &(node->data.func_decl.args_cap), &(node->data.func_decl.args_len), arg);
}

static inline bool is_ignore_token(int kind) {
    switch (kind) {
        case T_COMMENT_LINE: case T_COMMENT_BLOCK: case T_SPACE: return true;
        default: return false;
    }
}

static inline bool match(const Parser *p) {
    return p->offset < p->toklen;
}

// overflow warning: list length is not checked in this function
static inline Token *current(const Parser *p) {
    return p->tok_list[p->offset];
}

// overflow warning: list length is not checked in this function
static inline void advance(Parser *p) {
    p->offset++;
    while (match(p) && is_ignore_token(current(p)->kind)) p->offset++;
}

static inline bool advance_match(Parser *p) {
    advance(p);
    return match(p);
}

static inline bool is_unary_op(int op) {
    return op >= 0 && op <= 7;
}

static inline bool is_pre_unary_op(int op) {
    return op >= 0 && op <= 5;
}

static inline bool is_post_unary_op(int op) {
    return op >= 6 && op <= 7;
}

static inline bool is_binary_op(int op) {
    return op >= 8;
}

static bool is_literal(const Token *tok) {
    switch (tok->kind) {
        case T_INTEGER: case T_FLOAT: case T_CHAR: case T_STRING: case T_RSTRING: return true;
        case T_KEYWORD:
            switch (tok->id) {
                case KW_FALSE: case KW_NULL: case KW_TRUE: return true;
                default: return false;
            }
        default: return false;
    }
}

static int match_type(Parser *p, const Token *tok) {
    if (tok->kind == T_IDENT) return TYPE_IDENT;
    if (tok->kind != T_KEYWORD) return -1;
    switch (tok->id) {
        case KW_INT: return TYPE_INT;
        case KW_LONG: return TYPE_LONG;
        case KW_SHORT: return TYPE_SHORT;
        case KW_FLOAT: return TYPE_FLOAT;
        case KW_DOUBLE: return TYPE_DOUBLE;
        case KW_CHAR: return TYPE_CHAR;
        case KW_STRING: return TYPE_STRING;
        case KW_UINT: return TYPE_UINT;
        case KW_ULONG: return TYPE_ULONG;
        case KW_USHORT: return TYPE_USHORT;
        case KW_BOOL: return TYPE_BOOL;
        default: return -1;
    }
}

static inline void parser_expect(const Parser *p, const char *expect) {
    error_expect(p->file, current(p)->pos.line, current(p)->pos.column, expect);
}

static void parse_type(Parser *p, Type *type);

static void parse_func_decl(Parser *p, Node *node);

static Node *parse_arg_decl(Parser *p, Node *parent);

static Node *parse_ret_stmt(Parser *p, Node *parent);

static void parse_var_decl(Parser *p, Node *node);

static Node *parse_expr(Parser *p);

static void parse_type(Parser *p, Type *type) {
    const int kind = match_type(p, current(p));
    if (kind != -1) type->kind = kind;
    if (current(p)->kind == T_LPAR) {
        if (!advance_match(p)) parser_expect(p, "types");
        while (match(p) && current(p)->kind != T_RPAR) {
            advance(p);
        }
    }
}

static void parse_func_decl(Parser *p, Node *node) {
    node->kind = NODE_FUNC_DECL;
    init_func_decl(node);
    if (!advance_match(p) || current(p)->kind != T_IDENT) parser_expect(p, "identifier");
    node->data.func_decl.ident = current(p);
    if (!advance_match(p) || current(p)->kind != T_LPAR) parser_expect(p, "'('");
    if (!advance_match(p)) parser_expect(p, "')' or argument");
    while (current(p)->kind != T_RPAR) {
        consume_func_decl_arg(node, parse_arg_decl(p, node));
        if (!match(p)) {
            parser_expect(p, "')'");
            break;
        }
        if (current(p)->kind == T_RPAR) break;
        if (current(p)->kind == T_COMMA) {
            advance(p);
        } else {
            parser_expect(p, "','");
            break;
        }
    }
    advance(p);
    switch (current(p)->kind) {
        case T_COLON: break;
        case T_SEMI: return;
        default: parser_expect(p, "function body or ';'"); return;
    }
}

static Node *parse_arg_decl(Parser *p, Node *parent) {
    Node *arg = make_node(NODE_ARG_DECL, parent);
    init_arg_decl(arg);
    if (current(p)->kind == T_KEYWORD && current(p)->id == KW_CONST) {
        if (!advance_match(p) || current(p)->kind != T_COLON) parser_expect(p, "':'");
        arg->data.arg_decl.type->is_const = true;
        advance(p);
    }
    if (!match(p)) parser_expect(p, "type");
    parse_type(p, arg->data.arg_decl.type);
    if (!advance_match(p) || current(p)->kind != T_IDENT) parser_expect(p, "identifier");
    arg->data.arg_decl.ident = current(p);
    advance(p);
    return arg;
}

static Node *parse_ret_stmt(Parser *p, Node *parent) {
    Node *stmt = make_node(NODE_RET_STMT, parent);
    if (!advance_match(p)) parser_expect(p, "statement");
    stmt->data.stmt.expr = parse_expr(p);
    return stmt;
}

static void parse_var_decl(Parser *p, Node *node) {
    node->kind = NODE_VAR_DECL;
    init_var_decl(node);
    if (!advance_match(p)) parser_expect(p, "type or identifier");
    if (current(p)->kind == T_COLON) {
        if (!advance_match(p)) parser_expect(p, "type");
        parse_type(p, node->data.var_decl.type);
        if (!match(p)) parser_expect(p, "identifier");
    }
    node->data.var_decl.ident = current(p);
    if (!advance_match(p)) return;
    if (current(p)->kind == T_EQUAL) {
        node->data.var_decl.init = parse_expr(p);
    }
}

static Node *parse_expr(Parser *p) {
    Node *expr = make_node(NODE_EXPR, p->expr);
    Token *tok = current(p);
    bool is_static = false;
    if (tok->kind == T_STAR) {
        is_static = true;
        if (!advance_match(p)) parser_expect(p, "expression");
    }
    if (is_literal(tok)) {
        expr->kind = NODE_LITERAL;
        expr->data.literal.value = tok;
    } else if (tok->kind == T_IDENT) {
        expr->kind = NODE_IDENT;
        expr->data.ident.value = tok;
    } else if (tok->kind == T_KEYWORD) {
        switch (tok->id) {
            case KW_FUNC:
                if (is_static) expr->data.func_decl.type->is_static = true;
                parse_func_decl(p, expr);
                break;
            case KW_VAR:
                if (is_static) expr->data.var_decl.type->is_static = true;
                parse_var_decl(p, expr);
                break;
            case KW_CONST:
                expr->data.var_decl.type->is_const = true;
                parse_var_decl(p, expr);
                break;
            default:
                error_hypex();
                break;
        }
    } else {
        error_hypex();
    }
    return expr;
}

void parse(Parser *p) {
    while (match(p)) {
        consume_parse(p, parse_expr(p));
        advance(p);
    }
}

void free_parser(Parser *p) {
    free(p->tok_list);
    free(p->expr);
    free(p);
}
