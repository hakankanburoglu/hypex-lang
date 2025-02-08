#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>

#include "token.h"
#include "lexer.h"
#include "error.h"

#define KWLEN 41

const char *KWLIST[KWLEN] = {
    "if",
    "for", "int", "try", "use", "var",
    "base", "bool", "case", "char", "elif", "else", "enum", "func", "long", "null", "self", "true", "uint",
    "async", "await", "break", "catch", "class", "const", "false", "float", "short", "throw", "ulong", "while", "yield",
    "double", "object", "return", "string", "struct", "switch", "ushort",
    "continue",
    "interface"
};

Lexer *init_lexer(const char *input, const char *file) {
    Lexer *lex = (Lexer *)malloc(sizeof(Lexer));
    if (lex == NULL) error_hypex();
    if (input != NULL) {
        const size_t inputlen = strlen(input);
        lex->input = (char *)malloc(sizeof(char) * inputlen);
        if (lex->input == NULL) error_hypex();
        strcpy(lex->input, input);
        lex->inputlen = inputlen;
    } else {
        lex->input = NULL;
        lex->inputlen = 0;
    }
    if (file != NULL) {
        lex->file = (char *)malloc(sizeof(char) * (strlen(file) + 1));
        if (lex->file == NULL) error_hypex();
        strcpy(lex->file, file);
    } else {
        lex->file = NULL;
    }
    lex->tok_list = NULL;
    lex->len = 0;
    lex->offset = 0;
    lex->pos = (Pos){1, 1};
    lex->newline = false;
    lex->comment_eol = false;
    lex->potential_fstring = false;
    lex->potential_rstring = false;
    lex->fstring_body = false;
    lex->fstring_expr = false;
    return lex;
}

void consume_lex(Lexer *lex, Token *tok) {
    if (lex->tok_list == NULL) {
        lex->tok_list = (Token **)realloc(lex->tok_list, sizeof(Token));
        if (lex->tok_list == NULL) error_hypex();
        lex->len = 1;
        lex->tok_list[0] = tok;
    } else {
        lex->tok_list = (Token **)realloc(lex->tok_list, sizeof(Token) * (lex->len + 1));
        if (lex->tok_list == NULL) error_hypex();
        lex->len++;
        lex->tok_list[lex->len - 1] = tok;   
    }
}

static inline bool match_lex(const Lexer *lex) {
    return lex->offset < lex->inputlen;
}

// Overflow warning: Input length is not checked in this function.
static inline char current_lex(const Lexer *lex) {
    return lex->input[lex->offset];
}

// Overflow warning: Input length is not checked in this function.
static inline void advance_lex(Lexer *lex) {
    lex->offset++;
    lex->pos.column++;
}

// for platform-dependent line feeds
static inline bool is_eol(Lexer *lex) {
    if (current_lex(lex) == '\r') {
        advance_lex(lex);
        if (!match_lex(lex))
            return true;
        if (current_lex(lex) == '\n') //CRLF
            return true;
        return true; //CR
    }
    return current_lex(lex) == '\n'; //LF
}

static inline bool is_letter(const Lexer *lex) {
    return isalpha(current_lex(lex));
}

static inline bool is_number(const Lexer *lex) {
    return isdigit(current_lex(lex));
}

static inline bool is_exponent(const Token *buf, char c) {
    if (!buf->is_exponent) {
        if (buf->base == BASE_DEC && (c == 'e' || c == 'E')) return true;
        if (buf->base == BASE_HEX && (c == 'p' || c == 'P')) return true;
    }
    return false;
}

static inline bool is_ident_body(const Lexer *lex) {
    const char c = current_lex(lex);
    return isalnum(c) || c == '_';
}

static inline int match_keyword(const char *s) {
    const char **res = (const char **)bsearch(s, KWLIST, KWLEN, sizeof(KWLIST[0]), (int (*)(const void *, const void *))strcmp);
    return res ? (int)(res - KWLIST) : -1;
}

