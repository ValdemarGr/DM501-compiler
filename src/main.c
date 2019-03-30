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
#include "asm_code_gen/asm_code_gen.h"

int lineno;
int stmDeclNum;
Body *theexpression;
extern FILE *yyin;
bool printWithTypes = false;

int compile_file(FILE *file) {
    SymbolTable *globalScope = initSymbolTable();
    struct Type intStaticType = {.kind = typeIntK};

    yyin = file;

    int ei;
    Error *e;

    lineno = 1;
    yyparse();

    if (theexpression == NULL) {
        perror("Failed to parse.");
        return 1;
    }

    e = weedFunctionNames(theexpression);
    ei = writeError(e); if (ei != 0) return ei;
    e = weedFunctionReturns(theexpression);
    ei = writeError(e); if (ei != 0) return ei;
    e = decorateAstWithSymbols(theexpression, globalScope);
    ei = writeError(e); if (ei != 0) return ei;
    e = typeCheck(theexpression, &intStaticType);
    ei = writeError(e); if (ei != 0) return ei;

    prettyBody(theexpression);

    printf("\n");

    return 0;
}

int main(int argc, char *argv[]) {
    generate(stdout, NULL);

    bool gotFile = false;
    int r = 0;
    for (int i = 1; i < argc; i++) {
        char* arg = argv[i];

        //Parse arg
        if (arg[0] == '-') {
            switch (arg[1]) {
                case 't':
                    printWithTypes = true;
                    break;
                default:
                    printf("Please supply the correct argument for %s\n", arg);
                    break;
            }

            continue;
        }

        printf("File name: %s\n", arg);

        //We can also do smart decorateFunction like a preprocessor and bundle it all in an out file
        FILE *fp = fopen(arg, "r");
        r = compile_file(fp);

        if (r != 0) {
            return r;
        }

        gotFile = true;
    }

    if (!gotFile) {
        r = compile_file(stdin);
    }

    return r;
}
