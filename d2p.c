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
        for(i = 0; i < size; i++)  {printf(" %ld ", tab[i]);}
        printf("\n");
    }
}


void afficher_tableau_entiers ( int* tableau, int taille ) {
    int i;
    if( tableau != NULL && taille > 0) {
        for(i = 0 ; i < taille ; i++) {
            printf ("%ld ", tableau [i]);
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



int main(int argc, char* argv[]) {

    int NB_THREAD = 8;
    int n;
    scanf("%d", &n);
    
    int* tab = malloc(n * sizeof(int));
    int* tmp = malloc(n * sizeof(int));
    int count = 0;

    for (count = 0; count < n; count++) {
        scanf("%d", &tab[count]);
    }

    
    triFusionHolder(0, n-1, NB_THREAD, tab, tmp);
    disp(tab, n); 
    
    return 0;

}