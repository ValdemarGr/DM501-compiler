//
// Created by valde on 2/23/19.
//
#include "weeder.h"

Error *weeder(Body *body) {
    Error *e = NULL;

    if (e == NULL) e = weed_function_name(body);
    if (e == NULL) e = weed_function_return(body);

    return e;
}