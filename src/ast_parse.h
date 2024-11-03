#ifndef AST_PARSE_H
#define AST_PARSE_H

#include <stddef.h>

#include "token.h"
#include "node.h"
#include "ast_node.h"

typedef struct {
    int type;
    int current;
    int index;
    size_t len;
    Token data;
} ASTParser;

ASTParser init_ast_parser(const int type, const int current, const int index, const size_t len);

void push_ast_node(ASTNode **nodes, size_t *nodelen, const ASTNode node);

void consume_ast_node(ASTNode *node, const Token tok);

int is_var_def(ASTParser *parser);

ASTNode *parse_ast(const Node *nodes, const size_t node_len);

#endif //AST_PARSE_H