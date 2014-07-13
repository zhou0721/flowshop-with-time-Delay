#ifndef PR1_H_INCLUDED
#define PR1_H_INCLUDED
#include<stdio.h>
#include<stdlib.h>
#include <string.h>
#include <limits.h>



int * PR1(int * M, int m, int n,int x, int d);
void insert(int* joblist, int length, int p, int j);
int calculateCbarre(int * M, int m, int n, int* joblist, int jobcount);
int * RZ(int *M, int m, int n,int *PI);
int * NEH(int *M, int m, int n, int* dupU, int* dupPI, int d);



#endif // PR1_H_INCLUDED
