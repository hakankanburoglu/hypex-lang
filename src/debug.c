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

void print_token_kind(int kind) {
    switch (kind) {
        case T_UNKNOWN: printf("UNKNOWN"); break;
        case T_EQUAL: printf("EQUAL"); break;
        case T_PLUS: printf("PLUS"); break;
        case T_MINUS: printf("MINUS"); break;
        case T_STAR: printf("STAR"); break;
        case T_SLASH: printf("SLASH"); break;
        case T_LESS: printf("LESS"); break;
        case T_GREATER: printf("GREATER"); break;
        case T_AMPER: printf("AMPER"); break;
        case T_PIPE: printf("PIPE"); break;
        case T_EXCLAMATION: printf("EXCLAMATION"); break;
        case T_PERCENT: printf("PERCENT"); break;
        case T_DOT: printf("DOT"); break;
        case T_COMMA: printf("COMMA"); break;
        case T_COLON: printf("COLON"); break;
        case T_SEMI: printf("SEMI"); break;
        case T_UNDERSCORE: printf("UNDERSCORE"); break;
        case T_QUEST: printf("QUEST"); break;
        case T_LPAR: printf("LPAR"); break;
        case T_RPAR: printf("RPAR"); break;
        case T_LSQB: printf("LSQB"); break;
        case T_RSQB: printf("RSQB"); break;
        case T_LBRACE: printf("LBRACE"); break;
        case T_RBRACE: printf("RBRACE"); break;
        case T_CARET: printf("CARET"); break;
        case T_TILDE: printf("TILDE"); break;
        case T_AT: printf("AT"); break;
        case T_HASH: printf("HASH"); break;
        case T_ESCAPE: printf("ESCAPE"); break;
        case T_TWO_EQ: printf("TWO_EQ"); break;
        case T_PLUS_EQ: printf("PLUS_EQ"); break;
        case T_MINUS_EQ: printf("MINUS_EQ"); break;
        case T_STAR_EQ: printf("STAR_EQ"); break;
        case T_SLASH_EQ: printf("SLASH_EQ"); break;
        case T_LESS_EQ: printf("LESS_EQ"); break;
        case T_GREATER_EQ: printf("GREATER_EQ"); break;
        case T_AMPER_EQ: printf("AMPER_EQ"); break;
        case T_TWO_AMPER: printf("TWO_AMPER"); break;
        case T_PIPE_EQ: printf("PIPE_EQ"); break;
        case T_TWO_PIPE: printf("TWO_PIPE"); break;
        case T_EXCLAMATION_EQ: printf("EXCLAMATION_EQ"); break;
        case T_PERCENT_EQ: printf("PERCENT_EQ"); break;
        case T_CARET_EQ: printf("CARET_EQ"); break;
        case T_LSHIFT: printf("LSHIFT"); break;
        case T_RSHIFT: printf("RSHIFT"); break;
        case T_INCREASE: printf("INCREASE"); break;
        case T_DECREASE: printf("DECREASE"); break;
        case T_TWO_DOT: printf("TWO_DOT"); break;
        case T_LSHIFT_EQ: printf("LSHIFT_EQ"); break;
        case T_RSHIFT_EQ: printf("RSHIFT_EQ"); break;
        case T_ELLIPSIS: printf("ELLIPSIS"); break;
        case T_INTEGER: printf("INTEGER"); break;
        case T_FLOAT: printf("FLOAT"); break;
        case T_IDENT: printf("IDENT"); break;
        case T_KEYWORD: printf("KEYWORD"); break;
        case T_COMMENT_LINE: printf("COMMENT_LINE"); break;
        case T_COMMENT_BLOCK: printf("COMMENT_BLOCK"); break;
        case T_CHAR: printf("CHAR"); break;
        case T_STRING: printf("STRING"); break;
        case T_FSTRING_START: printf("FSTRING_START"); break;
        case T_FSTRING_BODY: printf("FSTRING_BODY"); break;
        case T_FSTRING_END: printf("FSTRING_END"); break;
        case T_RSTRING: printf("RSTRING"); break;
        case T_SPACE: printf("SPACE"); break;
        case T_INDENT: printf("INDENT"); break;
        case T_DEDENT: printf("DEDENT"); break;
        case T_NEWLINE: printf("NEWLINE"); break;
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
        case BASE_DEC: printf("dec"); break;
        case BASE_HEX: printf("hex"); break;
        case BASE_OCT: printf("oct"); break;
        case BASE_BIN: printf("bin"); break;
        default: printf("_base_%d", base); break;
    }
}

