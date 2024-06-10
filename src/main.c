#include <stdio.h>

#include "comp.h"

int main(int argc, char *argv[]) {
    if (argc == 2) {
        char *path = argv[1];
        comp(path);
    }
    return 0;
}