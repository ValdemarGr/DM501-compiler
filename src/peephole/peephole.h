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
} SimpleInstruction;

typedef enum {
    REMOVE_CONST_REGISTER_ADD,
    REMOVE_CONST_REGISTER_MUL,
    REMOVE_CONST_REGISTER_SUB,
    REMOVE_PUSH_POP,
    CONST_ZERO_TO_SET_ZERO,
    CONST_ADD_TO_LEA,
    ADD_TO_LEA,
    ALREADY_COMPLEX_MOVE_TEMPORARY_FROM_STACK,
    POP_PUSH_STACK_PTR_LOAD
} PeepholeApplyType;

typedef struct PeepholeTemplates {
    PeepholeApplyType apply;
    SimpleInstruction *simpleInstruction;
    struct PeepholeTemplates *next;
    struct PeepholeTemplates *last;
    int size;
} PeepholeTemplates;

typedef struct Kinds {
    InstructionKind kind;
    struct Kinds *next;
} Kinds;

typedef struct PeepholeTemplate {
    void* apply;
    Kinds *kinds;
} PeepholeTemplate;

typedef struct PeepholeTemplateList{
    PeepholeTemplate *peepholeTemplate;
    struct PeepholeTemplateList *next;
} PeepholeTemplateList;

Instructions *fetchPreviousInstructionThatModifiesRegister(Instructions *current, size_t regToLookFor);
bool peephole(Instructions *instructions);

#endif //HELLO_PEEPHOLE_H
