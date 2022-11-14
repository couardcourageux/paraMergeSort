#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>


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
    // printf(" %d -> %d\n", l, r);
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
void *triFusionTh(void* holder1) {
    info_holder* holder = (info_holder*)holder1;
    int i = holder->i;
    int j = holder->j;
    int prof = holder->prof;
    if (j <= i) {return NULL;}
    int m = (i+j)/2;

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
        int c;
        for( c = i; c <= j; c++) {holder->tab[c] = holder->tmp[c];}
    }
    else {
        // on passe en récursif classique
        triFusionClassique(i, j, holder->tab, holder->tmp);
    }

}

void triFusionHolder(int i, int j, int prof, int tab[], int tmp[]) {
    info_holder* temp = malloc(sizeof(info_holder));
    temp->i = i;
    temp->j = j;
    temp->prof = prof;
    temp->tab = tab;
    temp->tmp = tmp;  
    triFusionTh(temp); 
}



int main(int argc, char* argv[]) {

    if (argc < 3) {
        printf("usage: ./d2p inputFileName nbThread");
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
        clock_t begin = clock();
        triFusionHolder(0, n-1, nbThread, tab, tmp);
        clock_t end = clock();
        double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
        printf("n: %d, th: %d, time: %lf\n", n, nbThread, time_spent);
    }
    else {
        triFusionHolder(0, n-1, atoi(argv[2]), tab, tmp);
        disp(tab, n); 
    }


    
    return 0;

}