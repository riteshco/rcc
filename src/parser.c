#include "parser.h"
#include "lexer.h"
#include <stdlib.h>

int get_precedence(TokenType type) {
    switch (type) {
        case MUL:
        case DIV:
        case REM:
            return 50;
        case PLUS:
        case MINUS:
            return 45;
        case LESS_THAN: case LESS_THAN_OR_EQUAL: case GREATER_THAN: case GREATER_THAN_OR_EQUAL:
            return 35;
        case EQUAL_EQUAL: case NOT_EQUAL:
            return 30;
        case AND:
            return 10;
        case OR:
            return 5;
        default:
            return 0;
    }
}

Token peek(Parser* parser) {
    return parser->toks->tokens[parser->pos];
}

Token consume(Parser* parser) {
    return parser->toks->tokens[parser->pos++];
}

void expect(Parser* parser, TokenType expected_type) {
    Token tok = consume(parser);
    if (tok.type != expected_type) {
        printf("Syntax error: Expected token type to be \'%d\' but got \'%d\'\n", expected_type, tok.type);
        exit(EXIT_FAILURE);
    }
}


AST_EXPR parse_factor(Parser* parser) {
    AST_EXPR expr;
    Token next_tok = peek(parser);

    if(next_tok.type == INT_LITERAL) {
        Token tok = consume(parser);
        expr.type = EXPR_INT;
        expr.int_value = atoi(tok.value);
        return expr;
    } else if(next_tok.type == MINUS || next_tok.type == TILDE || next_tok.type == BANG) {
        Token op_tok = consume(parser);
        expr.type = EXPR_UNARY;
        expr.unop = (op_tok.type == MINUS) ? UNOP_NEGATE : UNOP_COMPLEMENT;
        expr.inner_expr = malloc(sizeof(AST_EXPR));
        *(expr.inner_expr) = parse_factor(parser);
        return expr;
    } else if(next_tok.type == OPAREN) {
        Token tok = consume(parser);
        expr = parse_expr(parser, 0);
        expect(parser, CPAREN);
        return expr;
    } else {
        printf("Syntax Error: Invalid expression. Token type is %d\n", next_tok.type);
        exit(EXIT_FAILURE);
    }
}

AST_EXPR parse_expr(Parser* parser, int min_prec) {
    AST_EXPR left = parse_factor(parser);
    Token next_tok = peek(parser);
    int prec = get_precedence(next_tok.type);

    while(prec >= min_prec && prec != 0) {
        Token op_tok = consume(parser);
        
        AST_EXPR right = parse_expr(parser, prec+1);

        AST_EXPR new_expr;
        new_expr.type = EXPR_BINARY;

        if(op_tok.type == PLUS) new_expr.binop = BINOP_ADD;
        else if (op_tok.type == MINUS) new_expr.binop = BINOP_SUB;
        else if (op_tok.type == MUL) new_expr.binop = BINOP_MUL;
        else if (op_tok.type == DIV) new_expr.binop = BINOP_DIV;
        else if (op_tok.type == REM) new_expr.binop = BINOP_MOD;
        else if (op_tok.type == AND) new_expr.binop = BINOP_AND;
        else if (op_tok.type == OR) new_expr.binop = BINOP_OR;
        else if (op_tok.type == EQUAL_EQUAL) new_expr.binop = BINOP_EQUAL;
        else if (op_tok.type == NOT_EQUAL) new_expr.binop = BINOP_NOT_EQUAL;
        else if (op_tok.type == LESS_THAN) new_expr.binop = BINOP_LESS_THAN;
        else if (op_tok.type == LESS_THAN_OR_EQUAL) new_expr.binop = BINOP_LESS_THAN_OR_EQUAL;
        else if (op_tok.type == GREATER_THAN) new_expr.binop = BINOP_GREATER_THAN;
        else if (op_tok.type == GREATER_THAN_OR_EQUAL) new_expr.binop = BINOP_GREATER_THAN_OR_EQUAL;

        new_expr.left = malloc(sizeof(AST_EXPR));
        *new_expr.left = left;

        new_expr.right = malloc(sizeof(AST_EXPR));
        *new_expr.right = right;

        left = new_expr;
        next_tok = peek(parser);
        prec = get_precedence(next_tok.type);
    }
    return left;
}

AST_STMT parse_stmt(Parser* parser) {
    AST_STMT stmt;

    expect(parser, RETURN);
    stmt.expr = parse_expr(parser, 0);
    expect(parser, SEMI_COLON);

    return stmt;
}

AST_FUNC parse_func(Parser* parser) {
    AST_FUNC func;

    expect(parser, INT_KEYWORD);
    expect(parser, MAIN);
    func.name = "main";
    expect(parser, OPAREN);
    expect(parser, VOID);
    expect(parser, CPAREN);
    expect(parser, OPEN_CURL);

    func.stmt = parse_stmt(parser);

    expect(parser, CLOSE_CURL);

    return func;
}

AST_PROG parse(Tokens* toks) {
    Parser parser;
    parser.toks = toks;
    parser.pos = 0;
    AST_PROG prog;
    prog.func = parse_func(&parser);

    return prog;
}

