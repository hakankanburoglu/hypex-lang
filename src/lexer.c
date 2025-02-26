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
    "async", "await", "base", "bool", "break", "case", "catch", "char", "class", "const",
    "continue", "double", "elif", "else", "enum", "false", "float", "for", "func", "if",
    "int", "interface", "long", "null", "object", "return", "self", "short", "string", "struct",
    "switch", "throw", "true", "try", "uint", "ulong", "use", "ushort", "var", "while", "yield"
};

Lexer *init_lexer(const char *input, const char *file) {
    Lexer *lex = (Lexer *)malloc(sizeof(Lexer));
    if (!lex) error_hypex();
    if (input) {
        const size_t inputlen = strlen(input);
        lex->input = (char *)malloc(sizeof(char) * inputlen);
        if (!lex->input) error_hypex();
        strcpy(lex->input, input);
        lex->inputlen = inputlen;
    } else {
        lex->input = NULL;
        lex->inputlen = 0;
    }
    if (file) {
        lex->file = (char *)malloc(sizeof(char) * (strlen(file) + 1));
        if (!lex->file) error_hypex();
        strcpy(lex->file, file);
    } else {
        lex->file = NULL;
    }
    lex->indent_stack = (int *)malloc(sizeof(int));
    if (!lex->indent_stack) error_hypex();
    lex->stack_len = 1;
    lex->stack_capacity = 1;
    lex->indent_stack[0] = 0;
    lex->indent = 0;
    lex->tok_list = NULL;
    lex->len = 0;
    lex->offset = 0;
    lex->pos = (Pos){1, 1};
    lex->newline = false;
    lex->potential_fstring = false;
    lex->potential_rstring = false;
    lex->fstring_body = false;
    lex->fstring_expr = false;
    return lex;
}

static inline void consume_lex(Lexer *lex, Token *tok) {
    if (!lex->tok_list) {
        lex->tok_list = (Token **)realloc(lex->tok_list, sizeof(Token));
        if (!lex->tok_list) error_hypex();
        lex->len = 1;
        lex->tok_list[0] = tok;
    } else {
        lex->tok_list = (Token **)realloc(lex->tok_list, sizeof(Token) * (lex->len + 1));
        if (!lex->tok_list) error_hypex();
        lex->len++;
        lex->tok_list[lex->len - 1] = tok;   
    }
}

static inline void push_indent(Lexer *lex, int indent) {
    if (lex->stack_len == lex->stack_capacity) {
        lex->stack_capacity *= 2;
        lex->indent_stack = (int *)realloc(lex->indent_stack, lex->stack_capacity * sizeof(int));
        if (!lex->indent_stack) error_hypex();
    }
    lex->indent_stack[lex->stack_len++] = indent;
}

static inline void pop_indent(Lexer *lex) {
    if (lex->stack_len == 0) error_hypex();
    (lex->stack_len)--;
}

static inline int peek_indent(const Lexer *lex) {
    if (lex->stack_len == 0) error_hypex();
    return lex->indent_stack[lex->stack_len - 1];
}

static inline void clear_indent(Lexer *lex) {
    lex->stack_len = 1;
    lex->stack_capacity = 1;
}

static inline bool empty_indent(const Lexer *lex) {
    return lex->stack_len == 1;
}

static inline Token *last_token(const Lexer *lex) {
    return lex->tok_list[lex->len - 1];
}

static inline bool match_lex(const Lexer *lex) {
    return lex->offset < lex->inputlen;
}

// overflow warning: input length is not checked in this function
static inline char current_lex(const Lexer *lex) {
    return lex->input[lex->offset];
}

// overflow warning: input length is not checked in this function
static inline void advance_lex(Lexer *lex) {
    lex->offset++;
    lex->pos.column++;
}

static inline bool is_number(const Lexer *lex) {
    return isdigit(current_lex(lex));
}

