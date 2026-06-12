#pragma once
#include "parser.h"
#include "tacker.h"
#include <stdio.h>

typedef enum {
    IMM,
    REG,
    PSEUDO,
    STACK
} OperandType;

typedef enum {
    REG_EAX,     // return register
    REG_R10D,    // scratch register 1
    REG_EDX,     // remainder register
    REG_R11D     // scratch register 2
} HardwareReg;

typedef struct {
    OperandType type;
    int imm_value; //only for type "immediate"

    HardwareReg reg;
    char* pseudo_name;
    int stack_offset;
} ASM_OPERAND;

typedef enum {
    MOV,
    RET,
    NEG,
    NOT,
    ALLOCATE,
    ADD,
    SUB,
    IMUL,
    IDIV,
    CDQ,         // Sign-extends EAX into EDX for division
    CMP,
    JMP,
    JE,
    JNE,
    LABEL,      // foo:
    SETE,       // ==
    SETNE,      // !=
    SETG,       // >
    SETGE,      // >=
    SETL,       // <
    SETLE       // <=
} InstructionType;

typedef struct {
    InstructionType type;
    ASM_OPERAND src;
    ASM_OPERAND dst;
    int allocate_bytes;
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

ASM_PROG generate_assembly(TackerProg* prog);
void replace_pseudo_registers(ASM_PROG* prog);
void fix_instructions(ASM_PROG* prog);
void emit_assembly(ASM_PROG* prog, FILE* output_file);