#include "lexer.h"

typedef struct {
    int value;
} AST_EXPR;

typedef struct {
    AST_EXPR expr;
} AST_STMT;

typedef struct {
    char* name;
    AST_STMT stmt;
} AST_FUNC;

typedef struct {
    AST_FUNC func;
} AST_PROG;

AST_PROG parse(Tokens* toks);