static inline bool is_letter(const Lexer *lex) {
    return isalpha(current_lex(lex));
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

static int match_base(char c) {
    switch (c) {
        case 'x': case 'X':
            return BASE_HEX;
        case 'o': case 'O':
            return BASE_OCT;
        case 'b': case 'B': 
            return BASE_BIN;
        default:
            return BASE_DEC;
    }
}

static inline int compare_keywords(const void *a, const void *b) {
    return strcmp(*(const char **)a, *(const char **)b);
}

static inline int match_keyword(const char *s) {
    const char **res = (const char **)bsearch(&s, KWLIST, KWLEN, sizeof(KWLIST[0]), compare_keywords);
    return res ? (int)(res - KWLIST) : -1;
}

static inline void lex_operator(Lexer *lex, Token *buf, int kind) {
    buf->kind = kind;
    buf->len++;
    advance_lex(lex);
}

static inline void lex_digit(Lexer *lex, Token *buf, char c) {
    consume_number(buf, c);
    advance_lex(lex);
}

static void lex_number(Lexer *lex, Token *buf) {
    buf->kind = T_INTEGER;
    init_number(buf);
    lex_digit(lex, buf, current_lex(lex));
    if (buf->value[0] == '0') {
        const int base = match_base(current_lex(lex));
        if (base != 0) {
            buf->base = base;
            lex_digit(lex, buf, current_lex(lex));
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
                lex_digit(lex, buf, c);
                continue;
            }
            if (!buf->is_negative && c == '-') {
                buf->is_negative = true;
                lex_digit(lex, buf, c);
                continue;
            }
        }
        if (buf->kind != T_FLOAT && c == '.') {
            buf->kind = T_FLOAT;
            lex_digit(lex, buf, c);
            continue;
        }
        if (is_exponent(buf, c)) {
            buf->is_exponent = true;
            lex_digit(lex, buf, c);
            continue;
        }
        switch (buf->base) {
            case BASE_DEC:
                if (isdigit(c)) {
                    lex_digit(lex, buf, c);
                    continue;
                }
                break;
            case BASE_HEX:
                if (isxdigit(c)) {
                    lex_digit(lex, buf, c);
                    continue;
                }
                break;
            case BASE_OCT:
                if (c >= '0' && c <= '7') {
                    lex_digit(lex, buf, c);
                    continue;
                }
                break;
            case BASE_BIN:
                if (c == '0' || c == '1') {
                    lex_digit(lex, buf, c);
                    continue;
                }
                break;
            default:
                error_hypex();
                break;
        }
        break;
    }
    if ( (buf->kind == T_FLOAT && buf->base == BASE_HEX && !buf->is_exponent)
        || (buf->value[buf->len - 1] == '_' || buf->value[buf->len - 1] == '.'))
        error_number(lex->file, lex->pos.line, lex->pos.column, buf->value);
}

static void lex_ident(Lexer *lex, Token *buf) {
    buf->kind = T_IDENT;
    consume_token(buf, current_lex(lex));
    advance_lex(lex);
    while (match_lex(lex) && is_ident_body(lex)) {
        consume_token(buf, current_lex(lex));
        advance_lex(lex);
    }
    const int id = match_keyword(buf->value);
    if (id != -1) {
        buf->kind = T_KEYWORD;
        free(buf->value);
        buf->value = NULL;
        buf->id = id;
    } else {
        if (strcmp(buf->value, "f") == 0)
            lex->potential_fstring = true;
        if (strcmp(buf->value, "r") == 0)
            lex->potential_rstring = true;
    }
}

static void lex_comment_eol(Lexer *lex, Token *buf) {
    consume_lex(lex, copy_token(buf));
    buf->value = NULL;
    buf->len = 0;
    consume_lex(lex, make_token(T_EOL, lex->pos));
    lex->tok_list[lex->len - 1]->is_comment = true;
    lex->pos.line++;
    lex->pos.column = 0;
    advance_lex(lex);
}

static void lex_comment_line(Lexer *lex, Token *buf) {
    buf->kind = T_COMMENT_LINE;
    advance_lex(lex);
    while (match_lex(lex) && !is_eol(lex)) {
        consume_token(buf, current_lex(lex));
        advance_lex(lex);
    }
}

static void lex_comment_block(Lexer *lex, Token *buf) {
    buf->kind = T_COMMENT_BLOCK;
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
    buf->kind = T_FSTRING_BODY;
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
        consume_lex(lex, make_token(T_FSTRING_END, lex->pos));
        advance_lex(lex);
    }
}

