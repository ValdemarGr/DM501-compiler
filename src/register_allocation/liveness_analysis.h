//
// Created by Mads Grau Kristensen on 2019-04-30.
//

#ifndef HELLO_LIVENESS_ANALYSIS_H
#define HELLO_LIVENESS_ANALYSIS_H

#include "../ast_to_ir/intermediate_representation.h"
#include "../utils/sortedset.h"
#include "register_allocator.h"

typedef int Line;

typedef struct IntBox {
    int value;
} IntBox;

typedef struct LineList {
    Line line;
    struct LineList *next;
} LineList;

typedef struct DataFlowEntry {
    SortedSet *defines;
    SortedSet *uses;
    SortedSet *in;
    SortedSet *out;
    LineList *successors;
    IntBox *function;
    Instructions *instruction;
    SortedSet *intermediates;
} DataFlowEntry;

LivenessAnalysisResult *livenessAnalysis(Instructions *instructions);

#endif //HELLO_LIVENESS_ANALYSIS_H
