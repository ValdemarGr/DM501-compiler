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

/* SYMBOL will be extended later.
   Function calls will take more parameters later.
*/

typedef struct Value {
    enum { typeK, typeFunctionK, symTypeClassK } kind;
    union {
        struct { struct Type *tpe; bool isTypedef; bool lhsIsConst; } typeD;
        struct { struct VarDelList *tpe; struct Type *returnType; } typeFunctionD;
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
SYMBOL *putSymbol(SymbolTable *t, char *name, struct Value *value, int symbol_stmDeclNum);

SYMBOL *getSymbol(SymbolTable *t, char *name);

void dumpSymbolTable(SymbolTable *t);

#endif
