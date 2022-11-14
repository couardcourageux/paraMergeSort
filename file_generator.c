#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h> 



int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("usage: ./generator n ");
    }

    int n = atoi(argv[1]);
    char* fileName = argv[2];
    int* tab = malloc(sizeof(int) * n);

    srand(time(NULL));
    int c;

    #pragma omp parallel for
        for (c = 0; c < n; c++) {
            tab[c] = rand();
        } 

    printf("%d ", n);
    for (int i = 0; i < n; i++) {
        printf("%d ", tab[i]);
    }
    printf("\n");


    return 0;
}