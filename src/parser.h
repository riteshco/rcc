#pragma once
#include "lexer.h"

typedef enum {
    BINOP_ADD,
    BINOP_SUB,
    BINOP_MUL,
    BINOP_DIV,
    BINOP_MOD,
    BINOP_AND,
    BINOP_OR,
    BINOP_EQUAL,
    BINOP_NOT_EQUAL,
    BINOP_LESS_THAN,
    BINOP_LESS_THAN_OR_EQUAL,
    BINOP_GREATER_THAN,
    BINOP_GREATER_THAN_OR_EQUAL
} BinaryOp;

typedef enum {
    EXPR_INT,
    EXPR_UNARY,
    EXPR_BINARY
} ExprType;

typedef enum {
    UNOP_NEGATE,
    UNOP_COMPLEMENT,
    UNOP_NOT
} UnaryOp;

typedef struct AST_EXPR{
    ExprType type;
    
    int int_value; // if type is EXPR_INT

    UnaryOp unop; // if type is EXPR_UNARY
    struct AST_EXPR* inner_expr;

    BinaryOp binop;
    struct AST_EXPR* left;
    struct AST_EXPR* right;
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
AST_EXPR parse_factor(Parser* parser);
AST_EXPR parse_expr(Parser* parser, int min_prec);