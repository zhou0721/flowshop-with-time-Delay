#include "PR4.h"
#include <sys/time.h>

int * PR4(int * M , int m, int n, int x, int d)
{
    int i,l=0;
    int L_max=2*n*n;
    int Cbarre;
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

        int *res_VNS=NEH(M, m, n,dupU,dupPI,d);
        int *PI_VNS=(int *) realloc (res_VNS, n*sizeof(int));
        res_VNS=NULL;

        //VNS

        int *res = VNS( M,  m,  n, PI_VNS, n, L_max);//res et PI_VNS pointent sur un meme place.
        Cbarre = calculateCbarre(M, m,n, res, n);

        int * res_NEH;
        int Cbarre_NEH;
        for(i=0;i<2;i++)
        {
            res_NEH = NEH2(M, m, n, res, n);
            Cbarre_NEH=calculateCbarre(M, m, n, res_NEH, n);


            if (Cbarre_NEH<Cbarre)
            {
                free(res);
                res=res_NEH;
                res_NEH=NULL;
                Cbarre=Cbarre_NEH;
            }
            else
            {
                free(res_NEH);
                res_NEH=NULL;

            }
        }


        //finale
        if(Cbarre<*(Resultat+n))
        {
            memcpy(Resultat, res, (n)*sizeof(int));
            Resultat[n]=Cbarre;
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

