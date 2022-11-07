#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>

typedef struct {
    int i;
    int j;
    double prof;
    int* tab;
    int* tmp;
} info_holder;


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

////////////////////////////////////////::
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
    disp(tmp, 8); 
}
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
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
        holderSon->j = m-1;
        holderSon->prof = prof / 2;
        holderSon->tab = holder->tab;
        holderSon->tmp = holder->tmp;  


        info_holder* holderLocal = malloc(sizeof(info_holder));
        holderLocal->i = m;
        holderLocal->j = j;
        holderLocal->prof = prof / 2;
        holderLocal->tab = holder->tab;
        holderLocal->tmp = holder->tmp; 

        pthread_t son;
        if (pthread_create(&son, NULL, triFusionTh, holderSon )) {
            free(holderSon);
        }
        triFusionTh((void*)holderLocal);
        free(holderLocal);
        pthread_join(son, NULL);

        fusionClassique(i, j, m, holder->tab, holder->tmp);
    }
    else {
        // on passe en récursif classique
        triFusionClassique(i, j, holder->tab, holder->tmp);
    }

    for(int c = i; c <= j; c++) {  //copier les éléments de tmp[] à tab[]
        holder->tab[c] = holder->tmp[c];
    }
}
void triFusionHolder(int i, int j, int prof, int tab[], int tmp[]) {
    info_holder* temp = malloc(sizeof(info_holder));
    temp->i = i;
    temp->j = j;
    temp->prof = prof / 2;
    temp->tab = tab;
    temp->tmp = tmp;  
    triFusionTh(temp); 
}
//////////////////////////////////////////////////
//////////////////////////////////////////////////

void import_file(const char* filename) {
    FILE* stream;
    stream = fopen(filename, "r");
    if (stream == NULL) {
        fprintf(stderr, "fichier introuvable");
        exit(-1);
    }
    int n;
    fscanf(stream, "%d", &n);
    
    int** tab = malloc(n * sizeof(int));
    int** tmp = malloc(n * sizeof(int));
    int count = 0;
    while (fscanf(stream, "%d", &tab[count]) == 1) {count++;}
    //buf est l'array avec lequel on veut travailler, il faudra faire une fonction de callBack dessus
}




int main() {
//   int  nbr, i, tab[100], tmp[100];
 
  int nbr = 8;
//   int* tab = malloc(nbr * sizeof(int));
//   int* tmp = malloc(nbr * sizeof(int));    
//     tab[0] = 6;
//     tab[3] = 14;
//     tab[2] = 5;
//     tab[1] = 1;

    int tab[] = {8, 7, 6, 5, 4, 3, 2, 1};
    int tmp[8] = {0, 0, 0, 0, 0, 0, 0, 0};


//   triFusionClassique(0, nbr-1, tab, tmp);
//   printf("oui");
  triFusionHolder(0, nbr-1, 2, tab, tmp);
 
  printf("\n Tableau trié : ");
  disp(tab, 8); 
  return 0;

}