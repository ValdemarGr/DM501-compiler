//
// Created by valde on 4/29/19.
//

#include <stdlib.h>
#include <stdio.h>

int doMagicalStuff() {
    int a = 22;

    printf("%i\n", a);

    return a;
}

int main() {

    int a = 1;

    printf("%i\n", a);

    a++;

    int (*fp)() = &doMagicalStuff;

    int b = fp();

    printf("%i\n", b);

    return 0;
}