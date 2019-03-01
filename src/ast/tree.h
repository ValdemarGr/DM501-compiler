#ifndef __tree_h
#define __tree_h

typedef struct Body Body;
typedef struct Type Type;
typedef struct Term Term;
typedef struct Variable Variable;
typedef struct SymbolTable SymbolTable;
typedef struct TypeList TypeList;
typedef struct Expression Expression;

typedef struct VarDelList {
    char *identifier;
    Type *type;
    struct VarDelList *next;
} VarDelList;

typedef enum {
    typeIdK, typeIntK, typeBoolK, typeArrayK, typeRecordK, typeLambdaK
} TypeKind;

typedef struct Type {
    TypeKind kind;
    union {
        struct { char *id; } idType;
        struct { struct Type *type; } arrayType;
        struct { VarDelList *types; } recordType;
        struct { TypeList *typeList; Type *returnType; } typeLambdaK;
    } val;
} Type;

typedef struct TypeList {
    Type *type;
    struct TypeList *next;
} TypeList;

typedef struct FunctionHead {
    SymbolTable *symbolTable;

    char *indentifier;
    VarDelList *declarationList;
    Type *returnType;
} FunctionHead;

typedef struct FunctionTail {
    int lineno;
    char *indentifier;
} FunctionTail;

typedef struct Lambda {
    VarDelList *declarationList;
    Type *returnType;
    Body *body;
} Lambda;

typedef struct Function {
    FunctionHead *head;
    Body *body;
    FunctionTail *tail;
} Function;

typedef struct Declaration {
    SymbolTable *symbolTable;

    int lineno;
    enum { declVarK, declVarsK, declTypeK, declFuncK, declValK } kind;
    union {
        struct { char* id; Type *type; } varD;
        struct { struct Declaration *var; struct Declaration *next; } varsD;
        struct { char* id; Type *type; } typeD;
        struct { Function *function; } functionD;
        struct { char* id; Expression *rhs; Type *tpe; } valK;
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
        opK, termK//, functionK
    } kind;
    union {
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
    enum { statReturnK, statWriteK, statAllocateK, statAllocateLenK, statIfK, statIfElK, statWhileK, stmListK, assignmentK } kind;
    SymbolTable *symbolTable;
    union {
        struct { Expression* exp; } returnD;
        struct { Expression* exp; } writeD;
        struct { Variable* var; } allocateD;
        struct { Variable* var; Expression* len; } allocateLenD;
        struct { Expression* exp; struct Statement *statement; } ifD;
        struct { Expression* exp; struct Statement *statement; struct Statement *elseStatement; } ifElD;
        struct { Expression* exp; struct Statement* statement; } whileD;
        struct { struct StatementList* statementList; } stmListD;
        struct { Variable* var; Expression* exp; } assignmentD;
    } val;
} Statement;

typedef struct StatementList {
    int lineno;
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
    enum { variableK, functionCallK, parenthesesK, negateK, absK, numK, trueK, falseK, nullK, lambdaK } kind;
    union {
        struct { struct Variable *var; } variableD;
        struct { char *functionId; ExpressionList *expressionList; } functionCallD;
        struct { Expression *expression; } parenthesesD;
        struct { Term* term; } negateD;
        struct { Expression* expression; } absD;
        struct { int num; } numD;
        struct { Lambda *lambda; } lambdaD;
    } val;
} Term;

typedef struct Body {
    DeclarationList *declarationList;
    StatementList *statementList;
} Body;

TypeList *makeTypeList(TypeList* next, Type *elem);

Expression *makeEXPFromTerm(Term *term);

Variable *makeVariable(char *id);

Variable *makeArraySubscript(Variable *variable, Expression *expression);

Variable *makeRecordSubscript(Variable* variable, char *id);

Term *makeTermFromVariable(Variable *variable);

Term *makeFunctionCallTerm(char *functionId, ExpressionList *expressionList);

Term *makeParentheses(Expression *expression);

Term *makeNegatedTerm(Term *term);

Term *makeAbsTerm(Expression *expression);

Term *makeNumTerm(int n);

Term *makeTrueTerm();

Term *makeFalseTerm();

Term *makeNullTerm();

Term *makeLambdaTerm(Lambda *lambda);

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

Declaration *makeValDeclaration(char* id, Expression *rhs);

StatementList *makeStatementList(Statement *statement, StatementList *next);

Statement *makeReturnStatement(Expression *exp);

Statement *makeIfStatement(Expression *exp, Statement *statement);

Statement *makeIfElseStatement(Expression *exp, Statement *statement, Statement *elseStatement);

Statement *makeAssignment(Variable* variable, Expression *exp);

Statement *makeAllocateStatement(Variable *var);

Statement *makeAllocateOfLenStatement(Variable *var, Expression *len);

Statement *makeWriteStatement(Expression *exp);

Statement *makeWhileStatement(Expression *exp, Statement *stm);

Statement *makeStatementFromList(StatementList *statementList);

Type *makeIdType(char* id);

Type *makeIntType();

Type *makeBoolType();

Type *makeArrayType(Type *type);

Type *makeRecordType(VarDelList *record);

Type *makeLambdaType(TypeList *typeList, Type *type);

Expression *makeEXPfunction(char *identifier, Expression *body);

VarDelList *makeVarDelList(char *identifier, Type *type, VarDelList *next);

Function *makeFunction(FunctionHead *head, Body *body, FunctionTail *tail);

Lambda *makeLambda(VarDelList* varDelList, Type *returnType, Body *body);

FunctionHead *makeFunctionHead(char *identifier, VarDelList *declerationList, Type *type);

FunctionTail *makeFunctionTail(char *identifier);

Body *makeBody(DeclarationList *declarationList, StatementList *statementList);

Declaration *makeFunctionDecleration(Function *function);

#endif