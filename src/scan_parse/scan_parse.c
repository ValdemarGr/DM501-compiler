#import "scan_parse.h"
#import "pretty.h"
#include "../error/error.h"
#include <stdio.h>

int lineno;
Body *theexpression;

int main() {
    lineno = 1;
    yyparse();
    prettyBody(theexpression);
    Error *e = weedFunctionNames(theexpression);
    writeError(e);
    e = weedFunctionReturns(theexpression);
    writeError(e);
    printf("\n");
    return 0;
}
