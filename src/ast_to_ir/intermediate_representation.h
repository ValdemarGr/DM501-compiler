//
// Created by valde on 4/12/19.
//

#ifndef HELLO_INTERMEDIATE_REPRESENTATION_H
#define HELLO_INTERMEDIATE_REPRESENTATION_H

#include <stdio.h>
#include "../ast/tree.h"
#include "../utils/memory.h"
#include "../symbol/symbol.h"
#include "abstract_asm_tree.h"

typedef struct Jump {
    char* label;
} Jump;

typedef struct JumpIfZero {
    char* label_true;
    char* label_false;
} JumpIfZero;

typedef struct Arithmetic {
    size_t lhsTemp;
    size_t rhsTemp;
} Arithmetic;

typedef enum {
    INSTRUCTION_ADD,
    INSTRUCTION_MINUS,
    INSTRUCTION_MUL,
    INSTRUCTION_DIV,
    INSTRUCTION_CONST,
    INSTRUCTION_PROGRAM_BEGIN,
    INSTRUCTION_FUNCTION_LABEL,
    INSTRUCTION_VAR,
    INSTRUCTION_VAL,
    INSTRUCTION_FUNCTION_END,
    INSTRUCTION_RETURN,
    INSTRUCTION_WRITE,
    INSTRUCTION_AND,
    INSTRUCTION_OR,

    COMPLEX_CONSTRAIN_BOOLEAN,

    METADATA_BEGIN_BODY_BLOCK,
    METADATA_END_BODY_BLOCK,
    METADATA_FUNCTION_ARGUMENT
} InstructionKind;

typedef struct Instructions {
    struct Instructions* next;
    Context context;
    InstructionKind kind;
    union {
        Arithmetic arithmetic; //INSTRUCTION_ADD.. INSTRUCTION_AND..
        struct {size_t value; size_t temp;} constant; //INSTRUCTION_CONST
        int var; //INSTRUCTION_VAR
        int val; //INSTRUCTION_VAL
        char* label; //INSTRUCTION_FUNCTION_LABEL & INSTRUCTION_FUNCTION_END
        int argNum; //METADATA_FUNCTION_ARGUMENT
        size_t tempToWrite; //INSTRUCTION_WRITE
        size_t tempToReturn; //INSTRUCTION_RETURN
        size_t tempToConstrain; //COMPLEX_CONSTRAIN_BOOLEAN
    } val;
} Instructions;

#endif //HELLO_INTERMEDIATE_REPRESENTATION_H
