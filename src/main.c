#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"
#include "parser.h"


int main(int argc, char** argv) {
    if (argc != 2 || strlen(argv[1]) < 2 || argv[1][strlen(argv[1])-1] != 'c' || argv[1][strlen(argv[1])-2] != '.') {
        printf("Usage: rcc <path/to/code.c>\n");
        return EXIT_FAILURE;
    }

    FILE *file = fopen(argv[1], "r");
    Tokens *toks = lexer(file);

    AST_PROG prog = parse(toks);

    return 0;
}
