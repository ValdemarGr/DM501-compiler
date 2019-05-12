//
// Created by valde on 5/12/19.
//

#ifndef HELLO_CONSTANT_FOLD_H
#define HELLO_CONSTANT_FOLD_H

#include "../ast/tree.h"
#include <stdlib.h>
#include "../type_checker/type_checker.h"

void constantFoldBody(Body *body);

#endif //HELLO_CONSTANT_FOLD_H
