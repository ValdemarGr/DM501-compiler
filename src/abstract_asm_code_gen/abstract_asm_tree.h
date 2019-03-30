#ifndef HELLO_ABSTRACT_ASM_TREE_H
#define HELLO_ABSTRACT_ASM_TREE_H

#include <stdio.h>
#include "../ast/tree.h"
#include "../utils/memory.h"
#include "../symbol/symbol.h"

typedef struct AsmValue AsmValue;

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
    VALUE_UID, VALUE_ADD
} ValueKind;

typedef struct AsmValue {
    ValueKind kind;
    union {
        int identifier;
        Add add;
    } val;
} AsmValue;


typedef enum {
    INSTRUCTION_ADD,
    INSTRUCTION_VAR
} InstructionKind;

typedef struct Instructions {
    struct Instructions* next;
    InstructionKind kind;
    union {
        Add add;
        int var;
    } val;
} Instructions;

Instructions *generateInstructionTree(Body* body);

#endif //HELLO_ABSTRACT_ASM_TREE_H
