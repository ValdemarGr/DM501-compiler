//
// Created by valde on 4/21/19.
//

#include "register_allocator.h"
#include "../ast_to_ir/intermediate_representation.h"

ConstMap *currentRegisterContext = NULL;
size_t timestamp = 0;

void dataFlowEntryFromArithmetic2(DataFlowEntry* dataFlowEntry, Arithmetic2 arithmetic2) {
    dataFlowEntry->defines = initHeadedSortedSet();
    insertSortedSet(dataFlowEntry->defines, (int)arithmetic2.dest);
    dataFlowEntry->uses = initHeadedSortedSet();
    insertSortedSet(dataFlowEntry->uses, (int)arithmetic2.source);
}

size_t line = 0;
DataFlowEntry *dataFlow;
DataFlowEntry *initDataFlowEntry() {
    DataFlowEntry *dataFlowEntry = NEW(DataFlowEntry);
    dataFlow[line] = *dataFlowEntry;
    line++;
    return dataFlowEntry;
}

IntBox *makeIntBox(int value) {
    IntBox *box = NEW(IntBox);
    box->value = value;
    return box;
}

LineList *makeLineList(int line) {
    LineList *list = NEW(LineList);
    list->line = line;
    return list;
}

LivenessAnalysisResult *livenessAnalysis(Instructions *instructions) {
    Instructions *iter = instructions;

    ConstMap *labels = initMap(2048);

    int line = 0;
    while(iter != NULL) {
        switch(iter->kind) {
            case METADATA_CREATE_MAIN: break;
            case METADATA_BEGIN_BODY_BLOCK: break;
            case METADATA_END_BODY_BLOCK: break;
            case METADATA_BEGIN_ARITHMETIC_EVALUATION: break;
            case METADATA_END_ARITHMETIC_EVALUATION: break;
            case METADATA_FUNCTION_ARGUMENT: break;
            case COMPLEX_ALLOCATE_END: break;
            case INSTRUCTION_FUNCTION_END: break;
            case INSTRUCTION_VAR: break;
            case INSTRUCTION_MOVE: break;
            case INSTRUCTION_PROGRAM_BEGIN: break;
            case INSTRUCTION_LABEL: {
                insert(labels, makeCharKey(iter->val.label), makeIntBox(line));
            } break;
            case INSTRUCTION_FUNCTION_LABEL: {
                line += 2;
                insert(labels, makeCharKey(iter->val.functionHead.label), makeIntBox(line));
            } break;
            case COMPLEX_MOVE_TEMPORARY_VALUE_TO_STACK_IN_SCOPE: {
                line += 2;
            } break;
            case COMPLEX_LOAD_POINTER_TO_STATIC_LINK_FRAME: {
                line += 2;
            } break;
            default: {
                line++;
            } break;
        }

        iter = iter->next;
    }

    iter = instructions;

    dataFlow = malloc(sizeof(DataFlowEntry) * line);
    int dataFlowSize = line;

    line = 0;
    DataFlowEntry *dataFlowEntry = NULL;
    while (iter != NULL) {
        switch (iter->kind) {
            case METADATA_CREATE_MAIN: break;
            case METADATA_BEGIN_BODY_BLOCK: break;
            case METADATA_END_BODY_BLOCK: break;
            case METADATA_BEGIN_ARITHMETIC_EVALUATION: break;
            case METADATA_END_ARITHMETIC_EVALUATION: break;
            case METADATA_FUNCTION_ARGUMENT: break;
            case COMPLEX_ALLOCATE_END: break;
            case INSTRUCTION_FUNCTION_END: break;
            case INSTRUCTION_VAR: break;
            case INSTRUCTION_MOVE: break;
            case INSTRUCTION_PROGRAM_BEGIN: break;
            case INSTRUCTION_LABEL: break;
            case INSTRUCTION_ADD: {
                dataFlowEntry = initDataFlowEntry();
                dataFlowEntryFromArithmetic2(dataFlowEntry, iter->val.arithmetic2);
                dataFlowEntry->successors = makeLineList(line + 1);
            } break;
            case INSTRUCTION_MINUS: {
                dataFlowEntry = initDataFlowEntry();
                dataFlowEntryFromArithmetic2(dataFlowEntry, iter->val.arithmetic2);
                dataFlowEntry->successors = makeLineList(line + 1);
            } break;
            case INSTRUCTION_MUL: {
                dataFlowEntry = initDataFlowEntry();
                dataFlowEntryFromArithmetic2(dataFlowEntry, iter->val.arithmetic2);
                dataFlowEntry->successors = makeLineList(line + 1);
            } break;
            case INSTRUCTION_DIV: {
                dataFlowEntry = initDataFlowEntry();
                dataFlowEntryFromArithmetic2(dataFlowEntry, iter->val.arithmetic2);
                dataFlowEntry->successors = makeLineList(line + 1);
            } break;
            case INSTRUCTION_CONST: {
                dataFlowEntry = initDataFlowEntry();
                dataFlowEntry->defines = initHeadedSortedSet();
                insertSortedSet(dataFlowEntry->defines, (int)iter->val.constant.temp);

                dataFlowEntry->uses = NULL;

                dataFlowEntry->successors = makeLineList(line + 1);
            } break;
            case INSTRUCTION_FUNCTION_LABEL: {
                dataFlowEntry = initDataFlowEntry();
                dataFlowEntry->defines = initHeadedSortedSet();
                insertSortedSet(dataFlowEntry->defines, (int)iter->val.saveTempToParentScope.intermediateTemp);
                dataFlowEntry->uses = NULL;

                dataFlowEntry->successors = makeLineList(line + 1);

                dataFlowEntry = initDataFlowEntry();
                dataFlowEntry->defines = NULL;
                dataFlowEntry->uses = initHeadedSortedSet();
                insertSortedSet(dataFlowEntry->uses, (int)iter->val.saveTempToParentScope.intermediateTemp);

                dataFlowEntry->successors = makeLineList(line + 1);
            } break;
            case INSTRUCTION_RETURN: {
                dataFlowEntry = initDataFlowEntry();
                dataFlowEntry->defines = NULL;

                dataFlowEntry->uses = initHeadedSortedSet();
                insertSortedSet(dataFlowEntry->uses, (int)iter->val.tempToReturn);
            } break;
            case INSTRUCTION_WRITE: {
                dataFlowEntry = initDataFlowEntry();
                dataFlowEntry->defines = NULL;

                dataFlowEntry->uses = initHeadedSortedSet();

                insertSortedSet(dataFlowEntry->uses, (int)iter->val.tempToReturn);

                dataFlowEntry->successors = makeLineList(line + 1);
            } break;
            case INSTRUCTION_AND: {
                dataFlowEntry = initDataFlowEntry();
                dataFlowEntryFromArithmetic2(dataFlowEntry, iter->val.arithmetic2);
                dataFlowEntry->successors = makeLineList(line + 1);
            } break;
            case INSTRUCTION_OR: {
                dataFlowEntry = initDataFlowEntry();
                dataFlowEntryFromArithmetic2(dataFlowEntry, iter->val.arithmetic2);
                dataFlowEntry->successors = makeLineList(line + 1);
            } break;
            case INSTRUCTION_PUSH: {
                dataFlowEntry = initDataFlowEntry();
                dataFlowEntry->defines = NULL;

                dataFlowEntry->uses = initHeadedSortedSet();

                insertSortedSet(dataFlowEntry->uses, (int)iter->val.tempToPush);

                dataFlowEntry->successors = makeLineList(line + 1);
            } break;
            case INSTRUCTION_POP: {
                dataFlowEntry = initDataFlowEntry();
                dataFlowEntry->defines = NULL;

                dataFlowEntry->uses = initHeadedSortedSet();

                insertSortedSet(dataFlowEntry->uses, (int)iter->val.tempToPopInto);

                dataFlowEntry->successors = makeLineList(line + 1);
            } break;
            case INSTRUCTION_NEGATE: {
                dataFlowEntry = initDataFlowEntry();
                dataFlowEntry->defines = initHeadedSortedSet();
                insertSortedSet(dataFlowEntry->defines, (int)iter->val.tempToNegate);

                dataFlowEntry->uses = initHeadedSortedSet();

                insertSortedSet(dataFlowEntry->uses, (int)iter->val.tempToNegate);

                dataFlowEntry->successors = makeLineList(line + 1);
            } break;
            case INSTRUCTION_ABS: {
                dataFlowEntry = initDataFlowEntry();
                dataFlowEntry->defines = initHeadedSortedSet();
                insertSortedSet(dataFlowEntry->defines, (int)iter->val.tempToAbs);

                dataFlowEntry->uses = initHeadedSortedSet();

                insertSortedSet(dataFlowEntry->uses, (int)iter->val.tempToAbs);

                dataFlowEntry->successors = makeLineList(line + 1);
            } break;
            case INSTRUCTION_FUNCTION_CALL: {
                dataFlowEntry = initDataFlowEntry();
                dataFlowEntry->defines = NULL;
                dataFlowEntry->uses = NULL;

                dataFlowEntry->successors = makeLineList(line + 1);
                int targetLine = ((IntBox*)get(labels, makeCharKey(iter->val.function))->v)->value;
                dataFlowEntry->successors->next = makeLineList(targetLine);
            } break;
            case INSTRUCTION_RIGHT_SHIFT: {
                dataFlowEntry = initDataFlowEntry();
                dataFlowEntry->defines = initHeadedSortedSet();
                insertSortedSet(dataFlowEntry->defines, (int)iter->val.rightShift.temp);

                dataFlowEntry->uses = initHeadedSortedSet();

                insertSortedSet(dataFlowEntry->uses, (int)iter->val.rightShift.temp);

                dataFlowEntry->successors = makeLineList(line + 1);
            } break;
            case INSTRUCTION_XOR: {
                dataFlowEntry = initDataFlowEntry();
                dataFlowEntryFromArithmetic2(dataFlowEntry, iter->val.arithmetic2);
                dataFlowEntry->successors = makeLineList(line + 1);
            } break;
            case INSTRUCTION_COPY: {
                dataFlowEntry = initDataFlowEntry();
                dataFlowEntryFromArithmetic2(dataFlowEntry, iter->val.arithmetic2);
                dataFlowEntry->successors = makeLineList(line + 1);
            } break;
            case INSTRUCTION_CMP: {
                dataFlowEntry = initDataFlowEntry();
                dataFlowEntryFromArithmetic2(dataFlowEntry, iter->val.arithmetic2);
                dataFlowEntry->successors = makeLineList(line + 1);
            } break;
            case INSTRUCTION_JE: {
                dataFlowEntry = initDataFlowEntry();
                dataFlowEntry->defines = NULL;
                dataFlowEntry->uses = NULL;

                dataFlowEntry->successors = makeLineList(line + 1);
                int targetLine = ((IntBox*)get(labels, makeCharKey(iter->val.label))->v)->value;
                dataFlowEntry->successors->next = makeLineList(targetLine);
            } break;
            case INSTRUCTION_JMP: {
                dataFlowEntry = initDataFlowEntry();
                dataFlowEntry->defines = NULL;
                dataFlowEntry->uses = NULL;

                int targetLine = ((IntBox*)get(labels, makeCharKey(iter->val.function))->v)->value;
                dataFlowEntry->successors = makeLineList(targetLine);
            } break;
            case COMPLEX_ALLOCATE: {
                dataFlowEntry = initDataFlowEntry();
                dataFlowEntry->defines = initHeadedSortedSet();
                insertSortedSet(dataFlowEntry->defines, (int)iter->val.allocate.ptrTemp);

                dataFlowEntry->uses = initHeadedSortedSet();

                insertSortedSet(dataFlowEntry->uses, (int)iter->val.allocate.timesTemp);

                dataFlowEntry->successors = makeLineList(line + 1);
            } break;
            case COMPLEX_CONSTRAIN_BOOLEAN: {
                dataFlowEntry = initDataFlowEntry();
                dataFlowEntry->defines = initHeadedSortedSet();
                insertSortedSet(dataFlowEntry->defines, (int)iter->val.tempToConstrain);

                dataFlowEntry->uses = initHeadedSortedSet();

                insertSortedSet(dataFlowEntry->uses, (int)iter->val.tempToConstrain);

                dataFlowEntry->successors = makeLineList(line + 1);
            } break;
            case COMPLEX_LOAD_VARIABLE_POINTER_FROM_STACK_IN_SCOPE: {
                dataFlowEntry = initDataFlowEntry();
                dataFlowEntry->defines = initHeadedSortedSet();
                insertSortedSet(dataFlowEntry->defines, (int)iter->val.loadTempFromParentScope.outputTemp);

                dataFlowEntry->uses = NULL;

                dataFlowEntry->successors = makeLineList(line + 1);
            } break;
            case COMPLEX_LOAD_VARIABLE_POINTER_FROM_STACK: {
                dataFlowEntry = initDataFlowEntry();
                dataFlowEntry->defines = initHeadedSortedSet();
                insertSortedSet(dataFlowEntry->defines, (int)iter->val.currentScopeLoad.temporary);

                dataFlowEntry->uses = NULL;

                dataFlowEntry->successors = makeLineList(line + 1);
            } break;
            case COMPLEX_MOVE_TEMPORARY_VALUE_INTO_POINTER: {
                dataFlowEntry = initDataFlowEntry();
                dataFlowEntry->defines = initHeadedSortedSet();
                insertSortedSet(dataFlowEntry->defines, (int)iter->val.currentScopeLoad.temporary);

                dataFlowEntry->uses = NULL;

                dataFlowEntry->successors = makeLineList(line + 1);
            } break;
            case COMPLEX_MOVE_TEMPORARY_VALUE_INTO_POINTER_IN_SCOPE: {
                dataFlowEntry = initDataFlowEntry();
                dataFlowEntry->defines = initHeadedSortedSet();
                insertSortedSet(dataFlowEntry->defines, (int)iter->val.saveTempToParentScope.intermediateTemp);

                dataFlowEntry->uses = initHeadedSortedSet();

                insertSortedSet(dataFlowEntry->uses, (int)iter->val.saveTempToParentScope.inputTemp);

                dataFlowEntry->successors = makeLineList(line + 1);

                dataFlowEntry = initDataFlowEntry();
                dataFlowEntry->defines = NULL;

                dataFlowEntry->uses = initHeadedSortedSet();
                insertSortedSet(dataFlowEntry->uses, (int)iter->val.saveTempToParentScope.inputTemp);
                insertSortedSet(dataFlowEntry->uses, (int)iter->val.saveTempToParentScope.intermediateTemp);

                dataFlowEntry->successors = makeLineList(line + 1);
            } break;
            case COMPLEX_SAVE_STATIC_LINK: {
                dataFlowEntry = initDataFlowEntry();
                dataFlowEntry->defines = initHeadedSortedSet();
                insertSortedSet(dataFlowEntry->defines, (int)iter->val.pushPopStaticLink.temporary);

                dataFlowEntry->uses = initHeadedSortedSet();
                insertSortedSet(dataFlowEntry->uses, (int)iter->val.pushPopStaticLink.temporary);

                dataFlowEntry->successors = makeLineList(line + 1);
            } break;
            case COMPLEX_RESTORE_STATIC_LINK: {
                dataFlowEntry = initDataFlowEntry();
                dataFlowEntry->defines = initHeadedSortedSet();
                insertSortedSet(dataFlowEntry->defines, (int)iter->val.pushPopStaticLink.temporary);

                dataFlowEntry->uses = initHeadedSortedSet();
                insertSortedSet(dataFlowEntry->uses, (int)iter->val.pushPopStaticLink.temporary);

                dataFlowEntry->successors = makeLineList(line + 1);
            } break;
            case COMPLEX_LOAD_POINTER_TO_STATIC_LINK_FRAME: {
                dataFlowEntry = initDataFlowEntry();
                dataFlowEntry->defines = initHeadedSortedSet();
                insertSortedSet(dataFlowEntry->defines, (int)iter->val.loadPtrToStaticLink.intermediateTemp);

                dataFlowEntry->uses = initHeadedSortedSet();
                insertSortedSet(dataFlowEntry->uses, (int)iter->val.loadPtrToStaticLink.ptrTemp);

                dataFlowEntry->successors = makeLineList(line + 1);

                dataFlowEntry = initDataFlowEntry();
                dataFlowEntry->defines = initHeadedSortedSet();
                insertSortedSet(dataFlowEntry->defines, (int)iter->val.loadPtrToStaticLink.intermediateTemp);

                dataFlowEntry->uses = initHeadedSortedSet();
                insertSortedSet(dataFlowEntry->uses, (int)iter->val.loadPtrToStaticLink.ptrTemp);
                insertSortedSet(dataFlowEntry->uses, (int)iter->val.loadPtrToStaticLink.intermediateTemp);

                dataFlowEntry->successors = makeLineList(line + 1);
            } break;
            case INSTRUCTION_ADD_CONST: {
                dataFlowEntry = initDataFlowEntry();
                dataFlowEntry->defines = initHeadedSortedSet();
                insertSortedSet(dataFlowEntry->defines, (int)iter->val.art2const.temp);

                dataFlowEntry->uses = initHeadedSortedSet();

                insertSortedSet(dataFlowEntry->uses, (int)iter->val.art2const.temp);

                dataFlowEntry->successors = makeLineList(line + 1);
            } break;
            case INSTRUCTION_MUL_CONST: {
                dataFlowEntry = initDataFlowEntry();
                dataFlowEntry->defines = initHeadedSortedSet();
                insertSortedSet(dataFlowEntry->defines, (int)iter->val.art2const.temp);

                dataFlowEntry->uses = initHeadedSortedSet();

                insertSortedSet(dataFlowEntry->uses, (int)iter->val.art2const.temp);

                dataFlowEntry->successors = makeLineList(line + 1);
            } break;
            case COMPLEX_LOAD_VARIABLE_VALUE_FROM_STACK: {
                dataFlowEntry = initDataFlowEntry();
                dataFlowEntry->defines = initHeadedSortedSet();
                insertSortedSet(dataFlowEntry->defines, (int)iter->val.currentScopeLoad.temporary);

                dataFlowEntry->uses = NULL;

                dataFlowEntry->successors = makeLineList(line + 1);
            } break;
            case COMPLEX_LOAD_VARIABLE_VALUE_FROM_STACK_IN_SCOPE: {
                dataFlowEntry = initDataFlowEntry();
                dataFlowEntry->defines = initHeadedSortedSet();
                insertSortedSet(dataFlowEntry->defines, (int)iter->val.loadTempFromParentScope.outputTemp);

                dataFlowEntry->uses = NULL;

                dataFlowEntry->successors = makeLineList(line + 1);
            } break;
            case COMPLEX_MOVE_TEMPORARY_VALUE_TO_STACK: {
                dataFlowEntry = initDataFlowEntry();
                dataFlowEntry->defines = NULL;

                dataFlowEntry->uses = initHeadedSortedSet();

                insertSortedSet(dataFlowEntry->uses, (int)iter->val.currentScopeSave.tempValue);

                dataFlowEntry->successors = makeLineList(line + 1);
            } break;
            case COMPLEX_MOVE_TEMPORARY_VALUE_TO_STACK_IN_SCOPE: {
                dataFlowEntry = initDataFlowEntry();
                dataFlowEntry->defines = initHeadedSortedSet();
                insertSortedSet(dataFlowEntry->defines, (int)iter->val.saveTempToParentScope.intermediateTemp);

                dataFlowEntry->uses = initHeadedSortedSet();

                insertSortedSet(dataFlowEntry->uses, (int)iter->val.saveTempToParentScope.inputTemp);

                dataFlowEntry->successors = makeLineList(line + 1);

                dataFlowEntry = initDataFlowEntry();
                dataFlowEntry->defines = NULL;

                dataFlowEntry->uses = initHeadedSortedSet();
                insertSortedSet(dataFlowEntry->uses, (int)iter->val.saveTempToParentScope.inputTemp);
                insertSortedSet(dataFlowEntry->uses, (int)iter->val.saveTempToParentScope.intermediateTemp);

                dataFlowEntry->successors = makeLineList(line + 1);
            } break;
            default:break;
        }

        iter = iter->next;
    }

    LineList *successor;
    LineList *predecessor;

    for (int i = 0; i < dataFlowSize; ++i) {
        successor = dataFlow[i].successors;

        while(successor != NULL) {
            predecessor = dataFlow[successor->line].predecessors;
            if (predecessor == NULL) {
                dataFlow[successor->line].predecessors = makeLineList(i);
            } else {
                while (predecessor->next != NULL) {
                    predecessor = predecessor->next;
                }

                predecessor->next = makeLineList(i);
            }
        }
    }

    for (int i = dataFlowSize - 1; i >= 0; --i) {
        dataFlowEntry = &dataFlow[i];

        dataFlowEntry->out = initHeadedSortedSet();

        successor = dataFlowEntry->predecessors;
        while(successor != NULL) {
            dataFlowEntry->out = sortedSetUnion(dataFlowEntry->out,
                                                dataFlow[successor->line].in);
            successor = successor->next;
        }

        dataFlowEntry->in = sortedSetUnion(dataFlowEntry->uses,
                                           sortedSetDiff(dataFlowEntry->out, dataFlowEntry->defines));
    }

    LivenessAnalysisResult *result = NEW(LivenessAnalysisResult);
    result->numberSets = dataFlowSize;
    result->sets = malloc(sizeof(SortedSet) * dataFlowSize);
    for (int i = 0; i < dataFlowSize; ++i) {
        result->sets[i] = sortedSetUnion(dataFlow[i].in, dataFlow[i].out);
    }

    return result;
}

