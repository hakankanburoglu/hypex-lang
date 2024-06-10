#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#include "token.h"
#include "lex.h"
#include "error.h"

#define KWLEN 41

const char *KW[KWLEN] = {
    "if",
    "for","int","let","try","use","var",
    "base","bool","case","char","elif","else","enum","func","long","null","self","true","uint",
    "async","await","break","catch","class","const","false","float","short","throw","ulong","while",
    "double","object","return","string","struct","switch","ushort",
    "continue",
    "interface"
};

void push_token(Token **tokens, size_t *len, const Token tok) {
    *tokens = (Token *)realloc(*tokens, (*len + 1) * sizeof(Token));
    if (*tokens == NULL) {
        error(ERR_LEX_MSG, "1", NULL);
        return;
    }
    (*tokens)[(*len)++] = tok;
}

void consume_token(Token *tok, const char c) {
    if (tok->data == NULL) {
        tok->data = (char *)realloc(tok->data, 2 * sizeof(char));
        if (tok->data == NULL) {
            error(ERR_LEX_MSG, "2", NULL);
            return;
        }
        tok->len = 1;
        tok->data[0] = c;
        tok->data[1] = '\0';
    } else {
        tok->data[tok->len++] = c;
        tok->data = (char *)realloc(tok->data, (tok->len + 1) * sizeof(char));
        if (tok->data == NULL) {
            error(ERR_LEX_MSG, "3", NULL);
            return;
        }
        tok->data[tok->len] = '\0';
    }
}

void consume_wtoken(Token *tok, const wchar_t c) {
    if (tok->wdata == NULL) {
        tok->wdata = (wchar_t *)realloc(tok->wdata, 2 * sizeof(wchar_t));
        if (tok->wdata == NULL) {
            error(ERR_LEX_MSG, "4", NULL);
            return;
        }
        tok->len = 1;
        tok->wdata[0] = c;
        tok->wdata[1] = L'\0';
    } else {
        tok->wdata[tok->len++] = c;
        tok->wdata = (wchar_t *)realloc(tok->wdata, (tok->len + 1) * sizeof(wchar_t));
        if (tok->wdata == NULL) {
            error(ERR_LEX_MSG, "5", NULL);
            return;
        }
        tok->wdata[tok->len] = L'\0';
    }
}

bool is_keyword(const char *s, const size_t len) {
    switch (len) {
        case 2:
            return strcmp(KW[0], s) == 0;
        case 3:
            for (int i = 1; i <= 6; i++)
                if (strcmp(KW[i], s) == 0)
                    return true;
            return false;
        case 4:
            for (int i = 7; i <= 19; i++)
                if (strcmp(KW[i], s) == 0)
                    return true;
            return false;
        case 5:
            for (int i = 20; i <= 31; i++)
                if (strcmp(KW[i], s) == 0)
                    return true;
            return false;
        case 6:
            for (int i = 32; i <= 38; i++)
                if (strcmp(KW[i], s) == 0)
                    return true;
            return false;
        case 8:
            return strcmp(KW[39], s) == 0;
        case 9:
            return strcmp(KW[40], s) == 0;
        default:
            return false;
    }
}

bool is_ident_body(const char c) {
    return isalnum(c) || c == '_';
}

