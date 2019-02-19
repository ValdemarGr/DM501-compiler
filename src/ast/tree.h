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
        expIdK, expNumK, expMultK, expDivK, expPlusK, expMinusK
    } kind;
    union {
        char *idE;
        int intconstE;
        struct {
            struct Expression *left;
            struct Expression *right;
        } timesE;
        struct {
            struct Expression *left;
            struct Expression *right;
        } divE;
        struct {
            struct Expression *left;
            struct Expression *right;
        } plusE;
        struct {
            struct Expression *left;
            struct Expression *right;
        } minusE;
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
    union {
        struct { struct Expression* lhs; struct Expression* rhs; } multD;
        struct { struct Expression* lhs; struct Expression* rhs; } divD;
        struct { struct Expression* lhs; struct Expression* rhs; } plusD;
        struct { struct Expression* lhs; struct Expression* rhs; } minusD;
        struct { struct Expression* lhs; struct Expression* rhs; } equalityD;
        struct { struct Expression* lhs; struct Expression* rhs; } inequalityD;
        struct { struct Expression* lhs; struct Expression* rhs; } greaterD;
        struct { struct Expression* lhs; struct Expression* rhs; } lessD;
        struct { struct Expression* lhs; struct Expression* rhs; } geqD;
        struct { struct Expression* lhs; struct Expression* rhs; } leqD;
        struct { struct Expression* lhs; struct Expression* rhs; } andD;
        struct { struct Expression* lhs; struct Expression* rhs; } orD;
    } val;
} Operator;

typedef struct Body {
    DeclarationList *declarationList;
    StatementList *statementList;
} Body;


Expression *makeEXPid(char *id);

Expression *makeEXPintconst(int intconst);

Expression *makeEXPtimes(Expression *left, Expression *right);

Expression *makeEXPdiv(Expression *left, Expression *right);

Expression *makeEXPplus(Expression *left, Expression *right);

Expression *makeEXPminus(Expression *left, Expression *right);

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