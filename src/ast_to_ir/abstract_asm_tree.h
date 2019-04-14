#ifndef HELLO_ABSTRACT_ASM_TREE_H
#define HELLO_ABSTRACT_ASM_TREE_H


#include <stdio.h>
#include "../ast/tree.h"
#include "../utils/memory.h"
#include "../symbol/symbol.h"
#include "./intermediate_representation.h"

Instructions *generateInstructionTree(Body* body, Instructions *preBodyLast);

#endif //HELLO_ABSTRACT_ASM_TREE_H
