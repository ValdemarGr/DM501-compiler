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

typedef enum { RaRead, RaWrite, RaReadWrite, RaIntermidiate } RaTemporariesKind;

typedef struct RaTemporaries {
    int id;
    RaTemporariesKind kind;
    int reg;
    bool assigned;
    struct RaTemporaries *next;
} RaTemporaries;

typedef struct RaVariableLocation {
    int offset;
    size_t scope;
    bool useScope;
} RaVariableLocation;

typedef struct RaState {
    Instructions *previous;
    Instructions *current;
    SortedSet *allRegs;
    SortedSet *regsInUse;
    SortedSet *tempsInUse;
    ConstMap *stackLocation;
} RaState;

Instructions *simpleRegisterAllocation(Instructions *head, int numberRegisters);

#endif //HELLO_REGISTER_ALLOCATOR_H

