#include <stdnoreturn.h>

noreturn void fatal_error(const char *format, ...);

void error(const char *file, int line, int column, const char *format, ...);

void error_hypex();
