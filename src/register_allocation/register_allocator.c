//
// Created by valde on 4/21/19.
//

#include "register_allocator.h"
#include "liveness_analysis.h"
#include "graph_coloring.h"
#include "../ast_to_ir/intermediate_representation.h"
#include "simplify_loads.h"

ConstMap *currentRegisterContext = NULL;
size_t timestamp = 0;


SymbolTable *currentSymbolTable = NULL;

DataFlowEntry *mergeDataFlowEntries(const DataFlowEntry *d1, const DataFlowEntry *d2) {
    if (d1 == NULL || d2 == NULL) {
        return NULL;
    }

    DataFlowEntry *newData = NEW(DataFlowEntry);

    newData->defines = sortedSetUnion(d1->defines, d2->defines);
    newData->uses = sortedSetUnion(d1->uses, d2->uses);
    newData->in = sortedSetUnion(d1->in, d2->in);
    newData->out = sortedSetUnion(d1->out, d2->out);
}

Instructions *makeDebugInstructions(char *text) {
    Instructions *instructions = newInstruction();
    instructions->kind = METADATA_DEBUG_INFO;
    instructions->val.debugInfo = text;
    instructions->previous = NULL;

    return instructions;
}

int getAssigned(int *colors, int temporary) {
    if (temporary == 0) {
        return 0;
    }

    if (temporary > -1) {
        if (colors[temporary] > -1) {
            return colors[temporary] + 1;
        }
    }

    return -1;
}

void appendToInstruction(Instructions* i1, Instructions *i2) {
    i2->next = i1->next;
    i2->previous = i1;
    i1->next->previous = i2;
    i1->next = i2;
}

void prependToInstruction(Instructions* i1, Instructions *i2) {
    i2->next = i1;
    i2->previous = i1->previous;
    i1->previous->next = i2;
    i1->previous = i2;
}

RaTemporaries *makeTemporary(int id, RaTemporariesKind kind) {
    RaTemporaries *result = NEW(RaTemporaries);
    result->id = id;
    result->kind = kind;
    result->assigned = false;
    result->next = NULL;
    return result;
}

int getTemporary(int *colors, int temporary, RaState *state) {
    int assigned = getAssigned(colors, temporary);
    if (assigned != -1) {
        return assigned;
    }

    SortedSet *regs = sortedSetDiff(state->allRegs, state->regsInUse);

    int reg = sortedSetFirst(regs);

    Instructions *pushInstruction = newInstruction();
    pushInstruction->kind = INSTRUCTION_PUSH;
    pushInstruction->val.tempToPush = reg;
    pushInstruction->dataFlowEntry = state->current->dataFlowEntry;
    appendToInstruction(state->previous, pushInstruction);

    Instructions *popInstruction = newInstruction();
    popInstruction->kind = INSTRUCTION_POP;
    popInstruction->val.tempToPopInto = reg;
    popInstruction->dataFlowEntry = state->current->dataFlowEntry;
    appendToInstruction(state->current, popInstruction);

    if (!state->hasAddedPop) {
        state->latest = popInstruction;
        state->hasAddedPop = true;
    }

    insertSortedSet(state->regsInUse, reg);
    return reg;
}

RaVariableLocation *getLocation(int temporary, RaState *state) {
    Pair *pair = get(state->stackLocation, makeIntKey((long) currentSymbolTable));

    ConstMap *map;
    if (pair == NULL) {
        map = initMap(128);
        insert(state->stackLocation, makeIntKey((long) currentSymbolTable), map);
    } else {
        map = pair->v;
    }

    pair = get(map, makeIntKey(temporary));

    RaVariableLocation *location;
    if (pair == NULL) {
        location = NEW(RaVariableLocation);
        location->useScope = false;
        location->offset = currentSymbolTable->nextSymbolId * POINTER_SIZE;
        location->scope = currentSymbolTable->distanceFromRoot;

        insert(map, makeIntKey(temporary), location);
        currentSymbolTable->nextSymbolId++;
    } else {
        location = pair->v;
    }

    return location;
}

