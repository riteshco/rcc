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
    PLUS,
    MUL,
    DIV,
    REM,
    BANG,        // !
    AND,
    OR,
    EQUAL_EQUAL,
    NOT_EQUAL,
    LESS_THAN,
    LESS_THAN_OR_EQUAL,
    GREATER_THAN,
    GREATER_THAN_OR_EQUAL
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