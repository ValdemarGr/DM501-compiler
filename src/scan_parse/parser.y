%{
#include <stdio.h>
#include "scan_parse.h"

extern char *yytext;
extern Body *theexpression;

void yyerror() {
   printf("syntax error before %s\n",yytext);
}
%}

%union {
   int intconst;
   char *stringconst;
   struct EXP *exp;
   struct Body *body;
   struct DeclarationList *declarationList;
   struct VarDelList *varDelList;
   struct Declaration *declaration;
   struct Type *type;
   struct Function *function;
   struct FunctionHead *functionHead;
   struct FunctionTail *functionTail;
}

%token <intconst> tINTCONST
%token <stringconst> tIDENTIFIER
%token tFUNC
%token tEND
%token tTYPE
%token tVAR
%token tINT
%token tBOOL

%type <exp> exp
%type <body> program body
%type <declarationList> decl_list
%type <varDelList> var_del_list par_decl_list
%type <declaration> declaration
%type <type> type
%type <function> function
%type <functionHead> head
%type <functionTail> tail

%start program

%left '+' '-'
%left '*' '/'

%%
program: body
         { theexpression = $1;}
;

body : decl_list
       {$$ = makeBody($1);}
;

decl_list :
            {$$ = NULL;}
            | declaration decl_list
            {$$ = makeDeclarationList($1, $2);}
;

declaration : tVAR var_del_list ';'
              {$$ = makeVarDeclarations($2); }
              | function
              {$$ = makeFunctionDecleration($1); }
              | tTYPE tIDENTIFIER '=' type ';'
              {$$ = makeTypeDeclaration($2, $4); }
;

type :  tINT
        {$$ = makeIntType(); }
        | tBOOL
        {$$ = makeBoolType(); }
;

function :  head body tail
            {$$ = makeFunction($1, $2, $3);}
;

head :  tFUNC tIDENTIFIER '(' par_decl_list ')' ':' type
        {$$ = makeFunctionHead($2, $4, $7);}
;

tail :  tEND tIDENTIFIER
        {$$ = makeFunctionTail($2);}
;

par_decl_list : var_del_list
                {$$=$1;}
                |
                {$$=NULL;}
;

var_del_list :  tIDENTIFIER ':' type ',' par_decl_list
                    {$$ = makeVarDelList($1, $3, $5);}
                | tIDENTIFIER ':' type
                    {$$ = makeVarDelList($1, $3, NULL); }
;

exp : tIDENTIFIER
      {$$ = makeEXPid($1);}
    | tINTCONST
      {$$ = makeEXPintconst($1);}
    | exp '*' exp
      {$$ = makeEXPtimes($1,$3);}
    | exp '/' exp
      {$$ = makeEXPdiv($1,$3);}
    | exp '+' exp
      {$$ = makeEXPplus($1,$3);}
    | exp '-' exp
      {$$ = makeEXPminus($1,$3);}
    | '(' exp ')'
      {$$ = $2;}
    |
;

%%