#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "ast.h"
#include "error.h"
#include "parser.h"
#include "token.h"

const int OP[][2] = {

    // Format: {Precedence, Associativity}

    // Precedence:
    //     Higher value means higher binding strength.

    // Associativity: 
    //     0 = left-associative
    //     1 = right-associative

    // PRE UNARY
    {80, 1},  // HYPEX_OP_NOT
    {80, 1},  // HYPEX_OP_BIT_NOT
    {80, 1},  // HYPEX_OP_NEG
    {85, 1},  // HYPEX_OP_PRE_INC
    {85, 1},  // HYPEX_OP_PRE_DEC
    {80, 1},  // HYPEX_OP_AMPER
    
    // POST UNARY
    {90, 0},  // HYPEX_OP_POST_INC
    {90, 0},  // HYPEX_OP_POST_DEC
    
    // BINARY
    {100, 0}, // HYPEX_OP_ACCESS
    {10, 1},  // HYPEX_OP_ASSIGN
    {50, 0},  // HYPEX_OP_ADD
    {50, 0},  // HYPEX_OP_SUB
    {60, 0},  // HYPEX_OP_MUL
    {60, 0},  // HYPEX_OP_DIV
    {60, 0},  // HYPEX_OP_MOD
    {10, 1},  // HYPEX_OP_A_ADD
    {10, 1},  // HYPEX_OP_A_SUB
    {10, 1},  // HYPEX_OP_A_MUL
    {10, 1},  // HYPEX_OP_A_DIV
    {10, 1},  // HYPEX_OP_A_MOD
    {40, 0},  // HYPEX_OP_EQ
    {40, 0},  // HYPEX_OP_NE
    {40, 0},  // HYPEX_OP_LT
    {40, 0},  // HYPEX_OP_GT
    {40, 0},  // HYPEX_OP_LE
    {40, 0},  // HYPEX_OP_GE
    {35, 0},  // HYPEX_OP_BIT_AND
    {30, 0},  // HYPEX_OP_BIT_OR
    {32, 0},  // HYPEX_OP_XOR
    {25, 0},  // HYPEX_OP_AND
    {20, 0},  // HYPEX_OP_OR
    {55, 0},  // HYPEX_OP_SHL
    {55, 0},  // HYPEX_OP_SHR
    {10, 1},  // HYPEX_OP_A_AND
    {10, 1},  // HYPEX_OP_A_OR
    {10, 1},  // HYPEX_OP_A_XOR
    {10, 1},  // HYPEX_OP_A_SHL
    {10, 1},  // HYPEX_OP_A_SHR
    
    // TERNARY
    {15, 1}   // HYPEX_OP_COND
};

hypex_parser *make_parser(hypex_lexer *lex) {
    hypex_parser *p = malloc(sizeof(*p));
    p->tok_list = lex->tokens.list;
    p->toklen = lex->tokens.len;
    p->expr = make_node(HYPEX_NODE_SOURCE, NULL);
    p->expr->data.block.body = NULL;
    p->expr->data.block.cap = 1;
    p->expr->data.block.len = 0;
    p->file = lex->file;
    p->offset = 0;
    p->tok = p->tok_list[0];
    return p;
}

static inline void init_block(hypex_node *node) {
    node->data.block.body = NULL;
    node->data.block.cap = 1;
    node->data.block.len = 0;
}

static inline void init_unary_op(hypex_node *node, hypex_op op) {
    node->data.unary_op.op = op;
    node->data.unary_op.operand = NULL;
    node->data.unary_op.prefix = false;
}

static inline void init_binary_op(hypex_node *node, hypex_op op) {
    node->kind = HYPEX_NODE_BINARY_OP;
    node->data.binary_op.op = op;
    node->data.binary_op.left = NULL;
    node->data.binary_op.right = NULL;
}

static inline void init_ternary_op(hypex_node *node) {
    node->data.ternary_op.cond = NULL;
    node->data.ternary_op.if_body = NULL;
    node->data.ternary_op.else_body = NULL;
}

static inline void init_ident(hypex_node *node, hypex_token *tok) {
    node->kind = HYPEX_TOK_IDENT;
    node->data.value = tok;
}

