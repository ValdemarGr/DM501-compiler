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
   struct Expression *expression;
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
   struct Operator *operator;
   struct Variable *variable;
   struct ExpressionList *expressionList;
   struct Term *term;
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
%token tEQ
%token tINEQ
%token tGREATER
%token tLESS
%token tGEQ
%token tLEQ
%token tAND
%token tOR
%token tTRUE
%token tFALSE
%token tNULL

%type <expression> expression
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
%type <operator> operator
%type <variable> variable
%type <expressionList> act_list exp_list
%type <term> term

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

statement : tRETURN expression ';'
        {$$ = makeReturnStatement($2);}
        | tIF expression tTHEN statement tELSE statement
        {$$ = makeIfElseStatement($2, $4, $6);}
        | tIF expression tTHEN statement
        {$$ = makeIfStatement($2, $4);}
        | tALLOCATE expression ';'
        {$$ = makeAllocateStatement($2);}
        | tALLOCATE expression tOF_LEN expression ';'
        {$$ = makeAllocateOfLenStatement($2, $4);}
        | tWRITE expression ';'
        {$$ = makeWriteStatement($2);}
        | tWHILE expression tDO statement
        {$$ = makeWhileStatement($2, $4);}
        | '{' stm_list '}'
        {$$ = makeStatementFromList($2);}
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

expression : tINTCONST
        {$$ = makeEXPintconst($1);}
        | '(' expression ')'
        {$$ = $2;}
;

expression : expression operator expression
        {$$ = makeEXPOpEXP($1, $2, $3);}
        | term
        {$$ = makeEXPFromTerm($1);}
;

act_list :  exp_list
            {$$ = $1;}
            |
            {$$ = NULL;}
;

exp_list :  expression
            {$$ = makeExpList($1, NULL); }
            | expression ',' exp_list
            {$$ = makeExpList($1, $3); }
;

operator : '*'
        {$$ = makeMultOp();}
        | '/'
        {$$ = makeDivOp();}
        | '+'
        {$$ = makePlusOp();}
        | '-'
        {$$ = makeMinusOp();}
        | tEQ
        {$$ = makeEqualityOp();}
        | tINEQ
        {$$ = makeInequalityOp();}
        | tGREATER
        {$$ = makeGreaterOp();}
        | tLESS
        {$$ = makeLessOp();}
        | tGEQ
        {$$ = makeGeqOp();}
        | tLEQ
        {$$ = makeLeqOp();}
        | tAND
        {$$ = makeAndOp();}
        | tOR
        {$$ = makeOrOp();}
;

variable : tIDENTIFIER
        {$$ = makeVariable($1);}
        | variable '[' expression ']'
        {$$ = makeArraySubscript($1, $3);}
        | variable '.' tIDENTIFIER
        {$$ = makeRecordSubscript($1, $3);}
;

term : variable
        {$$ = makeTermFromVariable($1);};
        | tIDENTIFIER '(' act_list ')'
        {$$ = makeFunctionCallTerm($1, $3);}
        | '(' expression ')'
        {$$ = makeParentheses($2);}
        | '!' term
        {$$ = makeNegatedTerm($2);}
        | '|' expression '|'
        {$$ = makeAbsTerm($2);}
        | tINTCONST
        {$$ = makeNumTerm($1);}
        | tTRUE
        {$$ = makeTrueTerm();}
        | tFALSE
        {$$ = makeFalseTerm();}
        | tNULL
        {$$ = makeNullTerm();}
;

%%