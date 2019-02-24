//
// Created by valde on 2/23/19.
//
#include "weeder.h"

Error *weeder(Body *body) {
    Error *e = NULL;

    if (e == NULL) e = weedFunctionNames(body);
    if (e == NULL) e = weedFunctionReturns(body);

    return e;
}