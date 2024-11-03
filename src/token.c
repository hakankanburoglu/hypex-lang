#include <stddef.h>

#include "token.h"

Token make_token(const int type, const int line, const int col) {
    Token tok;
    tok.type = type;
    tok.line = line;
    tok.col = col;
    tok.len = 0;
    tok.data = NULL;
    return tok;
}