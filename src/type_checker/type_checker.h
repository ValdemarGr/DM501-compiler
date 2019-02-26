//
// Created by valde on 2/24/19.
//

#ifndef HELLO_TYPE_CHECKER_H
#define HELLO_TYPE_CHECKER_H

#include <stdio.h>
#include <stdlib.h>
#include "../error/error.h"
#include "../ast/tree.h"
#include "../symbol/symbol.h"
#include "../symbol/decorate_ast.h"

Error *typeCheck(Body *body, TypeKind functionReturnType);

#endif //HELLO_TYPE_CHECKER_H
