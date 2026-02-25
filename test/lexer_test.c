#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "debug.h"
#include "error.h"
#include "file_reader.h"
#include "lexer.h"
#include "token.h"

#define LINE() printf("---------------------------------------------------------------------\n")

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("1 arguments were expected and %d were received\n", (argc - 1));
        return 1;
    }

    size_t pathlen = strlen(argv[1]);
    char *path = malloc(pathlen + 1);
    if (!path) hypex_internal_error();
    memcpy(path, argv[1], pathlen + 1);

    hypex_lexer *lex = hypex_lexer_make();
    lex->file = path;

    size_t len = 0;
    lex->input = hypex_file_read(path, &len);
    lex->inputlen = len;
    lex->cur = lex->input[0];
    hypex_lexer_tokenize(lex);

    LINE();
    printf("%s\n", lex->input);
    LINE();

    print_lexer(lex);
    
    hypex_lexer_free(lex);

    return 0;
}