SortedSet *evaluateRequiredRegistersForExpression(Instructions *instructions) {

    Instructions *iter = instructions;
    size_t registerDepth = 0;
    SortedSet *temporariesForExp = initHeadedSortedSet();

    while (iter != NULL) {
        switch (iter->kind) {
            case INSTRUCTION_ADD: {
                insertSortedSet(temporariesForExp, (int)iter->val.arithmetic2.dest);
            } break;
            case INSTRUCTION_MINUS: {
                insertSortedSet(temporariesForExp, (int)iter->val.arithmetic2.dest);
            } break;
            case INSTRUCTION_MUL: {
                insertSortedSet(temporariesForExp, (int)iter->val.arithmetic2.dest);
            } break;
            case INSTRUCTION_DIV: {
                insertSortedSet(temporariesForExp, (int)iter->val.arithmetic2.dest);
            } break;
            case INSTRUCTION_CONST: {
                insertSortedSet(temporariesForExp, (int)iter->val.constant.temp);
            } break;
            case INSTRUCTION_PROGRAM_BEGIN: {

            } break;
            case INSTRUCTION_FUNCTION_LABEL: {

            } break;
            case INSTRUCTION_VAR: {

            } break;
            case INSTRUCTION_FUNCTION_END: {

            } break;
            case INSTRUCTION_RETURN: {

            } break;
            case INSTRUCTION_WRITE: {
            } break;
            case INSTRUCTION_AND: {
                insertSortedSet(temporariesForExp, (int)iter->val.arithmetic2.dest);
            } break;
            case INSTRUCTION_OR: {
                insertSortedSet(temporariesForExp, (int)iter->val.arithmetic2.dest);
            } break;
            case INSTRUCTION_PUSH: {

            } break;
            case INSTRUCTION_POP: {
                insertSortedSet(temporariesForExp, (int)iter->val.arithmetic2.dest);
            } break;
            case INSTRUCTION_NEGATE: {

            } break;
            case INSTRUCTION_ABS: {

            } break;
            case INSTRUCTION_FUNCTION_CALL: {

            } break;
            case INSTRUCTION_RIGHT_SHIFT: {
                insertSortedSet(temporariesForExp, (int)iter->val.arithmetic2.dest);
            } break;
            case INSTRUCTION_XOR: {
                insertSortedSet(temporariesForExp, (int)iter->val.arithmetic2.dest);
            } break;
            case INSTRUCTION_COPY: {
                insertSortedSet(temporariesForExp, (int)iter->val.arithmetic2.dest);
            } break;
            case INSTRUCTION_CMP: {
            } break;
            case INSTRUCTION_LABEL: {

            } break;
            case INSTRUCTION_JE: {

            } break;
            case INSTRUCTION_JMP: {

            } break;
            case COMPLEX_ALLOCATE: {
                insertSortedSet(temporariesForExp, (int)iter->val.allocate.ptrTemp);
            } break;
            case COMPLEX_ALLOCATE_END: {

            } break;
            case COMPLEX_CONSTRAIN_BOOLEAN: {

            } break;
            case COMPLEX_LOAD_VARIABLE_POINTER_FROM_STACK_IN_SCOPE: {
                insertSortedSet(temporariesForExp, (int)iter->val.loadTempFromParentScope.outputTemp);
            } break;
            case COMPLEX_LOAD_VARIABLE_POINTER_FROM_STACK: {
                insertSortedSet(temporariesForExp, (int)iter->val.currentScopeLoad.temporary);
            } break;
            case COMPLEX_MOVE_TEMPORARY_VALUE_INTO_POINTER: {
                insertSortedSet(temporariesForExp, (int)iter->val.currentScopeSave.intermediate);
            } break;
            case COMPLEX_MOVE_TEMPORARY_VALUE_INTO_POINTER_IN_SCOPE: {
                insertSortedSet(temporariesForExp, (int)iter->val.saveTempToParentScope.intermediateTemp);
            } break;
            case COMPLEX_SAVE_STATIC_LINK: {
                insertSortedSet(temporariesForExp, (int)iter->val.pushPopStaticLink.temporary);
            } break;
            case COMPLEX_RESTORE_STATIC_LINK: {
                insertSortedSet(temporariesForExp, (int)iter->val.pushPopStaticLink.temporary);
            } break;
            case COMPLEX_LOAD_POINTER_TO_STATIC_LINK_FRAME: {
                insertSortedSet(temporariesForExp, (int)iter->val.loadPtrToStaticLink.intermediateTemp);
            } break;
            case METADATA_BEGIN_BODY_BLOCK: {

            } break;
            case METADATA_END_BODY_BLOCK: {

            } break;
            case METADATA_FUNCTION_ARGUMENT: {

            } break;
            case METADATA_CREATE_MAIN: {

            } break;
            case METADATA_BEGIN_ARITHMETIC_EVALUATION: {
                registerDepth = registerDepth + 1;
            } break;
            case METADATA_END_ARITHMETIC_EVALUATION: {
                if (registerDepth == 0) {
                    return temporariesForExp;
                }
            } break;
            case INSTRUCTION_MOVE: {

            } break;
            case INSTRUCTION_ADD_CONST: {

            } break;
            case INSTRUCTION_MUL_CONST: {

            } break;
        }

        iter = iter->next;
    }

    return temporariesForExp;
}