static int match_base(char c) {
    switch (c) {
        case 'x': return BASE_HEX;
        case 'o': return BASE_OCT;
        case 'b': return BASE_BIN;
        default: return BASE_DEC;
    }
}

static void lex_eol(Lexer *lex, Token *buf) {
    buf->type = EOL;
    buf->is_comment = false;
    lex->newline = true;
    advance_lex(lex);
}

static void lex_comment_eol(Lexer *lex, Token *buf) {
    consume_lex(lex, copy_token(buf));
    buf->value = NULL;
    buf->len = 0;
    consume_lex(lex, make_token(EOL, lex->pos));
    lex->tok_list[lex->len - 1]->is_comment = true;
    lex->pos.line++;
    lex->pos.column = 0;
    advance_lex(lex);
}

static void lex_comment_line(Lexer *lex, Token *buf) {
    buf->type = COMMENT_LINE;
    advance_lex(lex);
    while (match_lex(lex) && !is_eol(lex)) {
        consume_token(buf, current_lex(lex));
        advance_lex(lex);
    }
}

static void lex_comment_block(Lexer *lex, Token *buf) {
    buf->type = COMMENT_BLOCK;
    advance_lex(lex);
    while (match_lex(lex)) {
        if (is_eol(lex)) {
            lex_comment_eol(lex, buf);
            if (!match_lex(lex)) return;
        }
        if (current_lex(lex) == '*') {
            advance_lex(lex);
            if (!match_lex(lex)) return;
            if (current_lex(lex) == '/') {
                advance_lex(lex);
                return;
            }
            consume_token(buf, '*');
            if (is_eol(lex)) {
                lex_comment_eol(lex, buf);
                if (!match_lex(lex)) return;
            }
        }
        consume_token(buf, current_lex(lex));
        advance_lex(lex);
    }
}

static void lex_literal(Lexer *lex, Token *buf, char sepr) {
    bool escape = false;
    advance_lex(lex);
    while (match_lex(lex) && (current_lex(lex) != sepr || escape)) {
        if (escape)
            escape = false;
        if (current_lex(lex) == '\\')
            escape = true;
        consume_token(buf, current_lex(lex));
        advance_lex(lex);
    }
    advance_lex(lex);
}

static void lex_fstring_body(Lexer *lex, Token *buf) {
    buf->type = FSTRING_BODY;
    bool escape_fstring = false;
    bool fstring_end = false;
    while (match_lex(lex)) {
        if  (current_lex(lex) == '"' && !escape_fstring) {
            lex->fstring_body = false;
            fstring_end = true;
            break;
        }
        if (current_lex(lex) == '{') {
            lex->fstring_body = false;
            lex->fstring_expr = true;
            advance_lex(lex);
            break;
        }
        if (escape_fstring)
            escape_fstring = false;
        if (current_lex(lex) == '\\')
            escape_fstring = true;
        consume_token(buf, current_lex(lex));
        advance_lex(lex);
    }
    consume_lex(lex, buf);
    if (fstring_end) {
        fstring_end = false;
        consume_lex(lex, make_token(FSTRING_END, lex->pos));
        advance_lex(lex);
    }
}

static void lex_rstring(Lexer *lex) {
    Token *last = lex->tok_list[lex->len - 1];
    last->type = RSTRING;
    last->value = NULL;
    last->len = 0;
    advance_lex(lex);
    while (match_lex(lex) && current_lex(lex) != '"') {
        consume_token(last, current_lex(lex));
        advance_lex(lex);
    }
    advance_lex(lex);
}