void readFromStack(int* colors, int temporary, int reg, RaState *state) {
    RaVariableLocation *location = getLocation(temporary, state);

    Instructions *read = newInstruction();
    read->dataFlowEntry = state->current->dataFlowEntry;

    if (location->useScope) {
        read->kind = COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE;
        read->val.tempFromStackScope.inputTemp = reg;
        read->val.tempFromStackScope.intermediate = getTemporary(colors, -1, state);
        read->val.tempFromStackScope.intermediate2 = getTemporary(colors, -1, state);
        read->val.tempFromStackScope.offset = location->offset;
        read->val.tempFromStackScope.scopeToFindFrame = location->scope;
    } else {
        read->kind = COMPLEX_MOVE_TEMPORARY_FROM_STACK;
        read->val.tempFromStack.inputTemp = reg;
        read->val.tempFromStack.offset = location->offset;
    }

    prependToInstruction(state->current, read);

    char *buffer = malloc(sizeof(char) * 128);
    sprintf(buffer, "read %d from stack to %d", temporary, reg);
    prependToInstruction(read, makeDebugInstructions(buffer));
}

void writeToStack(int* colors, int temporary, int reg, RaState *state) {
    RaVariableLocation *location = getLocation(temporary, state);

    Instructions *write = newInstruction();
    write->dataFlowEntry = state->current->dataFlowEntry;

    if (location->useScope) {
        write->kind = COMPLEX_MOVE_TEMPORARY_INTO_STACK_IN_SCOPE;
        write->val.tempIntoStackScope.tempToMove = reg;
        write->val.tempIntoStackScope.intermediate = getTemporary(colors, -1, state);
        write->val.tempIntoStackScope.intermediate2 = getTemporary(colors, -1, state);
        write->val.tempIntoStackScope.offset = location->offset;
        write->val.tempIntoStackScope.scopeToFindFrame = location->scope;
    } else {
        write->kind = COMPLEX_MOVE_TEMPORARY_INTO_STACK;
        write->val.tempIntoStack.tempToMove = reg;
        write->val.tempIntoStack.offset = location->offset;
    }

    appendToInstruction(state->current, write);

    char *buffer = malloc(sizeof(char) * 128);
    sprintf(buffer, "Write %d to stack for %d", reg, temporary);
    prependToInstruction(write, makeDebugInstructions(buffer));
}

int getReadTemporary(int *colors, size_t temporary, RaState *state) {
    int assigned = getAssigned(colors, temporary);
    if (assigned != -1) {
        return assigned;
    }

    int reg = getTemporary(colors, temporary, state);

    readFromStack(colors, temporary, reg, state);

    return reg;
}

int getWriteTemporary(int *colors, size_t temporary, RaState *state) {
    int assigned = getAssigned(colors, temporary);
    if (assigned != -1) {
        return assigned;
    }

    int reg = getTemporary(colors, temporary, state);

    writeToStack(colors, temporary, reg, state);

    return reg;
}

int getReadWriteTemporary(int *colors, size_t temporary, RaState *state) {
    int assigned = getAssigned(colors, temporary);
    if (assigned != -1) {
        return assigned;
    }

    int reg = getTemporary(colors, temporary, state);

    readFromStack(colors, temporary, reg, state);
    writeToStack(colors, temporary, reg, state);

    return reg;
}

void getTemporaries(int *colors, struct RaTemporaries *temporaries, RaState *state) {
    struct RaTemporaries *temporary = temporaries;

    int assigned;
    while (temporary != NULL) {
        assigned = getAssigned(colors, temporary->id);
        if (assigned != -1) {
            if (exists(state->regsInUse, assigned)) {
                // Something has gone wrong in liveness analysis
            }
            temporary->reg = assigned;
            temporary->assigned = true;
            insertSortedSet(state->regsInUse, temporary->reg);
        }

        temporary = temporary->next;
    }

    temporary = temporaries;
    while (temporary != NULL) {
        if (temporary->assigned) {
            temporary = temporary->next;
            continue;
        }

        switch (temporary->kind) {
            case RaRead:temporary->reg = getReadTemporary(colors, temporary->id, state);
                break;
            case RaWrite:temporary->reg = getWriteTemporary(colors, temporary->id, state);
                break;
            case RaReadWrite:temporary->reg = getReadWriteTemporary(colors, temporary->id, state);
                break;
            case RaIntermidiate:temporary->reg = getTemporary(colors, temporary->id, state);
                break;
        }
        temporary->assigned = true;

        temporary = temporary->next;
    }
}

