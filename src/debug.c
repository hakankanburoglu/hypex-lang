#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "ast.h"
#include "debug.h"
#include "lexer.h"
#include "parser.h"
#include "token.h"

static inline const char *file_name(const char *file) {
    const char *res = strrchr(file, '/'); // for unix
    if (!res) res = strrchr(file, '\\'); // for non-unix
    return res ? res + 1 : file;
}

void print_token_kind(hypex_token_kind kind) {
    switch (kind) {
        case HYPEX_TOK_UNKNOWN: printf("UNKNOWN"); break;
        case HYPEX_TOK_EQUAL: printf("EQUAL"); break;
        case HYPEX_TOK_PLUS: printf("PLUS"); break;
        case HYPEX_TOK_MINUS: printf("MINUS"); break;
        case HYPEX_TOK_STAR: printf("STAR"); break;
        case HYPEX_TOK_SLASH: printf("SLASH"); break;
        case HYPEX_TOK_LESS: printf("LESS"); break;
        case HYPEX_TOK_GREATER: printf("GREATER"); break;
        case HYPEX_TOK_AMPER: printf("AMPER"); break;
        case HYPEX_TOK_PIPE: printf("PIPE"); break;
        case HYPEX_TOK_EXCLAMATION: printf("EXCLAMATION"); break;
        case HYPEX_TOK_PERCENT: printf("PERCENT"); break;
        case HYPEX_TOK_DOT: printf("DOT"); break;
        case HYPEX_TOK_COMMA: printf("COMMA"); break;
        case HYPEX_TOK_COLON: printf("COLON"); break;
        case HYPEX_TOK_SEMI: printf("SEMI"); break;
        case HYPEX_TOK_UNDERSCORE: printf("UNDERSCORE"); break;
        case HYPEX_TOK_QUEST: printf("QUEST"); break;
        case HYPEX_TOK_LPAR: printf("LPAR"); break;
        case HYPEX_TOK_RPAR: printf("RPAR"); break;
        case HYPEX_TOK_LSQB: printf("LSQB"); break;
        case HYPEX_TOK_RSQB: printf("RSQB"); break;
        case HYPEX_TOK_LBRACE: printf("LBRACE"); break;
        case HYPEX_TOK_RBRACE: printf("RBRACE"); break;
        case HYPEX_TOK_CARET: printf("CARET"); break;
        case HYPEX_TOK_TILDE: printf("TILDE"); break;
        case HYPEX_TOK_AT: printf("AT"); break;
        case HYPEX_TOK_HASH: printf("HASH"); break;
        case HYPEX_TOK_ESCAPE: printf("ESCAPE"); break;
        case HYPEX_TOK_TWO_EQ: printf("TWO_EQ"); break;
        case HYPEX_TOK_PLUS_EQ: printf("PLUS_EQ"); break;
        case HYPEX_TOK_MINUS_EQ: printf("MINUS_EQ"); break;
        case HYPEX_TOK_STAR_EQ: printf("STAR_EQ"); break;
        case HYPEX_TOK_SLASH_EQ: printf("SLASH_EQ"); break;
        case HYPEX_TOK_LESS_EQ: printf("LESS_EQ"); break;
        case HYPEX_TOK_GREATER_EQ: printf("GREATER_EQ"); break;
        case HYPEX_TOK_AMPER_EQ: printf("AMPER_EQ"); break;
        case HYPEX_TOK_TWO_AMPER: printf("TWO_AMPER"); break;
        case HYPEX_TOK_PIPE_EQ: printf("PIPE_EQ"); break;
        case HYPEX_TOK_TWO_PIPE: printf("TWO_PIPE"); break;
        case HYPEX_TOK_EXCLAMATION_EQ: printf("EXCLAMATION_EQ"); break;
        case HYPEX_TOK_PERCENT_EQ: printf("PERCENT_EQ"); break;
        case HYPEX_TOK_CARET_EQ: printf("CARET_EQ"); break;
        case HYPEX_TOK_LSHIFT: printf("LSHIFT"); break;
        case HYPEX_TOK_RSHIFT: printf("RSHIFT"); break;
        case HYPEX_TOK_INCREASE: printf("INCREASE"); break;
        case HYPEX_TOK_DECREASE: printf("DECREASE"); break;
        case HYPEX_TOK_TWO_DOT: printf("TWO_DOT"); break;
        case HYPEX_TOK_LSHIFT_EQ: printf("LSHIFT_EQ"); break;
        case HYPEX_TOK_RSHIFT_EQ: printf("RSHIFT_EQ"); break;
        case HYPEX_TOK_ELLIPSIS: printf("ELLIPSIS"); break;
        case HYPEX_TOK_INTEGER: printf("INTEGER"); break;
        case HYPEX_TOK_FLOAT: printf("FLOAT"); break;
        case HYPEX_TOK_IDENT: printf("IDENT"); break;
        case HYPEX_TOK_KEYWORD: printf("KEYWORD"); break;
        case HYPEX_TOK_COMMENT_LINE: printf("COMMENT_LINE"); break;
        case HYPEX_TOK_COMMENT_BLOCK: printf("COMMENT_BLOCK"); break;
        case HYPEX_TOK_CHAR: printf("CHAR"); break;
        case HYPEX_TOK_STRING: printf("STRING"); break;
        case HYPEX_TOK_FSTRING_START: printf("FSTRING_START"); break;
        case HYPEX_TOK_FSTRING_BODY: printf("FSTRING_BODY"); break;
        case HYPEX_TOK_FSTRING_END: printf("FSTRING_END"); break;
        case HYPEX_TOK_RSTRING: printf("RSTRING"); break;
        case HYPEX_TOK_SPACE: printf("SPACE"); break;
        case HYPEX_TOK_INDENT: printf("INDENT"); break;
        case HYPEX_TOK_DEDENT: printf("DEDENT"); break;
        case HYPEX_TOK_NEWLINE: printf("NEWLINE"); break;
        case HYPEX_TOK_EOF: printf("EOF"); break;
        default: printf("_T_%d", kind); break;
    }
}

