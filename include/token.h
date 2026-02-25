#ifndef TOKEN_H
#define TOKEN_H

#include <stdbool.h>
#include <stddef.h>

typedef enum {
    HYPEX_TOK_UNKNOWN,
    // SINGLE
    HYPEX_TOK_EQUAL, // =
    HYPEX_TOK_PLUS, // +
    HYPEX_TOK_MINUS, // -
    HYPEX_TOK_STAR, // *
    HYPEX_TOK_SLASH, // /
    HYPEX_TOK_LESS,  // <
    HYPEX_TOK_GREATER, // >
    HYPEX_TOK_AMPER, // &
    HYPEX_TOK_PIPE, // |
    HYPEX_TOK_EXCLAMATION, // !
    HYPEX_TOK_PERCENT, // %
    HYPEX_TOK_DOT, // .
    HYPEX_TOK_COMMA, // ,
    HYPEX_TOK_COLON, // :
    HYPEX_TOK_SEMI, // ;
    HYPEX_TOK_UNDERSCORE, // _
    HYPEX_TOK_QUEST, // ?
    HYPEX_TOK_LPAR, // (
    HYPEX_TOK_RPAR, // )
    HYPEX_TOK_LSQB, // [
    HYPEX_TOK_RSQB, // ]
    HYPEX_TOK_LBRACE, // {
    HYPEX_TOK_RBRACE, // }
    HYPEX_TOK_CARET, // ^
    HYPEX_TOK_TILDE, // ~
    HYPEX_TOK_AT, // @
    HYPEX_TOK_HASH, // #
    HYPEX_TOK_ESCAPE, // \ //
    // DUAL
    HYPEX_TOK_TWO_EQ, // ==
    HYPEX_TOK_PLUS_EQ, // +=
    HYPEX_TOK_MINUS_EQ, // -=
    HYPEX_TOK_STAR_EQ, // *=
    HYPEX_TOK_SLASH_EQ, // /=
    HYPEX_TOK_LESS_EQ, // <=
    HYPEX_TOK_GREATER_EQ, // >=
    HYPEX_TOK_AMPER_EQ, // &=
    HYPEX_TOK_TWO_AMPER, // &&
    HYPEX_TOK_PIPE_EQ, // |=
    HYPEX_TOK_TWO_PIPE, // ||
    HYPEX_TOK_EXCLAMATION_EQ, // !=
    HYPEX_TOK_PERCENT_EQ, // %=
    HYPEX_TOK_CARET_EQ, // ^=
    HYPEX_TOK_LSHIFT, // <<
    HYPEX_TOK_RSHIFT, // >>
    HYPEX_TOK_INCREASE, // ++
    HYPEX_TOK_DECREASE, // --
    HYPEX_TOK_TWO_DOT, // ..
    // TRIPLE
    HYPEX_TOK_LSHIFT_EQ, // <<=
    HYPEX_TOK_RSHIFT_EQ, // >>=
    HYPEX_TOK_ELLIPSIS, // ...
    //
    HYPEX_TOK_INTEGER,
    HYPEX_TOK_FLOAT,
    HYPEX_TOK_IDENT,
    HYPEX_TOK_KEYWORD,
    HYPEX_TOK_COMMENT_LINE,
    HYPEX_TOK_COMMENT_BLOCK,
    HYPEX_TOK_CHAR,
    HYPEX_TOK_STRING,
    HYPEX_TOK_FSTRING_START,
    HYPEX_TOK_FSTRING_BODY,
    HYPEX_TOK_FSTRING_END,
    HYPEX_TOK_RSTRING,
    HYPEX_TOK_SPACE,
    HYPEX_TOK_INDENT,
    HYPEX_TOK_DEDENT,
    HYPEX_TOK_NEWLINE,
    HYPEX_TOK_EOF
} hypex_token_kind;

typedef enum {
    HYPEX_KW_ASYNC,
    HYPEX_KW_AWAIT,
    HYPEX_KW_BASE,
    HYPEX_KW_BOOL,
    HYPEX_KW_BREAK,
    HYPEX_KW_BYTE,
    HYPEX_KW_CASE,
    HYPEX_KW_CATCH,
    HYPEX_KW_CHAR,
    HYPEX_KW_CLASS,
    HYPEX_KW_CONST,
    HYPEX_KW_CONTINUE,
    HYPEX_KW_DOUBLE,
    HYPEX_KW_ELIF,
    HYPEX_KW_ELSE,
    HYPEX_KW_ENUM,
    HYPEX_KW_FALSE,
    HYPEX_KW_FLOAT,
    HYPEX_KW_FOR,
    HYPEX_KW_FUNC,
    HYPEX_KW_IF,
    HYPEX_KW_INT,
    HYPEX_KW_INTERFACE,
    HYPEX_KW_LONG,
    HYPEX_KW_NULL,
    HYPEX_KW_OBJECT,
    HYPEX_KW_RETURN,
    HYPEX_KW_SBYTE,
    HYPEX_KW_SELF,
    HYPEX_KW_SHORT,
    HYPEX_KW_STRING,
    HYPEX_KW_STRUCT,
    HYPEX_KW_SWITCH,
    HYPEX_KW_THROW,
    HYPEX_KW_TRUE,
    HYPEX_KW_TRY,
    HYPEX_KW_UINT,
    HYPEX_KW_ULONG,
    HYPEX_KW_USE,
    HYPEX_KW_USHORT,
    HYPEX_KW_VAR,
    HYPEX_KW_WHILE,
    HYPEX_KW_YIELD
} hypex_keyword;

typedef enum {
    HYPEX_NUM_BASE_DEC,
    HYPEX_NUM_BASE_HEX,
    HYPEX_NUM_BASE_OCT,
    HYPEX_NUM_BASE_BIN
} hypex_num_base;

typedef struct {
    int line;
    int column;
} hypex_pos;

typedef struct {
    hypex_token_kind kind;
    char *value;
    size_t cap;
    size_t len;
    hypex_pos pos;
    union {
        // KEYWORD
        int id;
        // INTEGER, FLOAT
        struct {
            hypex_num_base base;
            bool is_exp;
            bool is_neg;
        } num;
        // INDENT, DEDENT
        int level;
        // NEWLINE
        bool comment;
    };
} hypex_token;

hypex_token *hypex_token_make(hypex_token_kind kind, hypex_pos pos);

hypex_token *hypex_token_copy(const hypex_token *tok);

void hypex_token_free(hypex_token *tok);

#endif // TOKEN_H
