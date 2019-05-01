#ifndef HELLO_SYMBOL_H
#define HELLO_SYMBOL_H

#define HashSize 317

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#if !__APPLE__
#include <malloc.h>
#endif

#include "../utils/memory.h"
#include <stdbool.h>
#include "../utils/enum_to_string.h"

extern size_t maxDistFromRoot;

typedef struct Value {
    enum { typeK, typeFunctionK, symTypeClassK } kind;
    union {
        struct { struct Type *tpe; bool isTypedef; } typeD;
        struct { struct VarDelList *tpe; struct Type *returnType; bool isLambda; int lambdaId; } typeFunctionD;
        struct {
            struct DeclarationList *declarationList;
            struct TypeList *extendedClasses;
            struct TypeList *generics; } typeClassD;
    } val;
} Value;

typedef struct SYMBOL {
    char *name;
    size_t distanceFromRoot;
    size_t uniqueIdForScope;
    bool isConst;
    int symbol_stmDeclNum;
    Value *value;
    struct SYMBOL *next;
} SYMBOL;

typedef struct SymbolTable {
    size_t distanceFromRoot;
    size_t nextSymbolId;
    SYMBOL *table[HashSize];
    struct SymbolTable *next;
} SymbolTable;

int Hash(char *str);

SymbolTable *initSymbolTable();

SymbolTable *scopeSymbolTable(SymbolTable *t);

//SYMBOL *putSymbol(SymbolTable *t, char *name, int value);
SYMBOL *putSymbol(SymbolTable *t, char *name, struct Value *value, int symbol_stmDeclNum, bool isConst);

SYMBOL *getSymbol(SymbolTable *t, char *name);

void dumpSymbolTable(SymbolTable *t);

#endif
