%{
#include "parser.h"
#include <string.h>

extern int lineno;
extern int fileno();
extern void yyerror(char const *s);
%}

%option noyywrap
%x comment

%%
    int comment_depth = 0;
[ \t\r]+        /* ignore */;
\n              lineno++;

#.*             ;
"(*"            { comment_depth = 1; BEGIN(comment); }
<comment>[^*()\n]*       ;
<comment>"*"+[^*()\n]*   ;
<comment>"("+[^*(\n]*    ;
<comment>[^*\n]+")"      ;
<comment>\n              lineno++;
<comment>"("+"*"         { comment_depth++;}
<comment>"*"+")"         { if (--comment_depth < 1) {BEGIN(INITIAL);}}
<comment><<EOF>>         { yyerror("unterminated comment"); BEGIN(INITIAL); }

"*"             return '*';
"/"             return '/';
"+"             return '+';
"-"             return '-';
"=="            return tEQ;
"!="            return tINEQ;
">"             return tGREATER;
"<"             return tLESS;
">="            return tGEQ;
"<="            return tLEQ;
"&&"            return tAND;
"||"            return tOR;
"|"             return '|';
"("             return '(';
")"             return ')';
"{"             return '{';
"}"             return '}';
"["             return '[';
"]"             return ']';
"."             return '.';
","             return ',';
";"             return ';';
"="             return '=';
":"             return ':';
"!"             return '!';
"true"          return tTRUE;
"false"         return tFALSE;
"null"          return tNULL;
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
"->"            return tLAMBDA_ARROW;
"val"           return tVAL;
"class"         return tCLASS;
"with"          return tWITH;
"void"          return tVOID;
"constructor"   return tCONSTRUCTOR;
"gc"            return tGC;

0|([1-9][0-9]*)        { yylval.intconst = atoi(yytext);
                       return tINTCONST; }
[a-zA-Z_][a-zA-Z0-9_]* { yylval.stringconst = (char *)malloc(strlen(yytext)+1);
                         sprintf(yylval.stringconst,"%s",yytext);
                         return tIDENTIFIER; }
.                       { yyerror("unexpected character"); }/* ignore */;
%%
