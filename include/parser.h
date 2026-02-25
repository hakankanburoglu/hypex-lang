#ifndef PARSER_H
#define PARSER_H

#include <stddef.h>

#include "ast.h"
#include "lexer.h"
#include "token.h"

typedef struct {
    hypex_token **tok_list;
    size_t toklen;
    hypex_node *expr;
    char *file;
    int offset;
    hypex_token *tok;
} hypex_parser;

hypex_parser *hypex_parser_make(hypex_lexer *lex);

void hypex_parser_parse(hypex_parser *p);

void hypex_parser_free(hypex_parser *p);

#endif // PARSER_H
