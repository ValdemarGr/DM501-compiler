#import "scan_parse.h"
#import "pretty.h"
#include <stdio.h>

int lineno;
EXP *theexpression;

int main() {
    lineno = 1;
    yyparse();
    prettyBody(theexpression);
    printf("\n");
    return 0;
}
