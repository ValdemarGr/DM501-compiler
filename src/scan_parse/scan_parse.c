#import "scan_parse.h"

int lineno;
EXP *theexpression;

int main() {
    lineno = 1;
    yyparse();
    // prettyEXP(theexpression);
    return 0;
}

EXP *parse(char *code) {
    lineno = 1;
    yy_scan_buffer(code);
    yyparse();
    return theexpression;
}