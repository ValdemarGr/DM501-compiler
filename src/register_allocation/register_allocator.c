//
// Created by valde on 4/21/19.
//

#include "register_allocator.h"
#include "liveness_analysis.h"
#include "graph_coloring.h"
#include "../ast_to_ir/intermediate_representation.h"

ConstMap *currentRegisterContext = NULL;
size_t timestamp = 0;

RaTemporaries *makeTemporary(int id, RaTemporariesKind kind) {
    RaTemporaries *result = NEW(RaTemporaries);
    result->id = id;
    result->kind = kind;
    result->assigned = false;
    result->next = NULL;
}

int getTemporary(int *colors, int temporary, RaState *state) {
    if (temporary > -1) {
        if (colors[temporary] != -1) {
            return colors[temporary];
        }
    }

    SortedSet *regs = sortedSetDiff(state->allRegs, state->regsInUse);

    int reg = sortedSetFirst(regs);

    Instructions *pushInstruction = NEW(Instructions);
    pushInstruction->kind = INSTRUCTION_PUSH;
    pushInstruction->val.tempToPush = reg;
    state->previous->next = pushInstruction;
    pushInstruction->next = state->current;

    Instructions *popInstruction = NEW(Instructions);
    popInstruction->kind = INSTRUCTION_POP;
    popInstruction->val.tempToPopInto = reg;
    popInstruction->next = state->current->next;
    state->current->next = popInstruction;

    insertSortedSet(state->regsInUse, reg);
    return reg;
}

void readFromStack(int* colors, int temporary, int reg, RaState *state) {
    Pair *pair = get(state->stackLocation, makeIntKey(temporary));

    if (pair == NULL) {
        return;
    }

    RaVariableLocation *location = pair->v;
    Instructions *read = NEW(Instructions);

    if (location->useScope) {
        read->kind = COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE;
        read->val.tempFromStackScope.inputTemp = reg;
        read->val.tempFromStackScope.intermediate = getTemporary(colors, -1, state);
        read->val.tempFromStackScope.offset = location->offset;
        read->val.tempFromStackScope.scopeToFindFrame = location->scope;
    } else {
        read->kind = COMPLEX_MOVE_TEMPORARY_FROM_STACK;
        read->val.tempFromStack.inputTemp = reg;
        read->val.tempFromStack.offset = location->offset;
    }

    state->previous = read;
    read->next = state->current;
}

void writeToStack(int* colors, int temporary, int reg, RaState *state) {
    Pair *pair = get(state->stackLocation, makeIntKey(temporary));

    if (pair == NULL) {
        return;
    }

    RaVariableLocation *location = pair->v;
    Instructions *write = NEW(Instructions);

    if (location->useScope) {
        write->kind = COMPLEX_MOVE_TEMPORARY_INTO_STACK_IN_SCOPE;
        write->val.tempIntoStackScope.tempToMove = reg;
        write->val.tempIntoStackScope.intermediate = getTemporary(colors, -1, state);
        write->val.tempIntoStackScope.offset = location->offset;
        write->val.tempIntoStackScope.scopeToFindFrame = location->scope;
    } else {
        write->kind = COMPLEX_MOVE_TEMPORARY_INTO_STACK;
        write->val.tempIntoStack.tempToMove = reg;
        write->val.tempIntoStack.offset = location->offset;
    }

    write->next = state->current->next;
    state->current->next = write;
}

int getReadTemporary(int *colors, size_t temporary, RaState *state) {
    if (colors[temporary] != -1) {
        return colors[temporary];
    }

    int reg = getTemporary(colors, temporary, state);

    readFromStack(colors, temporary, reg, state);

    return reg;
}

int getWriteTemporary(int *colors, size_t temporary, RaState *state) {
    if (colors[temporary] != -1) {
        return colors[temporary];
    }

    int reg = getTemporary(colors, temporary, state);

    writeToStack(colors, temporary, reg, state);

    return reg;
}

int getReadWriteTemporary(int *colors, size_t temporary, RaState *state) {
    if (colors[temporary] != -1) {
        return colors[temporary];
    }

    int reg = getTemporary(colors, temporary, state);

    readFromStack(colors, temporary, reg, state);
    writeToStack(colors, temporary, reg, state);

    return reg;
}

