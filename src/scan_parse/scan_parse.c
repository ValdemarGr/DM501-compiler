#import "scan_parse.h"
#import "pretty.h"
#include <stdio.h>

int lineno;
Expression *theexpression;

int main() {
    lineno = 1;
    yyparse();
    prettyBody(theexpression);
    printf("\n");
    return 0;
}
