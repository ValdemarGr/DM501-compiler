//
// Created by valde on 2/23/19.
//

#ifndef HELLO_ERROR_H
#define HELLO_ERROR_H

#include <stdlib.h>
#include <stdio.h>

typedef struct Error {
    enum {
        WEED_FUNC_HAS_NO_END,
        WEED_FUNC_HAS_NO_RETURN
    } error;
    union {
        struct { char* headId; int lineno; } WEED_FUNC_HAS_NO_END_S;
        struct { char* tailId; } WEED_FUNC_HAS_NO_RETURN_S;
    } val;
} Error;

int writeError(Error *e);

#endif //HELLO_ERROR_H
