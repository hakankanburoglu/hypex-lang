#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <wchar.h>

#include "token.h"
#include "lex.h"
#include "error.h"
#include "comp.h"

void print_token(const Token tok) {
    printf("%d, %d, ", (tok.line + 1), (tok.col + 1));
    switch (tok.type) {
        case UNKNOWN: printf("UNKNOWN"); break;
        case EQUAL: printf("EQUAL"); break;
        case PLUS: printf("PLUS"); break;
        case MINUS: printf("MINUS"); break;
        case STAR: printf("STAR"); break;
        case SLASH: printf("SLASH"); break;
        case GREATER: printf("GREATER"); break;
        case LESS: printf("LESS"); break;
        case AMPER: printf("AMPER"); break;
        case PIPE: printf("PIPE"); break;
        case EXCLAMATION: printf("EXCLAMATION"); break;
        case PERCENT: printf("PERCENT"); break;
        case DOT: printf("DOT"); break;
        case COMMA: printf("COMMA"); break;
        case COLON: printf("COLON"); break;
        case SEMI: printf("SEMI"); break;
        case UNDERSCORE: printf("UNDERSCORE"); break;
        case QUEST: printf("QUEST"); break;
        case LPAR: printf("LPAR"); break;
        case RPAR: printf("RPAR"); break;
        case LSQB: printf("LSQB"); break;
        case RSQB: printf("RSQB"); break;
        case LBRACE: printf("LBRACE"); break;
        case RBRACE: printf("RBRACE"); break;
        case TILDE: printf("TILDE"); break;
        case CARET: printf("CARET"); break;
        case AT: printf("AT"); break;
        case HASH: printf("HASH"); break;
        case TWO_EQ: printf("TWO_EQ"); break;
        case TWO_AMPER: printf("TWO_AMPER"); break;
        case TWO_PIPE: printf("TWO_PIPE"); break;
        case PLUS_EQ: printf("PLUS_EQ"); break;
        case MINUS_EQ: printf("MINUS_EQ"); break;
        case STAR_EQ: printf("STAR_EQ"); break;
        case SLASH_EQ: printf("SLASH_EQ"); break;
        case GREATER_EQ: printf("GREATER_EQ"); break;
        case LESS_EQ: printf("LESS_EQ"); break;
        case AMPER_EQ: printf("AMPER_EQ"); break;
        case PIPE_EQ: printf("PIPE_EQ"); break;
        case EXCLAMATION_EQ: printf("EXCLAMATION_EQ"); break;
        case PERCENT_EQ: printf("PERCENT_EQ"); break;
        case CARET_EQ: printf("CARET_EQ"); break;
        case LSHIFT: printf("LSHIFT"); break;
        case RSHIFT: printf("RSHIFT"); break;
        case INCREASE: printf("INCREASE"); break;
        case DECREASE: printf("DECREASE"); break;
        case LSHIFT_EQ: printf("LSHIFT_EQ"); break;
        case RSHIFT_EQ: printf("RSHIFT_EQ"); break;
        case INTEGER: printf("INTEGER"); break;
        case FLOAT: printf("FLOAT"); break;
        case IDENT: printf("IDENT"); break;
        case KEYWORD: printf("KEYWORD"); break;
        case COMMENT_LINE: printf("COMMENT_LINE"); break;
        case COMMENT_BLOCK: printf("COMMENT_BLOCK"); break;
        case CHAR: printf("CHAR"); break;
        case STRING: printf("STRING"); break;
        case FSTRING: printf("FSTRING"); break;
        case SPACE: printf("SPACE"); break;
        case _INDENT: printf("_INDENT"); break;
        case _DEDENT: printf("_DEDENT"); break;
        case EOL: printf("EOL"); break;
        default: printf("%d", tok.type); break;
    };
    printf(", %d, %d, %d, `", tok.is_continues, tok.is_wide, tok.len);
    if (!tok.is_wide && tok.data != NULL)
        printf("%s", tok.data);
    printf("`, `");
    if (tok.is_wide && tok.wdata != NULL)
        wprintf(L"%ls", tok.wdata);
    printf("`\n");
}

void print_lex(const Token *tokens, const size_t len) {
    printf("line, col, type, is_continues, is_wide, len, data, wdata\n");
    for (int i = 0; i < len; i++)
        print_token(tokens[i]);
}

void comp(const char *path) {
    FILE *file = fopen(path, "r");
    if (file == NULL)
        error(ERR_FILE_MSG, path, NULL);
    char *input = NULL;
    size_t inputlen = 0;
    Token *tokens = NULL;
    size_t toklen = 0;
    int last_indent = 0;
    bool is_continues = false;
    int line = 0;
    long read;
    while ((read = getline(&input, &inputlen, file)) != -1) {
        if (read > 0 && input[read - 1] == '\n')
            input[read - 1] = '\0';
        if (inputlen > 0) {
            int current = 0;
            while (input[current] == ' ') {
                current++;
            }
            if (current > last_indent) {
                Token tok = make_token(_INDENT, line, 0);
                tok.len = current;
                push_token(&tokens, &toklen, tok);
                last_indent = current;
            }
            if (current < last_indent) {
                Token tok = make_token(_DEDENT, line, 0);
                tok.len = current;
                push_token(&tokens, &toklen, tok);
                last_indent = current;
            }
            size_t buflen = 0;
            Token *buf = tokenize(input, &buflen, is_continues, line, current);
            is_continues = buf[buflen - 1].is_continues;
            tokens = (Token *)realloc(tokens, (toklen + buflen + 1) * sizeof(Token));
            for (int i = 0; i < buflen; i++)
                tokens[toklen + i] = buf[i];
            toklen += buflen;
            push_token(&tokens, &toklen, make_token(EOL, line, current + buflen));
        }
        line++;
    }
    free(input);
    fclose(file);
    print_lex(tokens, toklen);
}