void print_keyword(hypex_keyword keyword) {
    switch (keyword) {
        case HYPEX_KW_ASYNC: printf("KW_ASYNC"); break;
        case HYPEX_KW_AWAIT: printf("KW_AWAIT"); break;
        case HYPEX_KW_BASE: printf("KW_BASE"); break;
        case HYPEX_KW_BOOL: printf("KW_BOOL"); break;
        case HYPEX_KW_BREAK: printf("KW_BREAK"); break;
        case HYPEX_KW_BYTE: printf("KW_BYTE"); break;
        case HYPEX_KW_CASE: printf("KW_CASE"); break;
        case HYPEX_KW_CATCH: printf("KW_CATCH"); break;
        case HYPEX_KW_CHAR: printf("KW_CHAR"); break;
        case HYPEX_KW_CLASS: printf("KW_CLASS"); break;
        case HYPEX_KW_CONST: printf("KW_CONST"); break;
        case HYPEX_KW_CONTINUE: printf("KW_CONTINUE"); break;
        case HYPEX_KW_DOUBLE: printf("KW_DOUBLE"); break;
        case HYPEX_KW_ELIF: printf("KW_ELIF"); break;
        case HYPEX_KW_ELSE: printf("KW_ELSE"); break;
        case HYPEX_KW_ENUM: printf("KW_ENUM"); break;
        case HYPEX_KW_FALSE: printf("KW_FALSE"); break;
        case HYPEX_KW_FLOAT: printf("KW_FLOAT"); break;
        case HYPEX_KW_FOR: printf("KW_FOR"); break;
        case HYPEX_KW_FUNC: printf("KW_FUNC"); break;
        case HYPEX_KW_IF: printf("KW_IF"); break;
        case HYPEX_KW_INT: printf("KW_INT"); break;
        case HYPEX_KW_INTERFACE: printf("KW_INTERFACE"); break;
        case HYPEX_KW_LONG: printf("KW_LONG"); break;
        case HYPEX_KW_NULL: printf("KW_NULL"); break;
        case HYPEX_KW_OBJECT: printf("KW_OBJECT"); break;
        case HYPEX_KW_RETURN: printf("KW_RETURN"); break;
        case HYPEX_KW_SBYTE: printf("KW_SBYTE"); break;
        case HYPEX_KW_SELF: printf("KW_SELF"); break;
        case HYPEX_KW_SHORT: printf("KW_SHORT"); break;
        case HYPEX_KW_STRING: printf("KW_STRING"); break;
        case HYPEX_KW_STRUCT: printf("KW_STRUCT"); break;
        case HYPEX_KW_SWITCH: printf("KW_SWITCH"); break;
        case HYPEX_KW_THROW: printf("KW_THROW"); break;
        case HYPEX_KW_TRUE: printf("KW_TRUE"); break;
        case HYPEX_KW_TRY: printf("KW_TRY"); break;
        case HYPEX_KW_UINT: printf("KW_UINT"); break;
        case HYPEX_KW_ULONG: printf("KW_ULONG"); break;
        case HYPEX_KW_USE: printf("KW_USE"); break;
        case HYPEX_KW_USHORT: printf("KW_USHORT"); break;
        case HYPEX_KW_VAR: printf("KW_VAR"); break;
        case HYPEX_KW_WHILE: printf("KW_WHILE"); break;
        case HYPEX_KW_YIELD: printf("KW_YIELD"); break;
        default: printf("_KW_%d", keyword); break;
    }
}

