#include <parser.h>
#include "scan_parse.h"
#include "../ast/tree.h"

int lineno;
Body *theexpression;

int main() {
    SymbolTable *globalScope = initSymbolTable();
    struct Type intStaticType = {.kind = typeIntK};

    int ei;
    Error *e;

    lineno = 1;
    yyparse();
    prettyBody(theexpression);

    e = weedFunctionNames(theexpression);
    ei = writeError(e); if (ei != 0) return ei;
    e = weedFunctionReturns(theexpression);
    ei = writeError(e); if (ei != 0) return ei;
    e = decorateAstWithSymbols(theexpression, globalScope);
    ei = writeError(e); if (ei != 0) return ei;
    e = typeCheck(theexpression, &intStaticType);
    ei = writeError(e); if (ei != 0) return ei;

    printf("\n");
    return 0;
}
