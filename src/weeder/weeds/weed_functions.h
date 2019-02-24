//
// Created by valde on 2/23/19.
//

#ifndef HELLO_WEED_FUNCTION_H
#define HELLO_WEED_FUNCTION_H

#include "../../ast/tree.h"
#include "../../error/error.h"
#include <stddef.h>
#include <stdio.h>
#include "../../utils/map.h"

Error *weedFunctionNames(Body *body);

Error *weedFunctionReturns(Body *body);

#endif //HELLO_FUNCTION_H
