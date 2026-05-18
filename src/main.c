#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"
#include "parser.h"
#include "codegen.h"
#include "tacker.h"

int main(int argc, char** argv) {
    if (argc != 2 || strlen(argv[1]) < 2 || argv[1][strlen(argv[1])-1] != 'c' || argv[1][strlen(argv[1])-2] != '.') {
        printf("Usage: rcc <path/to/code.c>\n");
        return EXIT_FAILURE;
    }

    FILE *file = fopen(argv[1], "r");
    Tokens *toks = lexer(file);

    AST_PROG prog = parse(toks);
    
    TackerProg tacker_prog = generate_tacker(&prog);
    printf("Successfully generated TACKY IR! Generated %d instructions.\n", tacker_prog.func.count);

    ASM_PROG asm_prog = generate_assembly(&tacker_prog);
    replace_pseudo_registers(&asm_prog);
    fix_instructions(&asm_prog);

    FILE* output_file = fopen("assembly.s", "w");
    emit_assembly(&asm_prog, output_file);
    fclose(output_file);

    system("gcc -o return assembly.s");
    printf("Successful!\n");

    return 0;
}
