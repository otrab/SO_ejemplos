#include <stdio.h>
#include <stdlib.h>

//La función malloc en C se utiliza para asignar memoria dinámica. 
//El argumento de malloc especifica el número de bytes que deseas asignar. 
//En el caso de malloc(100e6), estás intentando asignar 100 millones de bytes (aproximadamente 100 megabytes) de memoria.

int main(int argc, char *argv[]) {
    printf("location of code : %p\n", main);
    printf("location of heap : %p\n", malloc(100e6));
    int x = 3;
    printf("location of stack: %p\n", &x);
    return x;
}