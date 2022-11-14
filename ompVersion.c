#include <stdio.h>
#include <stdlib.h>
#include <omp.h> 


typedef struct {
    int i;
    int j;
    int prof;
    int* tab;
    int* tmp;
} info_holder;

void disp(int tab[], int size) {
    if (size > 1000) {disp(tab, 100);disp(&tab[size - 101], 100);}
    else {
        int i;
        for( i = 0; i < size; i++)  {printf(" %d ", tab[i]);}
        printf("\n");
    }
}

////////////////////////////////////////::
void fusionClassique(int i, int j, int m, int tab[], int tmp[]) {
    int g = i;
    int d = m + 1;
    int c;
    for( c = i; c <= j; c++) {
        if(g == m + 1) { //le pointeur du sous-tableau de gauche a atteint la limite
            tmp[c] = tab[d];
            d++;
        }else if (d == j + 1) { //le pointeur du sous-tableau de droite a atteint la limite
            tmp[c] = tab[g];
            g++;
        }else if (tab[g] < tab[d]) { //le pointeur du sous-tableau de gauche pointe vers un élément plus petit
            tmp[c] = tab[g];
            g++;
        }else {  //le pointeur du sous-tableau de droite pointe vers un élément plus petit
            tmp[c] = tab[d];
            d++;
        }
    }    
}
//////////////////////////////////////////////
void triFusionClassique(int i, int j, int tab[], int tmp[]) {
    
    if(j <= i){ return;}
    int m = (i + j) / 2;
    
    triFusionClassique(i, m, tab, tmp);     //trier la moitié gauche récursivement
    triFusionClassique(m + 1, j, tab, tmp); //trier la moitié droite récursivement

    fusionClassique(i, j, m, tab, tmp);
// on boucle de i à j pour remplir chaque élément du tableau final fusionné
    int c;
    for(c = i; c <= j; c++) {  //copier les éléments de tmp[] à tab[]
        tab[c] = tmp[c];
    }
    
}

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
void triFusionOMP(int i, int j, int prof, int tab[], int tmp[]) {
    if (j <= i)
        return;
    long m = (i+j)/2;

    if (prof > 1){
        #pragma omp parallel sections
            {
                #pragma omp section
                    triFusionOMP(i, m, prof/2, tab, tmp);
                #pragma omp section
                    triFusionOMP(m + 1, j, prof/2, tab, tmp);
            }
        fusionClassique(i, j, m, tab, tmp);

        int c;
        #pragma omp parallel for
            for(c = i; c <= j; c++)
                tab[c] = tmp[c];
    } 
    else {
        triFusionClassique(i, j, tab, tmp);
    }
        
}
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////




int main(int argc, char* argv[]) {

    if (argc < 3) {
        printf("usage: ./d2omp inputFileName nbThread");
        return 1;
    }

    FILE* stream;
    stream = fopen(argv[1], "r");
    if (stream == NULL) {
        fprintf(stderr, "fichier introuvable");
        exit(-1);
    }
    int n;
    fscanf(stream, "%d", &n);
    
    int nbThread = atoi(argv[2]);
    int* tab = malloc(n * sizeof(int));
    int* tmp = malloc(n * sizeof(int));
    int count = 0;
    while (fscanf(stream, "%d", &tab[count]) == 1) {count++;}

    if (argc == 4) {
        double begin = omp_get_wtime();
        triFusionOMP(0, n-1, nbThread, tab, tmp);
        double end = omp_get_wtime();
        double time_spent = end - begin;
        printf("n: %d, th: %d, time: %lf\n", n, nbThread, time_spent);
    }
    else {
        triFusionOMP(0, n-1, nbThread, tab, tmp);
        disp(tab, n); 
    }


    
    return 0;

}