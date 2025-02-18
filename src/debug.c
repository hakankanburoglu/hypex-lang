#include <stdio.h>
#include <stddef.h>
#include <string.h>

#include "token.h"
#include "lexer.h"
#include "debug.h"

void print_token_type(int type) {
    switch (type) {
        case UNKNOWN: printf("UNKNOWN"); break;
        case EQUAL: printf("EQUAL"); break;
        case PLUS: printf("PLUS"); break;
        case MINUS: printf("MINUS"); break;
        case STAR: printf("STAR"); break;
        case SLASH: printf("SLASH"); break;
        case LESS: printf("LESS"); break;
        case GREATER: printf("GREATER"); break;
        case AMPER: printf("AMPER"); break;
        case PIPE: printf("PIPE"); break;
        case EXCLAMATION: printf("EXCLAMATION"); break;
        case PERCENT: printf("PERCENT"); break;
        case DOT: printf("DOT"); break;
        case COMMA: printf("COMMA"); break;
        case COLON: printf("COLON"); break;
        case SEMI: printf("SEMI"); break;
        case UNDERSCORE: printf("UNDERSCORE"); break;
        case QUEST: printf("QUEST"); break;
        case LPAR: printf("LPAR"); break;
        case RPAR: printf("RPAR"); break;
        case LSQB: printf("LSQB"); break;
        case RSQB: printf("RSQB"); break;
        case LBRACE: printf("LBRACE"); break;
        case RBRACE: printf("RBRACE"); break;
        case CARET: printf("CARET"); break;
        case TILDE: printf("TILDE"); break;
        case AT: printf("AT"); break;
        case HASH: printf("HASH"); break;
        case ESCAPE: printf("ESCAPE"); break;
        case TWO_EQ: printf("TWO_EQ"); break;
        case PLUS_EQ: printf("PLUS_EQ"); break;
        case MINUS_EQ: printf("MINUS_EQ"); break;
        case STAR_EQ: printf("STAR_EQ"); break;
        case SLASH_EQ: printf("SLASH_EQ"); break;
        case LESS_EQ: printf("LESS_EQ"); break;
        case GREATER_EQ: printf("GREATER_EQ"); break;
        case AMPER_EQ: printf("AMPER_EQ"); break;
        case TWO_AMPER: printf("TWO_AMPER"); break;
        case PIPE_EQ: printf("PIPE_EQ"); break;
        case TWO_PIPE: printf("TWO_PIPE"); break;
        case EXCLAMATION_EQ: printf("EXCLAMATION_EQ"); break;
        case PERCENT_EQ: printf("PERCENT_EQ"); break;
        case CARET_EQ: printf("CARET_EQ"); break;
        case LSHIFT: printf("LSHIFT"); break;
        case RSHIFT: printf("RSHIFT"); break;
        case INCREASE: printf("INCREASE"); break;
        case DECREASE: printf("DECREASE"); break;
        case TWO_DOT: printf("TWO_DOT"); break;
        case LSHIFT_EQ: printf("LSHIFT_EQ"); break;
        case RSHIFT_EQ: printf("RSHIFT_EQ"); break;
        case ELLIPSIS: printf("ELLIPSIS"); break;
        case INTEGER: printf("INTEGER"); break;
        case FLOAT: printf("FLOAT"); break;
        case IDENT: printf("IDENT"); break;
        case KEYWORD: printf("KEYWORD"); break;
        case COMMENT_LINE: printf("COMMENT_LINE"); break;
        case COMMENT_BLOCK: printf("COMMENT_BLOCK"); break;
        case CHAR: printf("CHAR"); break;
        case STRING: printf("STRING"); break;
        case FSTRING_START: printf("FSTRING_START"); break;
        case FSTRING_BODY: printf("FSTRING_BODY"); break;
        case FSTRING_END: printf("FSTRING_END"); break;
        case RSTRING: printf("RSTRING"); break;
        case SPACE: printf("SPACE"); break;
        case _INDENT: printf("_INDENT"); break;
        case _DEDENT: printf("_DEDENT"); break;
        case EOL: printf("EOL"); break;
        default: printf("TOK?[%d]", type); break;
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
        default: printf("KW?[%d]", id); break;
    }
}

void print_base(int base) {
    switch (base) {
        case BASE_DEC: printf("BASE_DEC"); break;
        case BASE_HEX: printf("BASE_HEX"); break;
        case BASE_OCT: printf("BASE_OCT"); break;
        case BASE_BIN: printf("BASE_BIN"); break;
        default: printf("BASE?[%d]", base); break;
    }
}

void print_token(Token tok) {
    printf("%d:%d ", tok.pos.line, tok.pos.column);
    (tok.type != KEYWORD) ? print_token_type(tok.type) : print_keyword(tok.id);
    if (tok.value != NULL) printf(" `%s`", tok.value);
    if (tok.type == INTEGER || tok.type == FLOAT) {
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
    if (tok.type == EOL && tok.is_comment) printf(" (comment)");
    if (tok.type == _INDENT || tok.type == _DEDENT) printf(" level:%d", tok.level);
    printf(" len:%d\n", tok.len);
}

void print_lexer(Lexer lex) {
    printf("lex:%s:%d:%d offset:%d len:%d stk_len:%d stk_cap:%d ind:%d newln:%d pot_fs:%d pot_rs:%d fs_body:%d fs_expr:%d\n\n", lex.file, lex.pos.line, lex.pos.column, lex.len, lex.stack_len, lex.stack_capacity, lex.indent, lex.offset, lex.newline, lex.potential_fstring, lex.potential_rstring, lex.fstring_body, lex.fstring_expr);
    for (int i = 0; i < lex.len; i++)
        print_token(*(lex.tok_list[i]));
}