#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#include "token.h"
#include "lex.h"
#include "error.h"

#define KWLEN 40

const char *KW[KWLEN] = {
    "if",
    "for","int","let","try","use","var",
    "base","bool","case","char","elif","else","enum","func","long","null","self","true","uint",
    "async","await","break","catch","class","const","false","float","short","throw","ulong","while",
    "double","object","return","string","struct","switch","ushort",
    "continue"
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
        default:
            return false;
    }
}

bool is_ident_body(const char c) {
    return isalnum(c) || c == '_';
}

Token *tokenize(const char *input, size_t *len, bool *start_block, const int line, int current) {
    Token *tokens = NULL;
    *len = 0;
    const size_t inputlen = strlen(input);
    bool potential_fstring = false;
    bool potential_rstring = false;
    while (current < inputlen) {
        Token buf = make_token(UNKNOWN, line, current);
        if (current == 0 && *start_block) {
            buf.type = COMMENT_BLOCK;
            while (current < inputlen) {
                if (input[current] == '*') {
                    current++;
                    if (current < inputlen) {
                        if (input[current] == '/') {
                            current++;
                            *start_block = false;
                            break;
                        } else {
                            consume_token(&buf, input[current]);
                        }
                    }
                } else {
                    consume_token(&buf, input[current]);
                }
                current++;
            }
            push_token(&tokens, len, buf);
            buf = make_token(UNKNOWN, line, current);
            if (current >= inputlen)
                break;
        }
        if (potential_fstring && input[current] == '"') {
            buf.type = FSTRING;
            buf.line = tokens[*len - 1].line;
            buf.col = tokens[*len - 1].col;
            current++;
            bool escape_string = false;
            while (current < inputlen && !(input[current] == '"' && !escape_string)) {
                if (input[current] == '\\') {
                    escape_string = true;
                    consume_token(&buf, input[current]);
                    current++;
                }
                if (current < inputlen) {
                    consume_token(&buf, input[current]);
                    current++;
                    if (escape_string)
                        escape_string = false;
                }
            }
            current++;
            potential_fstring = false;
            tokens[*len - 1] = buf;
            buf = make_token(UNKNOWN, line, current);
            if (current >= inputlen)
                break;
        }
        if (potential_rstring && input[current] == '"') {
            buf.type = RSTRING;
            buf.line = tokens[*len - 1].line;
            buf.col = tokens[*len - 1].col;
            current++;
            while (current < inputlen && input[current] != '"') {
                consume_token(&buf, input[current]);
                current++;
            }
            current++;
            potential_rstring = false;
            tokens[*len - 1] = buf;
            buf = make_token(UNKNOWN, line, current);
            if (current >= inputlen)
                break;
        }
        switch (input[current]) {
            case '=':
                buf.type = EQUAL;
                current++;
                if (current < inputlen) {
                    switch (input[current]) {
                        case '=':
                            buf.type = TWO_EQ;
                            current++;
                            break;
                    }
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
                if (current < inputlen) {
                    switch (input[current]) {
                        case '=':
                            buf.type = STAR_EQ;
                            current++;
                            break;
                    }
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
                            buf.type = COMMENT_LINE;
                            current++;
                            while (current < inputlen) {
                                consume_token(&buf, input[current]);
                                current++;
                            }
                            break;
                        case '*':
                            buf.type = COMMENT_BLOCK;
                            current++;
                            *start_block = true;
                            while (current < inputlen) {
                                if (input[current] == '*') {
                                    current++;
                                    if (current < inputlen) {
                                        if (input[current] == '/') {
                                            *start_block = false;
                                            current++;
                                            break;
                                        } else {
                                            consume_token(&buf, input[current]);
                                            current++;
                                        }
                                    }
                                } else {
                                    consume_token(&buf, input[current]);
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
                            if (current < inputlen) {
                                switch (input[current]) {
                                    case '=':
                                        buf.type = LSHIFT_EQ;
                                        current++;
                                        break;
                                }
                            }
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
                            if (current < inputlen) {
                                switch (input[current]) {
                                    case '=':
                                        buf.type = RSHIFT_EQ;
                                        current++;
                                        break;
                                }
                            }
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
                if (current < inputlen) {
                    switch (input[current]) {
                        case '=':
                            buf.type = EXCLAMATION_EQ;
                            current++;
                            break;
                    }
                }
                break;
            case '%':
                buf.type = PERCENT;
                current++;
                if (current < inputlen) {
                    switch (input[current]) {
                        case '=':
                            buf.type = PERCENT_EQ;
                            current++;
                            break;
                    }
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
                if (current < inputlen) {
                    switch (input[current]) {
                        case '=':
                            buf.type = CARET_EQ;
                            current++;
                            break;
                    }
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
                buf.type = CHAR;
                current++;
                bool escape_char = false;
                while (current < inputlen && !(input[current] == '\'' && !escape_char)) {
                    if (input[current] == '\\') {
                        escape_char = true;
                        consume_token(&buf, input[current]);
                        current++;
                    }
                    if (current < inputlen) {
                        consume_token(&buf, input[current]);
                        current++;
                        if (escape_char)
                            escape_char = false;
                    }
                }
                current++;
                break;
            case '"':
                buf.type = STRING;
                current++;
                bool escape_string = false;
                while (current < inputlen && !(input[current] == '"' && !escape_string)) {
                    if (input[current] == '\\') {
                        escape_string = true;
                        consume_token(&buf, input[current]);
                        current++;
                    }
                    if (current < inputlen) {
                        consume_token(&buf, input[current]);
                        current++;
                        if (escape_string)
                            escape_string = false;
                    }
                }
                current++;
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
        if (buf.type == IDENT && strcmp(buf.data, "f") == 0)
            potential_fstring = true;
        if (buf.type == IDENT && strcmp(buf.data, "r") == 0)
            potential_rstring = true;
        push_token(&tokens, len, buf);
    }
    push_token(&tokens, len, make_token(EOL, line, current));
    return tokens;
}