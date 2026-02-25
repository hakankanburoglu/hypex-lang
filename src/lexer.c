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

hypex_lexer *hypex_lexer_make(void) {
    hypex_lexer *lex = malloc(sizeof *lex);
    if (!lex) hypex_internal_error();
    lex->input = NULL;
    lex->inputlen = 0;
    lex->file = NULL;
    lex->tokens.list = malloc(sizeof *lex->tokens.list * 16);
    if (!lex->tokens.list) hypex_internal_error();
    lex->tokens.cap = 16;
    lex->tokens.len = 0;
    lex->indents.stack = malloc(sizeof *lex->indents.stack);
    if (!lex->indents.stack) hypex_internal_error();
    lex->indents.cap = 1;
    lex->indents.len = 1;
    lex->indents.stack[0] = 0;
    lex->offset = 0;
    lex->pos = (hypex_pos){1, 1};
    lex->cur = '\0';
    lex->state = STATE_NONE;
    return lex;
}

static inline void init_number(hypex_token *tok) {
    tok->num.base = HYPEX_NUM_BASE_DEC;
    tok->num.is_exp = false;
    tok->num.is_neg = false;
}

static inline void push_token(hypex_lexer *lex, hypex_token *tok) {
    if (lex->tokens.len == lex->tokens.cap) {
        lex->tokens.cap *= 2;
        lex->tokens.list = realloc(lex->tokens.list, sizeof *lex->tokens.list * lex->tokens.cap);
        if (!lex->tokens.list) hypex_internal_error();
    }
    lex->tokens.list[lex->tokens.len++] = tok;
}

static void write_token(hypex_token *tok, char c) {
    if (!tok->value) {
        tok->cap = 16;
        tok->value = realloc(tok->value, sizeof *tok->value * 17);
        if (!tok->value) hypex_internal_error();
    }
    if (tok->len == tok->cap) {
        tok->cap *= 2;
        tok->value = realloc(tok->value, sizeof *tok->value * (tok->cap + 1));
        if (!tok->value) hypex_internal_error();
    }
    tok->value[tok->len++] = c;
    tok->value[tok->len] = '\0';
}

static inline void push_indent(hypex_lexer *lex, int indent) {
    if (lex->indents.len == lex->indents.cap) {
        lex->indents.cap *= 2;
        lex->indents.stack = realloc(lex->indents.stack, sizeof *lex->indents.stack * lex->indents.cap);
        if (!lex->indents.stack) hypex_internal_error();
    }
    lex->indents.stack[lex->indents.len++] = indent;
}

static inline void pop_indent(hypex_lexer *lex) {
    if (lex->indents.len == 0) hypex_internal_error();
    (lex->indents.len)--;
}

static inline int peek_indent(const hypex_lexer *lex) {
    if (lex->indents.len == 0) hypex_internal_error();
    return lex->indents.stack[lex->indents.len - 1];
}

static inline void clear_indent(hypex_lexer *lex) {
    lex->indents.len = 1;
}

static inline bool empty_indent(const hypex_lexer *lex) {
    return lex->indents.len == 1;
}

static inline hypex_token *peek_token(const hypex_lexer *lex) {
    return lex->tokens.list[lex->tokens.len - 1];
}

static inline bool has_next(const hypex_lexer *lex) {
    return lex->offset < lex->inputlen;
}

static inline void next(hypex_lexer *lex) {
    lex->cur = lex->input[++lex->offset];
    lex->pos.column++;
}

static inline void consume(hypex_lexer *lex, hypex_token *tok) {
    write_token(tok, lex->cur);
    next(lex);
}

static inline void error_lexer(const hypex_lexer *lex, const char *format, ...) {
    va_list args;
    va_start(args, format);
    hypex_error(lex->file, lex->pos.line, lex->pos.column, format, args);
    va_end(args);
}

static inline bool is_number(const hypex_lexer *lex) {
    return isdigit(lex->cur);
}

static inline bool is_letter(const hypex_lexer *lex) {
    return isalpha(lex->cur);
}

static inline bool is_exponent(const hypex_token *tok, char c) {
    if (!tok->num.is_exp) {
        if (tok->num.base == HYPEX_NUM_BASE_DEC && (c == 'e' || c == 'E')) return true;
        return tok->num.base == HYPEX_NUM_BASE_HEX && (c == 'p' || c == 'P');
    }
    return false;
}

static inline bool is_ident(const hypex_lexer *lex) {
    return isalnum(lex->cur) || lex->cur == '_';
}

