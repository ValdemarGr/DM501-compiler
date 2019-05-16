//
// Created by Mads Grau Kristensen on 2019-05-16.
//

#include "simplify_loads.h"
#include "../ast_to_ir/intermediate_representation.h"
#include "register_allocator.h"
#include "liveness_analysis.h"

bool followTemp(const struct DataFlowEntry *dataFlowEntry,
        const struct DataFlowEntry *start,
        const struct DataFlowEntry *targetDataFlowEntry,
        size_t temp, LivenessAnalysisResult *livenessAnalysisResult) {
    DataFlowEntry **dataFlow = livenessAnalysisResult->dataFlow;
    int dataFlowSize = livenessAnalysisResult->dataFlowSize;
    LineList *successor;
    const DataFlowEntry *iter = dataFlowEntry;

    while (iter != NULL) {
        successor = dataFlowEntry->successors;
        if (successor != NULL) {
            if (successor->line < dataFlowSize) {
                iter = dataFlow[successor->line];
                if (iter == dataFlowEntry || iter == targetDataFlowEntry ||
                        iter == start) {
                    return true;
                }

                if (exists(iter->defines, temp)) {
                    return false;
                }

                if (exists(iter->out, temp)) {
                    return false;
                }
            } else {
                return true;
            }
            successor = successor->next;
        }

        if (successor != NULL) {
            return false;
        }
    }

    return true;
}

void simplifyLoads(Instructions *head, LivenessAnalysisResult *livenessAnalysisResult) {
    Instructions *iter = head;
    RaVariable *currentVariable = NULL;
    ConstMap *loadedVariables = initMap(128);
    SymbolTable *currentSymbolTable = NULL;

    while (iter != NULL) {
        if (iter->kind == INSTRUCTION_FUNCTION_LABEL) {
            currentSymbolTable = iter->val.functionHead.tableForFunction;
        }
        else if (iter->kind == METADATA_CREATE_MAIN) {
            currentSymbolTable = iter->val.mainHeader.tableForFunction;
        }
        else if (iter->kind == METADATA_ACCESS_VARIABLE_START) {
            if (iter->val.varAccess.temp == -1) {
                iter = iter->next;
                continue;
            }

            Pair *pair = get(loadedVariables, makeIntKey((long) currentSymbolTable));
            ConstMap *map;
            RaVariable *variable;
            if (pair != NULL) {
                map = pair->v;
                Pair *var = get(map, makeCharKey(iter->val.varAccess.accessId));
                if (var != NULL) {
                    variable = var->v;

                    bool isSafe = followTemp(variable->instructionEnd->dataFlowEntry,
                               variable->instructionStart->dataFlowEntry,
                               iter->dataFlowEntry,variable->reg,
                               livenessAnalysisResult);

                    if (!isSafe) {
                        iter = iter->next;
                        continue;
                    }

                    Instructions *removeTo = iter;
                    while (removeTo != NULL) {
                        if (removeTo->kind == METADATA_ACCESS_VARIABLE_END) {
                            break;
                        }

                        removeTo = removeTo->next;
                    }

                    if (removeTo == NULL) {
                        iter = iter->next;
                        continue;
                    }

                    Instructions *newInstructions = newInstruction();
                    newInstructions->kind = INSTRUCTION_COPY;
                    newInstructions->val.arithmetic2.dest = iter->val.varAccess.temp;
                    newInstructions->val.arithmetic2.source = variable->reg;
                    newInstructions->next = removeTo;
                    newInstructions->previous = iter;
                    removeTo->previous = newInstructions;
                    iter->next = newInstructions;
                    newInstructions->dataFlowEntry = mergeDataFlowEntries(iter->dataFlowEntry, removeTo->dataFlowEntry);
                }
            } else {
                map = initMap(128);
                insert(loadedVariables, makeIntKey((long) currentSymbolTable), map);
            }

            variable = NEW(RaVariable);
            variable->accessId = iter->val.varAccess.accessId;
            variable->instructionStart = iter;
            variable->reg = iter->val.varAccess.temp;

            currentVariable = variable;
            insert(map, makeCharKey(iter->val.varAccess.accessId), variable);
        }
        else if (iter->kind == METADATA_ACCESS_VARIABLE_END) {
            currentVariable->instructionEnd = iter;
        }
        iter = iter->next;
    }
}
