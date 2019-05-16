//
// Created by Mads Grau Kristensen on 2019-05-16.
//

#include "simplify_loads.h"
#include "../ast_to_ir/intermediate_representation.h"
#include "register_allocator.h"
#include "liveness_analysis.h"
#include "../peephole/peephole.h"

bool followTemp(const struct DataFlowEntry *dataFlowEntry,
                const struct DataFlowEntry *start,
                const struct DataFlowEntry *targetDataFlowEntry,
                size_t temp, LivenessAnalysisResult *livenessAnalysisResult) {
    DataFlowEntry **dataFlow = livenessAnalysisResult->dataFlow;
    int dataFlowSize = livenessAnalysisResult->dataFlowSize;
    LineList *successor;
    const DataFlowEntry *iter = dataFlowEntry;

    while (iter != NULL) {
        successor = iter->successors;
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

                if (!exists(iter->out, temp)) {
                    Instructions *it = fetchPreviousInstructionThatModifiesRegister(iter->instruction, temp);
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

    while (iter != NULL) {
        if (iter->kind == METADATA_ACCESS_VARIABLE_START) {
            if (iter->val.varAccess.temp == -1) {
                iter = iter->next;
                continue;
            }

            Instructions *it = fetchPreviousInstructionThatModifiesRegister(iter, iter->val.varAccess.temp);

            if (it == NULL) {
                iter = iter->next;
                continue;
            }

            if (it->kind == METADATA_ACCESS_VARIABLE_END) {
                if (strcmp(it->val.varAccess.accessId, iter->val.varAccess.accessId) == 0) {
                    if (it->val.varAccess.temp == -1) {
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
                    newInstructions->val.arithmetic2.source = it->val.varAccess.temp;
                    newInstructions->next = removeTo;
                    newInstructions->previous = iter;
                    removeTo->previous = newInstructions;
                    iter->next = newInstructions;
                    newInstructions->dataFlowEntry = mergeDataFlowEntries(iter->dataFlowEntry, removeTo->dataFlowEntry);
                }
            }
        }
        iter = iter->next;
    }
}