static inline void init_func_decl(hypex_node *node) {
    node->kind = HYPEX_NODE_FUNC_DECL;
    node->data.func_decl.ident = NULL;
    node->data.func_decl.type = make_type(HYPEX_TYPE_NOTYPE);
    node->data.func_decl.args = NULL;
    node->data.func_decl.args_cap = 1;
    node->data.func_decl.args_len = 0;
    node->data.func_decl.body = NULL;
}

static inline void init_var_decl(hypex_node *node, bool arg_decl) {
    if (!arg_decl) node->kind = HYPEX_NODE_VAR_DECL;
    node->data.var_decl.ident = NULL;
    node->data.var_decl.type = make_type(HYPEX_TYPE_NOTYPE);
    node->data.var_decl.init = NULL;
}

static inline void init_call_expr(hypex_node *node) {
    node->data.call_expr.callee = NULL;
    node->data.call_expr.len = 0;
    node->data.call_expr.args = NULL;
    node->data.call_expr.args_cap = 1;
    node->data.call_expr.args_len = 0;
}

static inline hypex_node *peek_node(hypex_node *node) {
    return node->data.block.body[node->data.block.len - 1];
}

static void push_node(hypex_node ***nodes, size_t *cap, size_t *len, hypex_node *node) {
    if (!*nodes) {
        *nodes = realloc(*nodes, sizeof(hypex_node *));
        if (!*nodes) internal_error();
    }
    if (*len == *cap) {
        *cap *= 2;
        *nodes = realloc(*nodes, *cap * sizeof(hypex_node *));
        if (!*nodes) internal_error();
    }
    (*nodes)[(*len)++] = node;
}

static inline void push_block(hypex_parser *p, hypex_node *node) {
    push_node(&(p->expr->data.block.body), &(p->expr->data.block.cap), &(p->expr->data.block.len), node);
}

static inline void push_arg(hypex_node *node, hypex_node *arg) {
    push_node(&(node->data.func_decl.args), &(node->data.func_decl.args_cap), &(node->data.func_decl.args_len), arg);
}

static inline void push_type(hypex_type *type, hypex_type *elm) {
    if (type->len == type->cap) {
        type->cap *= 2;
        type->types = realloc(type->types, sizeof(hypex_type) * type->cap);
        if (!type->types) internal_error();
    }
    type->types[type->len++] = elm;
}

static inline void set_flags(hypex_type *type, int flags) {
    type->flags |= flags;
}

static inline void unset_flags(hypex_type *type, int flags) {
    type->flags &= ~flags;
}

static inline void error_parser(const hypex_parser *p, const char *format, ...) {
    va_list args;
    va_start(args, format);
    error(p->file, p->tok->pos.line, p->tok->pos.column, format, args);
    va_end(args);
}

static inline void error_expect(const hypex_parser *p, const char *expect) {
    error_parser(p, "expected %s", expect);
}

static inline bool is_skip_hypex_token(const hypex_token *tok) {
    switch (tok->kind) {
        case HYPEX_TOK_COMMENT_LINE: case HYPEX_TOK_COMMENT_BLOCK: case HYPEX_TOK_SPACE:
            return true;
        case HYPEX_TOK_INDENT:
            return tok->level == 0;
        case HYPEX_TOK_NEWLINE:
            return tok->comment;
        default:
            return false;
    }
}

static inline bool has_next(const hypex_parser *p) {
    return p->offset < p->toklen;
}

static inline void next(hypex_parser *p) {
    p->tok = p->tok_list[++p->offset];
    while (has_next(p) && is_skip_hypex_token(p->tok))
        p->tok = p->tok_list[++p->offset];
}

static inline bool next_expect(hypex_parser *p) {
    next(p);
    return has_next(p);
}