static inline bool is_newline(hypex_lexer *lex) {
    if (lex->cur == '\r') {
        next(lex);
        return true;
    }
    return lex->cur == '\n';
}

static int match_base(char c) {
    switch (c) {
        case 'x': case 'X':
            return HYPEX_NUM_BASE_HEX;
        case 'o': case 'O':
            return HYPEX_NUM_BASE_OCT;
        case 'b': case 'B': 
            return HYPEX_NUM_BASE_BIN;
        default:
            return HYPEX_NUM_BASE_DEC;
    }
}

static inline int compare_keyword(const void *s1, const void *s2) {
    return strcmp(*(const char **)s1, *(const char **)s2);
}

static inline int match_keyword(const char *s) {
    const char **res = (const char **)bsearch(&s, KWLIST, KWLEN, sizeof(KWLIST[0]), compare_keyword);
    return res ? (int)(res - KWLIST) : -1;
}

static inline void lex_operator(hypex_lexer *lex, hypex_token *tok, hypex_token_kind kind) {
    tok->kind = kind;
    tok->len++;
    next(lex);
}

static void lex_number(hypex_lexer *lex, hypex_token *tok) {
    tok->kind = HYPEX_TOK_INTEGER;
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
        if (tok->kind != HYPEX_TOK_FLOAT && lex->cur == '.') {
            tok->kind = HYPEX_TOK_FLOAT;
            consume(lex, tok);
            continue;
        }
        if (is_exponent(tok, lex->cur)) {
            tok->num.is_exp = true;
            consume(lex, tok);
            continue;
        }
        switch (tok->num.base) {
            case HYPEX_NUM_BASE_DEC:
                if (isdigit(lex->cur)) {
                    consume(lex, tok);
                    continue;
                }
                break;
            case HYPEX_NUM_BASE_HEX:
                if (isxdigit(lex->cur)) {
                    consume(lex, tok);
                    continue;
                }
                break;
            case HYPEX_NUM_BASE_OCT:
                if (lex->cur >= '0' && lex->cur <= '7') {
                    consume(lex, tok);
                    continue;
                }
                break;
            case HYPEX_NUM_BASE_BIN:
                if (lex->cur == '0' || lex->cur == '1') {
                    consume(lex, tok);
                    continue;
                }
                break;
            default:
                hypex_internal_error();
                break;
        }
        break;
    }
    if ( (tok->kind == HYPEX_TOK_FLOAT && tok->num.base == HYPEX_NUM_BASE_HEX && !tok->num.is_exp)
        || (tok->value[tok->len - 1] == '_' || tok->value[tok->len - 1] == '.'))
        error_lexer(lex, "invalid number literal: %s", tok->value);
}