SortedSet *getTemporaries(int *colors, struct RaTemporaries *temporaries, RaState *state) {
    struct RaTemporaries *temporary = temporaries;

    while (temporary != NULL) {
        if (colors[temporary->id] != -1) {
            temporary->reg = colors[temporary->id];
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

void handleArithmetic2(int *colors, RaState *state) {
    RaTemporaries *temporaries = makeTemporary(state->current->val.arithmetic2.source, RaWrite);
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

/*
 * When we spill we put intermediate products on the stack
 * The position will be the context pointer: rbp + sizeof(var) * varsInContext
 */
Instructions *simpleRegisterAllocation(Instructions *head, int numberRegisters) {
    LivenessAnalysisResult *livenessAnalysisResult = livenessAnalysis(head);
    int *colors = colorGraph(livenessAnalysisResult->sets, livenessAnalysisResult->numberSets, numberRegisters);

    RaState *state = NEW(RaState);
    state->current = head;
    state->previous = NULL;
    state->regsInUse = NULL;
    state->allRegs = initHeadedSortedSet();
    state->stackLocation = initMap(128);
    Instructions *nextInstruction;

    for (int i = 0; i < numberRegisters; ++i) {
        insertSortedSet(state->allRegs, i);
    }

    RaTemporaries *temporaries;
    while (state->current != NULL) {
        nextInstruction = state->current->next;
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
            case INSTRUCTION_PROGRAM_BEGIN:break;
            case INSTRUCTION_FUNCTION_LABEL:break;
            case INSTRUCTION_VAR:break;
            case INSTRUCTION_FUNCTION_END:break;
            case INSTRUCTION_RETURN:break;
            case INSTRUCTION_WRITE:
                state->current->val.tempToWrite =
                        getReadTemporary(colors, state->current->val.tempToWrite, state);
                break;
            case INSTRUCTION_AND:handleArithmetic2(colors, state);
                break;
            case INSTRUCTION_OR:handleArithmetic2(colors, state);
                break;
            case INSTRUCTION_PUSH:
                state->current->val.tempToPush =
                        getReadTemporary(colors, state->current->val.tempToPush, state);
                break;
            case INSTRUCTION_POP:
                state->current->val.tempToPopInto =
                        getWriteTemporary(colors, state->current->val.tempToPopInto, state);
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
            case INSTRUCTION_COPY:break;
            case INSTRUCTION_CMP:break;
            case INSTRUCTION_LABEL:break;
            case INSTRUCTION_JE:break;
            case INSTRUCTION_JMP:break;
            case INSTRUCTION_MOVE:break;
            case INSTRUCTION_ADD_CONST:break;
            case INSTRUCTION_MUL_CONST:break;
            case INSTRUCTION_MOVE_TO_OFFSET:break;
            case INSTRUCTION_LEA_TO_OFFSET:break;
            case COMPLEX_ALLOCATE:break;
            case COMPLEX_ALLOCATE_END:break;
            case COMPLEX_CONSTRAIN_BOOLEAN:break;
            case COMPLEX_MOVE_TEMPORARY_INTO_STACK:
                insert(state->stackLocation,
                       makeIntKey(state->current->val.tempIntoStack.tempToMove),
                       makeVariableLocation(state->current->val.tempIntoStack.offset));

                state->current->val.tempIntoStack.tempToMove = getReadTemporary(colors, state->current->val.tempIntoStack.tempToMove, state);
                break;
            case COMPLEX_MOVE_TEMPORARY_INTO_STACK_IN_SCOPE:
                insert(state->stackLocation,
                       makeIntKey(state->current->val.tempIntoStackScope.tempToMove),
                       makeVariableLocationInScope(state->current->val.tempIntoStackScope.offset,
                                                   state->current->val.tempIntoStackScope.scopeToFindFrame));

                temporaries = makeTemporary(state->current->val.tempIntoStackScope.intermediate, RaIntermidiate);
                temporaries->next = makeTemporary(state->current->val.tempIntoStackScope.tempToMove, RaRead);
                getTemporaries(colors, temporaries, state);

                state->current->val.tempIntoStackScope.intermediate = temporaries->reg;

                state->current->val.tempIntoStackScope.tempToMove = temporaries->next->reg;
                break;
            case COMPLEX_MOVE_TEMPORARY_FROM_STACK:
                state->current->val.tempFromStack.inputTemp =
                        getWriteTemporary(colors, state->current->val.tempFromStack.inputTemp, state);
                break;
            case COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE:
                temporaries = makeTemporary(state->current->val.tempFromStackScope.intermediate, RaIntermidiate);
                temporaries->next = makeTemporary(state->current->val.tempFromStackScope.inputTemp, RaWrite);
                getTemporaries(colors, temporaries, state);

                state->current->val.tempFromStackScope.intermediate = temporaries->reg;

                state->current->val.tempFromStackScope.inputTemp = temporaries->next->reg;
                break;
            case COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET:break;
            case COMPLEX_SAVE_STATIC_LINK:break;
            case COMPLEX_RESTORE_STATIC_LINK:break;
            case COMPLEX_LOAD_POINTER_TO_STATIC_LINK_FRAME:break;
            case METADATA_BEGIN_BODY_BLOCK:break;
            case METADATA_END_BODY_BLOCK:break;
            case METADATA_FUNCTION_ARGUMENT:break;
            case METADATA_CREATE_MAIN:break;
            case METADATA_BEGIN_ARITHMETIC_EVALUATION:break;
            case METADATA_END_ARITHMETIC_EVALUATION:break;
        }

        freeSortedSet(state->regsInUse);
        state->regsInUse = initHeadedSortedSet();
        state->previous = state->current;
        state->current = nextInstruction;
    }

    return head;
}
