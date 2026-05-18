#include "parser.h"
#include "lexer.h"
#include <stdlib.h>

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

AST_EXPR parse_expr(Parser* parser) {
    AST_EXPR expr;
    Token next_tok = peek(parser);

    if(next_tok.type == INT_LITERAL) {
        Token tok = consume(parser);
        expr.type = EXPR_INT;
        expr.int_value = atoi(tok.value);
        return expr;
    } else if(next_tok.type == MINUS || next_tok.type == TILDE) {
        Token op_tok = consume(parser);
        expr.type = EXPR_UNARY;
        expr.unop = (op_tok.type == MINUS) ? UNOP_NEGATE : UNOP_COMPLEMENT;
        expr.inner_expr = malloc(sizeof(AST_EXPR));
        *(expr.inner_expr) = parse_expr(parser);
        return expr;
    } else if(next_tok.type == OPAREN) {
        Token tok = consume(parser);
        expr = parse_expr(parser);
        expect(parser, CPAREN);
        return expr;
    } else {
        printf("Syntax Error: Invalid expression. Token type is %d\n", next_tok.type);
        exit(EXIT_FAILURE);
    }
}

AST_STMT parse_stmt(Parser* parser) {
    AST_STMT stmt;

    expect(parser, RETURN);
    stmt.expr = parse_expr(parser);
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