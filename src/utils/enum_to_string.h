//
// Created by valde on 2/25/19.
//

#ifndef HELLO_ENUM_TO_STRING_H
#define HELLO_ENUM_TO_STRING_H

#include "../ast/tree.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

char *typeToString(Type *type);
char *typeEnumToString(TypeKind typeKind);
char *variableToString(Variable *variable);

#endif //HELLO_ENUM_TO_STRING_H
