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
    Token tok = consume(parser);
    if (tok.type != INT_LITERAL) {
        printf("Syntax error: Expected integer literal\n");
        exit(EXIT_FAILURE);
    }

    AST_EXPR expr;
    expr.value = atoi(tok.value);
    return expr;
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