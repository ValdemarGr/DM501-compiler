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


typedef struct LivenessAnalysisResult {
    int numberSets;
    SortedSet **sets;
} LivenessAnalysisResult;

void simpleRegisterAllocation(Instructions *head);

#endif //HELLO_REGISTER_ALLOCATOR_H

