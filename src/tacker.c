#include "tacker.h"
#include "parser.h"
#include <stdlib.h>

static int tmp_counter = 0;

char* make_temp() {
    char* name = malloc(16);
    sprintf(name, "tmp.%d", tmp_counter++);
    return name;
}

static int label_counter = 0;

char* make_label(const char* prefix) {
    char* name = malloc(32);
    sprintf(name, "%s.%d", prefix, label_counter++);
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
    } else if(expr->type == EXPR_BINARY) {
        if (expr->binop == BINOP_AND) {
            char* false_label = make_label("and_false");
            char* end_label = make_label("and_end");

            TackerVal v1 = emit_tacker_expr(expr->left, func);

            // if left is zero, jump straight to false label
            TackerInstruction jmp_false1 = {TACKER_JMP_IF_ZERO};
            jmp_false1.condition = v1;
            jmp_false1.target_name = false_label;
            tacker_instruction_append(func, jmp_false1);

            TackerVal v2 = emit_tacker_expr(expr->right, func);

            // if right is zero (false), jump to false label
            TackerInstruction jmp_false2 = {TACKER_JMP_IF_ZERO};
            jmp_false2.condition = v2;
            jmp_false2.target_name = false_label;
            tacker_instruction_append(func, jmp_false2);

            // both left and right true. Set result to 1
            TackerVal dst_val = {TACKER_VAL_VAR, .name = make_temp()};
            TackerInstruction copy_true = {TACKER_COPY, .src1 = {TACKER_VAL_INT, .int_val = 1}, .dst = dst_val};
            tacker_instruction_append(func, copy_true);

            TackerInstruction jmp_end = {TACKER_JUMP, .target_name = end_label};
            tacker_instruction_append(func, jmp_end);

            TackerInstruction lbl_false = {TACKER_LABEL, .target_name = false_label};
            tacker_instruction_append(func, lbl_false);

            TackerInstruction copy_false = {TACKER_COPY, .src1 = {TACKER_VAL_INT, .int_val = 0}, .dst = dst_val};
            tacker_instruction_append(func, copy_false);

            TackerInstruction lbl_end = {TACKER_LABEL, .target_name = end_label};
            tacker_instruction_append(func, lbl_end);

            return dst_val;
        } else if(expr->binop == BINOP_OR) {
            char* true_label = make_label("or_true");
            char* end_label = make_label("or_end");
            
            TackerVal v1 = emit_tacker_expr(expr->left, func);

            // if left is true, jump straight to true_label
            TackerInstruction jmp_true1 = {TACKER_JMP_IF_NOT_ZERO, .condition = v1, .target_name = true_label};
            tacker_instruction_append(func, jmp_true1);

            TackerVal v2 = emit_tacker_expr(expr->right, func);
            
            // if right is true, jump to true_label
            TackerInstruction jmp_true2 = {TACKER_JMP_IF_NOT_ZERO, .condition = v2, .target_name = true_label};
            tacker_instruction_append(func, jmp_true2);

            // if made it here, both were false. Set result to 0.
            TackerVal dst_val = {TACKER_VAL_VAR, .name = make_temp()};
            TackerInstruction copy_false = {TACKER_COPY, .src1 = {TACKER_VAL_INT, .int_val = 0}, .dst = dst_val};
            tacker_instruction_append(func, copy_false);

            TackerInstruction jmp_end = {TACKER_JUMP, .target_name = end_label};
            tacker_instruction_append(func, jmp_end);
            
            TackerInstruction lbl_true = {TACKER_LABEL, .target_name = true_label};
            tacker_instruction_append(func, lbl_true);
            
            TackerInstruction copy_true = {TACKER_COPY, .src1 = {TACKER_VAL_INT, .int_val = 1}, .dst = dst_val};
            tacker_instruction_append(func, copy_true);
            
            TackerInstruction lbl_end = {TACKER_LABEL, .target_name = end_label};
            tacker_instruction_append(func, lbl_end);
            
            return dst_val;
        } else {
            TackerVal v1 = emit_tacker_expr(expr->left, func);
            TackerVal v2 = emit_tacker_expr(expr->right, func);
            
            TackerVal dst_val;
            dst_val.type = TACKER_VAL_VAR;
            dst_val.name = make_temp();
            
            TackerInstruction inst;
            inst.type = TACKER_BINARY;
            inst.binop = expr->binop;
            inst.src1 = v1;
            inst.src2 = v2;
            inst.dst = dst_val;
            
            tacker_instruction_append(func, inst);
            return dst_val;
        }
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