static void lex_number(Lexer *lex, Token *buf) {
    buf->type = INTEGER;
    init_number(buf);
    consume_number(buf, current_lex(lex));
    advance_lex(lex);
    if (buf->value[0] == '0') {
        const int base = match_base(current_lex(lex));
        if (base != 0) {
            buf->base = base;
            consume_number(buf, current_lex(lex));
            advance_lex(lex);
        }
    }
    while (match_lex(lex)) {
        const char c = current_lex(lex);
        if (c == '_') {
            consume_token(buf, c);
            advance_lex(lex);
            continue;
        }
        if (buf->is_exponent) {
            if (isdigit(c)) {
                consume_number(buf, c);
                advance_lex(lex);
                continue;
            }
            if (!buf->is_negative && c == '-') {
                buf->is_negative = true;
                consume_number(buf, c);
                advance_lex(lex);
                continue;
            }
        }
        if (buf->type != FLOAT && c == '.') {
            buf->type = FLOAT;
            consume_number(buf, c);
            advance_lex(lex);
            continue;
        }
        if (is_exponent(buf, c)) {
            buf->is_exponent = true;
            consume_number(buf, c);
            advance_lex(lex);
            continue;
        }
        switch (buf->base) {
            case BASE_DEC:
                if (isdigit(c)) {
                    consume_number(buf, c);
                    advance_lex(lex);
                    continue;
                }
                break;
            case BASE_HEX:
                if (isxdigit(c)) {
                    consume_number(buf, c);
                    advance_lex(lex);
                    continue;
                }
                break;
            case BASE_OCT:
                if (c >= '0' && c <= '7') {
                    consume_number(buf, c);
                    advance_lex(lex);
                    continue;
                }
                break;
            case BASE_BIN:
                if (c == '0' || c == '1') {
                    consume_number(buf, c);
                    advance_lex(lex);
                    continue;
                }
                break;
            default:
                error_hypex();
                break;
        }
        break;
    }
    if (buf->type == FLOAT && buf->base == BASE_HEX && !buf->is_exponent) error_number(lex->file, lex->pos.line, lex->pos.column, buf->value);
    if (buf->value[buf->len - 1] == '_' || buf->value[buf->len - 1] == '.') error_number(lex->file, lex->pos.line, lex->pos.column, buf->value);
}

static void lex_ident(Lexer *lex, Token *buf) {
    buf->type = IDENT;
    consume_token(buf, current_lex(lex));
    advance_lex(lex);
    while (match_lex(lex) && is_ident_body(lex)) {
        consume_token(buf, current_lex(lex));
        advance_lex(lex);
    }
    const int id = match_keyword(buf->value);
    if (id != -1) {
        buf->type = KEYWORD;
        buf->value = NULL;
        buf->id = id;
    } else {
        if (strcmp(buf->value, "f") == 0)
            lex->potential_fstring = true;
        if (strcmp(buf->value, "r") == 0)
            lex->potential_rstring = true;
    }
}

