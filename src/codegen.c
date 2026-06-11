#include "codegen.h"
#include "parser.h"
#include "tacker.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define instruction_append(func, inst)\
    do {\
        if(func->count >= func->capacity) {\
            if(func->capacity == 0) func->capacity = 10;\
            else func->capacity *= 2;\
            func->instructions = realloc(func->instructions, func->capacity*sizeof(*(func->instructions)));\
        }\
        func->instructions[func->count++] = inst;\
    } while(0)

ASM_OPERAND gen_operand(TackerVal val) {
    ASM_OPERAND op;
    if (val.type == TACKER_VAL_INT) {
        op.type = IMM;
        op.imm_value = val.int_val;
    } else if (val.type == TACKER_VAL_VAR) {
        op.type = PSEUDO;
        op.pseudo_name = val.name;
    }
    return op;
}

ASM_PROG generate_assembly(TackerProg* tacker_prog) {
    ASM_PROG asm_prog;
    asm_prog.func.name = tacker_prog->func.name;
    asm_prog.func.instructions = NULL;
    asm_prog.func.count = 0;
    asm_prog.func.capacity = 0;

    for(int i=0;i<tacker_prog->func.count; i++) {
        TackerInstruction t_inst = tacker_prog->func.instructions[i];

        if (t_inst.type == TACKER_RET) {
            ASM_INSTRUCTION mov_inst = {MOV, gen_operand(t_inst.ret_val), {REG, .reg=REG_EAX}, 0};
            instruction_append((&asm_prog.func), mov_inst);
            
            ASM_INSTRUCTION ret_inst = {RET, {0}, {0}, 0};
            instruction_append((&asm_prog.func), ret_inst);
        } 
        else if (t_inst.type == TACKER_UNARY) {
            // movl src, dst
            ASM_INSTRUCTION mov_inst = {MOV, gen_operand(t_inst.src), gen_operand(t_inst.dst), 0};
            instruction_append((&asm_prog.func), mov_inst);
            
            // negl dst OR notl dst
            ASM_INSTRUCTION un_inst;
            un_inst.type = (t_inst.unop == UNOP_NEGATE) ? NEG : NOT;
            un_inst.src = gen_operand(t_inst.dst); 
            un_inst.dst = gen_operand(t_inst.dst);
            instruction_append((&asm_prog.func), un_inst);
        } else if (t_inst.type == TACKER_BINARY) {
            if (t_inst.binop == BINOP_ADD || t_inst.binop == BINOP_SUB || t_inst.binop == BINOP_MUL) {
                ASM_INSTRUCTION mov_inst = {MOV, gen_operand(t_inst.src1), gen_operand(t_inst.dst), 0};
                instruction_append((&asm_prog.func), mov_inst);

                ASM_INSTRUCTION math_inst;
                if (t_inst.binop == BINOP_ADD) math_inst.type = ADD;
                else if (t_inst.binop == BINOP_SUB) math_inst.type = SUB;
                else math_inst.type = IMUL;

                math_inst.src = gen_operand(t_inst.src2);
                math_inst.dst = gen_operand(t_inst.dst);
                instruction_append((&asm_prog.func), math_inst);
            }
            else if (t_inst.binop == BINOP_DIV || t_inst.binop == BINOP_MOD) {
                ASM_INSTRUCTION mov_eax = {MOV, gen_operand(t_inst.src1), {REG, .reg=REG_EAX, 0}};
                instruction_append((&asm_prog.func), mov_eax);

                ASM_INSTRUCTION cdq_inst = {CDQ, {0}, {0}, 0};
                instruction_append((&asm_prog.func), cdq_inst);

                ASM_INSTRUCTION div_inst = {IDIV, gen_operand(t_inst.src1), {REG, .reg=REG_EAX}, 0};
                instruction_append((&asm_prog.func), cdq_inst);

                ASM_OPERAND result_reg = {REG, .reg= (t_inst.binop == BINOP_DIV) ? REG_EAX : REG_EDX};
                ASM_INSTRUCTION mov_dst = {MOV, result_reg, gen_operand(t_inst.dst), 0};
                instruction_append((&asm_prog.func), mov_dst);
            }
        }
    }

    return asm_prog;
}

typedef struct {
    char* name;
    int offset;
} StackMap;

void replace_pseudo_registers(ASM_PROG *prog) {
    StackMap map[100];
    int map_count = 0;
    int current_offset = -4;

    for(int i=0; i < prog->func.count; i++) {
        ASM_INSTRUCTION* inst = &prog->func.instructions[i];
        ASM_OPERAND* operands[2] = {&inst->src, &inst->dst};

        for(int j=0;j<2;j++){
            if(operands[j]->type == PSEUDO) {
                int found_offset = 0;
                for(int k=0;k<map_count;k++){
                    if(strcmp(map[k].name, operands[j]->pseudo_name) == 0) {
                        found_offset = map[k].offset;
                        break;
                    }
                }

                if (found_offset == 0) {
                    map[map_count].name = operands[j]->pseudo_name;
                    map[map_count].offset = current_offset;
                    found_offset = current_offset;
                    map_count++;
                    current_offset -= 4;
                }

                operands[j]->type = STACK;
                operands[j]->stack_offset = found_offset;
            }
        }
    }
}

