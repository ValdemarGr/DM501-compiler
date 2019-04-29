//
// Created by valde on 4/23/19.
//

#ifndef HELLO_PEEPHOLE_H
#define HELLO_PEEPHOLE_H

#include "../ast_to_ir/intermediate_representation.h"

#define ANY 0
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

typedef struct Rule1 {
    size_t reg1;
} Rule1;

typedef struct Rule2 {
    size_t reg1;
    size_t reg2;
} Rule2;

typedef struct SimpleInstruction {
    InstructionKind kind;
    struct SimpleInstruction *next;
    union {
        Rule1 rule1;
        Rule2 rule2;
    } val;
} SimpleInstruction;

typedef enum {
    REMOVE_CONST_REGISTER_ADD,
    REMOVE_CONST_REGISTER_MUL
} PeepholeApplyType;

typedef struct PeepholeTemplates {
    PeepholeApplyType apply;
    SimpleInstruction *simpleInstruction;
    struct PeepholeTemplates *next;
} PeepholeTemplates;

void peephole(Instructions *instructions);

#endif //HELLO_PEEPHOLE_H