AbstractRegister *initRegister(char *name) {
    AbstractRegister *abstractRegister = (AbstractRegister*)malloc(sizeof(AbstractRegister));

    abstractRegister->useTimestamp = timestamp;
    abstractRegister->registerName = name;
    abstractRegister->boundInstruction = NULL;

    return abstractRegister;
}

ConstMap *registerMap() {
    ConstMap *registers = initMap(14);

    insert(registers, makeIntKey(0), initRegister("rax"));
    insert(registers, makeIntKey(1), initRegister("rcx"));
    insert(registers, makeIntKey(2), initRegister("rdx"));
    insert(registers, makeIntKey(3), initRegister("rbx"));
    insert(registers, makeIntKey(4), initRegister("rsi"));
    insert(registers, makeIntKey(5), initRegister("rdi"));
    insert(registers, makeIntKey(6), initRegister("r8"));
    insert(registers, makeIntKey(7), initRegister("r9"));
    insert(registers, makeIntKey(8), initRegister("r10"));
    insert(registers, makeIntKey(9), initRegister("r11"));
    insert(registers, makeIntKey(10), initRegister("r12"));
    insert(registers, makeIntKey(11), initRegister("r13"));
    insert(registers, makeIntKey(12), initRegister("r14"));
    insert(registers, makeIntKey(13), initRegister("r15"));

    return registers;
}

