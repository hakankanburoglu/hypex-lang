#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

#include "token.h"
#include "node.h"
#include "ast_node.h"
#include "lex.h"
#include "ast_parse.h"

ASTParser init_ast_parser(const int type, const int current, const int index, const size_t len) {
    ASTParser ast_parser;
    ast_parser.type = type;
    ast_parser.current = current;
    ast_parser.index = index;
    ast_parser.len = len;
}

void push_ast_node(ASTNode **nodes, size_t *nodelen, const ASTNode node) {
    *nodes = (ASTNode *)realloc(*nodes, (*nodelen + 1) * sizeof(Node));
    if (*nodes == NULL)
        error_ast_parse("1");
    (*nodes)[(*nodelen)++] = node;
}

void consume_ast_node(ASTNode *node, const Token tok) {
    node->data = (Token *)realloc(node->data, (node->len + 1) * sizeof(Token));
    if (node->data == NULL)
        error_ast_parse("2");
    node->data[(node->len)++] = tok;
}

int is_var_def(ASTParser *parser) {
    while (parser->index < parser->len) {
        switch (parser->index) {
            case 0:
                if (token_compare(parser->data, KEYWORD, "var"))
                    (parser->index)++;
                break;
            case 1:
                if (parser->data.type == SPACE)
                    (parser->index)++;
                break;
            case 2:
                if (parser->data.type == IDENT)
                    (parser->index)++;
                if (parser->data.type == EQUAL)
                    return VAR_INFER;
        }
    }
    return 0;
}

ASTNode *parse_ast(const Node *nodes, const size_t node_len) {
    ASTNode *ast_nodes = (ASTNode *)malloc(sizeof(ASTNode));
    ast_nodes[0] = make_ast_node(ROOT_AST_NODE, -1);
    ASTParser parser = init_ast_parser(0, 0, 0, 0);
    while (parser.current < node_len) {
        ASTNode buf = make_ast_node(parser.type, nodes[parser.index].parent);
        switch (parser.type) {
            case 1:
                is_var_def(&parser);
                break;
        }
    }
    return ast_nodes;
}