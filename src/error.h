#ifndef ERROR_H
#define ERROR_H

#define ERR_FILE_MSG "no such file or directory"
#define ERR_LEX_MSG "an unexpected lexer error"
#define ERR_CHR_MSG "unexpected character"

void error(const char *format, ...);

void errorp(const int line, const int pos, const char c, const char *format, ...);

#endif //ERROR_H