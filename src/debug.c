#include <stdio.h>
#include <stddef.h>
#include <string.h>

#include "token.h"
#include "lexer.h"
#include "debug.h"

static inline const char *file_name(const char *file) {
    const char *res = strrchr(file, '/'); //for Unix
    if (!res) res = strrchr(file, '\\'); //for Non-Unix
    return res ? res + 1 : file;
}

void print_token_kind(int kind) {
    switch (kind) {
        case T_UNKNOWN: printf("T_UNKNOWN"); break;
        case T_EQUAL: printf("T_EQUAL"); break;
        case T_PLUS: printf("T_PLUS"); break;
        case T_MINUS: printf("T_MINUS"); break;
        case T_STAR: printf("T_STAR"); break;
        case T_SLASH: printf("T_SLASH"); break;
        case T_LESS: printf("T_LESS"); break;
        case T_GREATER: printf("T_GREATER"); break;
        case T_AMPER: printf("T_AMPER"); break;
        case T_PIPE: printf("T_PIPE"); break;
        case T_EXCLAMATION: printf("T_EXCLAMATION"); break;
        case T_PERCENT: printf("T_PERCENT"); break;
        case T_DOT: printf("T_DOT"); break;
        case T_COMMA: printf("T_COMMA"); break;
        case T_COLON: printf("T_COLON"); break;
        case T_SEMI: printf("T_SEMI"); break;
        case T_UNDERSCORE: printf("T_UNDERSCORE"); break;
        case T_QUEST: printf("T_QUEST"); break;
        case T_LPAR: printf("T_LPAR"); break;
        case T_RPAR: printf("T_RPAR"); break;
        case T_LSQB: printf("T_LSQB"); break;
        case T_RSQB: printf("T_RSQB"); break;
        case T_LBRACE: printf("T_LBRACE"); break;
        case T_RBRACE: printf("T_RBRACE"); break;
        case T_CARET: printf("T_CARET"); break;
        case T_TILDE: printf("T_TILDE"); break;
        case T_AT: printf("T_AT"); break;
        case T_HASH: printf("T_HASH"); break;
        case T_ESCAPE: printf("T_ESCAPE"); break;
        case T_TWO_EQ: printf("T_TWO_EQ"); break;
        case T_PLUS_EQ: printf("T_PLUS_EQ"); break;
        case T_MINUS_EQ: printf("T_MINUS_EQ"); break;
        case T_STAR_EQ: printf("T_STAR_EQ"); break;
        case T_SLASH_EQ: printf("T_SLASH_EQ"); break;
        case T_LESS_EQ: printf("T_LESS_EQ"); break;
        case T_GREATER_EQ: printf("T_GREATER_EQ"); break;
        case T_AMPER_EQ: printf("T_AMPER_EQ"); break;
        case T_TWO_AMPER: printf("T_TWO_AMPER"); break;
        case T_PIPE_EQ: printf("T_PIPE_EQ"); break;
        case T_TWO_PIPE: printf("T_TWO_PIPE"); break;
        case T_EXCLAMATION_EQ: printf("T_EXCLAMATION_EQ"); break;
        case T_PERCENT_EQ: printf("T_PERCENT_EQ"); break;
        case T_CARET_EQ: printf("T_CARET_EQ"); break;
        case T_LSHIFT: printf("T_LSHIFT"); break;
        case T_RSHIFT: printf("T_RSHIFT"); break;
        case T_INCREASE: printf("T_INCREASE"); break;
        case T_DECREASE: printf("T_DECREASE"); break;
        case T_TWO_DOT: printf("T_TWO_DOT"); break;
        case T_LSHIFT_EQ: printf("T_LSHIFT_EQ"); break;
        case T_RSHIFT_EQ: printf("T_RSHIFT_EQ"); break;
        case T_ELLIPSIS: printf("T_ELLIPSIS"); break;
        case T_INTEGER: printf("T_INTEGER"); break;
        case T_FLOAT: printf("T_FLOAT"); break;
        case T_IDENT: printf("T_IDENT"); break;
        case T_KEYWORD: printf("T_KEYWORD"); break;
        case T_COMMENT_LINE: printf("T_COMMENT_LINE"); break;
        case T_COMMENT_BLOCK: printf("T_COMMENT_BLOCK"); break;
        case T_CHAR: printf("T_CHAR"); break;
        case T_STRING: printf("T_STRING"); break;
        case T_FSTRING_START: printf("T_FSTRING_START"); break;
        case T_FSTRING_BODY: printf("T_FSTRING_BODY"); break;
        case T_FSTRING_END: printf("T_FSTRING_END"); break;
        case T_RSTRING: printf("T_RSTRING"); break;
        case T_SPACE: printf("T_SPACE"); break;
        case T_INDENT: printf("T_INDENT"); break;
        case T_DEDENT: printf("T_DEDENT"); break;
        case T_EOL: printf("T_EOL"); break;
        default: printf("_T_%d", kind); break;
    }
}

