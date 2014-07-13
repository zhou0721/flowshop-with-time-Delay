#include "PR1.h"
#include "PR4.h"
void LR(int *M, int m, int n, TypeCellule **U, TypeCellule** PI,int l, int d)
{
    int i,j;
    int picked=0;    // le numero du job que on selecte apairtir du U
    int k=0,it=0,at=0,xi=INT_MAX;    //k est le nb des jobs qui est dans PI, it est idletime, at=C_m+C_lamda, xi est le resultat de la fonction d'index
    int K[m];    //initiale du K, K est une tableau de 3 , K[i] pour machine i
    int J[m];    // pour calculer le Cm pour le job courant
    float Lamda[m],moyenne;    //Lamda est un tableau du 3, pour calculer le Cm pour le job lamda (qui est le moyenne des job U-{j})
    TypeCellule * L=NULL;    //U est une liste pour les jobs dont l'ordre est pas encore déterminé
    TypeCellule * p=NULL,* pp=NULL, *ppp=NULL;    //pointeurs servent à l'itération


    for(i=0;i<m;i++)K[i]=0;
    k=0;
    for(i=0;i<n;i++)*U=Append(*U,i,1000);
    p=*U;
    while(p!=NULL)  //pour chaque element dans U
    {
        j=p->job[0];    //job[0] est le numero du job courant, job[1] est Cm du job courant
        it=0;
        at=0;
        for(i=0;i<m;i++)    //initiation du tableau J
        {
            J[i]=*(M+i*n+j);
            Lamda[i]=0;

        }
        pp=*U;
        moyenne=0;    //nb des jobs (sauf job j) dans U

         /*l'initiation du tableau Lamda*/
        while(pp!=NULL)
        {
            if(pp->job[0]==j)pp=pp->suivant;
            else
            {
                moyenne++;
                for(i=0;i<m;i++)
                {
                    Lamda[i]+=(int)(*(M+i*n+pp->job[0]));
                }
                pp=pp->suivant;
            }
        }
        if(moyenne>0)
        {
            for(i=0;i<m;i++)
                {
                    Lamda[i]=Lamda[i]/moyenne;
                }
        }
        /* Fin de l'initiation du tableau Lamda*/

        /*Mis a jour du tableau J et tableau Lamda*/
        J[0]+=K[0];
        Lamda[0]+=J[0];
        for(i=1;i<m;i++)
        {
            if(K[i]<J[i-1])
            {
                it+=(m*(J[i-1]-K[i]))/((i+1)+k*(m-i-1)/(n-2)); //on calcule le idle time for job j
                J[i]+=J[i-1];       //mis a jour le temps de finir pour le job j dans la machine i
            }
            else
            {
                it+=(m*0)/((i+1)+k*(m-i-1)/(n-2));
                J[i]=K[i]+J[i];
            }
            if(J[i]<Lamda[i-1])Lamda[i]+=Lamda[i-1];
            else Lamda[i]+=J[i];
        }
        at=J[m-1]+Lamda[m-1];    ////mis a jour le temps de finir pour le job lamda dans la machine i
        p->job[1]=(n-k-2)*it+at;
        p=p->suivant;
    }
    L = Tri(*U);
    ppp=L;
    for(i=0;i<l;i++)
    {
        if(ppp->suivant!=NULL)
			ppp=ppp->suivant;
    }
    picked=ppp->job[0];
	Liveration(&L);
	L = NULL;
	
   /*mis a jour du tableau K*/

   K[0]=(int)*(M+0*n+picked);    //tableau K conserve le Ci de les k jobs (dont l'ordre est determine)
   for(i=1;i<m;i++)
   {
        if(K[i]<K[i-1])K[i]=K[i-1]+(int)(*(M+i*n+picked));
        else K[i]=(int)(*(M+i*n+picked));
   }


   *U=Supprime(*U,picked);    //on supprime le job selecté dans U
   *PI=Append(*PI,picked,K[m-1]);   //on ajoute le job seletcé dans PI
   k++;


// l'algo commence
	while(k<d)
	{
		xi=INT_MAX;
		p=*U;
		while(p!=NULL)  //pour chaque element dans U
		{
			j=p->job[0];    //job[0] est le numero du job courant, job[1] est Cm du job courant
			it=0;
			at=0;
			for(i=0;i<m;i++)    //initiation du tableau J
			{
				J[i]=*(M+i*n+j);
				Lamda[i]=0;

			}
			pp=*U;
			moyenne=0;    //nb des jobs (sauf job j) dans U

			 /*l'initiation du tableau Lamda*/
			while(pp!=NULL)
			{
				if(pp->job[0]==j)pp=pp->suivant;
				else
				{
					moyenne++;
					for(i=0;i<m;i++)
					{
						Lamda[i]+=(int)(*(M+i*n+pp->job[0]));//M[i][pp->job[0]];
					}
					pp=pp->suivant;
				}
			}
			if(moyenne>0)
			{
				for(i=0;i<m;i++)
					{
						Lamda[i]=Lamda[i]/moyenne;
					}
			}
			/* Fin de l'initiation du tableau Lamda*/

			/*Mis a jour du tableau J et tableau Lamda*/
			J[0]+=K[0];
			Lamda[0]+=J[0];
			for(i=1;i<m;i++)
			{
				if(K[i]<J[i-1])
				{
					it+=(m*(J[i-1]-K[i]))/((i+1)+k*(m-i-1)/(n-2)); //on calcule le idle time for job j
					J[i]+=J[i-1];       //mis a jour le temps de finir pour le job j dans la machine i
				}
				else
				{
					it+=(m*0)/((i+1)+k*(m-i-1)/(n-2));
					J[i]=K[i]+J[i];
				}
				if(J[i]<Lamda[i-1])Lamda[i]+=Lamda[i-1];
				else Lamda[i]+=J[i];
			}
			at=J[m-1]+Lamda[m-1];    ////mis a jour le temps de finir pour le job lamda dans la machine i
			p->job[1]=(n-k-2)*it+at;
			if(p->job[1]<xi)
			{
				xi=p->job[1];    //on calcule le resultat de la fonction d'index
				picked=j;       // on prend le job qui a le plus petite valeur du xi
			}
			p=p->suivant;     //on passe à la job suivant qui est dans U

		}

		/*mis a jour du tableau K*/


	/*Modif Comme LR.c OK*/
		K[0]+=(int)(*(M+0*n+picked));    //tableau K conserve le Ci de les k jobs (dont l'ordre est determine)
	/*fin comme lr.c*/
		for(i=1;i<m;i++)
		{
				if(K[i]<K[i-1])
				{
					K[i]=K[i-1]+(int)(*(M+i*n+picked));
				}
				else K[i]=K[i]+(int)(*(M+i*n+picked));
		}


		*U=Supprime(*U,picked);    //on supprime le job selecté dans U
		*PI=Append(*PI,picked,K[m-1]);   //on ajoute le job seletcé dans PI
		k++;
    }
}
