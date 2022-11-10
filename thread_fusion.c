#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "omp.h"


typedef struct {
    long i;
    long j;
    int prof;     //nb threads
    long* tab;
    long* tmp;
} info_holder;

void disp(long tab[], long size) {
    long i;
    if (size > 1000) {disp(tab, 100);disp(&tab[size - 101], 100);}
    else {
        for(i = 0; i < size; i++)  {printf(" %d ", tab[i]);}
        printf("\n");
    }
}
void swap(int* a, int* b) {
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

int dycho(int x, int tab[], int p, int r){
    int m = (p-r)/2;
    if (tab[m] == x) {return m;}
    if (tab[m] < x) {
        return dycho(x, tab, m+1, r);
    }
    return dycho(x, tab, p, m-1);
}

void afficher_tableau_entiers ( int* tableau, int taille ) {
    int i;
    if( tableau != NULL && taille > 0) {
        for(i = 0 ; i < taille ; i++) {
            printf ("%d ", tableau [i]);
        }
    }
    printf ("\n");
}

////////////////////////////////////////::
void fusionClassique(long i, long j, long m, long tab[], long tmp[]) {
    long g = i;
    long d = m + 1;
    // printf(" %d -> %d\n", l, r);
    long c;
    for(c = i; c <= j; c++) {
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
void triFusionClassique(long i, long j, long tab[], long tmp[]) {

    if(j <= i){ return;}
    long m = (i + j) / 2;

    triFusionClassique(i, m, tab, tmp);     //trier la moitié gauche récursivement
    triFusionClassique(m + 1, j, tab, tmp); //trier la moitié droite récursivement

    fusionClassique(i, j, m, tab, tmp);
    long c;
// on boucle de i à j pour remplir chaque élément du tableau final fusionné
    for(c = i; c <= j; c++) {  //copier les éléments de tmp[] à tab[]
        tab[c] = tmp[c];
    }

}

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
void *triFusionTh(void* holder1) {
    info_holder* holder = (info_holder*)holder1;
    long i = holder->i;
    long j = holder->j;
    int prof = holder->prof;
    if (j <= i) {return NULL;}
    long m = (i+j)/2;

    if (prof > 1) {
        // on continue à paralléliser
        info_holder* holderSon = malloc(sizeof(info_holder));
        holderSon->i = i;
        holderSon->j = m;
        holderSon->prof = prof / 2;
        holderSon->tab = holder->tab;
        holderSon->tmp = holder->tmp;

        info_holder* holderLocal = malloc(sizeof(info_holder));
        holderLocal->i = m+1;
        holderLocal->j = j;
        holderLocal->prof = prof / 2;
        holderLocal->tab = holder->tab;
        holderLocal->tmp = holder->tmp;

        pthread_t son;
        if (pthread_create(&son, NULL, triFusionTh, holderSon )) {free(holderSon);}
        triFusionTh((void*)holderLocal);
        free(holderLocal);
        pthread_join(son, NULL);
        fusionClassique(i, j, m, holder->tab, holder->tmp);
        long c;
        for(c = i; c <= j; c++) {holder->tab[c] = holder->tmp[c];}
    }
    else {
        // on passe en récursif classique
        triFusionClassique(i, j, holder->tab, holder->tmp);
    }

}

void *triFusionOMP(long i, long j, int prof, long tab[], long tmp[]) {
    if (j <= i)
        return NULL;
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
        long c;
        for(c = i; c <= j; c++)
            tab[c] = tmp[c];
    } else
        triFusionClassique(i, j, tab, tmp);
}



void triFusionHolder(long i, long j, int prof, long tab[], long tmp[]) {
    info_holder* temp = malloc(sizeof(info_holder));
    temp->i = i;
    temp->j = j;
    temp->prof = prof;
    temp->tab = tab;
    temp->tmp = tmp;
    triFusionTh(temp);
}

void ptrheads(){
    /*if (argc < 3) {
        printf("usage: ./thread_fusion inputFileName nbThread");
        return 1;
    }

    FILE* stream;
    stream = fopen(argv[1], "r");
    if (stream == NULL) {
        fprintf(stderr, "fichier introuvable");
        exit(-1);
    }
    int n;
    fscanf(stream, "%d", &n);*/
    long n = 50000000;
    int nbThread = 48;
    long* tab = malloc(n * sizeof(long));
    long* tmp = malloc(n * sizeof(long));
    long i;

    srand( (long)time( NULL ) );

    for ( i = 0; i < n; ++i) {
        tab[i] = rand();
        //printf( "r[%d] = %d\n", i, r[i]);
    }
    /*int count = 0;
    while (fscanf(stream, "%d", &tab[count]) == 1) {count++;}*/

    //if (argc == 4) {
    double begin = omp_get_wtime();
    triFusionHolder(0, n-1, nbThread, tab, tmp);
    double end = omp_get_wtime();
    double time_spent = end - begin;
    printf("n: %d, th: %d, time: %lf\n", n, nbThread, time_spent);
    //}
    /*else {
        triFusionHolder(0, n-1, atoi(argv[2]), tab, tmp);

        disp(tab, n);
    }*/
}

void omp(){
    long n = 50000000;
    int nbThread = 2;
    long* tab = malloc(n * sizeof(long));
    long* tmp = malloc(n * sizeof(long));
    long i;

    srand( (long)time( NULL ) );

    for ( i = 0; i < n; ++i) {
        tab[i] = rand();
        //printf( "r[%d] = %d\n", i, r[i]);
    }
    //disp(tab, n);
    double begin = omp_get_wtime();
    triFusionOMP(0, n-1, nbThread, tab, tmp);
    double end = omp_get_wtime();
    double time_spent = end - begin;
    printf("n: %d, th: %d, time: %lf\n", n, nbThread, time_spent);
    //disp(tab, n);
}

int main(int argc, char* argv[]) {
    //ptrheads();
    omp();
    return 0;

}