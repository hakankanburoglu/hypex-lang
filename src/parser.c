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

static inline bool is_ignore_token(int type) {
    switch (type) {
        case COMMENT_LINE: case COMMENT_BLOCK: case SPACE:
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
    while (is_ignore_token(current(p)->type)) p->offset++;
}

static inline bool is_unary_op(int type) {
    return type == 10 || type == 24 || type == 25 || type == 45 || type == 46;
}

static inline bool is_binary_op(int type) {
    return (type >= 1 && type <= 9) || type == 11 || (type >= 29 && type <= 44) || type == 48 || type == 49;
}

static inline bool is_literal(int type) {
    switch (type) {
        case INTEGER: case FLOAT: case CHAR: case STRING: case RSTRING:
            return true;
        default:
            return false;
    }
}

static inline void *parse_var_decl(Parser *p, Node *node) {
    advance(p);
    if (current(p)->type == IDENT) {
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
        if (is_literal(tok->type)) {
            expr->kind = NODE_LITERAL;
            expr->data.literal.value = tok;
        } else if (tok->type == IDENT) {
            expr->kind = NODE_IDENT;
            expr->data.ident.name = tok->value;
            expr->data.ident.len = tok->len;
        } else if (tok->type == KEYWORD) {
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