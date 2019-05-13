#ifndef __tree_h
#define __tree_h

#include "../scan_parse/parser_extra.h"
#include "stdbool.h"

typedef struct Body Body;
typedef struct Type Type;
typedef struct Term Term;
typedef struct Variable Variable;
typedef struct SymbolTable SymbolTable;
typedef struct TypeList TypeList;
typedef struct Expression Expression;

typedef YYLTYPE Location;

typedef struct VarDelList {
    char *identifier;
    Type *type;
    struct VarDelList *next;
    Location location;
} VarDelList;

typedef enum {
    typeIdK, typeIntK, typeBoolK, typeArrayK, typeRecordK, typeLambdaK, typeClassK, typeGenericK, typeVoidK
} TypeKind;

typedef struct Type {
    TypeKind kind;
    union {
        struct {
            char *id;
        } idType;
        struct {
            struct Type *type;
        } arrayType;
        struct {
            VarDelList *types;
        } recordType;
        struct {
            TypeList *typeList;
            Type *returnType;
            int lambdaId;
        } typeLambdaK;
        struct {
            char *classId;
            TypeList *genericBoundValues;
        } typeClass;
        struct {
            char *genericName;
            char *subType;
            int typeIndex;
        } typeGeneric;
    } val;
    Location location;
} Type;

typedef struct TypeList {
    Type *type;
    struct TypeList *next;
    Location location;
} TypeList;

typedef struct FunctionHead {
    SymbolTable *symbolTable;

    char *indentifier;
    VarDelList *declarationList;
    Type *returnType;
    int lineno;
    Location location;
} FunctionHead;

typedef struct FunctionTail {
    int lineno;
    char *indentifier;
    Location location;
} FunctionTail;

typedef struct Lambda {
    VarDelList *declarationList;
    Type *returnType;
    Body *body;
    int id;
    bool inClassContext;
    Location location;
} Lambda;

typedef struct Function {
    FunctionHead *head;
    Body *body;
    FunctionTail *tail;
    Location location;
} Function;

typedef struct Constructor {
    VarDelList *declarationList;
    struct Body *body;
    Location location;
} Constructor;

typedef struct Declaration {
    SymbolTable *symbolTable;
    int internal_stmDeclNum;

    int lineno;
    enum {
        declVarK, declVarsK, declTypeK, declFuncK, declValK, declClassK, nodecl
    } kind;
    union {
        struct {
            char *id;
            Type *type;
        } varD;
        struct {
            struct Declaration *var;
            struct Declaration *next;
        } varsD;
        struct {
            char *id;
            Type *type;
        } typeD;
        struct {
            Function *function;
        } functionD;
        struct {
            char *id;
            Expression *rhs;
            Type *tpe;
        } valD;
        struct {
            char *id;
            struct DeclarationList *declarationList;
            struct TypeList *genericTypeParameters;
            struct TypeList *extendedClasses;
            struct Constructor *constructor;
        } classD;
    } val;
    Location location;
} Declaration;

typedef struct DeclarationList {
    int lineno;
    Declaration *declaration;
    struct DeclarationList *next;
    Location location;
} DeclarationList;

typedef struct Expression {
    int lineno;
    Location location;
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
    int internal_stmDeclNum;

    enum {
        statReturnK,
        statWriteK,
        statAllocateK,
        statAllocateLenK,
        statIfK,
        statIfElK,
        statWhileK,
        stmListK,
        assignmentK,
        emptyK,
        gcK,
        noop
    } kind;
    SymbolTable *symbolTable;
    union {
        struct {
            Expression *exp;
        } returnD;
        struct {
            Expression *exp;
        } writeD;
        struct {
            Variable *var;
            struct ExpressionList *constructorList;
        } allocateD;
        struct {
            Variable *var;
            Expression *len;
        } allocateLenD;
        struct {
            Expression *exp;
            struct Statement *statement;
        } ifD;
        struct {
            Expression *exp;
            struct Statement *statement;
            struct Statement *elseStatement;
        } ifElD;
        struct {
            Expression *exp;
            struct Statement *statement;
        } whileD;
        struct {
            struct StatementList *statementList;
        } stmListD;
        struct {
            Variable *var;
            Expression *exp;
        } assignmentD;
        struct {
            Expression *exp;
        } empty;
    } val;
    Location location;
} Statement;

typedef struct StatementList {
    int lineno;
    Statement *statement;
    struct StatementList *next;
    Location location;
} StatementList;

typedef struct Operator {
    int lineno;
    enum {
        opMultK,
        opDivK,
        opPlusK,
        opMinusK,
        opEqualityK,
        opInequalityK,
        opGreaterK,
        opLessK,
        opGeqK,
        opLeqK,
        opAndK,
        opOrK
    } kind;
} Operator;

typedef struct ExpressionList {
    int lineno;
    Location location;
    Expression *expression;
    struct ExpressionList *next;
} ExpressionList;

typedef struct Variable {
    int lineno;
    Location location;
    enum {
        varIdK, arrayIndexK, recordLookupK
    } kind;
    union {
        struct {
            char *id;
        } idD;
        struct {
            struct Variable *var;
            struct Expression *idx;
        } arrayIndexD;
        struct {
            struct Variable *var;
            char *id;
        } recordLookupD;
    } val;
} Variable;

