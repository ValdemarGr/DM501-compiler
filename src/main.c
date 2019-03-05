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
int stmDeclNum;
Body *theexpression;
extern FILE *yyin;

int main(int argc, char *argv[]) {
    for (int i = 1; i < argc; i++) {
        SymbolTable *globalScope = initSymbolTable();
        struct Type intStaticType = {.kind = typeIntK};

        char* arg = argv[i];

        printf("File name: %s\n", arg);

        //We can also do smart decorateFunction like a preprocessor and bundle it all in an out file
        FILE *fp = fopen(arg, "r");
        yyin = fp;

        int ei;
        Error *e;

        lineno = 1;
        yyparse();

        if (theexpression == NULL) {
            perror("Failed to parse.");
            return 1;
        }

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
