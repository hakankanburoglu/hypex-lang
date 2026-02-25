#ifndef ERROR_H
#define ERROR_H

#include <stdnoreturn.h>

noreturn void hypex_fatal_error(const char *format, ...);

void hypex_error(const char *file, int line, int column, const char *format, ...);

void hypex_internal_error(void);

#endif // ERROR_H
