#ifndef ERROR_H
#define ERROR_H

void error_hypex();

void error_file(const char *file);

void error_file_proc(const char *file);

void error_file_read(const char *file);

void error_char(const char *file, int line, int column, char c);

void error_number(const char *file, int line, int column, const char *value);

void error_indent(const char *file, int line, int column);

void error_expect(const char *file, int line, int column, const char *expect);

#endif //ERROR_H