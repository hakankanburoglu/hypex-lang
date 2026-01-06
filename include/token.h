#ifndef TOKEN_H
#define TOKEN_H

#include <stdbool.h>
#include <stddef.h>

enum {
    T_UNKNOWN,
    // SINGLE
    T_EQUAL, // =
    T_PLUS, // +
    T_MINUS, // -
    T_STAR, // *
    T_SLASH, // /
    T_LESS,  // <
    T_GREATER, // >
    T_AMPER, // &
    T_PIPE, // |
    T_EXCLAMATION, // !
    T_PERCENT, // %
    T_DOT, // .
    T_COMMA, // ,
    T_COLON, // :
    T_SEMI, // ;
    T_UNDERSCORE, // _
    T_QUEST, // ?
    T_LPAR, // (
    T_RPAR, // )
    T_LSQB, // [
    T_RSQB, // ]
    T_LBRACE, // {
    T_RBRACE, // }
    T_CARET, // ^
    T_TILDE, // ~
    T_AT, // @
    T_HASH, // #
    T_ESCAPE, // \ //
    // DUAL
    T_TWO_EQ, // ==
    T_PLUS_EQ, // +=
    T_MINUS_EQ, // -=
    T_STAR_EQ, // *=
    T_SLASH_EQ, // /=
    T_LESS_EQ, // <=
    T_GREATER_EQ, // >=
    T_AMPER_EQ, // &=
    T_TWO_AMPER, // &&
    T_PIPE_EQ, // |=
    T_TWO_PIPE, // ||
    T_EXCLAMATION_EQ, // !=
    T_PERCENT_EQ, // %=
    T_CARET_EQ, // ^=
    T_LSHIFT, // <<
    T_RSHIFT, // >>
    T_INCREASE, // ++
    T_DECREASE, // --
    T_TWO_DOT, // ..
    // TRIPLE
    T_LSHIFT_EQ, // <<=
    T_RSHIFT_EQ, // >>=
    T_ELLIPSIS, // ...
    //
    T_INTEGER,
    T_FLOAT,
    T_IDENT,
    T_KEYWORD,
    T_COMMENT_LINE,
    T_COMMENT_BLOCK,
    T_CHAR,
    T_STRING,
    T_FSTRING_START,
    T_FSTRING_BODY,
    T_FSTRING_END,
    T_RSTRING,
    T_SPACE,
    T_INDENT,
    T_DEDENT,
    T_NEWLINE
};

enum {
    KW_ASYNC,
    KW_AWAIT,
    KW_BASE,
    KW_BOOL,
    KW_BREAK,
    KW_CASE,
    KW_CATCH,
    KW_CHAR,
    KW_CLASS,
    KW_CONST,
    KW_CONTINUE,
    KW_DOUBLE,
    KW_ELIF,
    KW_ELSE,
    KW_ENUM,
    KW_FALSE,
    KW_FLOAT,
    KW_FOR,
    KW_FUNC,
    KW_IF,
    KW_INT,
    KW_INTERFACE,
    KW_LONG,
    KW_NULL,
    KW_OBJECT,
    KW_RETURN,
    KW_SELF,
    KW_SHORT,
    KW_STRING,
    KW_STRUCT,
    KW_SWITCH,
    KW_THROW,
    KW_TRUE,
    KW_TRY,
    KW_UINT,
    KW_ULONG,
    KW_USE,
    KW_USHORT,
    KW_VAR,
    KW_WHILE,
    KW_YIELD
};

enum {
    BASE_DEC,
    BASE_HEX,
    BASE_OCT,
    BASE_BIN
};

typedef struct {
    int line;
    int column;
} Pos;

typedef struct {
    int kind;
    char *value;
    size_t cap;
    size_t len;
    Pos pos;
    union {
        // KEYWORD
        int id;
        // INTEGER, FLOAT
        struct {
            int base;
            bool is_exp;
            bool is_neg;
        } num;
        // INDENT, DEDENT
        int level;
        // NEWLINE
        bool comment;
    };
} Token;

Token *make_token(int kind, Pos pos);

Token *copy_token(const Token *tok);

void free_token(Token *tok);

#endif // TOKEN_H