void tokenize(Lexer *lex) {

    while (match_lex(lex)) {

        if (lex->newline) {
            lex->newline = false;
            lex->pos.line++;
            lex->pos.column = 1;
        }

        if (lex->potential_fstring) {
            lex->potential_fstring = false;
            if (current_lex(lex) == '"') {
                consume_lex(lex, make_token(FSTRING_START, lex->pos));
                lex->fstring_body = true;
                advance_lex(lex);
                continue;
            }
        }
        if (lex->potential_rstring) {
            lex->potential_rstring = false;
            if (current_lex(lex) == '"') {
                lex_rstring(lex);
                continue;
            }
        }

        Token *buf = make_token(UNKNOWN, lex->pos);

        if (lex->fstring_expr && current_lex(lex) == '}') {
            lex->fstring_expr = false;
            lex->fstring_body = true;
            advance_lex(lex);
            continue;
        }
        if (lex->fstring_body) {
            lex_fstring_body(lex, buf);
            continue;
        }

        switch (current_lex(lex)) {
            case '=':
                buf->type = EQUAL;
                buf->len++;
                advance_lex(lex);
                if (match_lex(lex)) {
                    switch (current_lex(lex)) {
                        case '=':
                            buf->type = TWO_EQ;
                            buf->len++;
                            advance_lex(lex);
                            break;
                    }
                }
                break;
            case '+':
                buf->type = PLUS;
                buf->len++;
                advance_lex(lex);
                if (match_lex(lex)) {
                    switch (current_lex(lex)) {
                        case '=':
                            buf->type = PLUS_EQ;
                            buf->len++;
                            advance_lex(lex);
                            break;
                        case '+':
                            buf->type = INCREASE;
                            buf->len++;
                            advance_lex(lex);
                            break;
                    }
                }
                break;
            case '-':
                buf->type = MINUS;
                buf->len++;
                advance_lex(lex);
                if (match_lex(lex)) {
                    switch (current_lex(lex)) {
                        case '=':
                            buf->type = MINUS_EQ;
                            buf->len++;
                            advance_lex(lex);
                            break;
                        case '-':
                            buf->type = DECREASE;
                            buf->len++;
                            advance_lex(lex);
                            break;
                    }
                }
                break;
            case '*':
                buf->type = STAR;
                buf->len++;
                advance_lex(lex);
                if (match_lex(lex)) {
                    switch (current_lex(lex)) {
                        case '=':
                            buf->type = STAR_EQ;
                            buf->len++;
                            advance_lex(lex);
                            break;
                    }
                }
                break;
            case '/':
                buf->type = SLASH;
                buf->len++;
                advance_lex(lex);
                if (match_lex(lex)) {
                    switch (current_lex(lex)) {
                        case '=':
                            buf->type = SLASH_EQ;
                            buf->len++;
                            advance_lex(lex);
                            break;
                        case '/':
                            lex_comment_line(lex, buf);
                            break;
                        case '*':
                            lex_comment_block(lex, buf);
                            break;
                    }
                }
                break;
            case '<':
                buf->type = LESS;
                buf->len++;
                advance_lex(lex);
                if (match_lex(lex)) {
                    switch (current_lex(lex)) {
                        case '=':
                            buf->type = LESS_EQ;
                            buf->len++;
                            advance_lex(lex);
                            break;
                        case '<':
                            buf->type = LSHIFT;
                            buf->len++;
                            advance_lex(lex);
                            if (match_lex(lex)) {
                                switch (current_lex(lex)) {
                                    case '=':
                                        buf->type = LSHIFT_EQ;
                                        buf->len++;
                                        advance_lex(lex);
                                        break;
                                }
                            }
                            break;
                    }
                }
                break;
            case '>':
                buf->type = GREATER;
                buf->len++;
                advance_lex(lex);
                if (match_lex(lex)) {
                    switch (current_lex(lex)) {
                        case '=':
                            buf->type = GREATER_EQ;
                            buf->len++;
                            advance_lex(lex);
                            break;
                        case '>':
                            buf->type = RSHIFT;
                            buf->len++;
                            advance_lex(lex);
                            if (match_lex(lex)) {
                                switch (current_lex(lex)) {
                                    case '=':
                                        buf->type = RSHIFT_EQ;
                                        buf->len++;
                                        advance_lex(lex);
                                        break;
                                }
                            }
                            break;
                    }
                }
                break;
            case '&':
                buf->type = AMPER;
                buf->len++;
                advance_lex(lex);
                if (match_lex(lex)) {
                    switch (current_lex(lex)) {
                        case '=':
                            buf->type = AMPER_EQ;
                            buf->len++;
                            advance_lex(lex);
                            break;
                        case '&':
                            buf->type = TWO_AMPER;
                            buf->len++;
                            advance_lex(lex);
                            break;
                    }
                }
                break;
            case '|':
                buf->type = PIPE;
                buf->len++;
                advance_lex(lex);
                if (match_lex(lex)) {
                    switch (current_lex(lex)) {
                        case '=':
                            buf->type = PIPE_EQ;
                            buf->len++;
                            advance_lex(lex);
                            break;
                        case '|':
                            buf->type = TWO_PIPE;
                            buf->len++;
                            advance_lex(lex);
                            break;
                    }
                }
                break;
            case '!':
                buf->type = EXCLAMATION;
                buf->len++;
                advance_lex(lex);
                if (match_lex(lex)) {
                    switch (current_lex(lex)) {
                        case '=':
                            buf->type = EXCLAMATION_EQ;
                            buf->len++;
                            advance_lex(lex);
                            break;
                    }
                }
                break;
            case '%':
                buf->type = PERCENT;
                buf->len++;
                advance_lex(lex);
                if (match_lex(lex)) {
                    switch (current_lex(lex)) {
                        case '=':
                            buf->type = PERCENT_EQ;
                            buf->len++;
                            advance_lex(lex);
                            break;
                    }
                }
                break;
            case '.':
                buf->type = DOT;
                buf->len++;
                advance_lex(lex);
                if (match_lex(lex)) {
                    switch (current_lex(lex)) {
                        case '.':
                            buf->type = TWO_DOT;
                            buf->len++;
                            advance_lex(lex);
                            if (match_lex(lex)) {
                                switch (current_lex(lex)) {
                                    case '.':
                                        buf->type = ELLIPSIS;
                                        buf->len++;
                                        advance_lex(lex);
                                        break;
                                }
                            }
                            break;
                    }
                }
                break;
            case ',':
                buf->type = COMMA;
                buf->len++;
                advance_lex(lex);
                break;
            case ':':
                buf->type = COLON;
                buf->len++;
                advance_lex(lex);
                break;
            case ';':
                buf->type = SEMI;
                buf->len++;
                advance_lex(lex);
                break;
            case '_':
                buf->type = UNDERSCORE;
                buf->len++;
                advance_lex(lex);
                break;
            case '?':
                buf->type = QUEST;
                buf->len++;
                advance_lex(lex);
                break;
            case '(':
                buf->type = LPAR;
                buf->len++;
                advance_lex(lex);
                break;
            case ')':
                buf->type = RPAR;
                buf->len++;
                advance_lex(lex);
                break;
            case '[':
                buf->type = LSQB;
                buf->len++;
                advance_lex(lex);
                break;
            case ']':
                buf->type = RSQB;
                buf->len++;
                advance_lex(lex);
                break;
            case '{':
                buf->type = LBRACE;
                buf->len++;
                advance_lex(lex);
                break;
            case '}':
                buf->type = RBRACE;
                buf->len++;
                advance_lex(lex);
                break;
            case '^':
                buf->type = CARET;
                buf->len++;
                advance_lex(lex);
                if (match_lex(lex)) {
                    switch (current_lex(lex)) {
                        case '=':
                            buf->type = CARET_EQ;
                            buf->len++;
                            advance_lex(lex);
                            break;
                    }
                }
                break;
            case '~':
                buf->type = TILDE;
                buf->len++;
                advance_lex(lex);
                break;
            case '@':
                buf->type = AT;
                buf->len++;
                advance_lex(lex);
                break;
            case '#':
                buf->type = HASH;
                buf->len++;
                advance_lex(lex);
                break;
            case ' ':
                buf->type = SPACE;
                buf->len++;
                advance_lex(lex);
                while (match_lex(lex) && current_lex(lex) == ' ') {
                    buf->len++;
                    advance_lex(lex);
                }
                break;
            case '\'':
                buf->type = CHAR;
                lex_literal(lex, buf, '\'');
                break;
            case '"':
                buf->type = STRING;
                lex_literal(lex, buf, '"');
                break;
            default:
                if (is_eol(lex))
                    lex_eol(lex, buf);
                else if (is_number(lex))
                    lex_number(lex, buf);
                else if (is_letter(lex))
                    lex_ident(lex, buf);
                else
                    error_char(lex->file, lex->pos.line, lex->pos.column, current_lex(lex));
                break;
        }
        consume_lex(lex, buf);
    }
}

void free_lex(Lexer *lex) {
    free(lex->input);
    free(lex->file);
    for (int i = 0; i < lex->len; i++)
        free_token(lex->tok_list[i]);
    free(lex->tok_list);
    free(lex);
}