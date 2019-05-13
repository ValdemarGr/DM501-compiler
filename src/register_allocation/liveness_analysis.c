//
// Created by Mads Grau Kristensen on 2019-04-30.
//

#include "liveness_analysis.h"
#include "../utils/int_linked_list.h"
#include "../ast_to_ir/intermediate_representation.h"

int line = 0;

DataFlowEntry **dataFlow;

void dataFlowEntryFromArithmetic2(DataFlowEntry *dataFlowEntry, Arithmetic2 arithmetic2) {
    insertSortedSet(dataFlowEntry->defines, (int) arithmetic2.dest);

    insertSortedSet(dataFlowEntry->uses, (int) arithmetic2.source);
    insertSortedSet(dataFlowEntry->uses, (int) arithmetic2.dest);
}

IntBox *makeIntBox(int value) {
    IntBox *box = NEW(IntBox);
    box->value = value;
    return box;
}

LineList *makeLineList(int line) {
    LineList *list = NEW(LineList);
    list->line = line;
    list->next = NULL;
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
    freeSortedSet(entry->intermediates);

    freeLineList(entry->successors);

    free(entry);
}

Instructions *iter;

DataFlowEntry *initDataFlowEntry() {
    DataFlowEntry *dataFlowEntry = NEW(DataFlowEntry);
    dataFlowEntry->instruction = iter;
    dataFlowEntry->successors = NULL;
    dataFlowEntry->function = NULL;
    dataFlowEntry->out = NULL;
    dataFlowEntry->in = NULL;

    dataFlowEntry->uses = initHeadedSortedSet();
    dataFlowEntry->defines = initHeadedSortedSet();
    dataFlowEntry->intermediates = initHeadedSortedSet();
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
            case INSTRUCTION_PUSH_STACK:break;
            case INSTRUCTION_POP_STACK:break;
            case COMPLEX_LOAD_POINTER_TO_STATIC_LINK_FRAME:break;
            case COMPLEX_GARBAGE_COLLECT:break;
            case METADATA_BEGIN_GLOBAL_BLOCK:break;
            case METADATA_END_GLOBAL_BLOCK:break;
            case METADATA_DEBUG_INFO:break;
            case METADATA_CREATE_MAIN: break;
            case METADATA_BEGIN_BODY_BLOCK: break;
            case METADATA_END_BODY_BLOCK: break;
            case METADATA_BEGIN_ARITHMETIC_EVALUATION: break;
            case METADATA_END_ARITHMETIC_EVALUATION: break;
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
            default: {
                count++;
            }
                break;
        }

        iter = iter->next;
    }

    iter = instructions;

    int dataFlowSize = count;
    dataFlow = malloc(sizeof(DataFlowEntry) * dataFlowSize);

    line = -1;
    DataFlowEntry *dataFlowEntry = NULL;
    while (iter != NULL) {
        switch (iter->kind) {
            case METADATA_CREATE_MAIN: break;
            case METADATA_BEGIN_BODY_BLOCK: break;
            case METADATA_END_BODY_BLOCK: break;
            case METADATA_BEGIN_ARITHMETIC_EVALUATION: break;
            case METADATA_END_ARITHMETIC_EVALUATION: break;
            case COMPLEX_ALLOCATE_END: break;
            case INSTRUCTION_FUNCTION_END: break;
            case INSTRUCTION_VAR: break;
            case INSTRUCTION_MOVE: break;
            case INSTRUCTION_PROGRAM_BEGIN: break;
            case INSTRUCTION_LABEL: break;
            case METADATA_FUNCTION_ARGUMENT:
                dataFlowEntry = initDataFlowEntry();
                insertSortedSet(dataFlowEntry->defines, (int) iter->val.args.moveReg);

                dataFlowEntry->successors = makeLineList(line + 1);
                break;
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
                insertSortedSet(dataFlowEntry->defines, (int) iter->val.constant.temp);

                dataFlowEntry->successors = makeLineList(line + 1);
            }
                break;
            case INSTRUCTION_SET_ZERO:
                dataFlowEntry = initDataFlowEntry();
                insertSortedSet(dataFlowEntry->defines, (int) iter->val.tempToSetZero);

                dataFlowEntry->successors = makeLineList(line + 1);
                break;
            case INSTRUCTION_FUNCTION_LABEL: {
                dataFlowEntry = initDataFlowEntry();
                insertSortedSet(dataFlowEntry->defines, (int) iter->val.functionHead.temporary);

                dataFlowEntry->successors = makeLineList(line + 1);
            }
                break;
            case INSTRUCTION_RETURN: {
                dataFlowEntry = initDataFlowEntry();

                insertSortedSet(dataFlowEntry->uses, (int) iter->val.tempToReturn);
            }
                break;
            case INSTRUCTION_WRITE: {
                dataFlowEntry = initDataFlowEntry();

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

                insertSortedSet(dataFlowEntry->uses, (int) iter->val.tempToPush);

                dataFlowEntry->successors = makeLineList(line + 1);
            }
                break;
            case INSTRUCTION_POP: {
                dataFlowEntry = initDataFlowEntry();
                
                insertSortedSet(dataFlowEntry->defines, (int) iter->val.tempToPopInto);

                dataFlowEntry->successors = makeLineList(line + 1);
            }
                break;
            case INSTRUCTION_NEGATE: {
                dataFlowEntry = initDataFlowEntry();
                
                insertSortedSet(dataFlowEntry->defines, (int) iter->val.tempToNegate);
                
                insertSortedSet(dataFlowEntry->uses, (int) iter->val.tempToNegate);

                dataFlowEntry->successors = makeLineList(line + 1);
            }
                break;
            case INSTRUCTION_ABS: {
                dataFlowEntry = initDataFlowEntry();
                
                insertSortedSet(dataFlowEntry->defines, (int) iter->val.tempToAbs);

                insertSortedSet(dataFlowEntry->uses, (int) iter->val.tempToAbs);

                dataFlowEntry->successors = makeLineList(line + 1);
            }
                break;
            case INSTRUCTION_RIGHT_SHIFT: {
                dataFlowEntry = initDataFlowEntry();
                
                insertSortedSet(dataFlowEntry->defines, (int) iter->val.rightShift.temp);

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

                dataFlowEntry->successors = makeLineList(line + 1);
                int targetLine = ((IntBox *) get(labels, makeCharKey(iter->val.function))->v)->value;
                dataFlowEntry->function = makeIntBox(targetLine);
            }
                break;
            case INSTRUCTION_JE: {
                dataFlowEntry = initDataFlowEntry();

                dataFlowEntry->successors = makeLineList(line + 1);

                Pair *target = get(labels, makeCharKey(iter->val.label));

                if (target != NULL) {
                    int targetLine = ((IntBox *) target->v)->value;
                    dataFlowEntry->successors->next = makeLineList(targetLine);
                } else {

                }
            }
                break;
            case INSTRUCTION_JMP: {
                dataFlowEntry = initDataFlowEntry();

                Pair *target = get(labels, makeCharKey(iter->val.function));

                if (target != NULL) {
                    int targetLine = ((IntBox *) target->v)->value;
                    dataFlowEntry->successors = makeLineList(targetLine);
                }else {

                }
            }
                break;
            case COMPLEX_ALLOCATE: {
                dataFlowEntry = initDataFlowEntry();

                insertSortedSet(dataFlowEntry->uses, (int) iter->val.allocate.timesTemp);

                insertSortedSet(dataFlowEntry->intermediates, (int) iter->val.allocate.intermediate);

                dataFlowEntry->successors = makeLineList(line + 1);
            }
                break;
            case COMPLEX_CONSTRAIN_BOOLEAN: {
                dataFlowEntry = initDataFlowEntry();
                
                insertSortedSet(dataFlowEntry->defines, (int) iter->val.tempToConstrain);

                insertSortedSet(dataFlowEntry->uses, (int) iter->val.tempToConstrain);

                dataFlowEntry->successors = makeLineList(line + 1);
            }
                break;
            case COMPLEX_SAVE_STATIC_LINK: {
                dataFlowEntry = initDataFlowEntry();
                
                insertSortedSet(dataFlowEntry->defines, (int) iter->val.pushPopStaticLink.temporary);

                dataFlowEntry->successors = makeLineList(line + 1);
            }
                break;
            case COMPLEX_RESTORE_STATIC_LINK: {
                dataFlowEntry = initDataFlowEntry();
                
                insertSortedSet(dataFlowEntry->defines, (int) iter->val.pushPopStaticLink.temporary);

                dataFlowEntry->successors = makeLineList(line + 1);
            }
                break;
            case INSTRUCTION_ADD_CONST: {
                dataFlowEntry = initDataFlowEntry();
                
                insertSortedSet(dataFlowEntry->defines, (int) iter->val.art2const.temp);

                insertSortedSet(dataFlowEntry->uses, (int) iter->val.art2const.temp);

                dataFlowEntry->successors = makeLineList(line + 1);
            }
                break;
            case INSTRUCTION_MUL_CONST: {
                dataFlowEntry = initDataFlowEntry();
                
                insertSortedSet(dataFlowEntry->defines, (int) iter->val.art2const.temp);

                insertSortedSet(dataFlowEntry->uses, (int) iter->val.art2const.temp);

                dataFlowEntry->successors = makeLineList(line + 1);
            }
                break;
            case INSTRUCTION_MOVE_TO_OFFSET: {
                dataFlowEntry = initDataFlowEntry();

                insertSortedSet(dataFlowEntry->uses, (int) iter->val.moveToOffset.ptrTemp);
                insertSortedSet(dataFlowEntry->uses, (int) iter->val.moveToOffset.offsetTemp);
                insertSortedSet(dataFlowEntry->uses, (int) iter->val.moveToOffset.tempToMove);

                dataFlowEntry->successors = makeLineList(line + 1);
            }
                break;
            case INSTRUCTION_LEA_TO_OFFSET: {
                dataFlowEntry = initDataFlowEntry();

                insertSortedSet(dataFlowEntry->uses, (int) iter->val.moveToOffset.ptrTemp);
                insertSortedSet(dataFlowEntry->uses, (int) iter->val.moveToOffset.offsetTemp);
                insertSortedSet(dataFlowEntry->uses, (int) iter->val.moveToOffset.tempToMove);

                dataFlowEntry->successors = makeLineList(line + 1);
            }
                break;
            case COMPLEX_MOVE_TEMPORARY_INTO_STACK: {
                dataFlowEntry = initDataFlowEntry();

                insertSortedSet(dataFlowEntry->uses, (int) iter->val.tempIntoStack.tempToMove);

                dataFlowEntry->successors = makeLineList(line + 1);
            }
                break;
            case COMPLEX_MOVE_TEMPORARY_INTO_STACK_IN_SCOPE: {
                dataFlowEntry = initDataFlowEntry();

                insertSortedSet(dataFlowEntry->uses, (int) iter->val.tempIntoStackScope.tempToMove);

                insertSortedSet(dataFlowEntry->intermediates, (int) iter->val.tempIntoStackScope.intermediate);
                insertSortedSet(dataFlowEntry->intermediates, (int) iter->val.tempIntoStackScope.intermediate2);

                dataFlowEntry->successors = makeLineList(line + 1);
            }
                break;
            case COMPLEX_MOVE_TEMPORARY_FROM_STACK: {
                dataFlowEntry = initDataFlowEntry();
                
                insertSortedSet(dataFlowEntry->defines, (int) iter->val.tempFromStack.inputTemp);

                dataFlowEntry->successors = makeLineList(line + 1);
            }
                break;
            case COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE: {
                dataFlowEntry = initDataFlowEntry();
                
                insertSortedSet(dataFlowEntry->defines, (int) iter->val.tempFromStackScope.inputTemp);

                insertSortedSet(dataFlowEntry->intermediates, (int) iter->val.tempFromStackScope.intermediate);
                insertSortedSet(dataFlowEntry->intermediates, (int) iter->val.tempFromStackScope.intermediate2);

                dataFlowEntry->successors = makeLineList(line + 1);
            }
                break;
            case COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET: {
                dataFlowEntry = initDataFlowEntry();
                
                insertSortedSet(dataFlowEntry->defines, (int) iter->val.dereferenceOffset.returnTemp);

                insertSortedSet(dataFlowEntry->uses, (int) iter->val.dereferenceOffset.ptrTemp);
                insertSortedSet(dataFlowEntry->uses, (int) iter->val.dereferenceOffset.offsetTemp);

                dataFlowEntry->successors = makeLineList(line + 1);
            }
                break;
            case COMPLEX_RIP_LAMBDA_LOAD:
                dataFlowEntry = initDataFlowEntry();
                
                insertSortedSet(dataFlowEntry->defines, (int) iter->val.lambdaLoad.temporary);

                dataFlowEntry->successors = makeLineList(line + 1);
                break;
            case INSTRUCTION_REGISTER_CALL:
                dataFlowEntry = initDataFlowEntry();

                insertSortedSet(dataFlowEntry->uses, (int) iter->val.callRegister);

                dataFlowEntry->successors = makeLineList(line + 1);
                break;
            case INSTRUCTION_PUSH_STACK:break;
            case INSTRUCTION_POP_STACK:break;
            case COMPLEX_LOAD_POINTER_TO_STATIC_LINK_FRAME:break;
            case COMPLEX_GARBAGE_COLLECT:break;
            case COMPLEX_ABS_VALUE:
                dataFlowEntry = initDataFlowEntry();

                insertSortedSet(dataFlowEntry->defines, iter->val.arithmetic2.source);
                insertSortedSet(dataFlowEntry->defines, iter->val.arithmetic2.dest);

                insertSortedSet(dataFlowEntry->uses, iter->val.arithmetic2.source);

                dataFlowEntry->successors = makeLineList(line + 1);
                break;
            case METADATA_BEGIN_GLOBAL_BLOCK:break;
            case METADATA_END_GLOBAL_BLOCK:break;
            case METADATA_DEBUG_INFO:break;
            case INSTRUCTION_ADD_STACK_PTR:
                dataFlowEntry = initDataFlowEntry();
                dataFlowEntry->successors = makeLineList(line + 1);
                break;
            case COMPLEX_SAVE_ALL:
                dataFlowEntry = initDataFlowEntry();
                dataFlowEntry->successors = makeLineList(line + 1);
                break;
            case COMPLEX_RESTORE_ALL:
                dataFlowEntry = initDataFlowEntry();
                dataFlowEntry->successors = makeLineList(line + 1);
                break;
        }

        iter = iter->next;
    }

    LineList *successor;
    bool dataFlowChanged = false;
    int lastSize = 0;

    do {
        dataFlowChanged = false;
        for (int i = dataFlowSize - 1; i >= 0; i--) {
            dataFlowEntry = dataFlow[i];

            if (dataFlowEntry->out == NULL) {
                dataFlowEntry->out = initHeadedSortedSet();
            }

            successor = dataFlowEntry->successors;
            while (successor != NULL) {
                if (successor->line < dataFlowSize) {
                    lastSize = length(dataFlowEntry->out);
                    dataFlowEntry->out = sortedSetUnion(dataFlowEntry->out,
                                                        dataFlow[successor->line]->in);
                    if (length(dataFlowEntry->out) != lastSize) {
                        dataFlowChanged = true;
                    }
                }
                successor = successor->next;
            }

            if (dataFlowEntry->function != NULL) {
                lastSize = length(dataFlowEntry->out);
                dataFlowEntry->out = sortedSetUnion(dataFlowEntry->out,
                                                    dataFlow[dataFlowEntry->function->value]->in);
                if (length(dataFlowEntry->out) != lastSize) {
                    dataFlowChanged = true;
                }
            }

            lastSize = length(dataFlowEntry->in);
            dataFlowEntry->in = sortedSetUnion(dataFlowEntry->uses,
                                               sortedSetDiff(dataFlowEntry->out, dataFlowEntry->defines));
            if (length(dataFlowEntry->in) != lastSize) {
                dataFlowChanged = true;
            }
        }
    } while (dataFlowChanged);


    LivenessAnalysisResult *result = NEW(LivenessAnalysisResult);
    result->numberSets = dataFlowSize;
    result->sets = malloc(sizeof(SortedSet) * dataFlowSize);
    for (int i = 0; i < dataFlowSize; ++i) {
        result->sets[i] = sortedSetUnion(sortedSetUnion(dataFlow[i]->in, dataFlow[i]->out), dataFlow[i]->intermediates);
        freeDataFlowEntry(dataFlow[i]);
    }

    return result;
}
