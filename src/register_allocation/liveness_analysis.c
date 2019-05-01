//
// Created by Mads Grau Kristensen on 2019-04-30.
//

#include "liveness_analysis.h"
#include "../utils/int_linked_list.h"
#include "../ast_to_ir/intermediate_representation.h"

int line = 0;

DataFlowEntry **dataFlow;

void dataFlowEntryFromArithmetic2(DataFlowEntry *dataFlowEntry, Arithmetic2 arithmetic2) {
    dataFlowEntry->defines = initHeadedSortedSet();
    insertSortedSet(dataFlowEntry->defines, (int) arithmetic2.dest);
    dataFlowEntry->uses = initHeadedSortedSet();
    insertSortedSet(dataFlowEntry->uses, (int) arithmetic2.source);
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

void freeLineList(LineList *lineList) {
    LineList *list = lineList;
    LineList *next;
    while (list != NULL) {
        next = list->next;
        free(list);
        list = next;
    }
}

void freeDataFlowEntry(DataFlowEntry *entry) {
    freeSortedSet(entry->out);
    freeSortedSet(entry->in);
    freeSortedSet(entry->defines);
    freeSortedSet(entry->uses);

    freeLineList(entry->successors);

    free(entry);
}

Instructions *iter;

DataFlowEntry *initDataFlowEntry() {
    DataFlowEntry *dataFlowEntry = NEW(DataFlowEntry);
    dataFlowEntry->instruction = iter;
    line++;
    dataFlow[line] = dataFlowEntry;
    return dataFlowEntry;
}

LivenessAnalysisResult *livenessAnalysis(Instructions *instructions) {
    iter = instructions;
    ConstMap *labels = initMap(2048);

    int count = 0;
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
            case INSTRUCTION_LABEL: {
                insert(labels, makeCharKey(iter->val.label), makeIntBox(count));
            }
                break;
            case INSTRUCTION_FUNCTION_LABEL: {
                count += 1;
                insert(labels, makeCharKey(iter->val.functionHead.label), makeIntBox(count));
            }
                break;
            case COMPLEX_LOAD_POINTER_TO_STATIC_LINK_FRAME: {
                count += 2;
            }
                break;
            default: {
                count++;
            }
                break;
        }

        iter = iter->next;
    }

    iter = instructions;

    dataFlow = malloc(sizeof(DataFlowEntry) * line);
    int dataFlowSize = count;

    line = -1;
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
            }
                break;
            case INSTRUCTION_MINUS: {
                dataFlowEntry = initDataFlowEntry();
                dataFlowEntryFromArithmetic2(dataFlowEntry, iter->val.arithmetic2);
                dataFlowEntry->successors = makeLineList(line + 1);
            }
                break;
            case INSTRUCTION_MUL: {
                dataFlowEntry = initDataFlowEntry();
                dataFlowEntryFromArithmetic2(dataFlowEntry, iter->val.arithmetic2);
                dataFlowEntry->successors = makeLineList(line + 1);
            }
                break;
            case INSTRUCTION_DIV: {
                dataFlowEntry = initDataFlowEntry();
                dataFlowEntryFromArithmetic2(dataFlowEntry, iter->val.arithmetic2);
                dataFlowEntry->successors = makeLineList(line + 1);
            }
                break;
            case INSTRUCTION_CONST: {
                dataFlowEntry = initDataFlowEntry();
                dataFlowEntry->defines = initHeadedSortedSet();
                insertSortedSet(dataFlowEntry->defines, (int) iter->val.constant.temp);

                dataFlowEntry->uses = initHeadedSortedSet();

                dataFlowEntry->successors = makeLineList(line + 1);
            }
                break;
            case INSTRUCTION_FUNCTION_LABEL: {
                dataFlowEntry = initDataFlowEntry();
                dataFlowEntry->defines = initHeadedSortedSet();
                insertSortedSet(dataFlowEntry->defines, (int) iter->val.functionHead.temporary);
                dataFlowEntry->uses = initHeadedSortedSet();
                insertSortedSet(dataFlowEntry->uses, (int) iter->val.functionHead.temporary);

                dataFlowEntry->successors = makeLineList(line + 1);
            }
                break;
            case INSTRUCTION_RETURN: {
                dataFlowEntry = initDataFlowEntry();
                dataFlowEntry->defines = initHeadedSortedSet();

                dataFlowEntry->uses = initHeadedSortedSet();
                insertSortedSet(dataFlowEntry->uses, (int) iter->val.tempToReturn);
            }
                break;
            case INSTRUCTION_WRITE: {
                dataFlowEntry = initDataFlowEntry();
                dataFlowEntry->defines = initHeadedSortedSet();

                dataFlowEntry->uses = initHeadedSortedSet();

                insertSortedSet(dataFlowEntry->uses, (int) iter->val.tempToReturn);

                dataFlowEntry->successors = makeLineList(line + 1);
            }
                break;
            case INSTRUCTION_AND: {
                dataFlowEntry = initDataFlowEntry();
                dataFlowEntryFromArithmetic2(dataFlowEntry, iter->val.arithmetic2);
                dataFlowEntry->successors = makeLineList(line + 1);
            }
                break;
            case INSTRUCTION_OR: {
                dataFlowEntry = initDataFlowEntry();
                dataFlowEntryFromArithmetic2(dataFlowEntry, iter->val.arithmetic2);
                dataFlowEntry->successors = makeLineList(line + 1);
            }
                break;
            case INSTRUCTION_PUSH: {
                dataFlowEntry = initDataFlowEntry();
                dataFlowEntry->defines = initHeadedSortedSet();

                dataFlowEntry->uses = initHeadedSortedSet();

                insertSortedSet(dataFlowEntry->uses, (int) iter->val.tempToPush);

                dataFlowEntry->successors = makeLineList(line + 1);
            }
                break;
            case INSTRUCTION_POP: {
                dataFlowEntry = initDataFlowEntry();
                dataFlowEntry->defines = initHeadedSortedSet();

                dataFlowEntry->uses = initHeadedSortedSet();

                insertSortedSet(dataFlowEntry->uses, (int) iter->val.tempToPopInto);

                dataFlowEntry->successors = makeLineList(line + 1);
            }
                break;
            case INSTRUCTION_NEGATE: {
                dataFlowEntry = initDataFlowEntry();
                dataFlowEntry->defines = initHeadedSortedSet();
                insertSortedSet(dataFlowEntry->defines, (int) iter->val.tempToNegate);

                dataFlowEntry->uses = initHeadedSortedSet();

                insertSortedSet(dataFlowEntry->uses, (int) iter->val.tempToNegate);

                dataFlowEntry->successors = makeLineList(line + 1);
            }
                break;
            case INSTRUCTION_ABS: {
                dataFlowEntry = initDataFlowEntry();
                dataFlowEntry->defines = initHeadedSortedSet();
                insertSortedSet(dataFlowEntry->defines, (int) iter->val.tempToAbs);

                dataFlowEntry->uses = initHeadedSortedSet();

                insertSortedSet(dataFlowEntry->uses, (int) iter->val.tempToAbs);

                dataFlowEntry->successors = makeLineList(line + 1);
            }
                break;
            case INSTRUCTION_RIGHT_SHIFT: {
                dataFlowEntry = initDataFlowEntry();
                dataFlowEntry->defines = initHeadedSortedSet();
                insertSortedSet(dataFlowEntry->defines, (int) iter->val.rightShift.temp);

                dataFlowEntry->uses = initHeadedSortedSet();

                insertSortedSet(dataFlowEntry->uses, (int) iter->val.rightShift.temp);

                dataFlowEntry->successors = makeLineList(line + 1);
            }
                break;
            case INSTRUCTION_XOR: {
                dataFlowEntry = initDataFlowEntry();
                dataFlowEntryFromArithmetic2(dataFlowEntry, iter->val.arithmetic2);
                dataFlowEntry->successors = makeLineList(line + 1);
            }
                break;
            case INSTRUCTION_COPY: {
                dataFlowEntry = initDataFlowEntry();
                dataFlowEntryFromArithmetic2(dataFlowEntry, iter->val.arithmetic2);
                dataFlowEntry->successors = makeLineList(line + 1);
            }
                break;
            case INSTRUCTION_CMP: {
                dataFlowEntry = initDataFlowEntry();
                dataFlowEntryFromArithmetic2(dataFlowEntry, iter->val.arithmetic2);
                dataFlowEntry->successors = makeLineList(line + 1);
            }
                break;
            case INSTRUCTION_FUNCTION_CALL: {
                dataFlowEntry = initDataFlowEntry();
                dataFlowEntry->defines = initHeadedSortedSet();
                dataFlowEntry->uses = initHeadedSortedSet();
                dataFlowEntry->successors = makeLineList(line + 1);

                int targetLine = ((IntBox *) get(labels, makeCharKey(iter->val.function))->v)->value;
                dataFlowEntry->function = makeIntBox(targetLine);
            }
                break;
            case INSTRUCTION_JE: {
                dataFlowEntry = initDataFlowEntry();
                dataFlowEntry->defines = initHeadedSortedSet();
                dataFlowEntry->uses = initHeadedSortedSet();

                dataFlowEntry->successors = makeLineList(line + 1);
                int targetLine = ((IntBox *) get(labels, makeCharKey(iter->val.label))->v)->value;
                dataFlowEntry->successors->next = makeLineList(targetLine);
            }
                break;
            case INSTRUCTION_JMP: {
                dataFlowEntry = initDataFlowEntry();
                dataFlowEntry->defines = initHeadedSortedSet();
                dataFlowEntry->uses = initHeadedSortedSet();

                int targetLine = ((IntBox *) get(labels, makeCharKey(iter->val.function))->v)->value;
                dataFlowEntry->successors = makeLineList(targetLine);
            }
                break;
            case COMPLEX_ALLOCATE: {
                dataFlowEntry = initDataFlowEntry();
                dataFlowEntry->defines = initHeadedSortedSet();
                insertSortedSet(dataFlowEntry->defines, (int) iter->val.allocate.ptrTemp);

                dataFlowEntry->uses = initHeadedSortedSet();

                insertSortedSet(dataFlowEntry->uses, (int) iter->val.allocate.timesTemp);

                dataFlowEntry->successors = makeLineList(line + 1);
            }
                break;
            case COMPLEX_CONSTRAIN_BOOLEAN: {
                dataFlowEntry = initDataFlowEntry();
                dataFlowEntry->defines = initHeadedSortedSet();
                insertSortedSet(dataFlowEntry->defines, (int) iter->val.tempToConstrain);

                dataFlowEntry->uses = initHeadedSortedSet();

                insertSortedSet(dataFlowEntry->uses, (int) iter->val.tempToConstrain);

                dataFlowEntry->successors = makeLineList(line + 1);
            }
                break;
            case COMPLEX_SAVE_STATIC_LINK: {
                dataFlowEntry = initDataFlowEntry();
                dataFlowEntry->defines = initHeadedSortedSet();
                insertSortedSet(dataFlowEntry->defines, (int) iter->val.pushPopStaticLink.temporary);

                dataFlowEntry->uses = initHeadedSortedSet();
                insertSortedSet(dataFlowEntry->uses, (int) iter->val.pushPopStaticLink.temporary);

                dataFlowEntry->successors = makeLineList(line + 1);
            }
                break;
            case COMPLEX_RESTORE_STATIC_LINK: {
                dataFlowEntry = initDataFlowEntry();
                dataFlowEntry->defines = initHeadedSortedSet();
                insertSortedSet(dataFlowEntry->defines, (int) iter->val.pushPopStaticLink.temporary);

                dataFlowEntry->uses = initHeadedSortedSet();
                insertSortedSet(dataFlowEntry->uses, (int) iter->val.pushPopStaticLink.temporary);

                dataFlowEntry->successors = makeLineList(line + 1);
            }
                break;
            case COMPLEX_LOAD_POINTER_TO_STATIC_LINK_FRAME: {
                dataFlowEntry = initDataFlowEntry();
                dataFlowEntry->defines = initHeadedSortedSet();
                insertSortedSet(dataFlowEntry->defines, (int) iter->val.loadPtrToStaticLink.intermediateTemp);

                dataFlowEntry->uses = initHeadedSortedSet();
                insertSortedSet(dataFlowEntry->uses, (int) iter->val.loadPtrToStaticLink.ptrTemp);

                dataFlowEntry->successors = makeLineList(line + 1);

                dataFlowEntry = initDataFlowEntry();
                dataFlowEntry->defines = initHeadedSortedSet();
                insertSortedSet(dataFlowEntry->defines, (int) iter->val.loadPtrToStaticLink.intermediateTemp);

                dataFlowEntry->uses = initHeadedSortedSet();
                insertSortedSet(dataFlowEntry->uses, (int) iter->val.loadPtrToStaticLink.ptrTemp);
                insertSortedSet(dataFlowEntry->uses, (int) iter->val.loadPtrToStaticLink.intermediateTemp);

                dataFlowEntry->successors = makeLineList(line + 1);
            }
                break;
            case INSTRUCTION_ADD_CONST: {
                dataFlowEntry = initDataFlowEntry();
                dataFlowEntry->defines = initHeadedSortedSet();
                insertSortedSet(dataFlowEntry->defines, (int) iter->val.art2const.temp);

                dataFlowEntry->uses = initHeadedSortedSet();

                insertSortedSet(dataFlowEntry->uses, (int) iter->val.art2const.temp);

                dataFlowEntry->successors = makeLineList(line + 1);
            }
                break;
            case INSTRUCTION_MUL_CONST: {
                dataFlowEntry = initDataFlowEntry();
                dataFlowEntry->defines = initHeadedSortedSet();
                insertSortedSet(dataFlowEntry->defines, (int) iter->val.art2const.temp);

                dataFlowEntry->uses = initHeadedSortedSet();

                insertSortedSet(dataFlowEntry->uses, (int) iter->val.art2const.temp);

                dataFlowEntry->successors = makeLineList(line + 1);
            }
                break;
            case INSTRUCTION_MOVE_TO_OFFSET: {
                dataFlowEntry = initDataFlowEntry();
                dataFlowEntry->defines = initHeadedSortedSet();

                dataFlowEntry->uses = initHeadedSortedSet();
                insertSortedSet(dataFlowEntry->uses, (int) iter->val.moveToOffset.ptrTemp);
                insertSortedSet(dataFlowEntry->uses, (int) iter->val.moveToOffset.offsetTemp);
                insertSortedSet(dataFlowEntry->uses, (int) iter->val.moveToOffset.tempToMove);

                dataFlowEntry->successors = makeLineList(line + 1);
            }
                break;
            case INSTRUCTION_LEA_TO_OFFSET: {
                dataFlowEntry = initDataFlowEntry();
                dataFlowEntry->defines = initHeadedSortedSet();

                dataFlowEntry->uses = initHeadedSortedSet();
                insertSortedSet(dataFlowEntry->uses, (int) iter->val.moveToOffset.ptrTemp);
                insertSortedSet(dataFlowEntry->uses, (int) iter->val.moveToOffset.offsetTemp);
                insertSortedSet(dataFlowEntry->uses, (int) iter->val.moveToOffset.tempToMove);

                dataFlowEntry->successors = makeLineList(line + 1);
            }
                break;
            case COMPLEX_MOVE_TEMPORARY_INTO_STACK: {
                dataFlowEntry = initDataFlowEntry();
                dataFlowEntry->defines = initHeadedSortedSet();

                dataFlowEntry->uses = initHeadedSortedSet();
                insertSortedSet(dataFlowEntry->uses, (int) iter->val.tempIntoStack.tempToMove);

                dataFlowEntry->successors = makeLineList(line + 1);
            }
                break;
            case COMPLEX_MOVE_TEMPORARY_INTO_STACK_IN_SCOPE: {
                dataFlowEntry = initDataFlowEntry();
                dataFlowEntry->defines = initHeadedSortedSet();

                dataFlowEntry->uses = initHeadedSortedSet();
                insertSortedSet(dataFlowEntry->uses, (int) iter->val.tempIntoStackScope.tempToMove);

                dataFlowEntry->successors = makeLineList(line + 1);
            }
                break;
            case COMPLEX_MOVE_TEMPORARY_FROM_STACK: {
                dataFlowEntry = initDataFlowEntry();
                dataFlowEntry->defines = initHeadedSortedSet();
                insertSortedSet(dataFlowEntry->defines, (int) iter->val.tempFromStack.inputTemp);

                dataFlowEntry->uses = initHeadedSortedSet();

                dataFlowEntry->successors = makeLineList(line + 1);
            }
                break;
            case COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE: {
                dataFlowEntry = initDataFlowEntry();
                dataFlowEntry->defines = initHeadedSortedSet();
                insertSortedSet(dataFlowEntry->defines, (int) iter->val.tempFromStackScope.inputTemp);

                dataFlowEntry->uses = initHeadedSortedSet();

                dataFlowEntry->successors = makeLineList(line + 1);
            }
                break;
            case COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET: {
                dataFlowEntry = initDataFlowEntry();
                dataFlowEntry->defines = initHeadedSortedSet();
                insertSortedSet(dataFlowEntry->defines, (int) iter->val.dereferenceOffset.ptrTemp);

                dataFlowEntry->uses = initHeadedSortedSet();
                insertSortedSet(dataFlowEntry->uses, (int) iter->val.dereferenceOffset.ptrTemp);
                insertSortedSet(dataFlowEntry->uses, (int) iter->val.dereferenceOffset.offsetTemp);

                dataFlowEntry->successors = makeLineList(line + 1);
            }
                break;
        }

        iter = iter->next;
    }

    LineList *successor;

    for (int i = dataFlowSize - 1; i >= 0; --i) {
        dataFlowEntry = dataFlow[i];

        dataFlowEntry->out = initHeadedSortedSet();

        successor = dataFlowEntry->successors;
        while (successor != NULL) {
            if (successor->line < dataFlowSize) {
                dataFlowEntry->out = sortedSetUnion(dataFlowEntry->out,
                                                    dataFlow[successor->line]->in);
                successor = successor->next;
            }
        }

        if (dataFlowEntry->function != NULL) {
            dataFlowEntry->out = sortedSetUnion(dataFlowEntry->out,
                                                dataFlow[dataFlowEntry->function->value]->in);
        }

        dataFlowEntry->in = sortedSetUnion(dataFlowEntry->uses,
                                           sortedSetDiff(dataFlowEntry->out, dataFlowEntry->defines));
    }

    LivenessAnalysisResult *result = NEW(LivenessAnalysisResult);
    result->numberSets = dataFlowSize;
    result->sets = malloc(sizeof(SortedSet) * dataFlowSize);
    for (int i = 0; i < dataFlowSize; ++i) {
        result->sets[i] = sortedSetUnion(dataFlow[i]->in, dataFlow[i]->out);
        freeDataFlowEntry(dataFlow[i]);
    }

    return result;
}
