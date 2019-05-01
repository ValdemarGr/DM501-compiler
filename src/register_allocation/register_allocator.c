//
// Created by valde on 4/21/19.
//

#include "register_allocator.h"
#include "liveness_analysis.h"
#include "graph_coloring.h"

ConstMap *currentRegisterContext = NULL;
size_t timestamp = 0;


/*
 * When we spill we put intermediate products on the stack
 * The position will be the context pointer: rbp + sizeof(var) * varsInContext
 */
int *simpleRegisterAllocation(Instructions *head, int numberRegisters) {
    LivenessAnalysisResult *livenessAnalysisResult = livenessAnalysis(head);
    int *colors = colorGraph(livenessAnalysisResult->sets, livenessAnalysisResult->numberSets, numberRegisters);
    return colors;
}