void print_base(hypex_num_base base) {
    switch (base) {
        case HYPEX_NUM_BASE_DEC: printf("dec"); break;
        case HYPEX_NUM_BASE_HEX: printf("hex"); break;
        case HYPEX_NUM_BASE_OCT: printf("oct"); break;
        case HYPEX_NUM_BASE_BIN: printf("bin"); break;
        default: printf("_base_%d", base); break;
    }
}

void print_token(const hypex_token *tok) {
    if (!tok) {
        printf("null\n");
        return;
    }
    printf("%d:%d ", tok->pos.line, tok->pos.column);
    tok->kind != HYPEX_TOK_KEYWORD ? print_token_kind(tok->kind) : print_keyword(tok->id);
    if (tok->value) printf(" `%s`", tok->value);
    if (tok->kind == HYPEX_TOK_INTEGER || tok->kind == HYPEX_TOK_FLOAT) {
        if (tok->num.base != 0) {
            printf(" base:");
            print_base(tok->num.base);
        }
        if (tok->num.is_exp) {
            if (tok->num.is_neg) printf(" (neg_exp)");
            else printf(" (exp)");
        }
    }
    if (tok->kind == HYPEX_TOK_NEWLINE && tok->comment) printf(" (comment)");
    if (tok->kind == HYPEX_TOK_INDENT || tok->kind == HYPEX_TOK_DEDENT) printf(" level:%d", tok->level);
    printf(" len:%zu\n", tok->len);
}

void print_lexer(const hypex_lexer *lex) {
    if (!lex) {
        printf("\n");
        return;
    }
    for (size_t i = 0; i < lex->tokens.len; i++)
        print_token(lex->tokens.list[i]);
    printf("\nresults: total=%zu ind=", lex->tokens.len);
    if (lex->indents.stack) {
        printf("{");
        for (size_t i = 0; i < lex->indents.len; i++) {
            printf("%d", lex->indents.stack[i]);
            if (i != lex->indents.len - 1) printf(", ");
        }
        printf("}");
    } else {
        printf("null");
    }
    printf("\n\n");
}

void print_node_kind(hypex_node_kind kind) {
    switch (kind) {
        case HYPEX_NODE_SOURCE: printf("source"); break;
        case HYPEX_NODE_EXPR: printf("expr"); break;
        case HYPEX_NODE_BLOCK: printf("block"); break;
        case HYPEX_NODE_UNARY_OP: printf("unary_op"); break;
        case HYPEX_NODE_BINARY_OP: printf("binary_op"); break;
        case HYPEX_NODE_TERNARY_OP: printf("ternary_op"); break;
        case HYPEX_NODE_LITERAL: printf("literal"); break;
        case HYPEX_NODE_IDENT: printf("ident"); break;
        case HYPEX_NODE_PAREN_GROUP: printf("paren_group"); break;
        case HYPEX_NODE_FUNC_DECL: printf("func_decl"); break;
        case HYPEX_NODE_ARG_DECL: printf("arg_decl"); break;
        case HYPEX_NODE_RET_STMT: printf("ret_stmt"); break;
        case HYPEX_NODE_VAR_DECL: printf("var_decl"); break;
        case HYPEX_NODE_CALL_EXPR: printf("call_expr"); break;
        default: printf("_node_%d", kind); break;
    }
}

