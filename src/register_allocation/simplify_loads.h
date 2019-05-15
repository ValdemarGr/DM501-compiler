//
// Created by Mads Grau Kristensen on 2019-05-16.
//

#ifndef HELLO_SIMPLIFY_LOADS_H
#define HELLO_SIMPLIFY_LOADS_H

#include "register_allocator.h"

void simplifyLoads(Instructions *head, LivenessAnalysisResult *livenessAnalysisResult);

#endif //HELLO_SIMPLIFY_LOADS_H
