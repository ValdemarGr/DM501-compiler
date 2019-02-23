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

Error *weed_function_name(Body *body);

Error *weed_function_return(Body *body);

#endif //HELLO_FUNCTION_H