void print_token(Token tok) {
    printf("%d:%d ", tok.pos.line, tok.pos.column);
    tok.kind != T_KEYWORD ? print_token_kind(tok.kind) : print_keyword(tok.id);
    if (tok.value) printf(" `%s`", tok.value);
    if (tok.kind == T_INTEGER || tok.kind == T_FLOAT) {
        if (tok.num.base != 0) {
            printf(" base:");
            print_base(tok.num.base);
        }
        if (tok.num.is_exp) {
            if (tok.num.is_neg) printf(" (neg_exp)");
            else printf(" (exp)");
        }
    }
    if (tok.kind == T_NEWLINE && tok.comment) printf(" (comment)");
    if (tok.kind == T_INDENT || tok.kind == T_DEDENT) printf(" level:%d", tok.level);
    printf(" len:%d\n", tok.len);
}

void print_lexer(Lexer lex) {
    for (int i = 0; i < lex.tokens.len; i++)
        print_token(*(lex.tokens.list[i]));
    printf("\nresults: total=%d ind=", lex.tokens.len);
    if (lex.indents.stack) {
        printf("{");
        for (int i = 0; i < lex.indents.len; i++) {
            printf("%d", lex.indents.stack[i]);
            if (i != lex.indents.len - 1) printf(", ");
        }
        printf("}");
    } else {
        printf("null");
    }
    printf("\n\n");
}

void print_node_kind(int kind) {
    switch (kind) {
        case NODE_SOURCE: printf("source"); break;
        case NODE_EXPR: printf("expr"); break;
        case NODE_BLOCK: printf("block"); break;
        case NODE_UNARY_OP: printf("unary_op"); break;
        case NODE_BINARY_OP: printf("binary_op"); break;
        case NODE_TERNARY_OP: printf("ternary_op"); break;
        case NODE_LITERAL: printf("literal"); break;
        case NODE_IDENT: printf("ident"); break;
        case NODE_PAREN_GROUP: printf("paren_group"); break;
        case NODE_FUNC_DECL: printf("func_decl"); break;
        case NODE_ARG_DECL: printf("arg_decl"); break;
        case NODE_RET_STMT: printf("ret_stmt"); break;
        case NODE_VAR_DECL: printf("var_decl"); break;
        case NODE_CALL_EXPR: printf("call_expr"); break;
        default: printf("_node_%d", kind); break;
    }
}

void print_op(int op) {
    switch (op) {
        case OP_NOT: printf("not"); break;
        case OP_BIT_NOT: printf("bit_not"); break;
        case OP_NEG: printf("neg"); break;
        case OP_PRE_INC: printf("pre_inc"); break;
        case OP_PRE_DEC: printf("pre_dec"); break;
        case OP_AMPER: printf("amper"); break;
        case OP_POST_INC: printf("post_inc"); break;
        case OP_POST_DEC: printf("post_dec"); break;
        case OP_ACCESS: printf("access"); break;
        case OP_ASSIGN: printf("assign"); break;
        case OP_ADD: printf("add"); break;
        case OP_SUB: printf("sub"); break;
        case OP_MUL: printf("mul"); break;
        case OP_DIV: printf("div"); break;
        case OP_MOD: printf("mod"); break;
        case OP_A_ADD: printf("a_add"); break;
        case OP_A_SUB: printf("a_sub"); break;
        case OP_A_MUL: printf("a_mul"); break;
        case OP_A_DIV: printf("a_div"); break;
        case OP_A_MOD: printf("a_mod"); break;
        case OP_EQ: printf("eq"); break;
        case OP_NE: printf("ne"); break;
        case OP_LT: printf("lt"); break;
        case OP_GT: printf("gt"); break;
        case OP_LE: printf("le"); break;
        case OP_GE: printf("ge"); break;
        case OP_BIT_AND: printf("bit_and"); break;
        case OP_BIT_OR: printf("bit_or"); break;
        case OP_AND: printf("and"); break;
        case OP_OR: printf("or"); break;
        case OP_XOR: printf("xor"); break;
        case OP_SHL: printf("shl"); break;
        case OP_SHR: printf("shr"); break;
        case OP_A_AND: printf("a_and"); break;
        case OP_A_OR: printf("a_or"); break;
        case OP_A_XOR: printf("a_xor"); break;
        case OP_A_SHL: printf("a_shl"); break;
        case OP_A_SHR: printf("a_shr"); break;
        case OP_COND: printf("cond"); break;
        default: printf("_op_%d", op); break;
    }
}

