//
// Created by valde on 5/1/19.
//

#ifndef HELLO_CPLAYGROUND_C_H
#define HELLO_CPLAYGROUND_C_H

#include <stdio.h>

int test(int a, int b, int c, int d, int e, int f, int g, int h, int i, int j, int k, int l, int m, int n, int o, int p,
        int q, int r, int s, int t ,int u, int v, int x, int y, int z) {
    return a+ b+ c+ d+ e+ f+ g+ h+ i+ j+ k+ l+ m+ n+ o+ p, +q+ r+ s+ t+ u+ v+ x+ y+ z;
}

int main() {
    int a = 22;

    printf("%i\n", a);

    return test(1,2,3,4,5,6,7,8,9,10,1,2,3,4,5,6,7,8,9,10,1,2,3,4, 45);
}


#endif //HELLO_CPLAYGROUND_C_H
