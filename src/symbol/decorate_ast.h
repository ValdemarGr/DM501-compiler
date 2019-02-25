//
// Created by valde on 2/25/19.
//

#ifndef HELLO_SYMBOL_FROM_AST_H
#define HELLO_SYMBOL_FROM_AST_H

#include "symbol.h"
#include "../ast/tree.h"
#include "../error/error.h"


Error *decorateAstWithSymbols(Body *body, SymbolTable *symbolTable);

#endif //HELLO_SYMBOL_FROM_AST_H