typedef enum {
    variableK,
    functionCallK,
    parenthesesK,
    negateK,
    absK,
    numK,
    trueK,
    falseK,
    nullK,
    lambdaK,
    classDowncastk,
    shorthandCallK
} TermKind;


typedef struct Term {
    int lineno;
    Location location;
    TermKind kind;
    union {
        struct {
            struct Variable *var;
        } variableD;
        struct {
            char *functionId;
            ExpressionList *expressionList;
        } functionCallD;
        struct {
            Expression *expression;
        } parenthesesD;
        struct {
            Term *term;
        } negateD;
        struct {
            Expression *expression;
        } absD;
        struct {
            int num;
        } numD;
        struct {
            Lambda *lambda;
        } lambdaD;
        struct {
            Variable *var;
            Type *toCastTo;
        } classDowncastD;
        struct {
            struct Variable *var;
            ExpressionList *expressionList;
        } shorthandCallD;
    } val;
} Term;

typedef struct Body {
    DeclarationList *declarationList;
    StatementList *statementList;
    Location location;
} Body;


typedef struct VarType {
    char *identifier;
    Type *type;
    Location location;
} VarType;

TypeList *makeTypeList(TypeList *next, Type *elem, Location location);

TypeList *makeGenericTypeList(TypeList *next, char *id, char *subType, Location location);

Expression *makeEXPFromTerm(Term *term, Location location);

Variable *makeVariable(char *id, Location location);

Variable *makeArraySubscript(Variable *variable, Expression *expression, Location location);

Variable *makeRecordSubscript(Variable *variable, char *id, Location location);

Term *makeTermFromVariable(Variable *variable, Location location);

Term *makeFunctionCallTerm(char *functionId, ExpressionList *expressionList, Location location);

Statement *makeAllocateWithConstructorStatement(Variable *var, ExpressionList *expressionList, Location location);

Term *makeParentheses(Expression *expression, Location location);

Term *makeNegatedTerm(Term *term, Location location);

Term *makeAbsTerm(Expression *expression, Location location);

Term *makeNumTerm(int n, Location location);

Term *makeTrueTerm(Location location);

Term *makeFalseTerm(Location location);

Term *makeNullTerm(Location location);

Term *makeLambdaTerm(Lambda *lambda, Location location);

Term *makeDowncastTerm(Variable *var, Type *toCastTo, Location location);

Term *makeShorthandLambdaCall(Variable *access, ExpressionList *expressionList, Location location);

Expression *makeEXPOpEXP(Expression *lhs, Operator *op, Expression *rhs, Location location);

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

ExpressionList *makeExpList(Expression *exp, ExpressionList *next, Location location);

Constructor *makeClassConstructor(VarDelList *vdl, Body *body, Location location);

DeclarationList *makeDeclarationList(Declaration *declaration, DeclarationList *next, Location location);

Declaration *makeDeclaration(char *id, Type *type, Location location);

Declaration *makeTypeDeclaration(char *id, Type *type, Location location);

Declaration *makeVarDeclarations(VarDelList *vars, Location location);

Declaration *makeValDeclaration(char *id, Expression *rhs, Location location);

Declaration *
makeClassDeclaration(char *id, DeclarationList *declarationList, TypeList *typeList, TypeList *extensionList,
                     Constructor *constructor, Location location);

StatementList *makeStatementList(Statement *statement, StatementList *next, Location location);

Statement *makeReturnStatement(Expression *exp, Location location);

Statement *makeIfStatement(Expression *exp, Statement *statement, Location location);

Statement *makeIfElseStatement(Expression *exp, Statement *statement, Statement *elseStatement, Location location);

Statement *makeAssignment(Variable *variable, Expression *exp, Location location);

Statement *makeAllocateStatement(Variable *var, Location location);

Statement *makeAllocateOfLenStatement(Variable *var, Expression *len, Location location);

Statement *makeWriteStatement(Expression *exp, Location location);

Statement *makeWhileStatement(Expression *exp, Statement *stm, Location location);

Statement *makeStatementFromList(StatementList *statementList, Location location);

Statement *makeEmptyExpression(Expression *expression, Location location);

Statement *makeGCStatement(Location location);

Type *makeIdType(char *id, Location location);

Type *makeClassType(char *id, TypeList *genericBoundTypes, Location location);

Type *makeIntType(Location location);

Type *makeBoolType(Location location);

Type *makeArrayType(Type *type, Location location);

Type *makeRecordType(VarDelList *record, Location location);

Type *makeLambdaType(TypeList *typeList, Type *type, Location location);

Type *makeVoidType(Location location);

Expression *makeEXPfunction(char *identifier, Expression *body, Location location);

VarDelList *makeVarDelList(VarType *varType, VarDelList *next, Location location);

Function *makeFunction(FunctionHead *head, Body *body, FunctionTail *tail, Location location);

Lambda *makeLambda(VarDelList *varDelList, Type *returnType, Body *body, Location location);

FunctionHead *makeFunctionHead(char *identifier, VarDelList *declerationList, Type *type, Location location);

FunctionTail *makeFunctionTail(char *identifier, Location location);

Body *makeBody(DeclarationList *declarationList, StatementList *statementList, Location location);

Declaration *makeFunctionDecleration(Function *function, Location location);

TypeList *makeExtensionList(TypeList *next, char *class, TypeList *boundTypes, Location location);

VarType *makeVarType(char *id, Type *type, Location location);

#endif