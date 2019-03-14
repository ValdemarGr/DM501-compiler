//
// Created by Mads Grau Kristensen on 2019-02-12.
//

#ifndef HELLO_PRETTY_H
#define HELLO_PRETTY_H

#include "../ast/tree.h"
#include "../utils/enum_to_string.h"
#include "../type_checker/type_checker.h"

void prettyBody(Body *body);
void prettyEXP(Expression *e, SymbolTable *symbolTable);
void prettyTerm(Term *term, SymbolTable *symbolTable);

#endif //HELLO_PRETTY_H
