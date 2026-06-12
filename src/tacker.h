#pragma once
#include "parser.h"

typedef enum {
    TACKER_VAL_INT,
    TACKER_VAL_VAR
} TackerValType;

typedef struct {
    TackerValType type;
    int int_val;
    char* name;
} TackerVal;

typedef enum {
    TACKER_RET,
    TACKER_UNARY,
    TACKER_BINARY,
    TACKER_COPY,
    TACKER_JUMP,
    TACKER_JMP_IF_ZERO,
    TACKER_JMP_IF_NOT_ZERO,
    TACKER_LABEL
} TackerInstructionType;

typedef struct {
    TackerInstructionType type;
    
    TackerVal ret_val; // for type Return

    UnaryOp unop; // for type Unary
    TackerVal src;

    BinaryOp binop; // for type binary
    TackerVal src1;
    TackerVal src2;
    TackerVal dst; // dst, will be used for both unary and binary

    char* target_name; // for jumps
    TackerVal condition; // The value to check for JumpIfZero / JumpIfNotZero
} TackerInstruction;

typedef struct {
    char* name;
    TackerInstruction* instructions;
    int count;
    int capacity;
} TackerFunc;

typedef struct {
    TackerFunc func;
} TackerProg;

TackerProg generate_tacker(AST_PROG* ast);