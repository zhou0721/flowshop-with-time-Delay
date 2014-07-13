#include "PR1.h"
int * RZ(int *M, int m, int n, int* PI)
{
        int * PI_S = (int*)calloc(n, sizeof(int));
        int * PI_2 = (int*)calloc(n, sizeof(int));
        int i = 0,j=0,k=0,l=0,origPosi;
        int tem1,tem2;
        int Cbarre=INT_MAX;

        for(i=0; i<n; i++) //copier le contenu de la liste PI dans PI_S et PI_2
        {
           PI_S[i]=PI[i];
           PI_2[i]=PI[i];
        }
        int* PI_temp = (int*)calloc(n, sizeof(int));
        for(i=0;i<n;i++)
        {
            //chercher PI_S[i] dans PI_2
            for(j=0;j<n;j++)
            {
                if(PI_2[j]==PI_S[i])
                {
                    origPosi=j;
                    if(j==n-1);
                    else
                    {
                        //supprimer PI_S[i] dans PI_2
                        for(k=j+1;k<n;k++)
                        {
                            PI_2[k-1]=PI_2[k];
                        }

                    }
                    break;
                }
            }
           // for(i=0;i<n-1;i++)printf("%d ",PI_2[i]);

            //choisir la meilleur position pour PI_S[i] dans PI_2.
            int pos;//La position de l'insersion au final
            int temps;
            Cbarre=INT_MAX;
            for(k=0;k<n;k++)
            {
                if(k==origPosi);
                else
                {
                    l=0;
                    for(j=0;j<n;j++)
                    {
                        if(j==k)
                        {
                           PI_temp[j]=PI_S[i];

                        }
                        else
                        {
                            PI_temp[j]=PI_2[l];
                            l++;
                        }

                    }
                    //comparer les Cmax, choisir la meilleur
                    temps=calculateCbarre(M, m, n, PI_temp, n);
                    if(temps<Cbarre)
                    {
                        Cbarre=temps;
                        pos=k;
                    }
                }

            }
            //Insérer PI_S[i] à la position choisi
            insert(PI_2,n-1, pos, PI_S[i]);
            //for(i=0;i<n;i++)printf("%d \n",PI_2[i]);


        }
        free(PI_temp);
        tem1=calculateCbarre(M, m, n, PI, n);
        tem2=calculateCbarre(M, m, n, PI_2, n);

        if(tem1>tem2)
        {
            memcpy(PI, PI_2, (n)*sizeof(int));
            //printf("Cbarre= %d \n", tem2);
        }
        //else printf("Cbarre= %d \n", tem1);
        //for(i=0;i<n;i++)printf("%d ",PI_2[i]);

        free(PI_2);
        free(PI_S);
        return(PI);
}