void print_keyword(int id) {
    switch (id) {
        case KW_ASYNC: printf("KW_ASYNC"); break;
        case KW_AWAIT: printf("KW_AWAIT"); break;
        case KW_BASE: printf("KW_BASE"); break;
        case KW_BOOL: printf("KW_BOOL"); break;
        case KW_BREAK: printf("KW_BREAK"); break;
        case KW_CASE: printf("KW_CASE"); break;
        case KW_CATCH: printf("KW_CATCH"); break;
        case KW_CHAR: printf("KW_CHAR"); break;
        case KW_CLASS: printf("KW_CLASS"); break;
        case KW_CONST: printf("KW_CONST"); break;
        case KW_CONTINUE: printf("KW_CONTINUE"); break;
        case KW_DOUBLE: printf("KW_DOUBLE"); break;
        case KW_ELIF: printf("KW_ELIF"); break;
        case KW_ELSE: printf("KW_ELSE"); break;
        case KW_ENUM: printf("KW_ENUM"); break;
        case KW_FALSE: printf("KW_FALSE"); break;
        case KW_FLOAT: printf("KW_FLOAT"); break;
        case KW_FOR: printf("KW_FOR"); break;
        case KW_FUNC: printf("KW_FUNC"); break;
        case KW_IF: printf("KW_IF"); break;
        case KW_INT: printf("KW_INT"); break;
        case KW_INTERFACE: printf("KW_INTERFACE"); break;
        case KW_LONG: printf("KW_LONG"); break;
        case KW_NULL: printf("KW_NULL"); break;
        case KW_OBJECT: printf("KW_OBJECT"); break;
        case KW_RETURN: printf("KW_RETURN"); break;
        case KW_SELF: printf("KW_SELF"); break;
        case KW_SHORT: printf("KW_SHORT"); break;
        case KW_STRING: printf("KW_STRING"); break;
        case KW_STRUCT: printf("KW_STRUCT"); break;
        case KW_SWITCH: printf("KW_SWITCH"); break;
        case KW_THROW: printf("KW_THROW"); break;
        case KW_TRUE: printf("KW_TRUE"); break;
        case KW_TRY: printf("KW_TRY"); break;
        case KW_UINT: printf("KW_UINT"); break;
        case KW_ULONG: printf("KW_ULONG"); break;
        case KW_USE: printf("KW_USE"); break;
        case KW_USHORT: printf("KW_USHORT"); break;
        case KW_VAR: printf("KW_VAR"); break;
        case KW_WHILE: printf("KW_WHILE"); break;
        case KW_YIELD: printf("KW_YIELD"); break;
        default: printf("_KW_%d", id); break;
    }
}

void print_base(int base) {
    switch (base) {
        case BASE_DEC: printf("BASE_DEC"); break;
        case BASE_HEX: printf("BASE_HEX"); break;
        case BASE_OCT: printf("BASE_OCT"); break;
        case BASE_BIN: printf("BASE_BIN"); break;
        default: printf("_BASE_%d", base); break;
    }
}

void print_token(Token tok) {
    printf("%d:%d ", tok.pos.line, tok.pos.column);
    (tok.kind != T_KEYWORD) ? print_token_kind(tok.kind) : print_keyword(tok.id);
    if (tok.value) printf(" `%s`", tok.value);
    if (tok.kind == T_INTEGER || tok.kind == T_FLOAT) {
        if (tok.base != 0) {
            printf(" ");
            print_base(tok.base);
        }
        if (tok.is_exponent) {
            if (tok.is_negative) printf(" negative_exponent");
            else printf(" exponent");
        }
        printf(" num_value:`%s` num_len:%d", tok.num_value, tok.num_len);
    }
    if (tok.kind == T_EOL && tok.is_comment) printf(" (comment)");
    if (tok.kind == T_INDENT || tok.kind == T_DEDENT) printf(" level:%d", tok.level);
    printf(" len:%d\n", tok.len);
}

void print_lexer(Lexer lex) {
    printf("lex:%s:%d:%d offset:%d len:%d stk_len:%d stk_cap:%d ind:%d newln:%d pot_fs:%d pot_rs:%d fs_body:%d fs_expr:%d\n\n", file_name(lex.file), lex.pos.line, lex.pos.column, lex.len, lex.stack_len, lex.stack_capacity, lex.indent, lex.offset, lex.newline, lex.potential_fstring, lex.potential_rstring, lex.fstring_body, lex.fstring_expr);
    for (int i = 0; i < lex.len; i++)
        print_token(*(lex.tok_list[i]));
}