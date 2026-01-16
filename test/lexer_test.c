#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "debug.h"
#include "error.h"
#include "file_reader.h"
#include "lexer.h"
#include "parser.h"
#include "token.h"

#define LINE() printf("---------------------------------------------------------------------\n")

int main(int argc, char *argv[]) {

    if (argc != 2) {
        printf("1 arguments were expected and %d were received\n", (argc - 1));
        return 1;
    }

    Lexer *lex = make_lexer();

    size_t pathlen = strlen(argv[1]);
    char *path = malloc(pathlen + 1);
    if (!path) internal_error();
    memcpy(path, argv[1], pathlen + 1);

    lex->file = path;

    size_t len = 0;
    lex->input = file_read(path, &len);
    lex->inputlen = len;
    tokenize(lex);

    LINE();
    printf("%s\n", lex->input);
    LINE();

    print_lexer(*lex);
    
    free_lex(lex);

    return 0;
}
