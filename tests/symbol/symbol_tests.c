//
// Created by valde on 2/2/19.
//

#include <stdio.h>
#include "../../src/symbol/symbol.h"
#include "../test.h"
#include "../queue.h"

int addItemsTest(void *arg) {
    SymbolTable *scoped_table = (SymbolTable*)arg;
    int items = 50;

    for (int i = 0; i < items; ++i) {
        printf("Adding name %i by value %i\n", i, i);

        char str[15];
        sprintf(str, "%i", i);

        putSymbol(scoped_table, str, i);
    }

    return 1;
}

int getSymTest(void *arg) {
    SymbolTable *scoped_table = (SymbolTable*)arg;
    SYMBOL *sym = getSymbol(scoped_table, "15");

    if (sym == NULL) {
        return 0;
    } else if (sym->value == 15) {
        return 1;
    } else {
        return 0;
    }
}

int putRootTest(void *arg) {
    SymbolTable *st = (SymbolTable*)arg;

    putSymbol(st, "900", 900);

    return 1;
}

int getRootFromChildTest(void *arg) {
    SymbolTable *scoped_table = (SymbolTable*)arg;
    SYMBOL *sym = getSymbol(scoped_table, "900");

    if (sym == NULL) {
        return 0;
    } else if (sym->value == 900) {
        return 1;
    } else {
        return 0;
    }
}

int putCollision(void *arg) {
    //We put an item into arg
    SymbolTable *table = (SymbolTable*)arg;

    char *name = "3432";
    SYMBOL *s1 = putSymbol(table, name, 3432);
    SYMBOL *s2 = putSymbol(table, name, 54);

    if (s2 == NULL) {
        return 1;
    } else {
        return 0;
    }
}

int dumpTest(void *arg) {
    SymbolTable *scoped_table = (SymbolTable*)arg;
    dumpSymbolTable(scoped_table);
    return 1;
}

void symbol_tests() {
    Queue *q = initQueue();
    SymbolTable *st = initSymbolTable();
    SymbolTable *scopedTable = scopeSymbolTable(st);

    appendTask(q, addItemsTest, "Adding items to the scoped table", (void*)scopedTable);
    appendTask(q, getSymTest, "Getting one of the added items from the scoped table", (void*)scopedTable);
    appendTask(q, putRootTest, "Putting item into root node", (void*)st);
    appendTask(q, getRootFromChildTest, "Finding the newly root item using the scoped table", (void*)scopedTable);
    appendTask(q, dumpTest, "Dumping the table", (void*)scopedTable);
    appendTask(q, putCollision, "Checking put collision", (void*)st);

    runTaks(q);

    return;
}