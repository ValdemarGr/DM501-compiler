%{
#include "parser.h"
#include <string.h>

extern int lineno;
extern int fileno();
%}

%option noyywrap nounput noinput

%%
[ \t]+        /* ignore */;
\n              lineno++;

"*"             return '*';
"/"             return '/';
"+"             return '+';
"-"             return '-';
"("             return '(';
")"             return ')';
"{"             return '{';
"}"             return '}';
","             return ',';
";"             return ';';
"="             return '=';
":"             return ':';
"func"          return tFUNC;
"type"          return tTYPE;
"int"           return tINT;
"bool"          return tBOOL;
"var"           return tVAR;
"end"           return tEND;
"return"        return tRETURN;
"write"         return tWRITE;
"allocate"      return tALLOCATE;
"if"            return tIF;
"while"         return tWHILE;
"array of"      return tARRAY_OF;
"record of"     return tRECORD_OF;
"then"          return tTHEN;
"do"            return tDO;
"of length"     return tOF_LEN;
"else"          return tELSE;

0|([1-9][0-9]*)        { yylval.intconst = atoi(yytext);
                       return tINTCONST; }
[a-zA-Z_][a-zA-Z0-9_]* { yylval.stringconst = (char *)malloc(strlen(yytext)+1);
                         sprintf(yylval.stringconst,"%s",yytext);
                         return tIDENTIFIER; }
.               /* ignore */;
%%