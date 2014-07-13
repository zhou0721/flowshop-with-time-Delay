#include "PR4.h"
#include<time.h>
#define random(a,b) (rand()%(b-a))+a
#define true 1
#define false 0

int *VNS(int * M,int m,int n,int * PI, int nb_PI,int L_max)
{
    int l;
    int foi1=0;
    int foi2=0;
    int improved=true;
    int * PHI = (int *)malloc(nb_PI*sizeof(int));
    int C_barre_PI = calculateCbarre(M,  m, n, PI, nb_PI);

    for(l=1;l<=L_max;l++)
    {
        improved=true;
        while(improved==true)
        {
            memcpy(PHI, PI, (nb_PI)*sizeof(int));//on met PHI egale PI
            if(improved==true)
            {
                // pairwise
                pairwise(PI,PHI, nb_PI,foi1);
                foi1++;

            }
            else
            {
               insertion(PI,PHI, nb_PI, foi2);
               foi2++;
            }

            int C_barre_PHI  = calculateCbarre(M,  m, n, PHI, nb_PI);

            if(C_barre_PHI<=C_barre_PI)
            {
                memcpy(PI, PHI, (nb_PI)*sizeof(int));
                C_barre_PI=C_barre_PHI;
            }
            else improved=false;



        }
    }
    free(PHI);
    return PI;
}


// pairwise
void pairwise(int * PI, int * res, int nb_ele,int send)
{
    memcpy(res, PI, (nb_ele)*sizeof(int));
    srand(send);//*(int)time(0));
    int i=random(0,nb_ele);
    int j=random(0,nb_ele);
    res[i]=res[j];
    res[j]=PI[i];


}

 // insertion

 void insertion(int * PI, int * res, int nb_ele,int send)
 {

     //memcpy(res, PI, (nb_ele)*sizeof(int));
     srand(send);//*(int)time(0));
     int i=random(0,nb_ele);
     int j=random(0,nb_ele);
     while(j==i)
        j=random(0,nb_ele);
     res[j]=PI[i];
     int k=0;
     int l=0;
     while(k<nb_ele)
     {
         if(l==i)l++;
         if(k==j)k++;
         res[k]=PI[l];
         l++;
         k++;

    }
}