static void lex_ident(hypex_lexer *lex, hypex_token *tok) {
    tok->kind = HYPEX_TOK_IDENT;
    consume(lex, tok);
    while (is_ident(lex)) consume(lex, tok);
    const int id = match_keyword(tok->value);
    if (id != -1) {
        tok->kind = HYPEX_TOK_KEYWORD;
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

static void lex_comment_newline(hypex_lexer *lex, hypex_token *tok) {
    push_token(lex, hypex_token_copy(tok));
    free(tok->value);
    tok->value = NULL;
    tok->len = 0;
    push_token(lex, hypex_token_make(HYPEX_TOK_NEWLINE, lex->pos));
    lex->tokens.list[lex->tokens.len - 1]->comment = true;
    lex->pos.line++;
    lex->pos.column = 0;
    next(lex);
}

static void lex_comment_line(hypex_lexer *lex, hypex_token *tok) {
    tok->kind = HYPEX_TOK_COMMENT_LINE;
    next(lex);
    while (has_next(lex) && !is_newline(lex)) consume(lex, tok);
}

static void lex_comment_block(hypex_lexer *lex, hypex_token *tok) {
    tok->kind = HYPEX_TOK_COMMENT_BLOCK;
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

static void lex_literal(hypex_lexer *lex, hypex_token *tok, char delim) {
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

static void lex_fstring_body(hypex_lexer *lex, hypex_token *tok) {
    tok->kind = HYPEX_TOK_FSTRING_BODY;
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
        push_token(lex, hypex_token_make(HYPEX_TOK_FSTRING_END, lex->pos));
        next(lex);
    }
}

static void lex_rstring(hypex_lexer *lex) {
    hypex_token *last = lex->tokens.list[lex->tokens.len - 1];
    last->kind = HYPEX_TOK_RSTRING;
    free(last->value);
    last->value = NULL;
    last->len = 0;
    next(lex);
    while (has_next(lex) && lex->cur != '"') consume(lex, last);
    next(lex);
}

static void lex_indent(hypex_lexer *lex, hypex_token *tok) {
    tok->kind = HYPEX_TOK_INDENT;
    tok->level = 0;
    int peek = peek_indent(lex);
    if (tok->len > peek) {
        tok->level = 1;
        push_indent(lex, tok->len);
        return;
    }
    if (tok->len < peek) {
        tok->kind = HYPEX_TOK_DEDENT;
        while (peek_indent(lex) > tok->len) {
            pop_indent(lex);
            tok->level++;
        }
        if (peek_indent(lex) != tok->len) error_lexer(lex, "invalid indentation");
    }
}

static inline void lex_newline(hypex_lexer *lex, hypex_token *tok) {
    tok->kind = HYPEX_TOK_NEWLINE;
    tok->comment = false;
    lex->state = STATE_NEWL;
    next(lex);
}

static inline void lex_invalid(hypex_lexer *lex) {
    error_lexer(lex, "invalid character: '%c' (0x%x)", lex->cur, lex->cur);
    next(lex);
}

void hypex_lexer_tokenize(hypex_lexer *lex) {
    while (has_next(lex)) {
        if (lex->state == STATE_NEWL) {
            lex->state = STATE_NONE;
            lex->pos.line++;
            lex->pos.column = 1;
            if (lex->cur != ' ' && !empty_indent(lex)) {
                push_token(lex, hypex_token_make(HYPEX_TOK_DEDENT, lex->pos));
                lex->tokens.list[lex->tokens.len - 1]->level = -1;
                clear_indent(lex);
                next(lex);
            }
        }
        if (lex->state == STATE_BEGIN_F) {
            lex->state = STATE_NONE;
            if (lex->cur == '"') {
                push_token(lex, hypex_token_make(HYPEX_TOK_FSTRING_START, lex->pos));
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
        if (lex->state == STATE_F_EXPR && lex->cur == '}') {
            lex->state = STATE_F_BODY;
            next(lex);
            continue;
        }
        hypex_token *tok = hypex_token_make(HYPEX_TOK_UNKNOWN, lex->pos);
        if (lex->state == STATE_F_BODY) {
            lex_fstring_body(lex, tok);
            continue;
        }
        switch (lex->cur) {
            case '=':
                lex_operator(lex, tok, HYPEX_TOK_EQUAL);
                if (lex->cur == '=')
                    lex_operator(lex, tok, HYPEX_TOK_TWO_EQ);
                break;
            case '+':
                lex_operator(lex, tok, HYPEX_TOK_PLUS);
                switch (lex->cur) {
                    case '=':
                        lex_operator(lex, tok, HYPEX_TOK_PLUS_EQ);
                        break;
                    case '+':
                        lex_operator(lex, tok, HYPEX_TOK_INCREASE);
                        break;
                }
                break;
            case '-':
                lex_operator(lex, tok, HYPEX_TOK_MINUS);
                switch (lex->cur) {
                    case '=':
                        lex_operator(lex, tok, HYPEX_TOK_MINUS_EQ);
                        break;
                    case '-':
                        lex_operator(lex, tok, HYPEX_TOK_DECREASE);
                        break;
                }
                break;
            case '*':
                lex_operator(lex, tok, HYPEX_TOK_STAR);
                if (lex->cur == '=')
                    lex_operator(lex, tok, HYPEX_TOK_STAR_EQ);
                break;
            case '/':
                lex_operator(lex, tok, HYPEX_TOK_SLASH);
                switch (lex->cur) {
                    case '=':
                        lex_operator(lex, tok, HYPEX_TOK_SLASH_EQ);
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
                lex_operator(lex, tok, HYPEX_TOK_LESS);
                switch (lex->cur) {
                    case '=':
                        lex_operator(lex, tok, HYPEX_TOK_LESS_EQ);
                        break;
                    case '<':
                        lex_operator(lex, tok, HYPEX_TOK_LSHIFT);
                        if (lex->cur == '=')
                            lex_operator(lex, tok, HYPEX_TOK_LSHIFT_EQ);
                        break;
                }
                break;
            case '>':
                lex_operator(lex, tok, HYPEX_TOK_GREATER);
                switch (lex->cur) {
                    case '=':
                        lex_operator(lex, tok, HYPEX_TOK_GREATER_EQ);
                        break;
                    case '>':
                        lex_operator(lex, tok, HYPEX_TOK_RSHIFT);
                        if (lex->cur == '=')
                            lex_operator(lex, tok, HYPEX_TOK_RSHIFT_EQ);
                        break;
                }
                break;
            case '&':
                lex_operator(lex, tok, HYPEX_TOK_AMPER);
                switch (lex->cur) {
                    case '=':
                        lex_operator(lex, tok, HYPEX_TOK_AMPER_EQ);
                        break;
                    case '&':
                        lex_operator(lex, tok, HYPEX_TOK_TWO_AMPER);
                        break;
                }
                break;
            case '|':
                lex_operator(lex, tok, HYPEX_TOK_PIPE);
                switch (lex->cur) {
                    case '=':
                        lex_operator(lex, tok, HYPEX_TOK_PIPE_EQ);
                        break;
                    case '|':
                        lex_operator(lex, tok, HYPEX_TOK_TWO_PIPE);
                        break;
                }
                break;
            case '!':
                lex_operator(lex, tok, HYPEX_TOK_EXCLAMATION);
                if (lex->cur == '=')
                    lex_operator(lex, tok, HYPEX_TOK_EXCLAMATION_EQ);
                break;
            case '%':
                lex_operator(lex, tok, HYPEX_TOK_PERCENT);
                if (lex->cur == '=')
                    lex_operator(lex, tok, HYPEX_TOK_PERCENT_EQ);
                break;
            case '.':
                lex_operator(lex, tok, HYPEX_TOK_DOT);
                if (lex->cur == '.') {
                    lex_operator(lex, tok, HYPEX_TOK_TWO_DOT);
                    if (lex->cur == '.')
                        lex_operator(lex, tok, HYPEX_TOK_ELLIPSIS);
                }
                break;
            case ',':
                lex_operator(lex, tok, HYPEX_TOK_COMMA);
                break;
            case ':':
                lex_operator(lex, tok, HYPEX_TOK_COLON);
                break;
            case ';':
                lex_operator(lex, tok, HYPEX_TOK_SEMI);
                break;
            case '_':
                lex_operator(lex, tok, HYPEX_TOK_UNDERSCORE);
                if (is_ident(lex)) {
                    write_token(tok, '_');
                    lex_ident(lex, tok);
                }
                break;
            case '?':
                lex_operator(lex, tok, HYPEX_TOK_QUEST);
                break;
            case '(':
                lex_operator(lex, tok, HYPEX_TOK_LPAR);
                break;
            case ')':
                lex_operator(lex, tok, HYPEX_TOK_RPAR);
                break;
            case '[':
                lex_operator(lex, tok, HYPEX_TOK_LSQB);
                break;
            case ']':
                lex_operator(lex, tok, HYPEX_TOK_RSQB);
                break;
            case '{':
                lex_operator(lex, tok, HYPEX_TOK_LBRACE);
                break;
            case '}':
                lex_operator(lex, tok, HYPEX_TOK_RBRACE);
                break;
            case '^':
                lex_operator(lex, tok, HYPEX_TOK_CARET);
                if (lex->cur == '=')
                    lex_operator(lex, tok, HYPEX_TOK_CARET_EQ);
                break;
            case '~':
                lex_operator(lex, tok, HYPEX_TOK_TILDE);
                break;
            case '@':
                lex_operator(lex, tok, HYPEX_TOK_AT);
                break;
            case '#': 
                lex_operator(lex, tok, HYPEX_TOK_HASH);
                break;
            case '\\':
                lex_operator(lex, tok, HYPEX_TOK_ESCAPE);
                break;
            case ' ':
                lex_operator(lex, tok, HYPEX_TOK_SPACE);
                while (lex->cur == ' ') {
                    tok->len++;
                    next(lex);
                }
                if (peek_token(lex)->kind == HYPEX_TOK_NEWLINE)
                    lex_indent(lex, tok);
                break;
            case '\'':
                tok->kind = HYPEX_TOK_CHAR;
                lex_literal(lex, tok, '\'');
                break;
            case '"':
                tok->kind = HYPEX_TOK_STRING;
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
    push_token(lex, hypex_token_make(HYPEX_TOK_EOF, lex->pos));
}

void hypex_lexer_free(hypex_lexer *lex) {
    if(!lex) return;
    free(lex->input);
    free(lex->file);
    for (int i = 0; i < lex->tokens.len; i++)
        hypex_token_free(lex->tokens.list[i]);
    free(lex->tokens.list);
    free(lex->indents.stack);
    free(lex);
}