static void lex_rstring(Lexer *lex) {
    Token *last = lex->tok_list[lex->len - 1];
    last->kind = T_RSTRING;
    last->value = NULL;
    last->len = 0;
    advance_lex(lex);
    while (match_lex(lex) && current_lex(lex) != '"') {
        consume_token(last, current_lex(lex));
        advance_lex(lex);
    }
    advance_lex(lex);
}

static void lex_indent(Lexer *lex, Token *buf) {
    int indent = buf->len - lex->indent;
    buf->kind = T_INDENT;
    if (indent == 0) {
        buf->level++;
        return;
    }
    if (indent > 0) {
        buf->level = 1;
        push_indent(lex, indent);
        lex->indent += indent;
        return;
    }
    if (indent < 0) {
        buf->kind = T_DEDENT;
        size_t buflen = buf->len;
        for (int i = lex->stack_len - 1; i >= 0; i--) {
            if (buflen == 0) break;
            if (buflen < lex->indent_stack[i]) error_indent(lex->file, lex->pos.line, lex->pos.column);
            buflen -= lex->indent_stack[i];
            pop_indent(lex);
            buf->level++;
            lex->indent--;
        }
        return;
    }
}

static inline void lex_eol(Lexer *lex, Token *buf) {
    buf->kind = T_EOL;
    buf->is_comment = false;
    lex->newline = true;
    advance_lex(lex);
}

static inline void lex_invalid(Lexer *lex) {
    error_char(lex->file, lex->pos.line, lex->pos.column, current_lex(lex));
    advance_lex(lex);
}

