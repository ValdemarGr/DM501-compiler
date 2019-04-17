//
// Created by valde on 2/23/19.
//
#include "weeder.h"

Error *weeder(Body *body) {
    Error *e = NULL;

    e = weedFunctionNames(body);
    if (e != NULL) return e;
    e = weedFunctionReturns(body);
    return e;
}