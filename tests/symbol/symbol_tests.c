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

    runTaks(q);

    return;

/*
    TestBlock *tb = initTestBlock(9);

    testFor(tb, "Initialization of symbol table.");

    SymbolTable *st = initSymbolTable();

    if (st == NULL) {
        evaluateTest(0, tb);
    } else {
        evaluateTest(1, tb);
    }

    testFor(tb, "Scoping the symbol table down");

    SymbolTable *scoped_table = scopeSymbolTable(st);

    if (scoped_table == NULL) {
        evaluateTest(0, tb);
    } else {
        evaluateTest(1, tb);
    }

    testFor(tb, "Adding 20 items to the newly scoped table");
    evaluateTest(2, tb);

    const int ADDS = 20;

    TestBlock *addItems = initTestBlock(ADDS);
    for (int i = 0; i < ADDS; ++i) {
        testFor(addItems, "");
        printf(" Adding name %i by value %i", i, i);

        char str[15];
        sprintf(str, "%i", i);

        putSymbol(scoped_table, str, i);
        evaluateTest(1, addItems);
    }

    testFor(tb, "Adding items to the newly scoped table");
    evaluateTest(1, tb);

    testFor(tb, "Find symbol with name 15");

    SYMBOL *sym = getSymbol(scoped_table, "15");

    if (sym == NULL) {
        evaluateTest(0, tb);
    } else if (sym->value == 15) {
        evaluateTest(1, tb);
    } else {
        evaluateTest(0, tb);
    }

    testFor(tb, "Add unique item to root");
    putSymbol(st, "900", 900);
    evaluateTest(1, tb);

    testFor(tb, "Find item in root node by looking in child node (we look up a scope)");
    sym = getSymbol(scoped_table, "900");

    if (sym == NULL) {
        evaluateTest(0, tb);
    } else if (sym->value == 900) {
        evaluateTest(1, tb);
    } else {
        evaluateTest(0, tb);
    }

    testFor(tb, "Printing table contents");
    evaluateTest(2, tb);
    dumpSymbolTable(scoped_table);
    testFor(tb, "Printing table contents");
    evaluateTest(1, tb);

    free(tb);

    return 0;*/
}