#include <stdio.h>

#include "token.h"
#include "node.h"
#include "parse.h"
#include "out.h"

void print_token_type(FILE *file, const int tok_type) {
    switch (tok_type) {
        case UNKNOWN: fprintf(file, "UNKNOWN"); break;
        case EQUAL: fprintf(file, "EQUAL"); break;
        case PLUS: fprintf(file, "PLUS"); break;
        case MINUS: fprintf(file, "MINUS"); break;
        case STAR: fprintf(file, "STAR"); break;
        case SLASH: fprintf(file, "SLASH"); break;
        case GREATER: fprintf(file, "GREATER"); break;
        case LESS: fprintf(file, "LESS"); break;
        case AMPER: fprintf(file, "AMPER"); break;
        case PIPE: fprintf(file, "PIPE"); break;
        case EXCLAMATION: fprintf(file, "EXCLAMATION"); break;
        case PERCENT: fprintf(file, "PERCENT"); break;
        case DOT: fprintf(file, "DOT"); break;
        case COMMA: fprintf(file, "COMMA"); break;
        case COLON: fprintf(file, "COLON"); break;
        case SEMI: fprintf(file, "SEMI"); break;
        case UNDERSCORE: fprintf(file, "UNDERSCORE"); break;
        case QUEST: fprintf(file, "QUEST"); break;
        case LPAR: fprintf(file, "LPAR"); break;
        case RPAR: fprintf(file, "RPAR"); break;
        case LSQB: fprintf(file, "LSQB"); break;
        case RSQB: fprintf(file, "RSQB"); break;
        case LBRACE: fprintf(file, "LBRACE"); break;
        case RBRACE: fprintf(file, "RBRACE"); break;
        case TILDE: fprintf(file, "TILDE"); break;
        case CARET: fprintf(file, "CARET"); break;
        case AT: fprintf(file, "AT"); break;
        case HASH: fprintf(file, "HASH"); break;
        case TWO_EQ: fprintf(file, "TWO_EQ"); break;
        case TWO_AMPER: fprintf(file, "TWO_AMPER"); break;
        case TWO_PIPE: fprintf(file, "TWO_PIPE"); break;
        case PLUS_EQ: fprintf(file, "PLUS_EQ"); break;
        case MINUS_EQ: fprintf(file, "MINUS_EQ"); break;
        case STAR_EQ: fprintf(file, "STAR_EQ"); break;
        case SLASH_EQ: fprintf(file, "SLASH_EQ"); break;
        case GREATER_EQ: fprintf(file, "GREATER_EQ"); break;
        case LESS_EQ: fprintf(file, "LESS_EQ"); break;
        case AMPER_EQ: fprintf(file, "AMPER_EQ"); break;
        case PIPE_EQ: fprintf(file, "PIPE_EQ"); break;
        case EXCLAMATION_EQ: fprintf(file, "EXCLAMATION_EQ"); break;
        case PERCENT_EQ: fprintf(file, "PERCENT_EQ"); break;
        case CARET_EQ: fprintf(file, "CARET_EQ"); break;
        case LSHIFT: fprintf(file, "LSHIFT"); break;
        case RSHIFT: fprintf(file, "RSHIFT"); break;
        case INCREASE: fprintf(file, "INCREASE"); break;
        case DECREASE: fprintf(file, "DECREASE"); break;
        case LSHIFT_EQ: fprintf(file, "LSHIFT_EQ"); break;
        case RSHIFT_EQ: fprintf(file, "RSHIFT_EQ"); break;
        case INTEGER: fprintf(file, "INTEGER"); break;
        case FLOAT: fprintf(file, "FLOAT"); break;
        case IDENT: fprintf(file, "IDENT"); break;
        case KEYWORD: fprintf(file, "KEYWORD"); break;
        case COMMENT_LINE: fprintf(file, "COMMENT_LINE"); break;
        case COMMENT_BLOCK: fprintf(file, "COMMENT_BLOCK"); break;
        case CHAR: fprintf(file, "CHAR"); break;
        case STRING: fprintf(file, "STRING"); break;
        case FSTRING: fprintf(file, "FSTRING"); break;
        case RSTRING: fprintf(file, "RSTRING"); break;
        case SPACE: fprintf(file, "SPACE"); break;
        case _INDENT: fprintf(file, "_INDENT"); break;
        case _DEDENT: fprintf(file, "_DEDENT"); break;
        case EOL: fprintf(file, "EOL"); break;
        default: fprintf(file, "%d", tok_type); break;
    };
}

void print_token(FILE *file, const Token tok) {
    fprintf(file, "%d, %d, ", (tok.line + 1), (tok.col + 1));
    print_token_type(file, tok.type);
    fprintf(file, ", %d, `", (int)tok.len);
    if (tok.data != NULL)
        fprintf(file, "%s", tok.data);
    fprintf(file, "`\n");
}

