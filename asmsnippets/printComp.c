//
// Created by valde on 4/29/19.
//

#include <stdlib.h>
#include <stdio.h>

int doMagicalStuff() {
    int a = 1;
    int b = 1;
    int c = 1;
    int d = 1;
    int e = 1;
    int f = 1;

    printf("%i", a);
    printf("%i", b);
    printf("%i", c);
    printf("%i", d);
    printf("%i", e);
    printf("%i", f);

    return a + b + c + d + e + f;
}

int main() {

    int a = 1;
    int b = 1;
    int c = 1;
    int d = 1;
    int e = 1;
    int f = 1;

    printf("%i", a);
    printf("%i", b);
    printf("%i", c);
    printf("%i", d);
    printf("%i", e);
    printf("%i", f);

    a++;
    b = 5 + c;

    printf("%i", b);

    return 0;
}