//
// Created by valde on 4/12/19.
//

#ifndef HELLO_INTERMEDIATE_REPRESENTATION_H
#define HELLO_INTERMEDIATE_REPRESENTATION_H

#include <stdio.h>
#include "../ast/tree.h"
#include "../utils/memory.h"
#include "../symbol/symbol.h"

typedef struct AsmValue AsmValue;

typedef struct Inc {
    AsmValue *val;
} Inc;

typedef struct Jump {
    char* label;
} Jump;

typedef struct JumpIfZero {
    char* label_true;
    char* label_false;
} JumpIfZero;

typedef struct Add {
    AsmValue* left;
    AsmValue* right;
} Add;

typedef enum {
    VALUE_UID, VALUE_ADD, VALUE_CONST
} ValueKind;

typedef struct AsmValue {
    ValueKind kind;
    union {
        int identifier;
        Add add;
        int constant;
    } val;
} AsmValue;


typedef enum {
    INSTRUCTION_ADD,
    INSTRUCTION_PROGRAM_BEGIN,
    INSTRUCTION_FUNCTION_LABEL,
    INSTRUCTION_VAR,
    INSTRUCTION_FUNCTION_END,
    INSTRUCTION_RETURN,

    METADATA_BEGIN_BODY_BLOCK,
    METADATA_END_BODY_BLOCK,
    METADATA_FUNCTION_ARGUMENT
} InstructionKind;

typedef struct Instructions {
    struct Instructions* next;
    InstructionKind kind;
    union {
        Add add;
        int var;
        char* label;
        int argNum;
    } val;
} Instructions;

#endif //HELLO_INTERMEDIATE_REPRESENTATION_H
