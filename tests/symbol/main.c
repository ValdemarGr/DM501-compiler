//
// Created by valde on 2/2/19.
//

#include <stdio.h>
#include "../../src/symbol/symbol.h"
#include "../test.h"

int main() {
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

    return 0;
}