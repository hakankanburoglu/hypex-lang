#ifndef PARSER_H
#define PARSER_H

#include <stddef.h>

#include "parse/lexer.h"
#include "parse/node.h"
#include "parse/token.h"

typedef struct {
    Token **tok_list;
    size_t toklen;
    Node *expr;
    char *file;
    int offset;
    Token *tok;
} Parser;

Parser *make_parser(Lexer *lex);

void parse(Parser *p);

void free_parser(Parser *p);

#endif //PARSER_H
