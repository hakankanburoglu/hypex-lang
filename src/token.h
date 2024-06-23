#ifndef TOKEN_H
#define TOKEN_H

#include <stddef.h>

enum {
    UNKNOWN,
    // SINGLE
    EQUAL, // =
    PLUS, // +
    MINUS, // -
    STAR, // *
    SLASH, // /
    GREATER, // >
    LESS,  // <
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
    TILDE, // ~
    CARET, // ^
    AT, // @
    HASH, // #
    // DUAL
    TWO_EQ, // ==
    TWO_AMPER, // &&
    TWO_PIPE, // ||
    PLUS_EQ, // +=
    MINUS_EQ, // -=
    STAR_EQ, // *=
    SLASH_EQ, // /=
    GREATER_EQ, // >=
    LESS_EQ, // >=
    AMPER_EQ, // &=
    PIPE_EQ, // |=
    EXCLAMATION_EQ, // !=
    PERCENT_EQ, // %=
    CARET_EQ, // ^=
    LSHIFT, // <<
    RSHIFT, // >>
    INCREASE, // ++
    DECREASE, // --
    // TRIPLE
    LSHIFT_EQ, // <<=
    RSHIFT_EQ, // >>=
    //
    INTEGER,
    FLOAT,
    IDENT,
    KEYWORD,
    COMMENT_LINE,
    COMMENT_BLOCK,
    CHAR,
    STRING,
    FSTRING,
    RSTRING,
    SPACE,
    _INDENT,
    _DEDENT,
    EOL
};

typedef struct {
    int type;
    int line;
    int col;
    size_t len;
    char *data;
} Token;

Token make_token(const int type, const int line, const int col);

#endif //TOKEN_H