void tokenize(Lexer *lex) {

    while (match_lex(lex)) {

        if (lex->newline) {
            lex->newline = false;
            lex->pos.line++;
            lex->pos.column = 1;
            if (current_lex(lex) != ' ' && !empty_indent(lex)) {
                consume_lex(lex, make_token(T_DEDENT, lex->pos));
                lex->tok_list[lex->len - 1]->level = -1;
                clear_indent(lex);
                advance_lex(lex);
            }
        }

        if (lex->potential_fstring) {
            lex->potential_fstring = false;
            if (current_lex(lex) == '"') {
                consume_lex(lex, make_token(T_FSTRING_START, lex->pos));
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

        Token *buf = make_token(T_UNKNOWN, lex->pos);

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
                lex_operator(lex, buf, T_EQUAL);
                if (match_lex(lex) && current_lex(lex) == '=')
                    lex_operator(lex, buf, T_TWO_EQ);
                break;
            case '+':
                lex_operator(lex, buf, T_PLUS);
                if (match_lex(lex)) {
                    switch (current_lex(lex)) {
                        case '=':
                            lex_operator(lex, buf, T_PLUS_EQ);
                            break;
                        case '+':
                            lex_operator(lex, buf, T_INCREASE);
                            break;
                    }
                }
                break;
            case '-':
                lex_operator(lex, buf, T_MINUS);
                if (match_lex(lex)) {
                    switch (current_lex(lex)) {
                        case '=':
                            lex_operator(lex, buf, T_MINUS_EQ);
                            break;
                        case '-':
                            lex_operator(lex, buf, T_DECREASE);
                            break;
                    }
                }
                break;
            case '*':
                lex_operator(lex, buf, T_STAR);
                if (match_lex(lex) && current_lex(lex) == '=')
                    lex_operator(lex, buf, T_STAR_EQ);
                break;
            case '/':
                lex_operator(lex, buf, T_SLASH);
                if (match_lex(lex)) {
                    switch (current_lex(lex)) {
                        case '=':
                            lex_operator(lex, buf, T_SLASH_EQ);
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
                lex_operator(lex, buf, T_LESS);
                if (match_lex(lex)) {
                    switch (current_lex(lex)) {
                        case '=':
                            lex_operator(lex, buf, T_LESS_EQ);
                            break;
                        case '<':
                            lex_operator(lex, buf, T_LSHIFT);
                            if (match_lex(lex) && current_lex(lex) == '=')
                                lex_operator(lex, buf, T_LSHIFT_EQ);
                            break;
                    }
                }
                break;
            case '>':
                lex_operator(lex, buf, T_GREATER);
                if (match_lex(lex)) {
                    switch (current_lex(lex)) {
                        case '=':
                            lex_operator(lex, buf, T_GREATER_EQ);
                            break;
                        case '>':
                            lex_operator(lex, buf, T_RSHIFT);
                            if (match_lex(lex) && current_lex(lex) == '=')
                                lex_operator(lex, buf, T_RSHIFT_EQ);
                            break;
                    }
                }
                break;
            case '&':
                lex_operator(lex, buf, T_AMPER);
                if (match_lex(lex)) {
                    switch (current_lex(lex)) {
                        case '=':
                            lex_operator(lex, buf, T_AMPER_EQ);
                            break;
                        case '&':
                            lex_operator(lex, buf, T_TWO_AMPER);
                            break;
                    }
                }
                break;
            case '|':
                lex_operator(lex, buf, T_PIPE);
                if (match_lex(lex)) {
                    switch (current_lex(lex)) {
                        case '=':
                            lex_operator(lex, buf, T_PIPE_EQ);
                            break;
                        case '|':
                            lex_operator(lex, buf, T_TWO_PIPE);
                            break;
                    }
                }
                break;
            case '!':
                lex_operator(lex, buf, T_EXCLAMATION);
                if (match_lex(lex) && current_lex(lex) == '=')
                    lex_operator(lex, buf, T_EXCLAMATION_EQ);
                break;
            case '%':
                lex_operator(lex, buf, T_PERCENT);
                if (match_lex(lex) && current_lex(lex) == '=')
                    lex_operator(lex, buf, T_PERCENT_EQ);
                break;
            case '.':
                lex_operator(lex, buf, T_DOT);
                if (match_lex(lex) && current_lex(lex) == '.') {
                    lex_operator(lex, buf, T_TWO_DOT);
                    if (match_lex(lex) && current_lex(lex) == '.')
                        lex_operator(lex, buf, T_ELLIPSIS);
                }
                break;
            case ',':
                lex_operator(lex, buf, T_COMMA);
                break;
            case ':':
                lex_operator(lex, buf, T_COLON);
                break;
            case ';':
                lex_operator(lex, buf, T_SEMI);
                break;
            case '_':
                lex_operator(lex, buf, T_UNDERSCORE);
                if (match_lex(lex) && is_ident_body(lex)) {
                    consume_token(buf, '_');
                    lex_ident(lex, buf);
                }
                break;
            case '?':
                lex_operator(lex, buf, T_QUEST);
                break;
            case '(':
                lex_operator(lex, buf, T_LPAR);
                break;
            case ')':
                lex_operator(lex, buf, T_RPAR);
                break;
            case '[':
                lex_operator(lex, buf, T_LSQB);
                break;
            case ']':
                lex_operator(lex, buf, T_RSQB);
                break;
            case '{':
                lex_operator(lex, buf, T_LBRACE);
                break;
            case '}':
                lex_operator(lex, buf, T_RBRACE);
                break;
            case '^':
                lex_operator(lex, buf, T_CARET);
                if (match_lex(lex) && current_lex(lex) == '=')
                    lex_operator(lex, buf, T_CARET_EQ);
                break;
            case '~':
                lex_operator(lex, buf, T_TILDE);
                break;
            case '@':
                lex_operator(lex, buf, T_AT);
                break;
            case '#': 
                lex_operator(lex, buf, T_HASH);
                break;
            case '\\':
                lex_operator(lex, buf, T_ESCAPE);
                break;
            case ' ':
                lex_operator(lex, buf, T_SPACE);
                while (match_lex(lex) && current_lex(lex) == ' ') {
                    buf->len++;
                    advance_lex(lex);
                }
                if (last_token(lex)->kind == T_EOL)
                    lex_indent(lex, buf);
                break;
            case '\'':
                buf->kind = T_CHAR;
                lex_literal(lex, buf, '\'');
                break;
            case '"':
                buf->kind = T_STRING;
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
                    lex_invalid(lex);
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
    free(lex->indent_stack);
    free(lex);
}