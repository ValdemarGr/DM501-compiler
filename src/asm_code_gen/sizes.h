//
// Created by valde on 4/18/19.
//

#ifndef HELLO_SIZES_H
#define HELLO_SIZES_H

#include <stdio.h>
#include <stdlib.h>
#include "../ast/tree.h"
#include "consts.h"

#define POINTER_SIZE 8
#define INTEGER_SIZE 4

size_t getSizeForType(Type *tpe);

#endif //HELLO_SIZES_H
