#include "PR4.h"
int * NEH2(int *M, int m, int n, int* joblist, int jobcount)
{
        int * jobencours=joblist;
        int * sum = (int*)calloc(n, sizeof(int));
        //        int sum[n];
        int * seq = (int*)calloc(n, sizeof(int));
        int * res = (int*)calloc(n+1, sizeof(int));
        int i = 0,j = 0;
        //Calculer "total processing times"
        for(i=0; i<jobcount; i++)
        {
            for(j=0; j<m; j++ )
            {
                sum[*(jobencours+i)] += (int)(*(M+j*n+(*(jobencours+i))));//M[j][i];
            }
        }

        //print(sum, 4);
        //Trier la séquence
        int tmp = sum[0];
        seq[0]=0;
        for( i=0; i<n; i++) //pour remplir 'seq'
        {
            for(j=0; j<n; j++)
            {
                if(sum[j]>tmp)
                {
                    tmp = sum[j];
                    seq[i] = j;
                }
            }
            sum[seq[i]]=0;
            tmp = 0;
        }
        //print(seq, 4);
        //Maintenant on a la séquence triée
        int *resDup = (int*)calloc(n, sizeof(int));
        int pos;//La position de l'insersion au final
        int cmax;
        for( i=0; i<jobcount; i++)
        {
            cmax=INT_MAX;
            //j est la position de l'insersion
            for(j=0; j<=i; j++)
            {
                memcpy(resDup, res, n*sizeof(int));
                //print(resDup, 4);
                //printf("j=%d\n\n",j);
                //Insérer job i à la position j de res
                insert(resDup, i, j, seq[i]);
                //print(resDup, 4);
                tmp=calculateCbarre(M, m, n, resDup, i+1);//i+1 est le nombre de job
                if(tmp<cmax)
                {
                    cmax=tmp;
                    pos=j;
                }
            }
            insert(res,i ,pos, seq[i]);
        }
        free(sum);
        free(seq);
        free(resDup);
        res[jobcount]=cmax;
        return res;
}