void fix_instructions(ASM_PROG *prog) {
    ASM_INSTRUCTION* new_instructions = malloc(100 * sizeof(ASM_INSTRUCTION));
    int new_count = 0;

    int lowest_offset = 0;
    for(int i=0;i<prog->func.count;i++) {
        ASM_INSTRUCTION* inst = &prog->func.instructions[i];
        if (inst->src.type == STACK && inst->src.stack_offset < lowest_offset) lowest_offset = inst->src.stack_offset;
        if (inst->dst.type == STACK && inst->dst.stack_offset < lowest_offset) lowest_offset = inst->dst.stack_offset;
    }

    if(lowest_offset < 0) {
        ASM_INSTRUCTION alloc_inst = {ALLOCATE, {0}, {0}, -lowest_offset};
        new_instructions[new_count++] = alloc_inst;
    }

    for(int i=0;i<prog->func.count;i++){
        ASM_INSTRUCTION inst = prog->func.instructions[i];

        if((inst.type == MOV || inst.type == ADD || inst.type == SUB) && inst.src.type == STACK && inst.dst.type == STACK) {
            ASM_INSTRUCTION fix1 = {MOV, inst.src, {REG, .reg=REG_R10D}, 0};
            new_instructions[new_count++] = fix1;

            ASM_INSTRUCTION fix2 = {inst.type, {REG, .reg=REG_R10D}, inst.dst, 0};
            new_instructions[new_count++] = fix2;
        }
        else if (inst.type == IMUL && inst.dst.type == STACK) {
            ASM_INSTRUCTION fix1 = {MOV, inst.dst, {REG, .reg=REG_R11D}, 0};
            new_instructions[new_count++] = fix1;

            ASM_INSTRUCTION fix2 = {IMUL, inst.src, {REG, .reg=REG_R11D}, 0};
            new_instructions[new_count++] = fix2;

            ASM_INSTRUCTION fix3 = {MOV, {REG, .reg=REG_R11D}, inst.dst, 0};
            new_instructions[new_count++] = fix3;
        }
        else if (inst.type == IDIV && inst.src.type == IMM) {
            ASM_INSTRUCTION fix1 = {MOV, inst.src, {REG, .reg=REG_R10D}, 0};
            new_instructions[new_count++] = fix1;

            ASM_INSTRUCTION fix2 = {IDIV, {REG, .reg=REG_R10D}, {0}, 0};
            new_instructions[new_count++] = fix2;
        } else {
            new_instructions[new_count++] = inst;
        }
    }

    free(prog->func.instructions);
    prog->func.instructions = new_instructions;
    prog->func.count = new_count;
}

void emit_operand(ASM_OPERAND op, FILE* output_file) {
    if(op.type == IMM) {
        fprintf(output_file, "$%d", op.imm_value);
    } else if (op.type == REG) {
        if(op.reg == REG_EAX) fprintf(output_file, "%%eax");
        if(op.reg == REG_R10D) fprintf(output_file, "%%r10d");
        if(op.reg == REG_R11D) fprintf(output_file, "%%r11d");
        if(op.reg == REG_EDX) fprintf(output_file, "%%edx");
    } else if (op.type == STACK) {
        fprintf(output_file, "%d(%%rbp)", op.stack_offset);
    }
}

void emit_assembly(ASM_PROG* prog, FILE* output_file) {
    fprintf(output_file, ".globl %s\n", prog->func.name);
    fprintf(output_file, "%s:\n", prog->func.name);

    fprintf(output_file, "    pushq %%rbp\n");
    fprintf(output_file, "    movq %%rsp, %%rbp\n");

    for (int i = 0; i < prog->func.count; i++) {
        ASM_INSTRUCTION inst = prog->func.instructions[i];

        if (inst.type == ALLOCATE) {
            fprintf(output_file, "    subq $%d, %%rsp\n", inst.allocate_bytes);
        } else if (inst.type == MOV) {
            fprintf(output_file, "    movl ");
            emit_operand(inst.src, output_file);
            fprintf(output_file, ", ");
            emit_operand(inst.dst, output_file);
            fprintf(output_file, "\n");
        } else if (inst.type == NEG || inst.type == NOT) {
            if (inst.type == NEG) fprintf(output_file, "    negl ");
            else fprintf(output_file, "    notl ");
            emit_operand(inst.dst, output_file);
            fprintf(output_file, "\n");
        } else if (inst.type == RET) {
            fprintf(output_file, "    movq %%rbp, %%rsp\n");
            fprintf(output_file, "    popq %%rbp\n");
            fprintf(output_file, "    ret\n");
        } else if (inst.type == ADD || inst.type == SUB || inst.type == IMUL) {
            if (inst.type == ADD) fprintf(output_file, "    addl ");
            if (inst.type == SUB) fprintf(output_file, "    subl ");
            if (inst.type == IMUL) fprintf(output_file, "    imull ");
            emit_operand(inst.src, output_file);
            fprintf(output_file, ", ");
            emit_operand(inst.dst, output_file);
            fprintf(output_file, "\n");
        } else if (inst.type == IDIV) {
            fprintf(output_file, "    idivl ");
            emit_operand(inst.src, output_file);
            fprintf(output_file, "\n");
        } else if (inst.type == CDQ) {
            fprintf(output_file, "    cdq\n");
        }
    }
}