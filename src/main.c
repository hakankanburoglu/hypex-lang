#include <stdio.h>
#include <string.h>

#include "error.h"
#include "comp.h"

void config(const char *src, const char *out, const char *flags) {
    if (strcmp(flags, "-lex") == 0)
        lex_analysis(src, out);
    else if (strcmp(flags, "-parse") == 0)
        parse_analysis(src, out);
    else if (strcmp(flags, "-c") == 0)
        comp(src);
    else
        error_undef_arg(flags);
}


int main(int argc, char *argv[]) {
    switch (argc) {
        case 2:
            comp(argv[1]);
            break;
        case 3:
            config(argv[1], argv[2], "c");
            break;
        case 4:
            config(argv[1], argv[2], argv[3]);
            break;
        default:
            printf("this feature is not active yet\n");
            break;
    }
    return 0;
}