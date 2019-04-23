#include <stdio.h>
#include <stdlib.h>

int *l[5] = {1,2,3,4,5};
int o[5] = {1,2,3,4,5};

int main()  {
    l[0] = NULL;
    l[1] = NULL;
    l[2] = NULL;
    l[3] = NULL;
    l[4] = NULL;
    l[2] = NULL;

    o[0] = 5;
    o[1] = 7;

    printf("%ii\n", o[0]);
    printf("%ii\n", o[1]);
    printf("%ii\n", o[2]);
    printf("%ii\n", o[3]);
    printf("%ii\n", o[4]);

    return 0;
}
