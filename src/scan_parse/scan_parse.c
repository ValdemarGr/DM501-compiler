#include "scan_parse.h"
#include "../ast/tree.h"

int lineno;
Body *theexpression;

int main() {
    SymbolTable *globalScope = initSymbolTable();

    lineno = 1;
    yyparse();
    prettyBody(theexpression);
    Error *e = weedFunctionNames(theexpression);
    writeError(e);
    e = weedFunctionReturns(theexpression);
    writeError(e);
    e = decorateAstWithSymbols(theexpression, globalScope);
    writeError(e);

    dumpSymbolTable(theexpression->declarationList->next->next->declaration->val.functionD.function->body->declarationList->declaration->symbolTable);

    printf("\n");
    return 0;
}
