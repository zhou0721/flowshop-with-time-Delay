#include "PR2.h"
#include <sys/time.h>

int * PR2(int * M , int m, int n, int x, int d)
{
    int i,l=0;
    int L_max=2*n*n;
    int * Resultat =(int*)malloc((n+1)*sizeof(int));
    *(Resultat+n)=INT_MAX;
    TypeCellule * U=NULL;    //U est une liste pour les jobs dont l'ordre est pas encore déterminé
    TypeCellule * PI=NULL;    //PI est une liste pour les jobs dont l'ordre est déterminé
    TypeCellule * p;
    struct timeval tpstart,tpend;
    float timeuse=0.0;
    gettimeofday(&tpstart,NULL);

    while(l<x&&timeuse<=0.01)
    {
        //LR
        LR(M, m, n, &U, &PI, l,d);
        //NEH
        int * dupU = (int*)calloc((n-d), sizeof(int));
        int * dupPI = (int*)calloc(d, sizeof(int));

if(d==n)
{
    dupU=NULL;

}
else
{
    p=U;
    for(i=0;i<(n-d-1);i++)
    {
        *(dupU+i)= p->job[0];
        p=p->suivant;
    }
    *(dupU+i)=p->job[0];
}

    p=PI;
    for(i=0;i<(d-1);i++)
    {
        *(dupPI+i)=p->job[0];
        p=p->suivant;
    }
    *(dupPI+i)=p->job[0];

    int *res = NEH(M, m, n,dupU,dupPI,d);
    int *PI_VNS=(int *) realloc (res, n*sizeof(int));

//RZ

res = VNS(M,m,n,PI_VNS, n,L_max);

int Cbarre_VNS=calculateCbarre(M, m,n, res, n);


     if(Cbarre_VNS<*(Resultat+n))
     {
        memcpy(Resultat, res, (n)*sizeof(int));
        Resultat[n]=Cbarre_VNS;

     }
     Liveration(&U);
     Liveration(&PI);
     PI=NULL;
     U=NULL;
     free(res);
     free(dupU);
     free(dupPI);
     gettimeofday(&tpend,NULL);
     timeuse=(tpend.tv_sec-tpstart.tv_sec)+
    (tpend.tv_usec-tpstart.tv_usec)/1000000;
    //printf("Used Time:%f s\n",timeuse);
    l++;
}

//printf("\nle processus execute %d iterations:" ,l);
return Resultat;
}