Token *tokenize(const char *input, size_t *len, const bool is_continues, const int line, int current) {
    Token *tokens = NULL;
    *len = 0;
    const size_t inputlen = strlen(input);
    while (current < inputlen) {
        Token buf = make_token(UNKNOWN, line, current);
        if (current == 0 && is_continues) {
            buf = make_wtoken(COMMENT_BLOCK, line, current);
            buf.is_continues = true;
            while (is_continues) {
                if (input[current] == '*') {
                    current++;
                    if (input[current] == '/') {
                        current++;
                        // is_continues = false;
                    } else {
                        consume_wtoken(&buf, input[current]);
                    }
                } else {
                    consume_wtoken(&buf, input[current]);
                }
            }
        }
        switch (input[current]) {
            case '=':
                buf.type = EQUAL;
                current++;
                if (current < inputlen && input[current] == '=') {
                    buf.type = TWO_EQ;
                    current++;
                }
                break;
            case '+':
                buf.type = PLUS;
                current++;
                if (current < inputlen) {
                    switch (input[current]) {
                        case '=':
                            buf.type = PLUS_EQ;
                            current++;
                            break;
                        case '+':
                            buf.type = INCREASE;
                            current++;
                            break;
                    }
                }
                break;
            case '-':
                buf.type = MINUS;
                current++;
                if (current < inputlen) {
                    switch (input[current]) {
                        case '=':
                            buf.type = MINUS_EQ;
                            current++;
                            break;
                        case '-':
                            buf.type = DECREASE;
                            current++;
                            break;
                    }
                }
                break;
            case '*':
                buf.type = STAR;
                current++;
                if (current < inputlen && input[current] == '=') {
                    buf.type = STAR_EQ;
                    current++;
                }
                break;
            case '/':
                buf.type = SLASH;
                current++;
                if (current < inputlen) {
                    switch (input[current]) {
                        case '=':
                            buf.type = SLASH_EQ;
                            current++;
                            break;
                        case '/':
                            buf = make_wtoken(COMMENT_LINE, line, current);
                            current++;
                            while (current < inputlen) {
                                consume_wtoken(&buf, input[current]);
                                current++;
                            }
                            break;
                        case '*':
                            buf = make_wtoken(COMMENT_BLOCK, line, current);
                            buf.is_continues = true;
                            current++;
                            while (current < inputlen) {
                                if (input[current] == '*') {
                                    current++;
                                    if (current < inputlen && input[current] == '/') {
                                        buf.is_continues = false;
                                        current++;
                                        break;
                                    }
                                    consume_wtoken(&buf, L'*');
                                    consume_wtoken(&buf, input[current]);
                                    current++;
                                } else {
                                    consume_wtoken(&buf, input[current]);
                                    current++;
                                }
                            }
                            break;
                    }
                }
                break;
            case '<':
                buf.type = GREATER;
                current++;
                if (current < inputlen) {
                    switch (input[current]) {
                        case '=':
                            buf.type = GREATER_EQ;
                            current++;
                            break;
                        case '<':
                            buf.type = LSHIFT;
                            current++;
                            break;
                    }
                }
                break;
            case '>':
                buf.type = LESS;
                current++;
                if (current < inputlen) {
                    switch (input[current]) {
                        case '=':
                            buf.type = LESS_EQ;
                            current++;
                            break;
                        case '>':
                            buf.type = RSHIFT;
                            current++;
                            break;
                    }
                }
                break;
            case '&':
                buf.type = AMPER;
                current++;
                if (current < inputlen) {
                    switch (input[current]) {
                        case '&':
                            buf.type = TWO_AMPER;
                            current++;
                            break;
                        case '=':
                            buf.type = AMPER_EQ;
                            current++;
                            break;
                    }
                }
                break;
            case '|':
                buf.type = PIPE;
                current++;
                if (current < inputlen) {
                    switch (input[current]) {
                        case '|':
                            buf.type = TWO_PIPE;
                            current++;
                            break;
                        case '=':
                            buf.type = PIPE_EQ;
                            current++;
                            break;
                    }
                }
                break;
            case '!':
                buf.type = EXCLAMATION;
                current++;
                if (current < inputlen && input[current] == '=') {
                    buf.type = EXCLAMATION_EQ;
                    current++;
                }
                break;
            case '%':
                buf.type = PERCENT;
                current++;
                if (current < inputlen && input[current] == '=') {
                    buf.type = PERCENT_EQ;
                    current++;
                }
                break;
            case '.':
                buf.type = DOT;
                current++;
                break;
            case ',':
                buf.type = COMMA;
                current++;
                break;
            case ':':
                buf.type = COLON;
                current++;
                break;
            case ';':
                buf.type = SEMI;
                current++;
                break;
            case '_':
                buf.type = UNDERSCORE;
                consume_token(&buf, input[current]);
                current++;
                if (current < inputlen && is_ident_body(input[current])) {
                    buf.type = IDENT;
                    consume_token(&buf, input[current]);
                    current++;
                    while (current < inputlen && is_ident_body(input[current])) {
                        consume_token(&buf, input[current]);
                        current++;
                    }
                }
                break;
            case '?':
                buf.type = QUEST;
                current++;
                break;
            case '(':
                buf.type = LPAR;
                current++;
                break;
            case ')':
                buf.type = RPAR;
                current++;
                break;
            case '[':
                buf.type = LSQB;
                current++;
                break;
            case ']':
                buf.type = RSQB;
                current++;
                break;
            case '{':
                buf.type = LBRACE;
                current++;
                break;
            case '}':
                buf.type = RBRACE;
                current++;
                break;
            case '~':
                buf.type = TILDE;
                current++;
                break;
            case '^':
                buf.type = CARET;
                current++;
                if (current < inputlen && input[current] == '=') {
                    buf.type = CARET_EQ;
                    current++;
                }
                break;
            case '@':
                buf.type = AT;
                current++;
                break;
            case '#':
                buf.type = HASH;
                current++;
                break;
            case ' ':
                buf.type = SPACE;
                buf.len++;
                current++;
                while (current < inputlen && input[current] == ' ') {
                    buf.len++;
                    current++;
                }
                break;
            case '\'':
                break;
            default:
                if (isdigit(input[current])) {
                    buf.type = INTEGER;
                    consume_token(&buf, input[current]);
                    current++;
                    while (current < inputlen && (isdigit(input[current]) || (input[current] == '.' && buf.type != FLOAT))) {
                        if (input[current] == '.')
                            buf.type = FLOAT;
                        consume_token(&buf, input[current]);
                        current++;
                    }
                } else if (isalpha(input[current])) {
                    buf.type = IDENT;
                    consume_token(&buf, input[current]);
                    current++;
                    while (current < inputlen && is_ident_body(input[current])) {
                        consume_token(&buf, input[current]);
                        current++;
                    }
                } else {
                    errorp(line, current, input[current], ERR_CHR_MSG, NULL);
                }
                break;
        }
        if (buf.type == IDENT && is_keyword(buf.data, buf.len))
            buf.type = KEYWORD;
        if (buf.len == 0)
            buf.len = 1;
        push_token(&tokens, len, buf);
    }
    return tokens;
}