void print_op(hypex_op op) {
    switch (op) {
        case HYPEX_OP_NOT: printf("not"); break;
        case HYPEX_OP_BIT_NOT: printf("bit_not"); break;
        case HYPEX_OP_NEG: printf("neg"); break;
        case HYPEX_OP_AMPER: printf("amper"); break;
        case HYPEX_OP_PRE_INC: printf("pre_inc"); break;
        case HYPEX_OP_PRE_DEC: printf("pre_dec"); break;
        case HYPEX_OP_POST_INC: printf("post_inc"); break;
        case HYPEX_OP_POST_DEC: printf("post_dec"); break;
        case HYPEX_OP_ACCESS: printf("access"); break;
        case HYPEX_OP_ASSIGN: printf("assign"); break;
        case HYPEX_OP_ADD: printf("add"); break;
        case HYPEX_OP_SUB: printf("sub"); break;
        case HYPEX_OP_MUL: printf("mul"); break;
        case HYPEX_OP_DIV: printf("div"); break;
        case HYPEX_OP_MOD: printf("mod"); break;
        case HYPEX_OP_A_ADD: printf("a_add"); break;
        case HYPEX_OP_A_SUB: printf("a_sub"); break;
        case HYPEX_OP_A_MUL: printf("a_mul"); break;
        case HYPEX_OP_A_DIV: printf("a_div"); break;
        case HYPEX_OP_A_MOD: printf("a_mod"); break;
        case HYPEX_OP_EQ: printf("eq"); break;
        case HYPEX_OP_NE: printf("ne"); break;
        case HYPEX_OP_LT: printf("lt"); break;
        case HYPEX_OP_GT: printf("gt"); break;
        case HYPEX_OP_LE: printf("le"); break;
        case HYPEX_OP_GE: printf("ge"); break;
        case HYPEX_OP_BIT_AND: printf("bit_and"); break;
        case HYPEX_OP_BIT_OR: printf("bit_or"); break;
        case HYPEX_OP_AND: printf("and"); break;
        case HYPEX_OP_OR: printf("or"); break;
        case HYPEX_OP_XOR: printf("xor"); break;
        case HYPEX_OP_SHL: printf("shl"); break;
        case HYPEX_OP_SHR: printf("shr"); break;
        case HYPEX_OP_A_AND: printf("a_and"); break;
        case HYPEX_OP_A_OR: printf("a_or"); break;
        case HYPEX_OP_A_XOR: printf("a_xor"); break;
        case HYPEX_OP_A_SHL: printf("a_shl"); break;
        case HYPEX_OP_A_SHR: printf("a_shr"); break;
        case HYPEX_OP_COND: printf("cond"); break;
        default: printf("_op_%d", op); break;
    }
}

void print_type_kind(hypex_type_kind kind) {
    switch (kind) {
        case HYPEX_TYPE_NOTYPE: printf("notype"); break;
        case HYPEX_TYPE_IDENT: printf("ident"); break;
        case HYPEX_TYPE_INT: printf("int"); break;
        case HYPEX_TYPE_LONG: printf("long"); break;
        case HYPEX_TYPE_SHORT: printf("short"); break;
        case HYPEX_TYPE_FLOAT: printf("float"); break;
        case HYPEX_TYPE_DOUBLE: printf("double"); break;
        case HYPEX_TYPE_CHAR: printf("char"); break;
        case HYPEX_TYPE_UINT: printf("uint"); break;
        case HYPEX_TYPE_ULONG: printf("ulong"); break;
        case HYPEX_TYPE_USHORT: printf("ushort"); break;
        case HYPEX_TYPE_BOOL: printf("bool"); break;
        case HYPEX_TYPE_STRING: printf("string"); break;
        case HYPEX_TYPE_OBJECT: printf("object"); break;
        case HYPEX_TYPE_FUNC: printf("func"); break;
        case HYPEX_TYPE_ARRAY: printf("array"); break;
        case HYPEX_TYPE_FIXED_ARRAY: printf("fixed_array"); break;
        case HYPEX_TYPE_TUPLE: printf("tuple"); break;
        case HYPEX_TYPE_STRUCT: printf("struct"); break;
        case HYPEX_TYPE_CLASS: printf("class"); break;
        case HYPEX_TYPE_IFACE: printf("iface"); break;
        case HYPEX_TYPE_GENERIC: printf("generic"); break;
        default: printf("_type_%d", kind);
    }
}

