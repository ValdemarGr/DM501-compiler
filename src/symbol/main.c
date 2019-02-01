//
// Created by valde on 2/1/19.
//

#include <stdio.h>
#include "symbol.h"

int main() {
    printf("Hello world!");
    char* a = 0;

    int o = Hash("kitty");

    printf("The value for the string is %i", o);

    SymbolTable *t = initSymbolTable();

    return 0;
}