/*
 * When we spill we put intermediate products on the stack
 * The position will be the context pointer: rbp + sizeof(var) * varsInContext
 */
void simpleRegisterAllocation(Instructions *head) {
    if (currentRegisterContext == NULL) {
        currentRegisterContext = NEW(ConstMap);
    }

    Instructions *iter = head;

    int i = 0;

    while (iter != NULL) {

        switch (iter->kind) {
            case INSTRUCTION_ADD: {

            } break;
            case INSTRUCTION_MINUS: {

            } break;
            case INSTRUCTION_MUL: {

            } break;
            case INSTRUCTION_DIV: {

            } break;
            case INSTRUCTION_CONST: {

            } break;
            case INSTRUCTION_PROGRAM_BEGIN: {

            } break;
            case INSTRUCTION_FUNCTION_LABEL: {

            } break;
            case INSTRUCTION_VAR: {

            } break;
            case INSTRUCTION_FUNCTION_END: {

            } break;
            case INSTRUCTION_RETURN: {

            } break;
            case INSTRUCTION_WRITE: {

            } break;
            case INSTRUCTION_AND: {

            } break;
            case INSTRUCTION_OR: {

            } break;
            case INSTRUCTION_PUSH: {

            } break;
            case INSTRUCTION_POP: {

            } break;
            case INSTRUCTION_NEGATE: {

            } break;
            case INSTRUCTION_ABS: {

            } break;
            case INSTRUCTION_FUNCTION_CALL: {

            } break;
            case INSTRUCTION_RIGHT_SHIFT: {

            } break;
            case INSTRUCTION_XOR: {

            } break;
            case INSTRUCTION_COPY: {

            } break;
            case COMPLEX_CONSTRAIN_BOOLEAN: {

            } break;
            case COMPLEX_LOAD_VARIABLE_POINTER_FROM_STACK_IN_SCOPE: {

            } break;
            case COMPLEX_ALLOCATE: {

            } break;
            case COMPLEX_LOAD_VARIABLE_POINTER_FROM_STACK: {

            } break;
            case COMPLEX_MOVE_TEMPORARY_VALUE_INTO_POINTER: {

            } break;
            case COMPLEX_MOVE_TEMPORARY_VALUE_INTO_POINTER_IN_SCOPE: {

            } break;
            case METADATA_BEGIN_BODY_BLOCK: {

            } break;
            case METADATA_END_BODY_BLOCK: {

            } break;
            case METADATA_FUNCTION_ARGUMENT: {

            } break;
            case METADATA_CREATE_MAIN: {

            } break;
            case INSTRUCTION_CMP: {

            } break;
            case INSTRUCTION_LABEL: {

            } break;
            case INSTRUCTION_JE: {

            } break;
            case INSTRUCTION_JMP: {

            } break;
            case COMPLEX_ALLOCATE_END: {

            } break;
            case COMPLEX_SAVE_STATIC_LINK: {

            } break;
            case COMPLEX_RESTORE_STATIC_LINK: {

            } break;
            case COMPLEX_LOAD_POINTER_TO_STATIC_LINK_FRAME: {

            } break;
            case METADATA_BEGIN_ARITHMETIC_EVALUATION: {
                SortedSet *regs = evaluateRequiredRegistersForExpression(iter);
                size_t size = length(regs);
            } break;
            case METADATA_END_ARITHMETIC_EVALUATION: {

            } break;
            case INSTRUCTION_MOVE: {

            } break;
            case INSTRUCTION_ADD_CONST: {

            } break;
            case INSTRUCTION_MUL_CONST: {

            } break;
        }

        iter = iter->next;
    }
}
