#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

#include "token.h"
#include "node.h"
#include "parser.h"
#include "error.h"

Parser *init_parser(Lexer *lex) {
    Parser *p = (Parser *)malloc(sizeof(Parser));
    p->tok_list = lex->tok_list;
    p->toklen = lex->len;
    p->expr = make_node(NODE_SOURCE, NULL);
    p->offset = 0;
    return p;
}

// only for SOURCE_NODE
static inline void consume_parse(Parser *p, Node *node) {
    if (!p->expr->data.source.body) {
        p->expr->data.source.body = (Node **)realloc(p->expr->data.source.body, sizeof(Node));
        if (!p->expr->data.source.body) error_hypex();
        p->expr->data.source.len = 1;
        p->expr->data.source.body[0] = node;
    } else {
        p->expr->data.source.body = (Node **)realloc(p->expr->data.source.body, sizeof(Node) * (p->expr->data.source.len + 1));
        if (!p->expr->data.source.body) error_hypex();
        p->expr->data.source.len++;
        p->expr->data.source.body[p->expr->data.source.len - 1] = node;   
    }
}

static inline bool is_ignore_token(int kind) {
    switch (kind) {
        case T_COMMENT_LINE: case T_COMMENT_BLOCK: case T_SPACE:
            return true;
        default:
            return false;
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

static inline bool is_unary_op(int kind) {
    return kind == 10 || kind == 24 || kind == 25 || kind == 45 || kind == 46;
}

static inline bool is_binary_op(int kind) {
    return (kind >= 1 && kind <= 9) || kind == 11 || (kind >= 29 && kind <= 44) || kind == 48 || kind == 49;
}

static inline bool is_literal(int type) {
    switch (type) {
        case T_INTEGER: case T_FLOAT: case T_CHAR: case T_STRING: case T_RSTRING:
            return true;
        default:
            return false;
    }
}

static inline void *parse_var_decl(Parser *p, Node *node) {
    advance(p);
    if (current(p)->kind == T_IDENT) {
        node->data.var_decl.ident = current(p)->value;
        node->data.var_decl.len = current(p)->len;
        if (current(p)->value[0] == '_')
            node->data.var_decl.attr.is_private = true;
    } else {
        error_hypex();
    }
}

static inline Node *parse_expr(Parser *p) {
    Node *expr = make_node(NODE_EXPR, p->expr);
    if (match(p)) {
        Token *tok = current(p);
        if (is_literal(tok->kind)) {
            expr->kind = NODE_LITERAL;
            expr->data.literal.value = tok;
        } else if (tok->kind == T_IDENT) {
            expr->kind = NODE_IDENT;
            expr->data.ident.name = tok->value;
            expr->data.ident.len = tok->len;
        } else if (tok->kind == T_KEYWORD) {
            switch (tok->id) {
                case KW_VAR:
                    expr->kind = NODE_VAR_DECL;
                    parse_var_decl(p, expr);
                    break;
                case KW_CONST:
                    expr->kind = NODE_VAR_DECL;
                    expr->data.var_decl.attr.is_const = true;
                    parse_var_decl(p, expr);
                    break;
                default:
                    error_hypex();
                    break;
            }
        } else {
            error_hypex();
        }
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