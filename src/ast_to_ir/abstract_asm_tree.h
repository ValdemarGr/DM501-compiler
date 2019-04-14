#ifndef HELLO_ABSTRACT_ASM_TREE_H
#define HELLO_ABSTRACT_ASM_TREE_H


#include <stdio.h>
#include "../ast/tree.h"
#include "../utils/memory.h"
#include "../symbol/symbol.h"
#include "./intermediate_representation.h"
#include "../utils/stack.h"

typedef enum {
    ACCUMULATE
} ContextKind;

typedef struct Context {
    ContextKind kind;
    union {
        size_t temp; //ACCUMULATE
    } val;
} Context;

void generateInstructionTree(Body* body);

#endif //HELLO_ABSTRACT_ASM_TREE_H
