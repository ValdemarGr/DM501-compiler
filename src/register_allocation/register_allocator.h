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
    struct DataFlowEntry **dataFlow;
    int dataFlowSize;
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

typedef struct RaVariable {
    char *accessId;
    int line;
    int reg;
    Instructions *instructionStart;
    Instructions *instructionEnd;
} RaVariable;

typedef struct RaState {
    Instructions *previous;
    Instructions *current;
    Instructions *latest;
    bool hasAddedPop;
    SortedSet *allRegs;
    SortedSet *regsInUse;
    SortedSet *tempsInUse;
    ConstMap *stackLocation;
    ConstMap *loadedVariables;
    RaVariable *currentVariable;
} RaState;

void appendToInstruction(Instructions* i1, Instructions *i2);
void prependToInstruction(Instructions* i1, Instructions *i2);
Instructions *makeDebugInstructions(char *text);
int *simpleRegisterAllocation(Instructions *head, int numberRegisters);
struct DataFlowEntry *mergeDataFlowEntries(const struct DataFlowEntry *d1, const struct DataFlowEntry *d2);

#endif //HELLO_REGISTER_ALLOCATOR_H