int getTemporaryNoPushPop(int *colors, int temporary, RaState *state) {
    int assigned = getAssigned(colors, temporary);

    if (assigned != -1) {
        return assigned;
    }

    int offset = currentSymbolTable->nextSymbolId * POINTER_SIZE;
    currentSymbolTable->nextSymbolId++;


    SortedSet *regs = sortedSetDiff(state->allRegs, state->regsInUse);
    int reg = sortedSetFirst(regs);

    Instructions *pushInstruction = newInstruction();
    pushInstruction->dataFlowEntry = state->current->dataFlowEntry;
    pushInstruction->kind = COMPLEX_MOVE_TEMPORARY_INTO_STACK;
    pushInstruction->val.tempIntoStack.tempToMove = reg;
    pushInstruction->val.tempIntoStack.offset = offset;

    appendToInstruction(state->current, pushInstruction);

    Instructions *popInstruction = newInstruction();
    popInstruction->dataFlowEntry = state->current->dataFlowEntry;
    popInstruction->kind = COMPLEX_MOVE_TEMPORARY_FROM_STACK;
    popInstruction->val.tempFromStack.inputTemp = reg;
    popInstruction->val.tempFromStack.offset = offset;


    appendToInstruction(state->current, popInstruction);

    if (!state->hasAddedPop) {
        state->latest = popInstruction;
        state->hasAddedPop = true;
    }

    insertSortedSet(state->regsInUse, reg);
    return reg;
}

void handleArithmetic2(int *colors, RaState *state) {
    RaTemporaries *temporaries = makeTemporary(state->current->val.arithmetic2.source, RaRead);
    temporaries->next = makeTemporary(state->current->val.arithmetic2.dest, RaReadWrite);

    getTemporaries(colors, temporaries, state);

    state->current->val.arithmetic2.source = temporaries->reg;
    state->current->val.arithmetic2.dest = temporaries->next->reg;
}

RaVariableLocation *makeVariableLocationInScope(size_t scope, int offset) {
    RaVariableLocation *result = NEW(RaVariableLocation);
    result->offset = offset;
    result->scope = scope;
    result->useScope = true;
    return result;
}

RaVariableLocation *makeVariableLocation(int offset) {
    RaVariableLocation *result = NEW(RaVariableLocation);
    result->offset = offset;
    result->useScope = false;
    return result;
}

void handlePopInstruction(int *colors, RaState *state) {
    int assigned = getAssigned(colors, state->current->val.tempToPopInto);

    if (assigned != -1) {
        state->current->val.tempToPopInto = assigned;
        return;
    }

    RaVariableLocation *location = getLocation(state->current->val.tempToPopInto, state);

    state->current->kind = INSTRUCTION_POP_STACK;
    state->current->val.popPushStack.offset = location->offset;
}

void handlePushInstruction(int *colors, RaState *state) {
    int assigned = getAssigned(colors, state->current->val.tempToPush);

    if (assigned != -1) {
        state->current->val.tempToPush = assigned;
        return;
    }

    RaVariableLocation *location = getLocation(state->current->val.tempToPush, state);

    state->current->kind = INSTRUCTION_PUSH_STACK;
    state->current->val.popPushStack.offset = location->offset;
}

extern size_t maxTemporary;
/*
 * When we spill we put intermediate products on the stack
 * The position will be the context pointer: rbp + sizeof(var) * varsInContext
 */
