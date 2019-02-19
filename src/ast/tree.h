#ifndef __tree_h
#define __tree_h

typedef struct Body Body;
typedef struct Type Type;

typedef struct VarDelList {
    char *identifier;
    Type *type;
    struct VarDelList *next;
} VarDelList;

typedef struct Type {
    enum {
        idT, intT, boolT, arrayT, recordT
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
    enum { varK, varsK, typeK, functionK } kind;
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

typedef struct EXP {
    int lineno;
    enum {
        idK, intconstK, timesK, divK, plusK, minusK//, functionK
    } kind;
    union {
        char *idE;
        int intconstE;
        struct {
            struct EXP *left;
            struct EXP *right;
        } timesE;
        struct {
            struct EXP *left;
            struct EXP *right;
        } divE;
        struct {
            struct EXP *left;
            struct EXP *right;
        } plusE;
        struct {
            struct EXP *left;
            struct EXP *right;
        } minusE;
        struct {
            char *identifier;
            struct EXP *body;
        } functionE;
    } val;
} EXP;

typedef struct Statement {
    int lineno;
    enum { returnK, writeK, allocateK, allocateLenK, ifK, ifElK, whileK, whileSSK } kind;
    union {
        struct { EXP* exp; } returnD;
        struct { EXP* exp; } writeD;
        struct { EXP* exp; } allocateD;
        struct { EXP* exp; EXP* len; } allocateLenD;
        struct { EXP* exp; struct Statement *statement; } ifD;
        struct { EXP* exp; struct Statement *statement; struct Statement *elseStatement; } ifElD;
        struct { EXP* exp; Body* localBody; } whileD;
        struct { EXP* exp; struct Statement* statement; } whileSSD;
    } val;
} Statement;

typedef struct StatementList {
    int loneno;
    Statement *statement;
    struct StatementList *next;
} StatementList;

typedef struct Operator {
    int lineno;
    enum { multK, divK, plusK, minusK, equalityK, inequalityK, greaterK, lessK, geqK, leqK, andK, orK } kind;
    union {
        struct { EXP* lhs; EXP* rhs; } multD;
        struct { EXP* lhs; EXP* rhs; } divD;
        struct { EXP* lhs; EXP* rhs; } plusD;
        struct { EXP* lhs; EXP* rhs; } minusD;
        struct { EXP* lhs; EXP* rhs; } equalityD;
        struct { EXP* lhs; EXP* rhs; } inequalityD;
        struct { EXP* lhs; EXP* rhs; } greaterD;
        struct { EXP* lhs; EXP* rhs; } lessD;
        struct { EXP* lhs; EXP* rhs; } geqD;
        struct { EXP* lhs; EXP* rhs; } leqD;
        struct { EXP* lhs; EXP* rhs; } andD;
        struct { EXP* lhs; EXP* rhs; } orD;
    } val;
} Operator;

typedef struct Body {
    DeclarationList *declarationList;
    StatementList *statementList;
} Body;


EXP *makeEXPid(char *id);

EXP *makeEXPintconst(int intconst);

EXP *makeEXPtimes(EXP *left, EXP *right);

EXP *makeEXPdiv(EXP *left, EXP *right);

EXP *makeEXPplus(EXP *left, EXP *right);

EXP *makeEXPminus(EXP *left, EXP *right);

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

DeclarationList *makeDeclarationList(Declaration *declaration, DeclarationList *next);

Declaration *makeDeclaration(char* id, Type* type);

Declaration *makeTypeDeclaration(char* id, Type* type);

Declaration *makeVarDeclarations(VarDelList* vars);

StatementList *makeStatementList(Statement *statement, Statement *next);

Statement *makeReturnStatement(EXP *exp);

Statement *makeIfStatement(EXP *exp, Statement *statement);

Statement *makeIfElseStatement(EXP *exp, Statement *statement, Statement *elseStatement);

Statement *makeAllocateStatement(EXP *exp);

Statement *makeAllocateOfLenStatement(EXP *exp, EXP *len);

Statement *makeWriteStatement(EXP *exp);

Statement *makeWhileStatement(EXP *exp, Body *localBody);

Statement *makeWhileSingleStatement(EXP *exp, Statement *statement);

Type *makeIdType(char* id);

Type *makeIntType();

Type *makeBoolType();

Type *makeArrayType(Type *type);

Type *makeRecordType(VarDelList *record);

EXP *makeEXPfunction(char *identifier, EXP *body);

VarDelList *makeVarDelList(char *identifier, Type *type, VarDelList *next);

Function *makeFunction(FunctionHead *head, Body *body, FunctionTail *tail);

FunctionHead *makeFunctionHead(char *identifier, VarDelList *declerationList, Type *type);

FunctionTail *makeFunctionTail(char *identifier);

Body *makeBody(DeclarationList *declarationList, StatementList *statementList);

Declaration *makeFunctionDecleration(Function *function);

#endif