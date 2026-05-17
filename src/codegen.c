#include "codegen.h"
#include <stdlib.h>

#define instruction_append(func, inst)\
    do {\
        if(func->count >= func->capacity) {\
            if(func->capacity == 0) func->capacity = 10;\
            else func->capacity *= 2;\
            func->instructions = realloc(func->instructions, func->capacity*sizeof(*(func->instructions)));\
        }\
        func->instructions[func->count++] = inst;\
    } while(0)

ASM_OPERAND gen_operand(AST_EXPR expr) {
    ASM_OPERAND op;
    op.type = IMM;
    op.imm_value = expr.value;
    return op;
}

void gen_stmt(AST_STMT* stmt, ASM_FUNC* func) {

    ASM_INSTRUCTION mov_inst;
    mov_inst.type = MOV;
    mov_inst.src = gen_operand(stmt->expr);
    mov_inst.dst.type = REG;
    instruction_append(func, mov_inst);

    ASM_INSTRUCTION ret_inst;
    ret_inst.type = RET;
    instruction_append(func, ret_inst);

}

ASM_FUNC gen_func(AST_FUNC* c_func) {
    ASM_FUNC asm_func;
    asm_func.name = c_func->name;
    asm_func.instructions = NULL;
    asm_func.count = 0;
    asm_func.capacity = 0;

    gen_stmt(&(c_func->stmt), &asm_func);

    return asm_func;
}

ASM_PROG generate_assembly(AST_PROG* prog) {
    ASM_PROG asm_prog;
    asm_prog.func = gen_func(&(prog->func));
    return asm_prog;
}

void emit_assembly(ASM_PROG* prog, FILE* output_file) {
    fprintf(output_file, ".global %s\n", prog->func.name);
    fprintf(output_file, "%s:\n", prog->func.name);
    
    for(int i=0; i<prog->func.count; i++) {
        ASM_INSTRUCTION inst = prog->func.instructions[i];

        if(inst.type == MOV) {
            fprintf(output_file, "  movl $%d, %%eax\n", inst.src.imm_value);
        } else if(inst.type == RET) {
            fprintf(output_file, "  ret\n");
        }
    }
}