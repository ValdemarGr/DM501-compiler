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

#define NULL_KITTY_VALUE_INDICATOR ((void *)1)

typedef struct Error Error;

Type *unwrapTypedef(Type *type, SymbolTable *symbolTable);
SYMBOL *getSymbolForBaseVariable(Variable *variable, SymbolTable *symbolTable);
Type *unwrapVariable(Variable *variable, SymbolTable *symbolTable);
Error *typeCheck(Body *body, Type *functionReturnType);
Type *evaluateExpressionType(Expression *expression, SymbolTable *symbolTable);
#endif //HELLO_TYPE_CHECKER_H
