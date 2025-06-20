#ifndef PARSER_H
#define PARSER_H

#include <stddef.h>

#include "lexer.h"
#include "node.h"
#include "token.h"

typedef struct {
    Token **tok_list;
    size_t toklen;
    Node *expr;
    char *file;
    int offset;
    Token *tok;
} Parser;

Parser *init_parser(Lexer *lex);

void parse(Parser *p);

void free_parser(Parser *p);

#endif //PARSER_H
