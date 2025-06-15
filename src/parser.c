#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"
#include "node.h"
#include "parser.h"
#include "token.h"

const int OP[][2] = {
    // PRE UNARY
    {3, 1},  // OP_NOT
    {3, 1},  // OP_BIT_NOT
    {3, 1},  // OP_NEG
    {4, 1},  // OP_PRE_INC
    {4, 1},  // OP_PRE_DEC
    {5, 1},  // OP_AMPER
    // POST UNARY
    {4, 0},  // OP_POST_INC
    {4, 0},  // OP_POST_DEC
    // BINARY
    {0, 0},  // OP_ACCESS
    {1, 1},  // OP_ASSIGN
    {3, 0},  // OP_ADD
    {3, 0},  // OP_SUB
    {2, 0},  // OP_MUL
    {2, 0},  // OP_DIV
    {2, 0},  // OP_MOD
    {3, 0},  // OP_A_ADD
    {3, 0},  // OP_A_SUB
    {2, 0},  // OP_A_MUL
    {2, 0},  // OP_A_DIV
    {2, 0},  // OP_A_MOD
    {6, 0},  // OP_EQ
    {6, 0},  // OP_NE
    {6, 0},  // OP_LT
    {6, 0},  // OP_GT
    {6, 0},  // OP_LE
    {6, 0},  // OP_GE
    {4, 0},  // OP_BIT_AND
    {4, 0},  // OP_BIT_OR
    {7, 0},  // OP_AND
    {7, 0},  // OP_OR
    {5, 0},  // OP_XOR
    {2, 0},  // OP_SHL
    {2, 0},  // OP_SHR
    {4, 0},  // OP_A_AND
    {4, 0},  // OP_A_OR
    {5, 0},  // OP_A_XOR
    {1, 1},  // OP_A_SHL
    {1, 1},  // OP_A_SHR
    // TERNARY
    {8, 1}   // OP_COND
};

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
    p->tok = p->tok_list[0];
    return p;
}

static inline void init_block(Node *node) {
    node->data.block.body = NULL;
    node->data.block.cap = 1;
    node->data.block.len = 0;
}

static inline void init_unary_op(Node *node, int op) {
    node->data.unary_op.op = op;
    node->data.unary_op.operand = NULL;
    node->data.unary_op.prefix = false;
}

static inline void init_binary_op(Node *node, int op) {
    node->kind = NODE_BINARY_OP;
    node->data.binary_op.op = op;
    node->data.binary_op.left = NULL;
    node->data.binary_op.right = NULL;
}

static inline void init_ternary_op(Node *node) {
    node->data.ternary_op.cond = NULL;
    node->data.ternary_op.if_body = NULL;
    node->data.ternary_op.else_body = NULL;
}

static inline void init_ident(Node *node, Token *tok) {
    node->kind = T_IDENT;
    node->data.value = tok;
}

static inline void init_func_decl(Node *node) {
    node->kind = NODE_FUNC_DECL;
    node->data.func_decl.ident = NULL;
    node->data.func_decl.type = make_type(TYPE_NOTYPE);
    node->data.func_decl.args = NULL;
    node->data.func_decl.args_cap = 1;
    node->data.func_decl.args_len = 0;
    node->data.func_decl.body = NULL;
}

