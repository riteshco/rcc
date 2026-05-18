#include "tacker.h"
#include <stdlib.h>

int tmp_counter = 0;

char* make_temp() {
    char* name = malloc(16);
    sprintf(name, "tmp.%d", tmp_counter++);
    return name;
}

#define tacker_instruction_append(func, inst)\
    do {\
        if(func->count >= func->capacity) {\
            if(func->capacity == 0) func->capacity = 10;\
            else func->capacity *= 2;\
            func->instructions = realloc(func->instructions, func->capacity*sizeof(*(func->instructions)));\
        }\
        func->instructions[func->count++] = inst;\
    } while(0)

TackerVal emit_tacker_expr(AST_EXPR* expr, TackerFunc* func){
    if(expr->type == EXPR_INT) {
        TackerVal val;
        val.type = TACKER_VAL_INT;
        val.int_val = expr->int_value;
        return val;
    } else if(expr->type == EXPR_UNARY) {
        TackerVal src_val = emit_tacker_expr(expr->inner_expr, func);

        TackerVal dst_val;
        dst_val.type = TACKER_VAL_VAR;
        dst_val.name = make_temp();

        TackerInstruction inst;
        inst.type = TACKER_UNARY;
        inst.unop = expr->unop;
        inst.src = src_val;
        inst.dst = dst_val;

        tacker_instruction_append(func, inst);
        return dst_val;
    }
}

void emit_tacker_stmt(AST_STMT* stmt, TackerFunc* func) {
    TackerVal result_val = emit_tacker_expr(&(stmt->expr), func);

    TackerInstruction ret_inst;
    ret_inst.type = TACKER_RET;
    ret_inst.ret_val = result_val;
    tacker_instruction_append(func, ret_inst);
}

TackerFunc emit_tacker_func(AST_FUNC* ast_func) {
    TackerFunc func;
    func.name = ast_func->name;
    func.instructions = NULL;
    func.count = 0;
    func.capacity = 0;

    tmp_counter = 0;

    emit_tacker_stmt(&(ast_func->stmt), &func);

    return func;
}

TackerProg generate_tacker(AST_PROG* ast){
    TackerProg prog;
    prog.func = emit_tacker_func(&(ast->func));
    return prog;
}