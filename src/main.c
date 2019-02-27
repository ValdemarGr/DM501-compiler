#include "scan_parse/scan_parse.h"
#include "ast/tree.h"
#include "pretty_print/pretty.h"
#include "error/error.h"
#include <stdio.h>
#include "ast/tree.h"
#include "weeder/weeds/weed_functions.h"
#include "symbol/symbol.h"
#include "symbol/decorate_ast.h"
#include "type_checker/type_checker.h"
#include "../cmake-build-debug/src/scan_parse/parser.h"

int lineno;
Body *theexpression;
extern FILE *yyin;

int main(int argc, char *argv[]) {
    SymbolTable *globalScope = initSymbolTable();
    struct Type intStaticType = {.kind = typeIntK};

    for (int i = 1; i < argc; i++) {
        char* arg = argv[i];

        //We can also do smart stuff like a preprocessor and bundle it all in an out file
        FILE *fp = fopen(arg, "r");
        yyin = fp;

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

    }

    return 0;
}