int *simpleRegisterAllocation(Instructions *head, int numberRegisters) {
    LivenessAnalysisResult *livenessAnalysisResult = livenessAnalysis(head);
    int *colors = colorGraph(livenessAnalysisResult->sets, livenessAnalysisResult->numberSets, numberRegisters + 1);

    for (int i = 0; i < (int)maxTemporary; i++) {
        if (colors[i] < 0) {
            fprintf(stderr, "Temporary %d is spill\n", i);
        }
    }

    RaState *state = NEW(RaState);
    state->current = head;
    state->previous = NULL;
    state->regsInUse = NULL;
    state->allRegs = initHeadedSortedSet();
    state->stackLocation = initMap(128);
    Instructions *nextInstruction;

    for (int i = 0; i < numberRegisters; ++i) {
        insertSortedSet(state->allRegs, i + 1);
    }

    RaTemporaries *temporaries;
    while (state->current != NULL) {
        nextInstruction = state->current->next;
        state->latest = state->current;
        state->hasAddedPop = false;
        switch (state->current->kind) {
            case INSTRUCTION_ADD:state->regsInUse = initHeadedSortedSet();
                handleArithmetic2(colors, state);
                break;
            case INSTRUCTION_MINUS:handleArithmetic2(colors, state);
                break;
            case INSTRUCTION_MUL:handleArithmetic2(colors, state);
                break;
            case INSTRUCTION_DIV:handleArithmetic2(colors, state);
                break;
            case INSTRUCTION_CONST:
                state->current->val.constant.temp =
                        getWriteTemporary(colors, state->current->val.constant.temp, state);
                break;
            case INSTRUCTION_SET_ZERO:
                state->current->val.tempToSetZero =
                        getWriteTemporary(colors, state->current->val.tempToSetZero, state);
            case INSTRUCTION_PROGRAM_BEGIN:break;
            case INSTRUCTION_FUNCTION_LABEL:
                currentSymbolTable = state->current->val.functionHead.tableForFunction;
                currentSymbolTable->nextSymbolId++;

                state->current->val.functionHead.temporary =
                        getTemporary(colors, state->current->val.functionHead.temporary, state);
                break;
            case INSTRUCTION_VAR:break;
            case INSTRUCTION_FUNCTION_END:break;
            case INSTRUCTION_RETURN:
                state->current->val.tempToReturn =
                        getReadTemporary(colors, state->current->val.tempToReturn, state);
                break;
            case INSTRUCTION_WRITE:
                state->current->val.tempToWrite =
                        getReadTemporary(colors, state->current->val.tempToWrite, state);
                break;
            case INSTRUCTION_AND:handleArithmetic2(colors, state);
                break;
            case INSTRUCTION_OR:handleArithmetic2(colors, state);
                break;
            case INSTRUCTION_PUSH:
                handlePushInstruction(colors, state);
                break;
            case INSTRUCTION_POP:
                handlePopInstruction(colors, state);
                break;
            case INSTRUCTION_NEGATE:
                state->current->val.tempToNegate =
                        getReadWriteTemporary(colors, state->current->val.tempToNegate, state);
                break;
            case INSTRUCTION_ABS:
                state->current->val.tempToAbs =
                        getReadWriteTemporary(colors, state->current->val.tempToAbs, state);
                break;
            case INSTRUCTION_FUNCTION_CALL:break;
            case INSTRUCTION_RIGHT_SHIFT:
                state->current->val.rightShift.temp =
                        getReadWriteTemporary(colors, state->current->val.rightShift.temp, state);
                break;
            case INSTRUCTION_XOR:handleArithmetic2(colors, state);
                break;
            case INSTRUCTION_COPY:
                temporaries = makeTemporary(state->current->val.arithmetic2.source, RaRead);
                temporaries->next = makeTemporary(state->current->val.arithmetic2.dest, RaWrite);
                getTemporaries(colors, temporaries, state);

                state->current->val.arithmetic2.source = temporaries->reg;
                state->current->val.arithmetic2.dest = temporaries->next->reg;
                break;
            case INSTRUCTION_CMP:
                temporaries = makeTemporary(state->current->val.arithmetic2.source, RaRead);
                temporaries->next = makeTemporary(state->current->val.arithmetic2.dest, RaRead);
                getTemporaries(colors, temporaries, state);

                state->current->val.arithmetic2.source = temporaries->reg;
                state->current->val.arithmetic2.dest = temporaries->next->reg;
                break;
            case INSTRUCTION_LABEL:break;
            case INSTRUCTION_JE:break;
            case INSTRUCTION_JMP:break;
            case INSTRUCTION_MOVE:
                handleArithmetic2(colors, state);
                break;
            case INSTRUCTION_ADD_CONST:
                state->current->val.art2const.temp =
                        getReadWriteTemporary(colors, state->current->val.art2const.temp, state);
                break;
            case INSTRUCTION_MUL_CONST:
                state->current->val.art2const.temp =
                        getReadWriteTemporary(colors, state->current->val.art2const.temp, state);
                break;
            case INSTRUCTION_SUB_CONST:
                state->current->val.art2const.temp =
                        getReadWriteTemporary(colors, state->current->val.art2const.temp, state);
                break;
            case INSTRUCTION_MOVE_TO_OFFSET:
                temporaries = makeTemporary(state->current->val.moveToOffset.tempToMove, RaRead);
                temporaries->next = makeTemporary(state->current->val.moveToOffset.offsetTemp, RaRead);
                temporaries->next->next = makeTemporary(state->current->val.moveToOffset.ptrTemp, RaRead);
                getTemporaries(colors, temporaries, state);

                state->current->val.moveToOffset.tempToMove = temporaries->reg;
                state->current->val.moveToOffset.offsetTemp = temporaries->next->reg;
                state->current->val.moveToOffset.ptrTemp = temporaries->next->next->reg;
                break;
            case INSTRUCTION_LEA_TO_OFFSET:break;
            case COMPLEX_ALLOCATE:
                temporaries = makeTemporary(state->current->val.allocate.timesTemp, RaRead);
                temporaries->next = makeTemporary(state->current->val.allocate.intermediate, RaIntermidiate);

                getTemporaries(colors, temporaries, state);
                state->current->val.allocate.timesTemp = temporaries->reg;
                state->current->val.allocate.intermediate = temporaries->next->reg;
                break;
            case COMPLEX_ALLOCATE_END:break;
            case COMPLEX_CONSTRAIN_BOOLEAN:
                state->current->val.tempToConstrain =
                        getReadWriteTemporary(colors, state->current->val.tempToConstrain, state);
            break;
            case COMPLEX_MOVE_TEMPORARY_INTO_STACK:
                /*
                insert(state->stackLocation,
                       makeIntKey(state->current->val.tempIntoStack.tempToMove),
                       makeVariableLocation(state->current->val.tempIntoStack.offset));
               */

                state->current->val.tempIntoStack.tempToMove =
                        getReadTemporary(colors, state->current->val.tempIntoStack.tempToMove, state);
                break;
            case COMPLEX_MOVE_TEMPORARY_INTO_STACK_IN_SCOPE:
                /*
                insert(state->stackLocation,
                       makeIntKey(state->current->val.tempIntoStackScope.tempToMove),
                       makeVariableLocationInScope(state->current->val.tempIntoStackScope.offset,
                                                   state->current->val.tempIntoStackScope.scopeToFindFrame));
                */

                temporaries = makeTemporary(state->current->val.tempIntoStackScope.intermediate, RaIntermidiate);
                temporaries->next = makeTemporary(state->current->val.tempIntoStackScope.intermediate2, RaIntermidiate);
                temporaries->next->next = makeTemporary(state->current->val.tempIntoStackScope.tempToMove, RaRead);
                getTemporaries(colors, temporaries, state);

                state->current->val.tempIntoStackScope.intermediate = temporaries->reg;

                state->current->val.tempIntoStackScope.intermediate2 = temporaries->next->reg;

                state->current->val.tempIntoStackScope.tempToMove = temporaries->next->next->reg;
                break;
            case COMPLEX_MOVE_TEMPORARY_FROM_STACK:
                state->current->val.tempFromStack.inputTemp =
                        getWriteTemporary(colors, state->current->val.tempFromStack.inputTemp, state);
                break;
            case COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE:
                temporaries = makeTemporary(state->current->val.tempFromStackScope.intermediate, RaIntermidiate);
                temporaries->next = makeTemporary(state->current->val.tempFromStackScope.inputTemp, RaWrite);
                temporaries->next->next = makeTemporary(state->current->val.tempFromStackScope.intermediate2, RaIntermidiate);
                getTemporaries(colors, temporaries, state);

                state->current->val.tempFromStackScope.intermediate = temporaries->reg;

                state->current->val.tempFromStackScope.inputTemp = temporaries->next->reg;

                state->current->val.tempFromStackScope.intermediate2 = temporaries->next->next->reg;
                break;
            case COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET:
                temporaries = makeTemporary(state->current->val.dereferenceOffset.ptrTemp, RaReadWrite);
                temporaries->next = makeTemporary(state->current->val.dereferenceOffset.offsetTemp, RaRead);
                getTemporaries(colors, temporaries, state);

                state->current->val.dereferenceOffset.ptrTemp = temporaries->reg;
                state->current->val.dereferenceOffset.offsetTemp = temporaries->next->reg;
                break;
            case COMPLEX_SAVE_STATIC_LINK:
                state->current->val.pushPopStaticLink.temporary =
                        getTemporaryNoPushPop(colors, state->current->val.pushPopStaticLink.temporary, state);
                break;
            case COMPLEX_RESTORE_STATIC_LINK:
                state->current->val.pushPopStaticLink.temporary =
                        getTemporaryNoPushPop(colors, state->current->val.pushPopStaticLink.temporary, state);
                break;
            case METADATA_BEGIN_BODY_BLOCK:break;
            case METADATA_END_BODY_BLOCK:break;
            case METADATA_FUNCTION_ARGUMENT:
                state->current->val.args.moveReg =
                        getTemporary(colors, state->current->val.args.moveReg, state);
                break;
            case METADATA_CREATE_MAIN:
                currentSymbolTable = state->current->val.mainHeader.tableForFunction;
                currentSymbolTable->nextSymbolId++;
                break;
            case METADATA_BEGIN_ARITHMETIC_EVALUATION:break;
            case METADATA_END_ARITHMETIC_EVALUATION:break;
            case INSTRUCTION_REGISTER_CALL:
                state->current->val.callRegister =
                        getReadTemporary(colors, state->current->val.callRegister, state);
                break;
            case COMPLEX_RIP_LAMBDA_LOAD:
                state->current->val.lambdaLoad.temporary =
                        getWriteTemporary(colors, state->current->val.lambdaLoad.temporary, state);
                break;
            case METADATA_BEGIN_GLOBAL_BLOCK:break;
            case METADATA_END_GLOBAL_BLOCK:break;
            case INSTRUCTION_PUSH_STACK:break;
            case INSTRUCTION_POP_STACK:break;
            case METADATA_DEBUG_INFO:break;
            case COMPLEX_LOAD_POINTER_TO_STATIC_LINK_FRAME:break;
            case COMPLEX_GARBAGE_COLLECT:break;
            case INSTRUCTION_ADD_STACK_PTR:break;
            case COMPLEX_ABS_VALUE:
                temporaries = makeTemporary(state->current->val.arithmetic2.source, RaReadWrite);
                temporaries->next = makeTemporary(state->current->val.arithmetic2.dest, RaReadWrite);
                getTemporaries(colors, temporaries, state);

                state->current->val.arithmetic2.source = temporaries->reg;
                state->current->val.arithmetic2.dest = temporaries->next->reg;
                break;
            case COMPLEX_SAVE_ALL:break;
            case COMPLEX_RESTORE_ALL:break;
            case INSTRUCTION_WRITE_CHAR:
                state->current->val.tempToWrite =
                        getReadTemporary(colors, state->current->val.tempToWrite, state);
                break;
            case INSTRUCTION_WRITE_NL:break;
            case RUNTIME_ARRAY_BOUNDS_CHECK:
                temporaries = makeTemporary(state->current->val.arrayBounds.exprTemp, RaRead);
                temporaries->next = makeTemporary(state->current->val.arrayBounds.arrPtr, RaRead);
                getTemporaries(colors, temporaries, state);

                state->current->val.arrayBounds.exprTemp = temporaries->reg;
                state->current->val.arrayBounds.arrPtr = temporaries->next->reg;
                break;
            case RUNTIME_NEGATIVE_ALLOC:
                temporaries = makeTemporary(state->current->val.negLenTemp, RaRead);
                getTemporaries(colors, temporaries, state);

                state->current->val.negLenTemp = temporaries->reg;
                break;
            case RUNTIME_NULLPTR_CHECK:
                temporaries = makeTemporary(state->current->val.nullPtrCheck, RaRead);
                getTemporaries(colors, temporaries, state);

                state->current->val.nullPtrCheck = temporaries->reg;
                break;
            case RUNTIME_DIV_ZERO:
                temporaries = makeTemporary(state->current->val.divZeroTemp, RaRead);
                getTemporaries(colors, temporaries, state);

                state->current->val.divZeroTemp = temporaries->reg;
                break;
            case INSTRUCTION_LEA_ADD: state->regsInUse = initHeadedSortedSet();
                handleArithmetic2(colors, state);
                break;
            case INSTRUCTION_LEA_ADD_CONST:
                state->current->val.art2const.temp =
                        getReadWriteTemporary(colors, state->current->val.art2const.temp, state);
                break;
            case INSTRUCTION_LOAD_STACK_PTR:
                state->current->val.loadStackPtr.temp =
                        getWriteTemporary(colors, state->current->val.loadStackPtr.temp, state);
                break;
            case METADATA_ACCESS_VARIABLE_START:
                state->current->val.varAccess.temp = getAssigned(colors, state->current->val.varAccess.temp);
                break;
            case METADATA_ACCESS_VARIABLE_END:break;
            case NOOP:break;
        }

        freeSortedSet(state->regsInUse);
        state->regsInUse = initHeadedSortedSet();
        state->previous = state->latest;
        state->current = nextInstruction;
    }

    simplifyLoads(head, livenessAnalysisResult);
    return colors;
}
