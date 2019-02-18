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
        idK, intconstK, timesK, divK, plusK, minusK, functionK
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

typedef struct Body {
    DeclarationList *declarationList;
} Body;

EXP *makeEXPid(char *id);

EXP *makeEXPintconst(int intconst);

EXP *makeEXPtimes(EXP *left, EXP *right);

EXP *makeEXPdiv(EXP *left, EXP *right);

EXP *makeEXPplus(EXP *left, EXP *right);

EXP *makeEXPminus(EXP *left, EXP *right);

DeclarationList *makeDeclarationList(Declaration *declaration, DeclarationList *next);

Declaration *makeDeclaration(char* id, Type* type);

Declaration *makeTypeDeclaration(char* id, Type* type);

Declaration *makeVarDeclarations(VarDelList* vars);

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

Body *makeBody(DeclarationList *declarationList);

Declaration *makeFunctionDecleration(Function *function);

#endif