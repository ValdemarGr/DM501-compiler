#ifndef HELLO_ABSTRACT_ASM_TREE_H
#define HELLO_ABSTRACT_ASM_TREE_H


#include <stdio.h>
#include "../ast/tree.h"
#include "../utils/memory.h"
#include "../symbol/symbol.h"
#include "./intermediate_representation.h"
#include "../utils/stack.h"
#include "../type_checker/type_checker.h"
#include "../asm_code_gen/sizes.h"
#include "../ast/tree.h"
#include "../utils/sortedset.h"

void getPointerCountForDecl(SortedSet*, Declaration *declaration);
SortedSet *getPointerCountForBody(DeclarationList *declarationList);

typedef enum {
    ACCUMULATE
} ContextKind;

typedef struct Context {
    ContextKind kind;
    union {
        size_t temp; //ACCUMULATE
    } val;
} Context;

struct Instructions *newInstruction();
struct Instructions *generateInstructionTree(Body* body);

#endif //HELLO_ABSTRACT_ASM_TREE_H
