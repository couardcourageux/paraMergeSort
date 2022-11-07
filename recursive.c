#include <stdio.h>
#include <stdlib.h>


void disp(int tab[], int size) {
    if (size > 1000) {
        disp(tab, 100);
        disp(&tab[size - 101], 100);
    }
    else {
    for(int i = 0; i < size; i++)  {
        printf(" %d ", tab[i]);
        }
    printf("\n");
    }
}

void fusionClassique(int i, int j, int m, int tab[], int tmp[]) {
    int g = i;
    int d = m + 1;
    // printf(" %d -> %d\n", l, r);
    for(int c = i; c <= j; c++) {
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

void triFusionClassique(int i, int j, int tab[], int tmp[]) {
    
    if(j <= i){ return;}
    int m = (i + j) / 2;
    
    triFusionClassique(i, m, tab, tmp);     //trier la moitié gauche récursivement
    triFusionClassique(m + 1, j, tab, tmp); //trier la moitié droite récursivement

    fusionClassique(i, j, m, tab, tmp);
// on boucle de i à j pour remplir chaque élément du tableau final fusionné
    for(int c = i; c <= j; c++) {  //copier les éléments de tmp[] à tab[]
        tab[c] = tmp[c];
    }
    
}


int main(int argc, char* argv[]) {

    if (argc != 2) {
        printf("usage: ./recursive inputFileName");
        return 1;
    }

    // char* filename = "yolo.txt"; 
    FILE* stream;
    stream = fopen(argv[1], "r");
    if (stream == NULL) {
        fprintf(stderr, "fichier introuvable");
        exit(-1);
    }
    int n;
    fscanf(stream, "%d", &n);
    
    int* tab = malloc(n * sizeof(int));
    int* tmp = malloc(n * sizeof(int));
    int count = 0;
    while (fscanf(stream, "%d", &tab[count]) == 1) {count++;}


    triFusionClassique(0, n-1, tab, tmp);
    disp(tab, n); 
    return 0;

}