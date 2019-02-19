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
   struct StatementList *statementList;
   struct Statement *statement;
}

%token <intconst> tINTCONST
%token <stringconst> tIDENTIFIER
%token tFUNC
%token tEND
%token tTYPE
%token tVAR
%token tINT
%token tBOOL
%token tRETURN
%token tWRITE
%token tALLOCATE
%token tIF
%token tWHILE
%token tARRAY_OF
%token tRECORD_OF
%token tTHEN
%token tDO
%token tOF_LEN
%token tELSE

%type <exp> exp
%type <body> program body
%type <declarationList> decl_list
%type <varDelList> var_decl_list par_decl_list
%type <declaration> declaration
%type <type> type
%type <function> function
%type <functionHead> head
%type <functionTail> tail
%type <statementList> stm_list
%type <statement> statement

%start program

%left '+' '-'
%left '*' '/'

%%
program: body
         { theexpression = $1;}
;

body : decl_list stm_list
       {$$ = makeBody($1, $2);}
;

decl_list :
            {$$ = NULL;}
            | declaration decl_list
            {$$ = makeDeclarationList($1, $2);}
;

stm_list :
            {$$ = NULL;}
            | statement stm_list
            {$$ = makeStatementList($1, $2);}
;

declaration : tVAR var_decl_list ';'
              {$$ = makeVarDeclarations($2); }
              | function
              {$$ = makeFunctionDecleration($1); }
              | tTYPE tIDENTIFIER '=' type ';'
              {$$ = makeTypeDeclaration($2, $4); }
;

statement : tRETURN exp ';'
        {$$ = makeReturnStatement($2);}
        | tIF exp tTHEN statement tELSE statement
        {$$ = makeIfElseStatement($2, $4, $6);}
        | tIF exp tTHEN statement
        {$$ = makeIfStatement($2, $4);}
        | tALLOCATE exp ';'
        {$$ = makeAllocateStatement($2);}
        | tALLOCATE exp tOF_LEN exp ';'
        {$$ = makeAllocateOfLenStatement($2, $4);}
        | tWRITE exp ';'
        {$$ = makeWriteStatement($2);}
        | tWHILE exp tDO '{' body '}'
        {$$ = makeWhileStatement($2, $5);}
        | tWHILE exp tDO statement
        {$$ = makeWhileSingleStatement($2, $4);}
;

type :  tIDENTIFIER
        {$$ = makeIdType($1); }
        | tINT
        {$$ = makeIntType(); }
        | tBOOL
        {$$ = makeBoolType(); }
        | tARRAY_OF type
        {$$ = makeArrayType($2); }
        | tRECORD_OF '{' var_decl_list '}'
        {$$ = makeRecordType($3); }
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

par_decl_list : var_decl_list
                {$$=$1;}
                |
                {$$=NULL;}
;

var_decl_list :  tIDENTIFIER ':' type ',' par_decl_list
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

op : '*'
        {$$ = makeMultOp();}
        '/'
        {$$ = makeDivOp();}
        '+'
        {$$ = makePlusOp();}
        '-'
        {$$ = makeMinusOp();}
        '=='
        {$$ = makeEqualityOp();}
        '!='
        {$$ = makeInequalityOp();}
        '>'
        {$$ = makeGreaterOp();}
        '<'
        {$$ = makeLessOp();}
        '>='
        {$$ = makeGeqOp();}
        '<='
        {$$ = makeLeqOp();}
        '&&'
        {$$ = makeAndOp();}
        '||'
        {$$ = makeOrOp();}
;

%%