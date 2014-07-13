#include "PR1.h"
int * NEH(int *M, int m, int n, int* dupU, int* dupPI, int d)//joblist=U,jobcount=n-d
{
    int * res = (int*)calloc(n+1, sizeof(int));
    memcpy(res, dupPI, d*sizeof(int));
    if(d==n)
    {
        res[n]=calculateCbarre(M, m, n, dupPI, d);
        return res;

    }
    else
    {
        int jobcount=n-d;
        int * jobencours=dupU;
        int * sum = (int*)calloc(n, sizeof(int));
        //        int sum[n];
        int * seq = (int*)calloc(n, sizeof(int));
        //int * res = (int*)calloc(n+1, sizeof(int));
        //memcpy(res, dupPI, d*sizeof(int));
        int i = 0,j = 0;
        //Calculer "total processing times"
        for(i=0; i<jobcount; i++)
        {
            for(j=0; j<m; j++ )
            {
                sum[*(jobencours+i)] += (int)(*(M+j*n+(*(jobencours+i))));//M[j][i];
            }
        }
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

        //Maintenant on a la séquence triée :seq(belta)
        int *resDup = (int*)calloc(n, sizeof(int));
        int pos;//La position de l'insersion au final
        int cmax;
        int k;


       for(k=0;k<n-d;k++)
       {
            cmax=INT_MAX;
            for(j=0; j<=k+d; j++)//j est la position de l'insersion
            {
                memcpy(resDup, res, (k+d)*sizeof(int));
                insert(resDup, d+k, j, seq[k]);
                tmp=calculateCbarre(M, m, n, resDup, k+d+1);
                if(tmp<cmax)
                {
                    cmax=tmp;
                    pos=j;
                }
            }
            insert(res,d+k ,pos, seq[k]);
        }

        free(sum);
        free(seq);
        free(resDup);
        res[n]=cmax;
        return res;
    }
}

//Insérer job j à la position p de res
void insert(int* joblist, int length, int p, int j)
{
    int i;
    for(i=length-1; i>=p; i--)
    {
        joblist[i+1]=joblist[i];
    }
    joblist[p]=j;

}

//Calculer le cmax de joblist
int calculateCbarre(int *M, int m, int n, int* joblist, int jobcount)
{
    int *subM = (int*)malloc(m*jobcount*sizeof(int));
    int i,j;
    int sum=0;
    for(i=0; i<m; i++)
    {
        for(j=0; j<jobcount; j++)
        {
            if(i==0 && j==0)
                *(subM+i*jobcount+j)= (int)(*(M+i*n+joblist[j]));
            else if(i==0)
            {
                *(subM+i*jobcount+j)= *(subM+i*jobcount+j-1) + (int)(*(M+i*n+joblist[j]));
            }
            else if(j==0)
            {
                *(subM+i*jobcount+j)= *(subM+(i-1)*jobcount+j)+ (int)(*(M+i*n+joblist[j]));
            }
            else
            {
                if(*(subM+i*jobcount+j-1)>*(subM+(i-1)*jobcount+j))
                {
                    *(subM+i*jobcount+j)= *(subM+i*jobcount+j-1)+ (int)(*(M+i*n+joblist[j]));
                }
                else
                {
                    *(subM+i*jobcount+j)= *(subM+(i-1)*jobcount+j)+ (int)(*(M+i*n+joblist[j]));
                }
            }
        }
    }
    for(i=0;i<jobcount;i++)
    {
        sum+=*(subM+(m-1)*jobcount+i);
    }
    free(subM);
    return sum;

}
