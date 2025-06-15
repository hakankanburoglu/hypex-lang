#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"
#include "lexer.h"
#include "token.h"

#define KWLEN 41

const char *KWLIST[KWLEN] = {
    "async", "await", "base", "bool", "break", "case", "catch", "char", "class", "const",
    "continue", "double", "elif", "else", "enum", "false", "float", "for", "func", "if",
    "int", "interface", "long", "null", "object", "return", "self", "short", "string", "struct",
    "switch", "throw", "true", "try", "uint", "ulong", "use", "ushort", "var", "while", "yield"
};

enum {
    STATE_NONE,
    STATE_NEWL,
    STATE_BEGIN_F,
    STATE_BEGIN_R,
    STATE_F_BODY,
    STATE_F_EXPR
};

Lexer *init_lexer(const char *input, const char *file) {
    Lexer *lex = (Lexer *)malloc(sizeof(Lexer));
    if (!lex) error_hypex();
    if (input) {
        const size_t inputlen = strlen(input);
        lex->input = (char *)malloc(sizeof(char) * (inputlen + 1));
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
    lex->tokens.list = (Token **)malloc(sizeof(Token) * 16);
    lex->tokens.cap = 16;
    lex->tokens.len = 0;
    lex->indents.stack = (int *)malloc(sizeof(int));
    if (!lex->indents.stack) error_hypex();
    lex->indents.cap = 1;
    lex->indents.len = 1;
    lex->indents.stack[0] = 0;
    lex->offset = 0;
    lex->pos = (Pos){1, 1};
    lex->cur = lex->input ? lex->input[0] : '\0';
    lex->state = STATE_NONE;
    lex->indent = 0;
    return lex;
}

static inline void init_number(Token *tok) {
    tok->num.base = BASE_DEC;
    tok->num.is_exp = false;
    tok->num.is_neg = false;
}

static inline void push_token(Lexer *lex, Token *tok) {
    if (lex->tokens.len == lex->tokens.cap) {
        lex->tokens.cap *= 2;
        lex->tokens.list = (Token **)realloc(lex->tokens.list, sizeof(Token) * lex->tokens.cap);
        if (!lex->tokens.list) error_hypex();
    }
    lex->tokens.list[lex->tokens.len++] = tok;
}

static void write_token(Token *tok, char c) {
    if (!tok->value) {
        tok->cap = 16;
        tok->value = (char *)realloc(tok->value, sizeof(char) * 17);
        if (!tok->value) error_hypex();
    }
    if (tok->len == tok->cap) {
        tok->cap *= 2;
        tok->value = (char *)realloc(tok->value, sizeof(char) * (tok->cap + 1));
        if (!tok->value) error_hypex();
    }
    tok->value[tok->len++] = c;
    tok->value[tok->len] = '\0';
}

static inline void push_indent(Lexer *lex, int indent) {
    if (lex->indents.len == lex->indents.cap) {
        lex->indents.cap *= 2;
        lex->indents.stack = (int *)realloc(lex->indents.stack, lex->indents.cap * sizeof(int));
        if (!lex->indents.stack) error_hypex();
    }
    lex->indents.stack[lex->indents.len++] = indent;
}

static inline void pop_indent(Lexer *lex) {
    if (lex->indents.len == 0) error_hypex();
    (lex->indents.len)--;
}

static inline int peek_indent(const Lexer *lex) {
    if (lex->indents.len == 0) error_hypex();
    return lex->indents.stack[lex->indents.len - 1];
}

static inline void clear_indent(Lexer *lex) {
    lex->indents.len = 1;
}

static inline bool empty_indent(const Lexer *lex) {
    return lex->indents.len == 1;
}

static inline Token *peek_token(const Lexer *lex) {
    return lex->tokens.list[lex->tokens.len - 1];
}

static inline bool has_next(const Lexer *lex) {
    return lex->offset < lex->inputlen;
}

static inline void next(Lexer *lex) {
    lex->cur = lex->input[++lex->offset];
    lex->pos.column++;
}

static inline void consume(Lexer *lex, Token *tok) {
    write_token(tok, lex->cur);
    next(lex);
}

static inline void error_lexer(const Lexer *lex, const char *format, ...) {
    va_list args;
    va_start(args, format);
    error(lex->file, lex->pos.line, lex->pos.column, format, args);
    va_end(args);
}

static inline bool is_number(const Lexer *lex) {
    return isdigit(lex->cur);
}

static inline bool is_letter(const Lexer *lex) {
    return isalpha(lex->cur);
}

static inline bool is_exponent(const Token *tok, char c) {
    if (!tok->num.is_exp) {
        if (tok->num.base == BASE_DEC && (c == 'e' || c == 'E')) return true;
        return tok->num.base == BASE_HEX && (c == 'p' || c == 'P');
    }
    return false;
}

static inline bool is_ident(const Lexer *lex) {
    return isalnum(lex->cur) || lex->cur == '_';
}

static inline bool is_newline(Lexer *lex) {
    if (lex->cur == '\r') {
        next(lex);
        return true;
    }
    return lex->cur == '\n';
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

static inline int compare_keyword(const void *s1, const void *s2) {
    return strcmp(*(const char **)s1, *(const char **)s2);
}

static inline int match_keyword(const char *s) {
    const char **res = (const char **)bsearch(&s, KWLIST, KWLEN, sizeof(KWLIST[0]), compare_keyword);
    return res ? (int)(res - KWLIST) : -1;
}

static inline void lex_operator(Lexer *lex, Token *tok, int kind) {
    tok->kind = kind;
    tok->len++;
    next(lex);
}

static void lex_number(Lexer *lex, Token *tok) {
    tok->kind = T_INTEGER;
    init_number(tok);
    consume(lex, tok);
    if (tok->value[0] == '0') {
        const int base = match_base(lex->cur);
        if (base != 0) {
            tok->num.base = base;
            consume(lex, tok);
        }
    }
    while (has_next(lex)) {
        if (lex->cur == '_') {
            next(lex);
            continue;
        }
        if (tok->num.is_exp) {
            if (isdigit(lex->cur)) {
                consume(lex, tok);
                continue;
            }
            if (!tok->num.is_neg && lex->cur == '-') {
                tok->num.is_neg = true;
                consume(lex, tok);
                continue;
            }
        }
        if (tok->kind != T_FLOAT && lex->cur == '.') {
            tok->kind = T_FLOAT;
            consume(lex, tok);
            continue;
        }
        if (is_exponent(tok, lex->cur)) {
            tok->num.is_exp = true;
            consume(lex, tok);
            continue;
        }
        switch (tok->num.base) {
            case BASE_DEC:
                if (isdigit(lex->cur)) {
                    consume(lex, tok);
                    continue;
                }
                break;
            case BASE_HEX:
                if (isxdigit(lex->cur)) {
                    consume(lex, tok);
                    continue;
                }
                break;
            case BASE_OCT:
                if (lex->cur >= '0' && lex->cur <= '7') {
                    consume(lex, tok);
                    continue;
                }
                break;
            case BASE_BIN:
                if (lex->cur == '0' || lex->cur == '1') {
                    consume(lex, tok);
                    continue;
                }
                break;
            default:
                error_hypex();
                break;
        }
        break;
    }
    if ( (tok->kind == T_FLOAT && tok->num.base == BASE_HEX && !tok->num.is_exp)
        || (tok->value[tok->len - 1] == '_' || tok->value[tok->len - 1] == '.'))
        error_lexer(lex, "invalid number literal: %s", tok->value);
}

static void lex_ident(Lexer *lex, Token *tok) {
    tok->kind = T_IDENT;
    consume(lex, tok);
    while (is_ident(lex)) consume(lex, tok);
    const int id = match_keyword(tok->value);
    if (id != -1) {
        tok->kind = T_KEYWORD;
        free(tok->value);
        tok->value = NULL;
        tok->id = id;
    } else {
        if (strcmp(tok->value, "f") == 0)
            lex->state = STATE_BEGIN_F;
        if (strcmp(tok->value, "r") == 0)
            lex->state = STATE_BEGIN_R;
    }
}

static void lex_comment_newline(Lexer *lex, Token *tok) {
    push_token(lex, copy_token(tok));
    tok->value = NULL;
    tok->len = 0;
    push_token(lex, make_token(T_NEWLINE, lex->pos));
    lex->tokens.list[lex->tokens.len - 1]->comment = true;
    lex->pos.line++;
    lex->pos.column = 0;
    next(lex);
}

static void lex_comment_line(Lexer *lex, Token *tok) {
    tok->kind = T_COMMENT_LINE;
    next(lex);
    while (has_next(lex) && !is_newline(lex)) consume(lex, tok);
}

static void lex_comment_block(Lexer *lex, Token *tok) {
    tok->kind = T_COMMENT_BLOCK;
    next(lex);
    while (has_next(lex)) {
        if (is_newline(lex)) {
            lex_comment_newline(lex, tok);
            if (!has_next(lex)) return;
        }
        if (lex->cur == '*') {
            next(lex);
            if (!has_next(lex)) return;
            if (lex->cur == '/') {
                next(lex);
                return;
            }
            write_token(tok, '*');
            if (is_newline(lex)) {
                lex_comment_newline(lex, tok);
                if (!has_next(lex)) return;
            }
        }
        consume(lex, tok);
    }
}

static void lex_literal(Lexer *lex, Token *tok, char delim) {
    bool escape = false;
    next(lex);
    while (has_next(lex) && (lex->cur != delim || escape)) {
        if (!escape && lex->cur == '\n')
            delim == '"' ? error_lexer(lex, "expected '\"'") : error_lexer(lex, "expected \"'\"");
        if (escape)
            escape = false;
        if (lex->cur == '\\')
            escape = true;
        consume(lex, tok);
    }
    next(lex);
}

static void lex_fstring_body(Lexer *lex, Token *tok) {
    tok->kind = T_FSTRING_BODY;
    bool escape = false;
    bool done = false;
    while (has_next(lex)) {
        if  (lex->cur == '"' && !escape) {
            lex->state = STATE_NONE;
            done = true;
            break;
        }
        if (lex->cur == '{') {
            lex->state = STATE_F_EXPR;
            next(lex);
            break;
        }
        if (escape)
            escape = false;
        if (lex->cur == '\\')
            escape = true;
        consume(lex, tok);
    }
    push_token(lex, tok);
    if (done) {
        push_token(lex, make_token(T_FSTRING_END, lex->pos));
        next(lex);
    }
}

static void lex_rstring(Lexer *lex) {
    Token *last = lex->tokens.list[lex->tokens.len - 1];
    last->kind = T_RSTRING;
    last->value = NULL;
    last->len = 0;
    next(lex);
    while (has_next(lex) && lex->cur != '"') consume(lex, last);
    next(lex);
}

static void lex_indent(Lexer *lex, Token *tok) {
    int indent = tok->len - lex->indent;
    tok->kind = T_INDENT;
    if (indent == 0) {
        tok->level = 0;
        return;
    }
    if (indent > 0) {
        tok->level = 1;
        push_indent(lex, indent);
        lex->indent += indent;
        return;
    }
    if (indent < 0) {
        tok->kind = T_DEDENT;
        size_t len = tok->len;
        for (int i = lex->indents.len - 1; i >= 0; i--) {
            if (len == 0) break;
            if (len < lex->indents.stack[i]) error_lexer(lex, "invalid indentation");
            len -= lex->indents.stack[i];
            pop_indent(lex);
            tok->level++;
            lex->indent--;
        }
    }
}

static inline void lex_newline(Lexer *lex, Token *tok) {
    tok->kind = T_NEWLINE;
    tok->comment = false;
    lex->state = STATE_NEWL;
    next(lex);
}

static inline void lex_invalid(Lexer *lex) {
    error_lexer(lex, "invalid character: '%c' (0x%x)", lex->cur, lex->cur);
    next(lex);
}

void tokenize(Lexer *lex) {
    while (has_next(lex)) {
        if (lex->state == STATE_NEWL) {
            lex->state = STATE_NONE;
            lex->pos.line++;
            lex->pos.column = 1;
            if (lex->cur != ' ' && !empty_indent(lex)) {
                push_token(lex, make_token(T_DEDENT, lex->pos));
                lex->tokens.list[lex->tokens.len - 1]->level = -1;
                clear_indent(lex);
                next(lex);
            }
        }
        if (lex->state == STATE_BEGIN_F) {
            lex->state = STATE_NONE;
            if (lex->cur == '"') {
                push_token(lex, make_token(T_FSTRING_START, lex->pos));
                lex->state = STATE_F_BODY;
                next(lex);
                continue;
            }
        }
        if (lex->state == STATE_BEGIN_R) {
            lex->state = STATE_NONE;
            if (lex->cur == '"') {
                lex_rstring(lex);
                continue;
            }
        }
        Token *tok = make_token(T_UNKNOWN, lex->pos);
        if (lex->state == STATE_F_EXPR && lex->cur == '}') {
            lex->state = STATE_F_BODY;
            next(lex);
            continue;
        }
        if (lex->state == STATE_F_BODY) {
            lex_fstring_body(lex, tok);
            continue;
        }
        switch (lex->cur) {
            case '=':
                lex_operator(lex, tok, T_EQUAL);
                if (lex->cur == '=')
                    lex_operator(lex, tok, T_TWO_EQ);
                break;
            case '+':
                lex_operator(lex, tok, T_PLUS);
                switch (lex->cur) {
                    case '=':
                        lex_operator(lex, tok, T_PLUS_EQ);
                        break;
                    case '+':
                        lex_operator(lex, tok, T_INCREASE);
                        break;
                }
                break;
            case '-':
                lex_operator(lex, tok, T_MINUS);
                switch (lex->cur) {
                    case '=':
                        lex_operator(lex, tok, T_MINUS_EQ);
                        break;
                    case '-':
                        lex_operator(lex, tok, T_DECREASE);
                        break;
                }
                break;
            case '*':
                lex_operator(lex, tok, T_STAR);
                if (lex->cur == '=')
                    lex_operator(lex, tok, T_STAR_EQ);
                break;
            case '/':
                lex_operator(lex, tok, T_SLASH);
                switch (lex->cur) {
                    case '=':
                        lex_operator(lex, tok, T_SLASH_EQ);
                        break;
                    case '/':
                        lex_comment_line(lex, tok);
                        break;
                    case '*':
                        lex_comment_block(lex, tok);
                        break;
                }
                break;
            case '<':
                lex_operator(lex, tok, T_LESS);
                switch (lex->cur) {
                    case '=':
                        lex_operator(lex, tok, T_LESS_EQ);
                        break;
                    case '<':
                        lex_operator(lex, tok, T_LSHIFT);
                        if (lex->cur == '=')
                            lex_operator(lex, tok, T_LSHIFT_EQ);
                        break;
                }
                break;
            case '>':
                lex_operator(lex, tok, T_GREATER);
                switch (lex->cur) {
                    case '=':
                        lex_operator(lex, tok, T_GREATER_EQ);
                        break;
                    case '>':
                        lex_operator(lex, tok, T_RSHIFT);
                        if (lex->cur == '=')
                            lex_operator(lex, tok, T_RSHIFT_EQ);
                        break;
                }
                break;
            case '&':
                lex_operator(lex, tok, T_AMPER);
                switch (lex->cur) {
                    case '=':
                        lex_operator(lex, tok, T_AMPER_EQ);
                        break;
                    case '&':
                        lex_operator(lex, tok, T_TWO_AMPER);
                        break;
                }
                break;
            case '|':
                lex_operator(lex, tok, T_PIPE);
                switch (lex->cur) {
                    case '=':
                        lex_operator(lex, tok, T_PIPE_EQ);
                        break;
                    case '|':
                        lex_operator(lex, tok, T_TWO_PIPE);
                        break;
                }
                break;
            case '!':
                lex_operator(lex, tok, T_EXCLAMATION);
                if (lex->cur == '=')
                    lex_operator(lex, tok, T_EXCLAMATION_EQ);
                break;
            case '%':
                lex_operator(lex, tok, T_PERCENT);
                if (lex->cur == '=')
                    lex_operator(lex, tok, T_PERCENT_EQ);
                break;
            case '.':
                lex_operator(lex, tok, T_DOT);
                if (lex->cur == '.') {
                    lex_operator(lex, tok, T_TWO_DOT);
                    if (lex->cur == '.')
                        lex_operator(lex, tok, T_ELLIPSIS);
                }
                break;
            case ',':
                lex_operator(lex, tok, T_COMMA);
                break;
            case ':':
                lex_operator(lex, tok, T_COLON);
                break;
            case ';':
                lex_operator(lex, tok, T_SEMI);
                break;
            case '_':
                lex_operator(lex, tok, T_UNDERSCORE);
                if (is_ident(lex)) {
                    write_token(tok, '_');
                    lex_ident(lex, tok);
                }
                break;
            case '?':
                lex_operator(lex, tok, T_QUEST);
                break;
            case '(':
                lex_operator(lex, tok, T_LPAR);
                break;
            case ')':
                lex_operator(lex, tok, T_RPAR);
                break;
            case '[':
                lex_operator(lex, tok, T_LSQB);
                break;
            case ']':
                lex_operator(lex, tok, T_RSQB);
                break;
            case '{':
                lex_operator(lex, tok, T_LBRACE);
                break;
            case '}':
                lex_operator(lex, tok, T_RBRACE);
                break;
            case '^':
                lex_operator(lex, tok, T_CARET);
                if (lex->cur == '=')
                    lex_operator(lex, tok, T_CARET_EQ);
                break;
            case '~':
                lex_operator(lex, tok, T_TILDE);
                break;
            case '@':
                lex_operator(lex, tok, T_AT);
                break;
            case '#': 
                lex_operator(lex, tok, T_HASH);
                break;
            case '\\':
                lex_operator(lex, tok, T_ESCAPE);
                break;
            case ' ':
                lex_operator(lex, tok, T_SPACE);
                while (lex->cur == ' ') {
                    tok->len++;
                    next(lex);
                }
                if (peek_token(lex)->kind == T_NEWLINE)
                    lex_indent(lex, tok);
                break;
            case '\'':
                tok->kind = T_CHAR;
                lex_literal(lex, tok, '\'');
                break;
            case '"':
                tok->kind = T_STRING;
                lex_literal(lex, tok, '"');
                break;
            default:
                if (is_newline(lex))
                    lex_newline(lex, tok);
                else if (is_number(lex))
                    lex_number(lex, tok);
                else if (is_letter(lex))
                    lex_ident(lex, tok);
                else
                    lex_invalid(lex);
                break;
        }
        push_token(lex, tok);
    }
}

void free_lex(Lexer *lex) {
    free(lex->input);
    free(lex->file);
    for (int i = 0; i < lex->tokens.len; i++)
        free_token(lex->tokens.list[i]);
    free(lex->tokens.list);
    free(lex->indents.stack);
    free(lex);
}
