#ifndef ERROR_H
#define ERROR_H

#include <stdnoreturn.h>

noreturn void fatal_error(const char *format, ...);

void error(const char *file, int line, int column, const char *format, ...);

void internal_error(void);

#endif // ERROR_H