static inline bool has_flags(hypex_type *type, int flags) {
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

static bool is_literal(const hypex_token *tok) {
    switch (tok->kind) {
        case HYPEX_TOK_INTEGER: case HYPEX_TOK_FLOAT: case HYPEX_TOK_CHAR: case HYPEX_TOK_STRING: case HYPEX_TOK_RSTRING: return true;
        case HYPEX_TOK_KEYWORD:
            switch (tok->id) {
                case HYPEX_KW_FALSE: case HYPEX_KW_NULL: case HYPEX_KW_TRUE: return true;
                default: return false;
            }
        default: return false;
    }
}

static int match_type(hypex_parser *p, const hypex_token *tok) {
    if (tok->kind == HYPEX_TOK_IDENT) return HYPEX_TYPE_IDENT;
    if (tok->kind != HYPEX_TOK_KEYWORD) return -1;
    switch (tok->id) {
        case HYPEX_KW_INT: return HYPEX_TYPE_INT;
        case HYPEX_KW_LONG: return HYPEX_TYPE_LONG;
        case HYPEX_KW_SHORT: return HYPEX_TYPE_SHORT;
        case HYPEX_KW_FLOAT: return HYPEX_TYPE_FLOAT;
        case HYPEX_KW_DOUBLE: return HYPEX_TYPE_DOUBLE;
        case HYPEX_KW_CHAR: return HYPEX_TYPE_CHAR;
        case HYPEX_KW_STRING: return HYPEX_TYPE_STRING;
        case HYPEX_KW_UINT: return HYPEX_TYPE_UINT;
        case HYPEX_KW_ULONG: return HYPEX_TYPE_ULONG;
        case HYPEX_KW_USHORT: return HYPEX_TYPE_USHORT;
        case HYPEX_KW_BOOL: return HYPEX_TYPE_BOOL;
        default: return -1;
    }
}

static int match_pre_unary_op(int kind) {
    switch (kind) {
        case HYPEX_TOK_MINUS: return HYPEX_OP_NEG;
        case HYPEX_TOK_AMPER: return HYPEX_OP_AMPER;
        case HYPEX_TOK_EXCLAMATION: return HYPEX_OP_NOT;
        case HYPEX_TOK_TILDE: return HYPEX_OP_BIT_NOT;
        case HYPEX_TOK_INCREASE: return HYPEX_OP_PRE_INC;
        case HYPEX_TOK_DECREASE: return HYPEX_OP_PRE_DEC;
        default: return -1;
    }
}

static int match_op(int kind) {
    switch (kind) {
        case HYPEX_TOK_EQUAL: return HYPEX_OP_ASSIGN;
        case HYPEX_TOK_PLUS: return HYPEX_OP_ADD;
        case HYPEX_TOK_MINUS: return HYPEX_OP_SUB;
        case HYPEX_TOK_STAR: return HYPEX_OP_MUL;
        case HYPEX_TOK_SLASH: return HYPEX_OP_DIV;
        case HYPEX_TOK_LESS: return HYPEX_OP_LT;
        case HYPEX_TOK_GREATER: return HYPEX_OP_GT;
        case HYPEX_TOK_AMPER: return HYPEX_OP_BIT_AND;
        case HYPEX_TOK_PIPE: return HYPEX_OP_BIT_OR;
        case HYPEX_TOK_PERCENT: return HYPEX_OP_MOD;
        case HYPEX_TOK_DOT: return HYPEX_OP_ACCESS;
        case HYPEX_TOK_QUEST: return HYPEX_OP_COND;
        case HYPEX_TOK_CARET: return HYPEX_OP_XOR;
        case HYPEX_TOK_TWO_EQ: return HYPEX_OP_EQ;
        case HYPEX_TOK_PLUS_EQ: return HYPEX_OP_A_ADD;
        case HYPEX_TOK_MINUS_EQ: return HYPEX_OP_A_SUB;
        case HYPEX_TOK_STAR_EQ: return HYPEX_OP_A_MUL;
        case HYPEX_TOK_SLASH_EQ: return HYPEX_OP_A_DIV;
        case HYPEX_TOK_LESS_EQ: return HYPEX_OP_LE;
        case HYPEX_TOK_GREATER_EQ: return HYPEX_OP_GE;
        case HYPEX_TOK_AMPER_EQ: return HYPEX_OP_A_AND;
        case HYPEX_TOK_TWO_AMPER: return HYPEX_OP_AND;
        case HYPEX_TOK_PIPE_EQ: return HYPEX_OP_A_OR;
        case HYPEX_TOK_TWO_PIPE: return HYPEX_OP_OR;
        case HYPEX_TOK_EXCLAMATION_EQ: return HYPEX_OP_NE;
        case HYPEX_TOK_PERCENT_EQ: return HYPEX_OP_A_MOD;
        case HYPEX_TOK_CARET_EQ: return HYPEX_OP_A_XOR;
        case HYPEX_TOK_LSHIFT: return HYPEX_OP_SHL;
        case HYPEX_TOK_RSHIFT: return HYPEX_OP_SHR;
        case HYPEX_TOK_INCREASE: return HYPEX_OP_POST_INC;
        case HYPEX_TOK_DECREASE: return HYPEX_OP_POST_DEC;
        case HYPEX_TOK_LSHIFT_EQ: return HYPEX_OP_A_SHL;
        case HYPEX_TOK_RSHIFT_EQ: return HYPEX_OP_A_SHR;
        default: return -1;
    }
}

static void parse_type(hypex_parser *p, hypex_type *type);

static void parse_func_decl(hypex_parser *p, hypex_node *expr);

static hypex_node *parse_arg_decl(hypex_parser *p, hypex_node *parent);

static void parse_var_decl(hypex_parser *p, hypex_node *expr);

static hypex_node *parse_primary(hypex_parser *p, hypex_node *parent);

static hypex_node *parse_expr(hypex_parser *p, hypex_node *parent, int min_prec);

static hypex_node *parse_stmt(hypex_parser *p);

static void parse_type(hypex_parser *p, hypex_type *type) {
    const int kind = match_type(p, p->tok);
    if (kind != -1) type->kind = kind;
    if (p->tok->kind == HYPEX_TOK_LPAR) {
        if (!next_expect(p)) error_expect(p, "types");
        while (has_next(p) && p->tok->kind != HYPEX_TOK_RPAR) {
            next(p);
        }
    }
}

static void parse_func_decl(hypex_parser *p, hypex_node *expr) {
    if (!next_expect(p)) error_expect(p, "identifier or ':'");
    if (p->tok->kind == HYPEX_TOK_COLON) {
        if (!next_expect(p)) error_expect(p, "type");
        parse_type(p, expr->data.func_decl.type);
        if (!next_expect(p)) error_expect(p, "identifier");
    }
    if (p->tok->kind != HYPEX_TOK_IDENT) error_expect(p, "identifier");
    expr->data.func_decl.ident = p->tok;
    if (!next_expect(p) || p->tok->kind != HYPEX_TOK_LPAR) error_expect(p, "'('");
    if (!next_expect(p)) error_expect(p, "')' or argument");
    while (p->tok->kind != HYPEX_TOK_RPAR) {
        push_arg(expr, parse_arg_decl(p, expr));
        if (!has_next(p)) {
            error_expect(p, "')'");
            break;
        }
        if (p->tok->kind == HYPEX_TOK_RPAR) break;
        if (p->tok->kind == HYPEX_TOK_COMMA) {
            next(p);
        } else {
            error_expect(p, "','");
            break;
        }
    }
    next(p);
    switch (p->tok->kind) {
        case HYPEX_TOK_COLON: break;
        case HYPEX_TOK_SEMI: return;
        default: error_expect(p, "function body or ';'");
    }
}

static hypex_node *parse_arg_decl(hypex_parser *p, hypex_node *parent) {
    hypex_node *arg = make_node(HYPEX_NODE_ARG_DECL, parent);
    init_var_decl(arg, 1);
    if (p->tok->kind == HYPEX_TOK_KEYWORD && p->tok->id == HYPEX_KW_CONST) {
        if (!next_expect(p) || p->tok->kind != HYPEX_TOK_COLON) error_expect(p, "':'");
        set_flags(arg->data.var_decl.type, FLAG_CONST);
        next(p);
    }
    if (!has_next(p)) error_expect(p, "type");
    parse_type(p, arg->data.var_decl.type);
    if (!next_expect(p) || p->tok->kind != HYPEX_TOK_IDENT) error_expect(p, "identifier");
    arg->data.var_decl.ident = p->tok;
    next(p);
    return arg;
}

static void parse_var_decl(hypex_parser *p, hypex_node *expr) {
    if (!next_expect(p)) error_expect(p, "type or identifier");
    if (p->tok->kind == HYPEX_TOK_COLON) {
        if (!next_expect(p)) error_expect(p, "type");
        parse_type(p, expr->data.var_decl.type);
        if (!has_next(p)) error_expect(p, "identifier");
    }
    expr->data.var_decl.ident = p->tok;
    if (!next_expect(p)) return;
    if (p->tok->kind == HYPEX_TOK_EQUAL) {
        if (!next_expect(p)) error_expect(p, "expression");
        expr->data.var_decl.init = parse_expr(p, expr, 0);
    }
}

static hypex_node *parse_primary(hypex_parser *p, hypex_node *parent) {
    hypex_token *tok = p->tok;
    hypex_node *node = NULL;
    if (tok->kind == HYPEX_TOK_LPAR) {
        if (!next_expect(p)) error_expect(p, "expression");
        node = parse_expr(p, parent, 0);
        if (!next_expect(p) && p->tok->kind != HYPEX_TOK_LPAR) error_expect(p, "')'");
        return node;
    }
    int op = match_pre_unary_op(tok->kind);
    if (op != -1) {
        if (!next_expect(p)) error_expect(p, "operand");
        hypex_node *unary = make_node(HYPEX_NODE_UNARY_OP, parent);
        unary->data.unary_op.op = op;
        unary->data.unary_op.prefix = true;
        unary->data.unary_op.operand = parse_expr(p, unary, OP[op][0]);
        unary->data.unary_op.operand->parent = unary;
        return unary;
    }
    if (tok->kind == HYPEX_TOK_IDENT) {
        node = make_node(HYPEX_NODE_IDENT, parent);
        node->data.value = tok;
        next(p);
        return node;
    }
    if (is_literal(tok)) {
        node = make_node(HYPEX_NODE_LITERAL, parent);
        node->data.value = tok;
        next(p);
        return node;
    }
    error_parser(p, "invalid expression");
    return NULL;
}

static hypex_node *parse_expr(hypex_parser *p, hypex_node *parent, int min_prec) {
    hypex_node *lhs = parse_primary(p, parent);
    while (true) {
        int op = match_op(p->tok->kind);
        if (op == -1 || !is_binary_op(op)) break;
        int prec = OP[op][0];
        if (prec < min_prec) break;
        bool assoc = OP[op][1];
        int next_min_prec = assoc ? prec : prec + 1;
        next(p);
        hypex_node *rhs = parse_expr(p, parent, next_min_prec);
        hypex_node *binop = make_node(HYPEX_NODE_BINARY_OP, parent);
        init_binary_op(binop, op);
        binop->data.binary_op.left = lhs;
        binop->data.binary_op.right = rhs;
        lhs->parent = binop;
        rhs->parent = binop;
        lhs = binop;
    }
    return lhs;
}

static hypex_node *parse_stmt(hypex_parser *p) {
    hypex_node *stmt = make_node(HYPEX_NODE_EXPR, p->expr);
    const hypex_token *tok = p->tok;
    bool is_stat = false;
    if (tok->kind == HYPEX_TOK_STAR) {
        is_stat = true;
        if (!next_expect(p)) error_expect(p, "expression");
    }
    if (tok->kind == HYPEX_TOK_KEYWORD) {
        switch (tok->id) {
            case HYPEX_KW_FUNC:
                init_func_decl(stmt);
                if (is_stat) set_flags(stmt->data.func_decl.type, FLAG_STAT);
                parse_func_decl(p, stmt);
                break;
            case HYPEX_KW_VAR:
                init_var_decl(stmt, 0);
                if (is_stat) set_flags(stmt->data.var_decl.type, FLAG_STAT);
                parse_var_decl(p, stmt);
                break;
            case HYPEX_KW_CONST:
                init_var_decl(stmt, 0);
                set_flags(stmt->data.var_decl.type, FLAG_CONST);
                if (is_stat) set_flags(stmt->data.var_decl.type, FLAG_STAT);
                parse_var_decl(p, stmt);
                break;
            case HYPEX_KW_RETURN:
                stmt->kind = HYPEX_NODE_RET_STMT;
                if (!next_expect(p)) error_expect(p, "expression");
                stmt->data.expr = parse_expr(p, stmt, 0);
                break;
            case HYPEX_KW_IF:
                stmt->kind = HYPEX_NODE_IF_STMT;
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

void parse(hypex_parser *p) {
    while (has_next(p)) {
        push_block(p, parse_stmt(p));
        next(p);
        while (has_next(p) && p->tok->kind == HYPEX_TOK_NEWLINE) next(p);
    }
}

void free_parser(hypex_parser *p) {
    free(p->tok_list);
    free(p->expr);
    free(p);
}