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

typedef struct {
    Tokens* toks;
    int pos;
} Parser;

AST_PROG parse(Tokens* toks);
AST_FUNC parse_func(Parser* parser);
AST_STMT parse_stmt(Parser* parser);
AST_EXPR parse_expr(Parser* parser);