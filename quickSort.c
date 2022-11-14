#include <stdio.h>
#include <stdlib.h>
#include <omp.h> 


// function to swap elements
void swap(int *a, int *b) {
  int t = *a;
  *a = *b;
  *b = t;
}
// function to find the partition position
int partition(int array[], int low, int high) {
  int pivot = array[high];
  int i = (low - 1);
  int j;
  for ( j = low; j < high; j++) {
    if (array[j] <= pivot) {
      i++;
      swap(&array[i], &array[j]);
    }
  }
  swap(&array[i + 1], &array[high]);
  return (i + 1);
}

void quickSort(int array[], int low, int high) {
  if (low < high) {
    int pi = partition(array, low, high);
    quickSort(array, low, pi - 1);
    quickSort(array, pi + 1, high);
  }
}


// main function
int main(int argc, char* argv[]) {
  
    FILE* stream;
    stream = fopen(argv[1], "r");
    if (stream == NULL) {
        fprintf(stderr, "fichier introuvable");
        exit(-1);
    }
    int n;
    fscanf(stream, "%d", &n);

    int* tab = malloc(n * sizeof(int));
    int count = 0;
    while (fscanf(stream, "%d", &tab[count]) == 1) {count++;}

    double begin = omp_get_wtime();
    quickSort(tab, 0, n - 1);
    double end = omp_get_wtime();
    double time_spent = end - begin;
    printf("n: %d, time: %lf\n", n, time_spent);

    return 0;
}