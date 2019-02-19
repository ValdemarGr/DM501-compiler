#ifndef __tree_h
#define __tree_h

typedef struct Body Body;
typedef struct Type Type;
typedef struct Term Term;

typedef struct VarDelList {
    char *identifier;
    Type *type;
    struct VarDelList *next;
} VarDelList;

typedef struct Type {
    enum {
        typeIdK, typeIntK, typeBoolK, typeArrayK, typeRecordK
    } kind;
    union {
        struct { char *id; } idType;
        struct { struct Type *type; } arrayType;
        struct { VarDelList *types; } recordType;
    } val;
} Type;

typedef struct FunctionHead {
    char *indentifier;
    VarDelList *declarationList;
    Type *returnType;
} FunctionHead;

typedef struct FunctionTail {
    char *indentifier;
} FunctionTail;

typedef struct Function {
    FunctionHead *head;
    Body *body;
    FunctionTail *tail;
} Function;

typedef struct Declaration {
    int lineno;
    enum { declVarK, declVarsK, declTypeK, declFuncK } kind;
    union {
        struct { char* id; Type *type; } varD;
        struct { struct Declaration *var; struct Declaration *next; } varsD;
        struct { char* id; Type *type; } typeD;
        struct { Function *function; } functionD;
    } val;
} Declaration;

typedef struct DeclarationList {
    int lineno;
    Declaration *declaration;
    struct DeclarationList *next;
} DeclarationList;

typedef struct Expression {
    int lineno;
    enum {
        idK, intconstK, opK, termK//, functionK
    } kind;
    union {
        char *idE;
        int intconstE;
        struct {
            struct Expression *left;
            struct Operator *operator;
            struct Expression *right;
        } op;
        struct {
            char *identifier;
            struct Expression *body;
        } functionE;
        struct {
            Term *term;
        } termD;
    } val;
} Expression;

typedef struct Statement {
    int lineno;
    enum { statReturnK, statWriteK, statAllocateK, statAllocateLenK, statIfK, statIfElK, statWhileK, statWhileSSK } kind;
    union {
        struct { Expression* exp; } returnD;
        struct { Expression* exp; } writeD;
        struct { Expression* exp; } allocateD;
        struct { Expression* exp; Expression* len; } allocateLenD;
        struct { Expression* exp; struct Statement *statement; } ifD;
        struct { Expression* exp; struct Statement *statement; struct Statement *elseStatement; } ifElD;
        struct { Expression* exp; Body* localBody; } whileD;
        struct { Expression* exp; struct Statement* statement; } whileSSD;
    } val;
} Statement;

typedef struct StatementList {
    int loneno;
    Statement *statement;
    struct StatementList *next;
} StatementList;

typedef struct Operator {
    int lineno;
    enum { opMultK, opDivK, opPlusK, opMinusK, opEqualityK, opInequalityK, opGreaterK, opLessK, opGeqK, opLeqK, opAndK, opOrK } kind;
} Operator;

typedef struct ExpressionList {
    int lineno;
    Expression *expression;
    struct ExpressionList *next;
} ExpressionList;

typedef struct Variable {
    int lineno;
    enum { varIdK, arrayIndexK, recordLookupK } kind;
    union {
        struct { char* id; } idD;
        struct { struct Variable *var; struct Expression *idx; } arrayIndexD;
        struct { struct Variable *var; char *id; } recordLookupD;
    } val;
} Variable;

typedef struct Term {
    int lineno;
    enum { variableK, functionCallK, negateK, absK, trueK, falseK, nullK } kind;
    union {
        struct { struct Variable *var; } variableD;
        struct { char *functionId; ExpressionList *expressionList; } functionCallD;
        struct { Term* term; } negateD;
        struct { Expression* expression; } absD;
    } val;
} Term;

typedef struct Body {
    DeclarationList *declarationList;
    StatementList *statementList;
} Body;

Expression *makeEXPFromTerm(Term *term);

Variable *makeVariable(char *id);

Variable *makeArraySubscript(Variable *variable, Expression *expression);

Variable *makeRecordSubscript(Variable* variable, char *id);

Term *makeTermFromVariable(Variable *variable);

Term *makeFunctionCallTerm(char *functionId, ExpressionList *expressionList);

Term *makeNegatedTerm(Term *term);

Term *makeAbsTerm(Expression *expression);

Term *makeTrueTerm();

Term *makeFalseTerm();

Term *makeNullTerm();

Expression *makeEXPid(char *id);

Expression *makeEXPintconst(int intconst);

Expression *makeEXPOpEXP(Expression *lhs, Operator *op, Expression *rhs);


//OPERATORS START
Operator *makeMultOp();

Operator *makeDivOp();

Operator *makePlusOp();

Operator *makeMinusOp();

Operator *makeEqualityOp();

Operator *makeInequalityOp();

Operator *makeGreaterOp();

Operator *makeLessOp();

Operator *makeGeqOp();

Operator *makeLeqOp();

Operator *makeAndOp();

Operator *makeOrOp();
//OPERATORS END

ExpressionList *makeExpList(Expression *exp, ExpressionList *next);

DeclarationList *makeDeclarationList(Declaration *declaration, DeclarationList *next);

Declaration *makeDeclaration(char* id, Type* type);

Declaration *makeTypeDeclaration(char* id, Type* type);

Declaration *makeVarDeclarations(VarDelList* vars);

StatementList *makeStatementList(Statement *statement, StatementList *next);

Statement *makeReturnStatement(Expression *exp);

Statement *makeIfStatement(Expression *exp, Statement *statement);

Statement *makeIfElseStatement(Expression *exp, Statement *statement, Statement *elseStatement);

Statement *makeAllocateStatement(Expression *exp);

Statement *makeAllocateOfLenStatement(Expression *exp, Expression *len);

Statement *makeWriteStatement(Expression *exp);

Statement *makeWhileStatement(Expression *exp, Body *localBody);

Statement *makeWhileSingleStatement(Expression *exp, Statement *statement);

Type *makeIdType(char* id);

Type *makeIntType();

Type *makeBoolType();

Type *makeArrayType(Type *type);

Type *makeRecordType(VarDelList *record);

Expression *makeEXPfunction(char *identifier, Expression *body);

VarDelList *makeVarDelList(char *identifier, Type *type, VarDelList *next);

Function *makeFunction(FunctionHead *head, Body *body, FunctionTail *tail);

FunctionHead *makeFunctionHead(char *identifier, VarDelList *declerationList, Type *type);

FunctionTail *makeFunctionTail(char *identifier);

Body *makeBody(DeclarationList *declarationList, StatementList *statementList);

Declaration *makeFunctionDecleration(Function *function);

#endif