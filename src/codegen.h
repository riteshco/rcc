#pragma once
#include "parser.h"
#include <stdio.h>

typedef enum {
    IMM,
    REG
} OperandType;

typedef struct {
    OperandType type;
    int imm_value; //only for type "immediate"
} ASM_OPERAND;

typedef enum {
    MOV,
    RET
} InstructionType;

typedef struct {
    InstructionType type;
    ASM_OPERAND src;
    ASM_OPERAND dst;
} ASM_INSTRUCTION;

typedef struct {
    char* name;
    ASM_INSTRUCTION* instructions;
    int count;
    int capacity; 
} ASM_FUNC;

typedef struct {
    ASM_FUNC func;
} ASM_PROG;

ASM_PROG generate_assembly(AST_PROG* prog);
void emit_assembly(ASM_PROG* prog, FILE* output_file);