#ifndef PR4_H_INCLUDED
#define PR4_H_INCLUDED
#include<stdio.h>
#include<stdlib.h>
#include <string.h>
#include <limits.h>


typedef struct Cell
{
    int job[2];
    struct Cell *suivant;


}TypeCellule;

int Affichage(TypeCellule* L);
TypeCellule * Append(TypeCellule *L, int jobnumero, int cm);
TypeCellule * Supprime(TypeCellule *L,int jobnumero);
void Liveration(TypeCellule **pL);
TypeCellule *Tri (TypeCellule *L);

int * PR4(int * M, int m, int n,int x, int d);
void insert(int* joblist, int length, int p, int j);
int calculateCbarre(int * M, int m, int n, int* joblist, int jobcount);
void LR(int *M, int m, int n, TypeCellule ** U, TypeCellule** PI,int l, int d);
int * NEH(int *M, int m, int n, int* dupU, int* dupPI, int d);
int * NEH2(int *M, int m, int n, int* joblist, int jobcount);
int *VNS(int * M,int m,int n,int * PI, int nb_PI,int L_max);
void pairwise(int * PI, int * res, int nb_ele, int send);
void insertion(int * PI, int * res, int nb_ele,int send);



#endif // PR4_H_INCLUDED
