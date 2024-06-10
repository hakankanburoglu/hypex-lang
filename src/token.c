#include <stdlib.h>
#include <stdbool.h>

#include "token.h"

Token make_token(const int type, const int line, const int col) {
    Token tok;
    tok.type = type;
    tok.line = line;
    tok.col = col;
    tok.is_continues = false;
    tok.is_wide = false;
    tok.len = 0;
    tok.data = NULL;
    return tok;
}

Token make_wtoken(const int type, const int line, const int col) {
    Token tok;
    tok.type = type;
    tok.line = line;
    tok.col = col;
    tok.is_continues = false;
    tok.is_wide = true;
    tok.len = 0;
    tok.wdata = NULL;
    return tok;
}