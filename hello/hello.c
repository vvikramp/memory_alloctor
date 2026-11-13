#include <stdio.h>

int square(int x) { 
    return x * x; 
}

int main(void) {
    printf("Hello, C!\n");
    for (int i = 1; i <= 5; i++) {
        printf("square(%d) = %d\n", i, square(i));
    }
    return 0;
}
