#include "lexer.h"
#include "file_lexer.h"

#ifdef DEBUG
    #include "debug.h"
#endif

int main(int argc, char *argv[]) {
    if (argc == 2) {
        const char *path = argv[1];
        Lexer *lex = init_lexer(NULL, path);
        lex_file(lex);
        tokenize(lex);
        #ifdef DEBUG
            print_lexer(*lex);
        #endif
        free_lex(lex);
    }
    return 0;
}