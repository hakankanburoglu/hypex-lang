#ifndef PARSER_H
#define PARSER_H

#include <stddef.h>

#include "token.h"
#include "lexer.h"
#include "node.h"

typedef struct {
    Token **tok_list;
    size_t toklen;
    Node *expr;
    char *file;
    int offset;
} Parser;

Parser *init_parser(Lexer *lex);

void parse(Parser *p);

void free_parser(Parser *p);

#endif //PARSER_H
