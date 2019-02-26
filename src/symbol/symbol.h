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

/* SYMBOL will be extended later.
   Function calls will take more parameters later.
*/

typedef struct SYMBOL {
  char *name;
  struct Type *tpe;
  bool isFunc;
  struct SYMBOL *next;
} SYMBOL;

typedef struct SymbolTable {
    SYMBOL *table[HashSize];
    struct SymbolTable *next;
} SymbolTable;

int Hash(char *str);

SymbolTable *initSymbolTable();

SymbolTable *scopeSymbolTable(SymbolTable *t);

//SYMBOL *putSymbol(SymbolTable *t, char *name, int value);
SYMBOL *putSymbol(SymbolTable *t, char *name, struct Type *tpe, bool isFunc);

SYMBOL *getSymbol(SymbolTable *t, char *name);

void dumpSymbolTable(SymbolTable *t);

#endif
