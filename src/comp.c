#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

#include "token.h"
#include "node.h"
#include "lex.h"
#include "parse.h"
#include "error.h"
#include "out.h"
#include "comp.h"

void push_indent(int **indents, int *indentlen, const int indent) {
    *indents = (int *)realloc(*indents, (*indentlen + 1) * sizeof(int));
    if (*indents == NULL)
        error_lex("4");
    (*indents)[(*indentlen)++] = indent;
}

void pop_indent(int **indents, int *indentlen) {
    *indents = (int *)realloc(*indents, (*indentlen - 1) * sizeof(int));
    if (*indents == NULL)
        error_lex("5");
    (*indentlen)--;
}

void concat_token(Token **tokens, size_t *toklen, const Token *buf, const size_t buflen) {
    *tokens = (Token *)realloc(*tokens, (*toklen + buflen) * sizeof(Token));
    if (*tokens == NULL)
        error_lex("6");
    for (int i = 0; i < buflen; i++)
        (*tokens)[*toklen + i] = buf[i];
    *toklen += buflen;
}

Token *lex(const char *src, size_t *len) {
    FILE *file = fopen(src, "r");
    if (file == NULL)
        error_file(src);
    char *input = NULL;
    size_t inputlen = 0;
    Token *tokens = NULL;
    (*len) = 0;
    int *indents = (int *)malloc(sizeof(int));
    indents[0] = 0;
    int indentlen = 1;
    int total_indent = 0;
    bool start_block = false;
    int line = 0;
    long read;
    while ((read = getline(&input, &inputlen, file)) != -1) {
        int current = 0;
        if (read > 0 && input[read - 1] == '\n')
            input[read - 1] = '\0';
        if (!start_block) {
            while (input[current] == ' ')
                current++;
            if (current > total_indent) {
                Token tok = make_token(_INDENT, line, 0);
                tok.len = current - total_indent;
                push_token(&tokens, len, tok);
                push_indent(&indents, &indentlen, current - total_indent);
                total_indent = current;
            }
            if (current < total_indent) {
                int i = 0;
                while (true) {
                    if (indents[indentlen - 1] > total_indent - current)
                        error_indent(line, current);
                    Token tok = make_token(_DEDENT, line, 0);
                    push_token(&tokens, len, tok);
                    pop_indent(&indents, &indentlen);
                    total_indent = current;
                    i++;
                    if (i >= indentlen)
                        break;
                }
            }
        }
        size_t buflen = 0;
        Token *buf = tokenize(input, &buflen, &start_block, line, current);
        concat_token(&tokens, len, buf, buflen);
        free(buf);
        line++;
    }
    fclose(file);
    free(input);
    return tokens;
}

Node *parse(const char *path, size_t *len) {
    size_t toklen = 0;
    Token *tokens = lex(path, &toklen);
    Node *nodes = parse_expr(tokens, toklen, len);
    free(tokens);
    return nodes;
}

void comp(const char *path) {
    // size_t len = 0;
    // Node *nodes = parse(path, &len);
    printf("this feature is not active yet\n");
}

void lex_analysis(const char *src, const char *out) {
    size_t len = 0;
    Token *tokens = lex(src, &len);
    FILE *file = fopen(out, "w");
    if (file == NULL)
        error_file(out);
    print_tokens(file, tokens, len);
    fclose(file);
    free(tokens);
}

void parse_analysis(const char *src, const char *out) {
    size_t len = 0;
    Node *nodes = parse(src, &len);
    FILE *file = fopen(out, "w");
    if (file == NULL)
        error_file(out);
    print_nodes(file, nodes, len);
    fclose(file);
    free(nodes);
}