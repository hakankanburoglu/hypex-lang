#ifndef TOKEN_H
#define TOKEN_H

#include <stddef.h>
#include <stdbool.h>

enum {
    UNKNOWN,
    // SINGLE
    EQUAL, // =
    PLUS, // +
    MINUS, // -
    STAR, // *
    SLASH, // /
    LESS,  // <
    GREATER, // >
    AMPER, // &
    PIPE, // |
    EXCLAMATION, // !
    PERCENT, // %
    DOT, // .
    COMMA, // ,
    COLON, // :
    SEMI, // ;
    UNDERSCORE, // _
    QUEST, // ?
    LPAR, // (
    RPAR, // )
    LSQB, // [
    RSQB, // ]
    LBRACE, // {
    RBRACE, // }
    CARET, // ^
    TILDE, // ~
    AT, // @
    HASH, // #
    ESCAPE, // \ //
    // DUAL
    TWO_EQ, // ==
    PLUS_EQ, // +=
    MINUS_EQ, // -=
    STAR_EQ, // *=
    SLASH_EQ, // /=
    LESS_EQ, // <=
    GREATER_EQ, // >=
    AMPER_EQ, // &=
    TWO_AMPER, // &&
    PIPE_EQ, // |=
    TWO_PIPE, // ||
    EXCLAMATION_EQ, // !=
    PERCENT_EQ, // %=
    CARET_EQ, // ^=
    LSHIFT, // <<
    RSHIFT, // >>
    INCREASE, // ++
    DECREASE, // --
    TWO_DOT, // ..
    // TRIPLE
    LSHIFT_EQ, // <<=
    RSHIFT_EQ, // >>=
    ELLIPSIS, // ...
    //
    INTEGER,
    FLOAT,
    IDENT,
    KEYWORD,
    COMMENT_LINE,
    COMMENT_BLOCK,
    CHAR,
    STRING,
    FSTRING_START,
    FSTRING_BODY,
    FSTRING_END,
    RSTRING,
    SPACE,
    _INDENT,
    _DEDENT,
    EOL
};

enum {
    KW_IF,
    KW_FOR,
    KW_INT,
    KW_TRY,
    KW_USE,
    KW_VAR,
    KW_BASE,
    KW_BOOL,
    KW_CASE,
    KW_CHAR,
    KW_ELIF,
    KW_ELSE,
    KW_ENUM,
    KW_FUNC,
    KW_LONG,
    KW_NULL,
    KW_SELF,
    KW_TRUE,
    KW_UINT,
    KW_ASYNC,
    KW_AWAIT,
    KW_BREAK,
    KW_CATCH,
    KW_CLASS,
    KW_CONST,
    KW_FALSE,
    KW_FLOAT,
    KW_SHORT,
    KW_THROW,
    KW_ULONG,
    KW_WHILE,
    KW_YIELD,
    KW_DOUBLE,
    KW_OBJECT,
    KW_RETURN,
    KW_STRING,
    KW_STRUCT,
    KW_SWITCH,
    KW_USHORT,
    KW_CONTINUE,
    KW_INTERFACE
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
    int type;
    char *value;
    size_t len;
    Pos pos;
    union {
        //KEYWORD
        int id;
        //INTEGER, FLOAT
        struct {
            int base;
            char *num_value;
            size_t num_len;
            bool is_exponent;
            bool is_negative;
        };
        //EOL
        bool is_comment;
    };
} Token;

Token *make_token(int type, Pos pos);

void init_number(Token *tok);

void consume_token(Token *tok, char c);

void consume_number(Token *tok, char c);

Token *copy_token(const Token *tok);

//bool token_compare(Token tok, int type, const char *value);

//bool is_operator_token(Token tok);

//bool is_number_token(Token tok);

void free_token(Token *tok);

#endif //TOKEN_H