void print_type_kind(int kind) {
    switch (kind) {
        case TYPE_NOTYPE: printf("notype"); break;
        case TYPE_IDENT: printf("ident"); break;
        case TYPE_INT: printf("int"); break;
        case TYPE_LONG: printf("long"); break;
        case TYPE_SHORT: printf("short"); break;
        case TYPE_FLOAT: printf("float"); break;
        case TYPE_DOUBLE: printf("double"); break;
        case TYPE_CHAR: printf("char"); break;
        case TYPE_UINT: printf("uint"); break;
        case TYPE_ULONG: printf("ulong"); break;
        case TYPE_USHORT: printf("ushort"); break;
        case TYPE_BOOL: printf("bool"); break;
        case TYPE_STRING: printf("string"); break;
        case TYPE_OBJECT: printf("object"); break;
        case TYPE_FUNC: printf("func"); break;
        case TYPE_ARRAY: printf("array"); break;
        case TYPE_FIXED_ARRAY: printf("fixed_array"); break;
        case TYPE_TUPLE: printf("tuple"); break;
        case TYPE_STRUCT: printf("struct"); break;
        case TYPE_CLASS: printf("class"); break;
        case TYPE_IFACE: printf("iface"); break;
        case TYPE_GENERIC: printf("generic"); break;
        default: printf("_type_%d", kind);
    }
}

static inline void print_indent(int level) {
    for (int i = 0; i < level; i++)
        printf("    ");
}

static inline int has_flags(Type *type, int flags) {
    return (type->flags & flags) == flags;
}

void print_type(Type *type) {
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

void print_node(Node *node, int level, const char *tag) {
    print_indent(level);
    if (tag) printf("%s: ", tag);
    if (!node) {
        printf("null");
        return;
    }
    print_node_kind(node->kind);
    printf(":");
    switch (node->kind) {
        case NODE_SOURCE: case NODE_BLOCK:
            printf("\n");
            for (int i = 0; i < node->data.block.len; i++)
                print_node(node->data.block.body[i], level + 1, NULL);
            break;
        case NODE_EXPR: case NODE_RET_STMT:
            printf("\n");
            print_node(node->data.expr, level + 1, "expr");
            break;
        case NODE_UNARY_OP:
            printf(" op=");
            print_op(node->data.unary_op.op);
            printf(", prefix=");
            node->data.unary_op.prefix ? printf("true") : printf("false");
            printf("\n");
            print_node(node->data.unary_op.operand, level + 1, "operand");
            break;
        case NODE_BINARY_OP:
            printf(" op=");
            print_op(node->data.binary_op.op);
            printf("\n");
            print_node(node->data.binary_op.left, level + 1, "left");
            print_node(node->data.binary_op.right, level + 1, "right");
            break;
        case NODE_TERNARY_OP:
            printf("\n");
            print_node(node->data.ternary_op.cond, level + 1, "cond");
            print_node(node->data.ternary_op.if_body, level + 1, "if");
            print_node(node->data.ternary_op.else_body, level + 1, "else");
            break;
        case NODE_LITERAL: case NODE_IDENT:
            printf(" value=\"%s\"\n", node->data.value->value);
            break;
        case NODE_FUNC_DECL:
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
        case NODE_VAR_DECL: case NODE_ARG_DECL:
            printf(" ident=`%s` type=", node->data.var_decl.ident->value);
            print_type(node->data.var_decl.type);
            printf("\n");
            print_node(node->data.var_decl.init, level + 1, "init");
            break;
        case NODE_CALL_EXPR:
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

void print_parser(Parser p) {
    p.expr ? print_node(p.expr, 0, NULL) : printf("null expr");
}