void print_tokens(FILE *file, const Token *tokens, const size_t len) {
    fprintf(file, "line, col, type, len, data\n");
    for (int i = 0; i < len; i++)
        print_token(file, tokens[i]);
}

void print_token_data(FILE *file, const Token tok) {
    switch (tok.type) {
        case EQUAL: fprintf(file, "="); break;
        case PLUS: fprintf(file, "+"); break;
        case MINUS: fprintf(file, "-"); break;
        case STAR: fprintf(file, "*"); break;
        case SLASH: fprintf(file, "/"); break;
        case GREATER: fprintf(file, ">"); break;
        case LESS: fprintf(file, "<"); break;
        case AMPER: fprintf(file, "&"); break;
        case PIPE: fprintf(file, "|"); break;
        case EXCLAMATION: fprintf(file, "!"); break;
        case PERCENT: fprintf(file, "%%"); break;
        case DOT: fprintf(file, "."); break;
        case COMMA: fprintf(file, ","); break;
        case COLON: fprintf(file, ":"); break;
        case SEMI: fprintf(file, ";"); break;
        case UNDERSCORE: fprintf(file, "_"); break;
        case QUEST: fprintf(file, "?"); break;
        case LPAR: fprintf(file, "("); break;
        case RPAR: fprintf(file, ")"); break;
        case LSQB: fprintf(file, "["); break;
        case RSQB: fprintf(file, "]"); break;
        case LBRACE: fprintf(file, "{"); break;
        case RBRACE: fprintf(file, "}"); break;
        case TILDE: fprintf(file, "~"); break;
        case CARET: fprintf(file, "^"); break;
        case AT: fprintf(file, "@"); break;
        case HASH: fprintf(file, "#"); break;
        case TWO_EQ: fprintf(file, "=="); break;
        case TWO_AMPER: fprintf(file, "&&"); break;
        case TWO_PIPE: fprintf(file, "||"); break;
        case PLUS_EQ: fprintf(file, "+="); break;
        case MINUS_EQ: fprintf(file, "-="); break;
        case STAR_EQ: fprintf(file, "*="); break;
        case SLASH_EQ: fprintf(file, "/="); break;
        case GREATER_EQ: fprintf(file, ">="); break;
        case LESS_EQ: fprintf(file, "<="); break;
        case AMPER_EQ: fprintf(file, "&="); break;
        case PIPE_EQ: fprintf(file, "|="); break;
        case EXCLAMATION_EQ: fprintf(file, "!="); break;
        case PERCENT_EQ: fprintf(file, "%%="); break;
        case CARET_EQ: fprintf(file, "^="); break;
        case LSHIFT: fprintf(file, "<<"); break;
        case RSHIFT: fprintf(file, ">>"); break;
        case INCREASE: fprintf(file, "++"); break;
        case DECREASE: fprintf(file, "--"); break;
        case LSHIFT_EQ: fprintf(file, "<<="); break;
        case RSHIFT_EQ: fprintf(file, ">>="); break;
        case INTEGER:
        case FLOAT:
        case IDENT:
        case KEYWORD:
            fprintf(file, "%s", tok.data);
            break;
        case COMMENT_LINE: fprintf(file, "//%s", tok.data); break; break;
        case COMMENT_BLOCK: fprintf(file, "/*%s*/", tok.data); break;
        case CHAR: fprintf(file, "'%s'", tok.data); break;
        case STRING: fprintf(file, "\"%s\"", tok.data); break;
        case FSTRING: fprintf(file, "f\"%s\"", tok.data); break;
        case RSTRING: fprintf(file, "r\"%s\"", tok.data); break;
        case SPACE: fprintf(file, " "); break;
        case _INDENT:
        case _DEDENT:
            for (int i = 0; i < tok.len; i++)
                fprintf(file, " ");
            break;
    };
}

void print_node(FILE *file, const Node node, const int index) {
    switch (node.type) {
        case ROOT_NODE: fprintf(file, "root_node"); break;
        case EXPR_NODE: fprintf(file, "expr_node"); break;
        case INDENT_BLOCK: fprintf(file, "indent_block"); break;
        case PAREN_EXPR: fprintf(file, "paren_expr"); break;
        case SQB_EXPR: fprintf(file, "sqb_expr"); break;
        case BRACE_EXPR: fprintf(file, "brace_expr"); break;
        case COMMA_EXPR: fprintf(file, "comma_expr"); break;
        default: fprintf(file, "%d", node.type); break;
    }
    fprintf(file, ": ");
    for (int i = 0; i < node.len; i++)
        print_token_data(file, node.data[i]);
    fprintf(file, "\n");
}

void print_indent(FILE *file, const int n) {
    for (int i = 0; i < n; i++)
        fprintf(file, "    ");
}

void print_nodes(FILE *file, const Node *nodes, const size_t len) {
    for (int i = 0; i < len; i++) {
        print_indent(file, get_generation(nodes, len, i));
        print_node(file, nodes[i], i);
    }
}