static inline void print_indent(int level) {
    for (int i = 0; i < level; i++)
        printf("    ");
}

static inline int has_flags(hypex_type *type, int flags) {
    return (type->flags & flags) == flags;
}

void print_type(hypex_type *type) {
    if (!type) {
        printf("null");
        return;
    }
    printf("<");
    if (has_flags(type, FLAG_CONST)) printf("const ");
    if (has_flags(type, FLAG_STAT)) printf("stat ");
    if (has_flags(type, FLAG_PRIV)) printf("priv ");
    if (has_flags(type, FLAG_PROT)) printf("prot ");
    if (has_flags(type, FLAG_REF)) printf("ref ");
    print_type_kind(type->kind);
    if (type->types) {
        printf(" ");
        for (int i = 0; i < type->len; i++)
            print_type(type->types[i]);
    }
    printf(">");
}

void print_node(hypex_node *node, int level, const char *tag) {
    print_indent(level);
    if (tag) printf("%s: ", tag);
    if (!node) {
        printf("null");
        return;
    }
    print_node_kind(node->kind);
    printf(":");
    switch (node->kind) {
        case HYPEX_NODE_SOURCE: case HYPEX_NODE_BLOCK:
            printf("\n");
            for (int i = 0; i < node->data.block.len; i++)
                print_node(node->data.block.body[i], level + 1, NULL);
            break;
        case HYPEX_NODE_EXPR: case HYPEX_NODE_RET_STMT:
            printf("\n");
            print_node(node->data.expr, level + 1, "expr");
            break;
        case HYPEX_NODE_UNARY_OP:
            printf(" op=");
            print_op(node->data.unary_op.op);
            printf(", prefix=");
            node->data.unary_op.prefix ? printf("true") : printf("false");
            printf("\n");
            print_node(node->data.unary_op.operand, level + 1, "operand");
            break;
        case HYPEX_NODE_BINARY_OP:
            printf(" op=");
            print_op(node->data.binary_op.op);
            printf("\n");
            print_node(node->data.binary_op.left, level + 1, "left");
            print_node(node->data.binary_op.right, level + 1, "right");
            break;
        case HYPEX_NODE_TERNARY_OP:
            printf("\n");
            print_node(node->data.ternary_op.cond, level + 1, "cond");
            print_node(node->data.ternary_op.if_body, level + 1, "if");
            print_node(node->data.ternary_op.else_body, level + 1, "else");
            break;
        case HYPEX_NODE_LITERAL: case HYPEX_NODE_IDENT:
            printf(" value=\"%s\"\n", node->data.value->value);
            break;
        case HYPEX_NODE_FUNC_DECL:
            printf(" ident=`%s` type=", node->data.func_decl.ident->value);
            print_type(node->data.func_decl.type);
            printf("\n");
            if (node->data.func_decl.args_len > 0) {
                print_indent(level + 1);
                printf("args:\n");
                for (int i = 0; i < node->data.func_decl.args_len; i++) {
                    print_node(node->data.func_decl.args[i], level + 2, NULL);
                    printf("\n");
                }
            }
            print_node(node->data.func_decl.body, level + 1, "body");
            break;
        case HYPEX_NODE_VAR_DECL: case HYPEX_NODE_ARG_DECL:
            printf(" ident=`%s` type=", node->data.var_decl.ident->value);
            print_type(node->data.var_decl.type);
            printf("\n");
            print_node(node->data.var_decl.init, level + 1, "init");
            break;
        case HYPEX_NODE_CALL_EXPR:
            printf(" callee=`%s`\n", node->data.call_expr.callee);
            for (int i = 0; i < node->data.call_expr.args_len; i++) {
                print_node(node->data.call_expr.args[i], level + 1, "arg");
                printf("\n");
            }
            break;
        default:
            break;
    }
    if (level == 0) printf("\n");
}

void print_parser(hypex_parser p) {
    p.expr ? print_node(p.expr, 0, NULL) : printf("null expr");
}
