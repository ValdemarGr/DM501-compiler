#include <stdio.h>

#include "scan_parse/scan_parse.h"
#include "ast/tree.h"
#include "pretty_print/pretty.h"
#include "error/error.h"
#include "weeder/weeds/weed_functions.h"
#include "symbol/symbol.h"
#include "symbol/decorate_ast.h"
#include "type_checker/type_checker.h"
#include "asm_code_gen/asm_code_gen.h"
#include "utils/stack.h"
#include "register_allocation/register_allocator.h"
#include "peephole/peephole.h"
#include "constant_fold/constant_fold.h"

int lineno;
int stmDeclNum;
Body *theexpression;
size_t currentTemporary = 1;
size_t maxTemporary = 0;
extern FILE *yyin;
bool printWithTypes = false;
bool prettyPrint = false;
bool verbose = false;
bool registerAllocation = false;
size_t maxDistFromRoot = 0;
int initialGcSizeMB = 10;
extern char *filename;

extern void yyparse();

extern Instructions *instructionHead;
extern Instructions *currentInstruction;
extern bool mainCreated;
extern size_t ifCounter;
extern size_t whileCounter;
extern size_t returnReg;
extern bool inClassContextCurrent;
extern int classGenericCount;
extern bool inLambdaContext;
extern bool currentlyInConstructorContext;
extern int lambdaDefineScope;
extern int lambdaArgCount;
extern int staticLinkDepth;

void resetAbstractGenGlobals() {
    instructionHead = NULL;
    currentInstruction = NULL;
    mainCreated = false;
    ifCounter = 0;
    whileCounter = 0;
    returnReg = 0;
    inClassContextCurrent = false;
    classGenericCount = 0;
    inLambdaContext = false;
    currentlyInConstructorContext = false;
    lambdaDefineScope = 0;
    lambdaArgCount = 0;
    staticLinkDepth = -1;
}

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
    e = weedMainReturn(theexpression);
    ei = writeError(e); if (ei != 0) return ei;
    e = decorateAstWithSymbols(theexpression, globalScope);
    ei = writeError(e); if (ei != 0) return ei;
    e = typeCheck(theexpression, &intStaticType);
    ei = writeError(e); if (ei != 0) return ei;

    if (prettyPrint) {
        prettyBody(theexpression);
    }

    //This must be done twice because of limitations
    constantFoldBody(theexpression);
    constantFoldBody(theexpression);

    resetAbstractGenGlobals();

    Instructions *instructions = generateInstructionTree(theexpression);

    //simpleRegisterAllocation(instructions, 13);

    //peephole(instructions);

    generate(stdout, instructions);

    return 0;
}

int main(int argc, char *argv[]) {
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
                case 'p':
                    prettyPrint = true;
                    break;
                case 'v':
                    verbose = true;
                    break;
                case 'l':
                    registerAllocation = true;
                    break;
                default:
                    if (strstr(arg, "mem=")) {
                        char *size = strstr(arg, "mem=") + strlen("mem=");
                        initialGcSizeMB = (int)atoi(size);
                    } else {
                        printf("Please supply the correct argument for %s\n", arg);
                    }
                    break;
            }

            continue;
        }

        if (verbose) {
            printf("File name: %s\n", arg);
        }

        //We can also do smart decorateFunction like a preprocessor and bundle it all in an out file
        FILE *fp = fopen(arg, "r");
        filename = arg;
        r = compile_file(fp);

        if (r != 0) {
            return r;
        }

        gotFile = true;
    }

    if (!gotFile) {
        filename = "(stdin)";
        r = compile_file(stdin);
    }

    return r;
}
