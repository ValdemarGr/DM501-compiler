#include <stdio.h>
#include <stdlib.h>

int f(int a, int b, int c, int d, int e, int x1, int x2, int x3) {
//    return (a+b) * (c+d) / e + (x1 - x2) * x3 * a * b * c * x1;
	return (a + 5) + (b + 5) * (c + 3) + (d + 1) * (e - 19) + (x1 + 9) * (x2 - 2) + (x3 - 4);
}


int main()  {
    f(1,2,3,4,5, 6, 7, 8);

    return 0;
}
