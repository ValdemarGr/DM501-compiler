//
// Created by valde on 4/21/19.
//

#ifndef HELLO_REGISTER_ALLOCATOR_H
#define HELLO_REGISTER_ALLOCATOR_H

#include "../ast_to_ir/intermediate_representation.h"
#include "../asm_code_gen/consts.h"
#include "../utils/optional.h"
#include "../utils/sortedset.h"

typedef struct AbstractRegister{
    char *registerName;
    Instructions *boundInstruction;
    size_t useTimestamp;
} AbstractRegister;

typedef struct RaFunction{
    char *id;
    int dataFlowIndex;
} RaFunction;

typedef struct IntBox {
    int value;
} IntBox;

typedef struct RaAllocation{
    enum { raRegister, raSpill } kind;
    size_t givenRegister;
} RaAllocation;

typedef struct VariableList {
    int id;
    struct VariableList *next;
} VariableList;

typedef struct LineList {
    int line;
    struct LineList *next;
} LineList;

typedef struct DataFlowEntry {
    SortedSet *defines;
    SortedSet *uses;
    SortedSet *in;
    SortedSet *out;
    LineList *successors;
    LineList *predecessors;
} DataFlowEntry;

void simpleRegisterAllocation(Instructions *head);

#endif //HELLO_REGISTER_ALLOCATOR_H

