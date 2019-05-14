%locations
%code requires {
    char *filename;

    #include "parser_extra.h"

    #define YYLTYPE_IS_DECLARED 1

    # define YYLLOC_DEFAULT(Current, Rhs, N)                               \
        do                                                                 \
            if (N) {                                                       \
                (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;     \
                (Current).first_column = YYRHSLOC (Rhs, 1).first_column;   \
                (Current).last_line    = YYRHSLOC (Rhs, N).last_line;      \
                (Current).last_column  = YYRHSLOC (Rhs, N).last_column;    \
                (Current).filename     = YYRHSLOC (Rhs, 1).filename;       \
            } else { /* empty RHS */                                       \
                (Current).first_line   = (Current).last_line   =           \
                YYRHSLOC (Rhs, 0).last_line;                               \
                (Current).first_column = (Current).last_column =           \
                YYRHSLOC (Rhs, 0).last_column;                             \
                (Current).filename  = NULL;                     /* new */  \
            }                                                              \
        while (0)
}

%code {
#include <stdio.h>
#include <string.h>
#include "scan_parse.h"

extern char *yytext;
extern int yylex();
extern Body *theexpression;
extern int lineno;
extern int numberErrors;
extern void YYABORT;


void yyerror(char const *s) {
    if (yylloc.first_line > 0) {
        fprintf(stderr, "%s:%d.%d-%d.%d: error at %s: %s\n", yylloc.filename,
            yylloc.first_line, yylloc.first_column, yylloc.last_line, yylloc.last_column,
            yytext, s);
    }
    numberErrors++;
    if (numberErrors >= 10) {
        YYABORT;
    }
}


void lyyerror(YYLTYPE t, char const *s) {
    if (t.first_line > 0) {
        fprintf(stderr, "%s:%d.%d-%d.%d: error at %s: %s\n", t.filename,
            t.first_line, t.first_column, t.last_line, t.last_column, yytext, s);
    }
    numberErrors++;
    if (numberErrors >= 10) {
        YYABORT;
    }
}


}

%union {
   int intconst;
   char *stringconst;
   char *charconst;
   struct Expression *expression;
   struct Body *body;
   struct DeclarationList *declarationList;
   struct VarDelList *varDelList;
   struct Declaration *declaration;
   struct Type *type;
   struct Function *function;
   struct Lambda *lambda;
   struct FunctionHead *functionHead;
   struct FunctionTail *functionTail;
   struct StatementList *statementList;
   struct Statement *statement;
   struct Variable *variable;
   struct ExpressionList *expressionList;
   struct Term *term;
   struct TypeList *typeList;
   struct Constructor *constructor;
   struct VarType *varType;
}

%token <intconst> tINTCONST
%token <stringconst> tIDENTIFIER
%token <charconst> tCHAR
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
%token tLAMBDA_ARROW
%token tVAL
%token tCLASS
%token tWITH
%token tVOID
%token tCONSTRUCTOR
%token tGC
%token tGCDEBUG
%token tWRITEANY
%token tCHARTYPE
%token tWRITENL

%type <expression> expression
%type <lambda> lambda
%type <body> program body
%type <declarationList> decl_list
%type <varDelList> var_decl_list par_decl_list
%type <declaration> declaration
%type <type> type voidType
%type <function> function
%type <functionHead> head
%type <functionTail> tail
%type <statementList> stm_list
%type <statement> statement
%type <variable> variable
%type <expressionList> act_list exp_list
%type <term> term
%type <typeList> type_list generic_type_list class_extension_list
%type <constructor> constructor
%type <varType> var_type

%start program

%left tEQ tINEQ
%left  tLESS tGREATER tGEQ tLEQ
%left tOR
%left  tAND
%left '+' '-'
%left '*' '/'

%%
program: body
         { theexpression = $1;}
;

body : decl_list stm_list
       {$$ = makeBody($1, $2, @$);}
;

decl_list : %empty
            {$$ = NULL;}
            | declaration decl_list
            {$$ = makeDeclarationList($1, $2, @$);}
;

stm_list :  %empty
            {$$ = NULL;}
            | statement stm_list
            {$$ = makeStatementList($1, $2, @$);}
;

generic_type_list : tIDENTIFIER ':' tIDENTIFIER ',' generic_type_list
        {$$ = makeGenericTypeList($5, $1, $3, @$);}
        | tIDENTIFIER ',' generic_type_list
        {$$ = makeGenericTypeList($3, $1, NULL, @$);}
        | tIDENTIFIER
        {$$ = makeGenericTypeList(NULL, $1, NULL, @$);}
        | tIDENTIFIER ':' tIDENTIFIER
        {$$ = makeGenericTypeList(NULL, $1, $3, @$);}
;

class_extension_list : tIDENTIFIER
                {$$ = makeExtensionList(NULL, $1, NULL, @$);}
                | tIDENTIFIER '[' type_list ']'
                {$$ = makeExtensionList(NULL, $1, $3, @$);}
                | tIDENTIFIER tWITH class_extension_list
                {$$ = makeExtensionList($3, $1, NULL, @$);}
                | tIDENTIFIER '[' type_list ']' tWITH class_extension_list
                {$$ = makeExtensionList($6, $1, $3, @$);}
;

constructor : tCONSTRUCTOR '(' par_decl_list ')' '{' body '}' ';'
              {$$ = makeClassConstructor($3, $6, @$); }
;

declaration : tVAR var_decl_list ';'
              {$$ = makeVarDeclarations($2, @$); }
              | function
              {$$ = makeFunctionDecleration($1, @$); }
              | tTYPE tIDENTIFIER '=' type ';'
              {$$ = makeTypeDeclaration($2, $4, @$); }
              | tVAL tIDENTIFIER '=' expression ';'
              {$$ = makeValDeclaration($2, $4, @$);}
              | tCLASS tIDENTIFIER '{' decl_list '}' ';'
              {$$ = makeClassDeclaration($2, $4, NULL, NULL, NULL, @$);}
              | tCLASS tIDENTIFIER '[' generic_type_list ']' '{' decl_list '}' ';'
              {$$ = makeClassDeclaration($2, $7, $4, NULL, NULL, @$);}
              | tCLASS tIDENTIFIER '[' generic_type_list ']' tWITH class_extension_list '{' decl_list '}' ';'
              {$$ = makeClassDeclaration($2, $9, $4, $7, NULL, @$);}
              | tCLASS tIDENTIFIER tWITH class_extension_list '{' decl_list '}' ';'
              {$$ = makeClassDeclaration($2, $6, NULL, $4, NULL, @$);}
              | tCLASS tIDENTIFIER '{' constructor decl_list '}' ';'
              {$$ = makeClassDeclaration($2, $5, NULL, NULL, $4, @$);}
              | tCLASS tIDENTIFIER '[' generic_type_list ']' '{' constructor decl_list '}' ';'
              {$$ = makeClassDeclaration($2, $8, $4, NULL, $7, @$);}
              | tCLASS tIDENTIFIER '[' generic_type_list ']' tWITH class_extension_list '{' constructor decl_list '}' ';'
              {$$ = makeClassDeclaration($2, $10, $4, $7, $9, @$);}
              | tCLASS tIDENTIFIER tWITH class_extension_list '{' constructor decl_list '}' ';'
              {$$ = makeClassDeclaration($2, $7, NULL, $4, $6, @$);}
              | error ';'
              {};
;

statement : tRETURN expression ';'
        {$$ = makeReturnStatement($2, @$);}
        | tIF expression tTHEN statement tELSE statement
        {$$ = makeIfElseStatement($2, $4, $6, @$);}
        | tIF expression tTHEN statement
        {$$ = makeIfStatement($2, $4, @$);}
        | tALLOCATE variable ';'
        {$$ = makeAllocateStatement($2, @$);}
        | tALLOCATE variable '(' act_list ')' ';'
        {$$ = makeAllocateWithConstructorStatement($2, $4, @$);}
        | tALLOCATE variable tOF_LEN expression ';'
        {$$ = makeAllocateOfLenStatement($2, $4, @$);}
        | variable '=' expression ';'
        {$$ = makeAssignment($1, $3, @$);}
        | tWRITE expression ';'
        {$$ = makeWriteStatement($2, @$);}
        | tWRITEANY expression ';'
        {$$ = makeWriteAnyStatement($2, @$);}
        | tWHILE expression tDO statement
        {$$ = makeWhileStatement($2, $4, @$);}
        | '{' stm_list '}'
        {$$ = makeStatementFromList($2, @$);}
        | expression ';'
        {$$ = makeEmptyExpression($1, @$);}
        | tWRITENL ';'
        {$$ = makeWriteNlStatement(@$);}
        | tGC ';'
        {$$ = makeGCStatement(@$);}
        | tGCDEBUG ';'
        {$$ = makeGCDebugStatement(@$);}
        | error ';'
        {};
        | error '}'
        {};
;

type_list : type ',' type_list
        {$$ = makeTypeList($3, $1, @$);}
        | type
        {$$ = makeTypeList(NULL, $1, @$);}
        | %empty
        {$$ = NULL;}
;

type :  tIDENTIFIER
        {$$ = makeIdType($1, @$); }
        | tCLASS tIDENTIFIER
        {$$ = makeClassType($2, NULL, @$); }
        | tCLASS tIDENTIFIER '[' type_list ']'
        {$$ = makeClassType($2, $4, @$); }
        | tINT
        {$$ = makeIntType(@$); }
        | tBOOL
        {$$ = makeBoolType(@$); }
        | tARRAY_OF type
        {$$ = makeArrayType($2, @$); }
        | tRECORD_OF '{' var_decl_list '}'
        {$$ = makeRecordType($3, @$); }
        | '(' type_list ')' tLAMBDA_ARROW type
        {$$ = makeLambdaType($2, $5, @$);}
        | '(' type_list ')' tLAMBDA_ARROW voidType
        {$$ = makeLambdaType($2, $5, @$);}
        | tCHARTYPE
        {$$ = makeCharType(@$);}
;

voidType :  tVOID
        {$$ = makeVoidType(@$); }
;

lambda : '(' par_decl_list ')' ':' type tLAMBDA_ARROW '{' body '}'
        {$$ = makeLambda($2, $5, $8, @$);}
        | '(' par_decl_list ')' ':' voidType tLAMBDA_ARROW '{' body '}'
        {$$ = makeLambda($2, $5, $8, @$);}
;

function :  head body tail
            {$$ = makeFunction($1, $2, $3, @$);}
;

head :  tFUNC tIDENTIFIER '(' par_decl_list ')' ':' type
        {$$ = makeFunctionHead($2, $4, $7, @$);}
;

tail :  tEND tIDENTIFIER
        {$$ = makeFunctionTail($2, @$);}
;

par_decl_list : %empty
                {$$=NULL;}
                | var_decl_list
                {$$=$1;}
;

var_type :      tIDENTIFIER ':' type
                {$$=makeVarType($1, $3, @$);}

var_decl_list :  var_type ',' var_decl_list
                    {$$ = makeVarDelList($1, $3, @$);}
                | var_type
                    {$$ = makeVarDelList($1, NULL, @$); }
;

expression : expression '*' expression
        {$$ = makeEXPOpEXP($1, makeMultOp(), $3, @$);}
        | expression '/' expression
        {$$ = makeEXPOpEXP($1, makeDivOp(), $3, @$);}
        | expression '+' expression
        {$$ = makeEXPOpEXP($1, makePlusOp(), $3, @$);}
        | expression '-' expression
        {$$ = makeEXPOpEXP($1, makeMinusOp(), $3, @$);}
        | expression tEQ expression
        {$$ = makeEXPOpEXP($1, makeEqualityOp(), $3, @$);}
        | expression tINEQ expression
        {$$ = makeEXPOpEXP($1, makeInequalityOp(), $3, @$);}
        | expression tGREATER expression
        {$$ = makeEXPOpEXP($1, makeGreaterOp(), $3, @$);}
        | expression tLESS expression
        {$$ = makeEXPOpEXP($1, makeLessOp(), $3, @$);}
        | expression tGEQ expression
        {$$ = makeEXPOpEXP($1, makeGeqOp(), $3, @$);}
        | expression tLEQ expression
        {$$ = makeEXPOpEXP($1, makeLeqOp(), $3, @$);}
        | expression tAND expression
        {$$ = makeEXPOpEXP($1, makeAndOp(), $3, @$);}
        | expression tOR expression
        {$$ = makeEXPOpEXP($1, makeOrOp(), $3, @$);}
        | term
        {$$ = makeEXPFromTerm($1, @$);}
;

act_list :  exp_list
            {$$ = $1;}
            | %empty
            {$$ = NULL;}
;

exp_list :  expression
            {$$ = makeExpList($1, NULL, @$); }
            | expression ',' exp_list
            {$$ = makeExpList($1, $3, @$); }
;

variable : tIDENTIFIER
        {$$ = makeVariable($1, @$);}
        | variable '[' expression ']'
        {$$ = makeArraySubscript($1, $3, @$);}
        | variable '.' tIDENTIFIER
        {$$ = makeRecordSubscript($1, $3, @$);}
;

term : variable
        {$$ = makeTermFromVariable($1, @$);};
        | tIDENTIFIER '(' act_list ')'
        {$$ = makeFunctionCallTerm($1, $3, @$);}
        | '(' expression ')'
        {$$ = makeParentheses($2, @$);}
        | '!' term
        {$$ = makeNegatedTerm($2, @$);}
        | '|' expression '|'
        {$$ = makeAbsTerm($2, @$);}
        | tINTCONST
        {$$ = makeNumTerm($1, @$);}
        | tCHAR
        {$$ = makeCharTerm($1, @$);}
        | tTRUE
        {$$ = makeTrueTerm(@$);}
        | tFALSE
        {$$ = makeFalseTerm(@$);}
        | tNULL
        {$$ = makeNullTerm(@$);}
        | lambda
        {$$ = makeLambdaTerm($1, @$);}
        | variable ':' type
        {$$ = makeDowncastTerm($1, $3, @$);}
        | variable '(' act_list ')'
        {$$ = makeShorthandLambdaCall($1, $3, @$);}
        | tOR
        { lyyerror(@1, "Nested absolute and length terms need spaces between bars."); YYERROR; }
;

%%