#include <stdio.h>
#include <stdlib.h>

typedef enum {
    EXIT,
    INT_LITERAL,
    OPAREN,
    CPAREN,
    RETURN,
    SEMI_COLON,
    OPEN_CURL,
    CLOSE_CURL,
} TokenType;

typedef struct {
    TokenType type;
    char* value;
} Token;

void lexer(FILE *file) {
    char curr_char = fgetc(file);

    while(curr_char != EOF) {
        
        printf("%c", curr_char);
        curr_char = fgetc(file);
    }
}

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Usage: egie <path/to/code.egg>\n");
        return EXIT_FAILURE;
    }

    FILE *file = fopen(argv[1], "r");
    lexer(file);
    return 0;
}