static inline void init_var_decl(Node *node, bool arg_decl) {
    if (!arg_decl) node->kind = NODE_VAR_DECL;
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

static inline Node *peek_node(Node *node) {
    return node->data.block.body[node->data.block.len - 1];
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

static inline void push_block(Parser *p, Node *node) {
    push_node(&(p->expr->data.block.body), &(p->expr->data.block.cap), &(p->expr->data.block.len), node);
}

static inline void push_arg(Node *node, Node *arg) {
    push_node(&(node->data.func_decl.args), &(node->data.func_decl.args_cap), &(node->data.func_decl.args_len), arg);
}

static inline void push_type(Type *type, Type *elm) {
    if (type->len == type->cap) {
        type->cap *= 2;
        type->types = (Type **)realloc(type->types, sizeof(Type) * type->cap);
        if (!type->types) error_hypex();
    }
    type->types[type->len++] = elm;
}

static inline void set_flags(Type *type, int flags) {
    type->flags |= flags;
}

static inline void unset_flags(Type *type, int flags) {
    type->flags &= ~flags;
}

static inline void error_parser(const Parser *p, const char *format, ...) {
    va_list args;
    va_start(args, format);
    error(p->file, p->tok->pos.line, p->tok->pos.column, format, args);
    va_end(args);
}

static inline void error_expect(const Parser *p, const char *expect) {
    error_parser(p, "expected %s", expect);
}

static inline bool is_skip_token(const Token *tok) {
    switch (tok->kind) {
        case T_COMMENT_LINE: case T_COMMENT_BLOCK: case T_SPACE:
            return true;
        case T_NEWLINE:
            return tok->comment;
        default:
            return false;
    }
}

static inline bool has_next(const Parser *p) {
    return p->offset < p->toklen;
}

static inline void next(Parser *p) {
    p->tok = p->tok_list[++p->offset];
    while (has_next(p) && is_skip_token(p->tok))
        p->tok = p->tok_list[++p->offset];
}

static inline bool next_expect(Parser *p) {
    next(p);
    return has_next(p);
}

static inline bool has_flags(Type *type, int flags) {
    return (type->flags & flags) == flags;
}

static inline bool is_unary_op(int op) {
    return op >= 0 && op <= 7;
}

static inline bool is_pre_unary_op(int op) {
    return op >= 0 && op <= 5;
}

static inline bool is_post_unary_op(int op) {
    return op == 6 || op == 7;
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

static int match_pre_unary_op(int kind) {
    switch (kind) {
        case T_MINUS: return OP_NEG;
        case T_AMPER: return OP_AMPER;
        case T_EXCLAMATION: return OP_NOT;
        case T_TILDE: return OP_BIT_NOT;
        case T_INCREASE: return OP_PRE_INC;
        case T_DECREASE: return OP_PRE_DEC;
        default: return -1;
    }
}

static int match_op(int kind) {
    switch (kind) {
        case T_EQUAL: return OP_ASSIGN;
        case T_PLUS: return OP_ADD;
        case T_MINUS: return OP_SUB;
        case T_STAR: return OP_MUL;
        case T_SLASH: return OP_DIV;
        case T_LESS: return OP_LT;
        case T_GREATER: return OP_GT;
        case T_AMPER: return OP_BIT_AND;
        case T_PIPE: return OP_BIT_OR;
        case T_PERCENT: return OP_MOD;
        case T_DOT: return OP_ACCESS;
        case T_QUEST: return OP_COND;
        case T_CARET: return OP_XOR;
        case T_TWO_EQ: return OP_EQ;
        case T_PLUS_EQ: return OP_A_ADD;
        case T_MINUS_EQ: return OP_A_SUB;
        case T_STAR_EQ: return OP_A_MUL;
        case T_SLASH_EQ: return OP_A_DIV;
        case T_LESS_EQ: return OP_LE;
        case T_GREATER_EQ: return OP_GE;
        case T_AMPER_EQ: return OP_A_AND;
        case T_TWO_AMPER: return OP_AND;
        case T_PIPE_EQ: return OP_A_OR;
        case T_TWO_PIPE: return OP_OR;
        case T_EXCLAMATION_EQ: return OP_NE;
        case T_PERCENT_EQ: return OP_A_MOD;
        case T_CARET_EQ: return OP_A_XOR;
        case T_LSHIFT: return OP_SHL;
        case T_RSHIFT: return OP_SHR;
        case T_INCREASE: return OP_POST_INC;
        case T_DECREASE: return OP_POST_DEC;
        case T_LSHIFT_EQ: return OP_A_SHL;
        case T_RSHIFT_EQ: return OP_A_SHR;
        default: return -1;
    }
}

static void parse_type(Parser *p, Type *type);

static void parse_func_decl(Parser *p, Node *expr);

static Node *parse_arg_decl(Parser *p, Node *parent);

static void parse_var_decl(Parser *p, Node *expr);

static Node *parse_primary(Parser *p, Node *parent);

static Node *parse_expr(Parser *p, Node *parent, int min_prec);

static Node *parse_stmt(Parser *p);

static void parse_type(Parser *p, Type *type) {
    const int kind = match_type(p, p->tok);
    if (kind != -1) type->kind = kind;
    if (p->tok->kind == T_LPAR) {
        if (!next_expect(p)) error_expect(p, "types");
        while (has_next(p) && p->tok->kind != T_RPAR) {
            next(p);
        }
    }
}

static void parse_func_decl(Parser *p, Node *expr) {
    if (!next_expect(p)) error_expect(p, "identifier or ':'");
    if (p->tok->kind == T_COLON) {
        if (!next_expect(p)) error_expect(p, "type");
        parse_type(p, expr->data.func_decl.type);
        if (!next_expect(p)) error_expect(p, "identifier");
    }
    if (p->tok->kind != T_IDENT) error_expect(p, "identifier");
    expr->data.func_decl.ident = p->tok;
    if (!next_expect(p) || p->tok->kind != T_LPAR) error_expect(p, "'('");
    if (!next_expect(p)) error_expect(p, "')' or argument");
    while (p->tok->kind != T_RPAR) {
        push_arg(expr, parse_arg_decl(p, expr));
        if (!has_next(p)) {
            error_expect(p, "')'");
            break;
        }
        if (p->tok->kind == T_RPAR) break;
        if (p->tok->kind == T_COMMA) {
            next(p);
        } else {
            error_expect(p, "','");
            break;
        }
    }
    next(p);
    switch (p->tok->kind) {
        case T_COLON: break;
        case T_SEMI: return;
        default: error_expect(p, "function body or ';'");
    }
}

static Node *parse_arg_decl(Parser *p, Node *parent) {
    Node *arg = make_node(NODE_ARG_DECL, parent);
    init_var_decl(arg, 1);
    if (p->tok->kind == T_KEYWORD && p->tok->id == KW_CONST) {
        if (!next_expect(p) || p->tok->kind != T_COLON) error_expect(p, "':'");
        set_flags(arg->data.var_decl.type, FLAG_CONST);
        next(p);
    }
    if (!has_next(p)) error_expect(p, "type");
    parse_type(p, arg->data.var_decl.type);
    if (!next_expect(p) || p->tok->kind != T_IDENT) error_expect(p, "identifier");
    arg->data.var_decl.ident = p->tok;
    next(p);
    return arg;
}

static void parse_var_decl(Parser *p, Node *expr) {
    if (!next_expect(p)) error_expect(p, "type or identifier");
    if (p->tok->kind == T_COLON) {
        if (!next_expect(p)) error_expect(p, "type");
        parse_type(p, expr->data.var_decl.type);
        if (!has_next(p)) error_expect(p, "identifier");
    }
    expr->data.var_decl.ident = p->tok;
    if (!next_expect(p)) return;
    if (p->tok->kind == T_EQUAL) {
        if (!next_expect(p)) error_expect(p, "expression");
        expr->data.var_decl.init = parse_expr(p, expr, 0);
    }
}

static Node *parse_primary(Parser *p, Node *parent) {
    Token *tok = p->tok;
    Node *node = NULL;
    if (tok->kind == T_LPAR) {
        if (!next_expect(p)) error_expect(p, "expression");
        node = parse_expr(p, parent, 0);
        if (!next_expect(p) && p->tok->kind != T_LPAR) error_expect(p, "')'");
        return node;
    }
    int op = match_pre_unary_op(tok->kind);
    if (op != -1) {
        if (!next_expect(p)) error_expect(p, "operand");
        Node *unary = make_node(NODE_UNARY_OP, parent);
        unary->data.unary_op.op = op;
        unary->data.unary_op.prefix = true;
        unary->data.unary_op.operand = parse_expr(p, unary, OP[op][0]);
        unary->data.unary_op.operand->parent = unary;
        return unary;
    }
    if (tok->kind == T_IDENT) {
        node = make_node(NODE_IDENT, parent);
        node->data.value = tok;
        next(p);
        return node;
    }
    if (is_literal(tok)) {
        node = make_node(NODE_LITERAL, parent);
        node->data.value = tok;
        next(p);
        return node;
    }
    error_parser(p, "invalid expression");
    return NULL;
}

static Node *parse_expr(Parser *p, Node *parent, int min_prec) {
    Node *lhs = parse_primary(p, parent);
    while (true) {
        int op = match_op(p->tok->kind);
        if (op == -1 || !is_binary_op(op)) break;
        int prec = OP[op][0];
        if (prec < min_prec) break;
        bool assoc = OP[op][1];
        int next_min_prec = assoc ? prec : prec + 1;
        next(p);
        Node *rhs = parse_expr(p, parent, next_min_prec);
        Node *binop = make_node(NODE_BINARY_OP, parent);
        init_binary_op(binop, op);
        binop->data.binary_op.left = lhs;
        binop->data.binary_op.right = rhs;
        lhs->parent = binop;
        rhs->parent = binop;
        lhs = binop;
    }
    return lhs;
}

static Node *parse_stmt(Parser *p) {
    Node *stmt = make_node(NODE_EXPR, p->expr);
    const Token *tok = p->tok;
    bool is_stat = false;
    if (tok->kind == T_STAR) {
        is_stat = true;
        if (!next_expect(p)) error_expect(p, "expression");
    }
    if (tok->kind == T_KEYWORD) {
        switch (tok->id) {
            case KW_FUNC:
                init_func_decl(stmt);
                if (is_stat) set_flags(stmt->data.func_decl.type, FLAG_STAT);
                parse_func_decl(p, stmt);
                break;
            case KW_VAR:
                init_var_decl(stmt, 0);
                if (is_stat) set_flags(stmt->data.var_decl.type, FLAG_STAT);
                parse_var_decl(p, stmt);
                break;
            case KW_CONST:
                init_var_decl(stmt, 0);
                set_flags(stmt->data.var_decl.type, FLAG_CONST);
                if (is_stat) set_flags(stmt->data.var_decl.type, FLAG_STAT);
                parse_var_decl(p, stmt);
                break;
            case KW_RETURN:
                stmt->kind = NODE_RET_STMT;
                if (!next_expect(p)) error_expect(p, "expression");
                stmt->data.expr = parse_expr(p, stmt, 0);
                break;
            case KW_IF:
                stmt->kind = NODE_IF_STMT;
                if (!next_expect(p)) error_expect(p, "expression");
                stmt->data.expr = parse_expr(p, stmt, 0);
                break;
            default:
                error_parser(p, "invalid syntax");
                break;
        }
    }
    return stmt;
}

void parse(Parser *p) {
    while (has_next(p)) {
        push_block(p, parse_stmt(p));
        next(p);
        while (has_next(p) && p->tok->kind == T_NEWLINE) next(p);
    }
}

void free_parser(Parser *p) {
    free(p->tok_list);
    free(p->expr);
    free(p);
}
