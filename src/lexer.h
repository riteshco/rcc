#pragma once
#include <stdio.h>

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
    TILDE,
    MINUS,
    DECREMENT,
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

Tokens* lexer(FILE* file);