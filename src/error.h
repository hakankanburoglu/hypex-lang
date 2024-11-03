#ifndef ERROR_H
#define ERROR_H

#define ERR_UNDEF_ARG_MSG "undefined argument"
#define ERR_FILE_MSG "no such file or directory"
#define ERR_INDENT_MSG "undefined indentation"
#define ERR_CHR_MSG "unexpected character"
#define ERR_LEX_MSG "an unexpected lexer error"
#define ERR_PARSE_MSG "an unexpected parser error"
#define ERR_AST_PARSE_MSG "an unexpected ast parser error"

void error_undef_arg(const char *arg);

void error_file(const char *path);

void error_indent(const int line, const int pos);

void error_char(const int line, const int col, const char c);

void error_lex(const char *code);

void error_parse(const char *code);

void error_ast_parse(const char *code);

#endif // ERROR_H