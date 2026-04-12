#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    EXIT,
    INT_LITERAL,
    INT_KEYWORD,
    OPAREN,
    CPAREN,
    RETURN,
    SEMI_COLON,
    OPEN_CURL,
    CLOSE_CURL,
    MAIN,
    VOID,
} TokenType;

typedef struct {
    TokenType type;
    char* value;
} Token;

typedef struct {
    Token* tokens;
    int count;
    int capacity;
} Tokens;

#define token_append(ts, a)\
    do {\
        if(ts->count >= ts->capacity) {\
            if(ts->capacity == 0) ts->capacity = 256;\
            else ts->capacity *= 2;\
            ts->tokens = realloc(ts->tokens, ts->capacity*sizeof(*(ts->tokens)));\
        }\
        ts->tokens[ts->count++] = a;\
    } while(0)
        

Tokens* lexer(FILE *file) {
    char curr_char = fgetc(file);
    Tokens* tokens = malloc(sizeof(Tokens));
    tokens->tokens = NULL;
    tokens->count = 0;
    tokens->capacity = 0;
    while(curr_char != EOF) {
        char buffer[256] = {0};
        int len = 0;
        if(isspace(curr_char)){
            curr_char = fgetc(file);
            continue;
        }
        if (isdigit(curr_char)) {
            buffer[len++] = curr_char;
            while(isalnum(curr_char = fgetc(file))){
                if(isalpha(curr_char)){
                    printf("error in enumerating digit\n");
                    return tokens;
                }
                buffer[len++] = curr_char;
            }
            Token token = {INT_LITERAL, strdup(buffer)};
            token_append(tokens, token);
        }
        else if(isalpha(curr_char)) {
            buffer[len++] = curr_char;
            while(isalnum(curr_char = fgetc(file)) || curr_char == '_'){
                buffer[len++] = curr_char;
            }
            buffer[len] = '\0';
            if(strcmp(buffer, "main") == 0) {
                Token token = {MAIN};
                token_append(tokens, token);
            } else if (strcmp(buffer, "int") == 0) {
                Token token = {INT_KEYWORD};
                token_append(tokens, token);
            } else if (strcmp(buffer, "void") == 0) {
                Token token = {VOID};
                token_append(tokens, token);
            } else if (strcmp(buffer, "return") == 0) {
                Token token = {RETURN};
                token_append(tokens, token);
            } else {
                printf("Hasn't implemented!\n");
            }
        } else if (curr_char == '(') {
            Token token = {OPAREN};
            token_append(tokens, token);
            curr_char = fgetc(file);
        } else if (curr_char == ')') {
            Token token = {CPAREN};
            token_append(tokens, token);
            curr_char = fgetc(file);
        } else if (curr_char == '{') {
            Token token = {OPEN_CURL};
            token_append(tokens, token);
            curr_char = fgetc(file);
        } else if (curr_char == '}') {
            Token token = {CLOSE_CURL};
            token_append(tokens, token);
            curr_char = fgetc(file);
        } else if (curr_char == ';') {
            Token token = {SEMI_COLON};
            token_append(tokens, token);
            curr_char = fgetc(file);
        } else {
            curr_char = fgetc(file);
        }
    }

    for(int i=0;i<tokens->count;i++){
        printf("Token type: %d\n" , tokens->tokens[i].type);
        if(tokens->tokens[i].value){
            printf("Token's value : %s\n", tokens->tokens[i].value);
        }
    }
    return tokens;
}

int main(int argc, char** argv) {
    if (argc != 2 || strlen(argv[1]) < 2 || argv[1][strlen(argv[1])-1] != 'c' || argv[1][strlen(argv[1])-2] != '.') {
        printf("Usage: rcc <path/to/code.c>\n");
        return EXIT_FAILURE;
    }

    FILE *file = fopen(argv[1], "r");
    Tokens *toks = lexer(file);

    return 0;
}
