#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <string.h>
#ifdef _CRAY
#  include <fortran.h>
#  define cdt CDT
#else
#  if !defined(_AIX) && !defined(__hpux)
#    define cdt cdt_
#  endif
#endif


/**
 * TYPES
 */

/* Type pour un travail, défini par ses m durées opératoires et ses intervalles de délais. */
typedef struct
{
	int *p;			/* Durées opératoires. */
	int *alpha;		/* Décalages min. */
	int *beta;		/* Décalages max. */
	int *a;			/* Paramètres pour */
	int *b;			/* le no-wait. */
} typejobF3d;
/* Type pour un travail de nowait, défini par ses m durées opératoires*/
typedef struct
{
    int *pnowait;
    int *beta;
} typejobnowait;

/* Type pour un noeud, défini par les éléments suivants : */
typedef struct { 
	int niveau;		/* Niveau du noeud dans l'arbre. */
	int* seqpar;	/* Séquence partielle pour ce noeud. */
	int* d;		/* Date de dispo des machines. */
	int binf, bsup;		/* Bornes inf et sup pour ce noeud. */
	int* reste;	/* Reste de la séquence pour ce noeud. */
	int cbarre; 		/*cbarre de ce noeud*/
} typenoeud;


int maxtableau(int *ptr,int length) 
{ 
 int max = 0; 
 int i;
 for(i=0;i<length;i++) 
 { 
    if (ptr[i]>max) max = ptr[i]; 
 } 
 return(max); 
}

int mintableau(int *ptr,int length) 
{ 
 int min = 2147483647; 
 int i;
 for(i=0;i<length;i++) 
 { 
    if (ptr[i]<min) min = ptr[i]; 
 } 
 return(min); 
}
/*
int delta(int j1, int j2, typenoeud* pnd, typejobF3d *job, int n, int nbmachines)
{

	int* tableau_de_t;
	int dist; //valeur retournée (max de tous les opération
	int nbdet; //nombre de t dans un calcul de delta = nombre de case dans le tableau tableau_de_t
	int iboucle, kboucle, mboucle, pboucle;
	dist=0;	

	int t1mJ1, t1mJ2, tmJ2, t1J1, t1iJ1, timJ2, t1J2; // ils seront toujours présents dans les calculs
	
	
	t1mJ1=0; t1mJ2=0; tmJ2=0; t1J1=0; t1iJ1=0; timJ2=0; t1J2=0;
	nbdet = n*(nbmachines+1)/2;
	

	tableau_de_t    = (int *)calloc(nbdet,sizeof(int));

	if (j1!=n)
	{
		for (iboucle=0 ; iboucle<nbmachines ; iboucle++)
		{
			t1mJ1 += job[pnd->reste[j1]].pclassique[iboucle];
			
		}
		t1J1 = job[pnd->reste[j1]].pclassique[0];
		//p printf("valeur de t1mJ1 : %d\n", t1mJ1);
		//p printf("valeur de t1J1 : %d\n", t1J1);
	}
	if (j2!=n)
	{
		for (iboucle=0 ; iboucle<nbmachines ; iboucle++)
		{
			t1mJ2 += job[pnd->reste[j2]].pclassique[iboucle];
			
		}
		tmJ2 = job[pnd->reste[j2]].pclassique[nbmachines-1];
		//p printf("valeur de t1mJ2 : %d\n", t1mJ2);
		//p printf("valeur de tmJ2 : %d\n", tmJ2);
	}

	//p printf("apres les calculs des t1mdsfh\n");
	
	tableau_de_t[0] = t1mJ2-tmJ2;
	//p printf("tableau de t [0] : t1mJ2 - tmJ2 %d\n", tableau_de_t[0]);
	tableau_de_t[1] = t1mJ1-t1J1;
	//p printf("tableau de t [1] : t1mJ1 - t1J1 %d\n", tableau_de_t[1]);
	//calcul des t1iJ1 timJ2
	kboucle=0;
	for (kboucle=2; kboucle<nbmachines; kboucle++)
	{	
		t1iJ1=0; timJ2=0;
		if(j1!=n){
			for(mboucle=0; mboucle<kboucle; mboucle++)
				{t1iJ1 += job[pnd->reste[j1]].pclassique[mboucle];
				}
				//p printf("j1 différent de n et t1%dJ1 = %d\n", kboucle, t1iJ1);
		}
		if(j2!=n){
			for(pboucle=kboucle-1; pboucle<nbmachines; pboucle++)
				{
					//printf("pboucle = %d\n",pboucle);
					timJ2 += job[pnd->reste[j2]].pclassique[pboucle];
				}
				//p printf("j2 différent de n et t%dmJ2 = %d\n", kboucle, timJ2);
		}
		tableau_de_t[kboucle]=	(t1iJ1+timJ2)-(t1J1+tmJ2);
		//p printf("tableau_de_t[%d]=%d\n",kboucle, tableau_de_t[kboucle]);
	}

	

	//si j1 vaut n ou si j2 vaut n alors formule différente
	// n=omega = l'élement imaginaire

	
	// si j1 = j2 alors ca vaut 0
	if (j1==j2)
	{//p printf("j1=j2 dist=0 \n");
		dist=0;
	}

	else if (j1==n)
	{	//p printf("j1 = omega\n");
		dist=(t1mJ2-tmJ2);
	}
	
	else if (j2==n)
	{	//p printf("j2 = omega\n");
		dist=0;
	}
	else 
	{
		dist=maxtableau(tableau_de_t,nbdet);
		//p printf("dist est le max du tableau de t : %d\n",dist);

	}
	
	//p printf("valeur de dist qu'on retourne : %d\n\n", dist);
	free(tableau_de_t);
	return dist;
}
*/
int delta(int j1, int j2, typenoeud* pnd, typejobF3d *job, int n, int nbmachines,int indice)
{

	int* tableau_de_t;
	int dist; //valeur retournée (max de tous les opération
	int nbdet; //nombre de t dans un calcul de delta = nombre de case dans le tableau tableau_de_t
	int iboucle, kboucle, mboucle, pboucle;
	dist=0;	

	int t1mJ1, t1mJ2, tmJ2, t1J1, t1iJ1, timJ2, t1J2; // ils seront toujours présents dans les calculs
	
	/*init*/
	t1mJ1=0; t1mJ2=0; tmJ2=0; t1J1=0; t1iJ1=0; timJ2=0; t1J2=0;
	nbdet = n*(nbmachines+1)/2;
	
	/* Allocations. */
	tableau_de_t    = (int *)calloc(nbdet,sizeof(int));

	
	/* Calculs des t1mJ1, t1mJ2, tmJ2, t1J1 */
	if (j1!=n)
	{
		for (iboucle=indice ; iboucle<nbmachines-1 ; iboucle++)
		{
			t1mJ1 += job[pnd->reste[j1]].p[iboucle+1]+job[pnd->reste[j1]].alpha[iboucle];
			
		}
		//t1J1 = job[pnd->reste[j1]].p[0];
		//p printf("valeur de t1mJ1 : %d\n", t1mJ1);
		//p printf("valeur de t1J1 : %d\n", t1J1);
	}
	if (j2!=n)
	{
		for (iboucle=indice ; iboucle<nbmachines-1 ; iboucle++)
		{
			t1mJ2 += job[pnd->reste[j2]].p[iboucle]+job[pnd->reste[j2]].alpha[iboucle];
			
		}
		//tmJ2 = job[pnd->reste[j2]].p[nbmachines-1];
		//p printf("valeur de t1mJ2 : %d\n", t1mJ2);
		//p printf("valeur de tmJ2 : %d\n", tmJ2);
	}

	//p printf("apres les calculs des t1mdsfh\n");
	/* Remplissage du tableau de t */
	tableau_de_t[0] = t1mJ2;
	//p printf("tableau de t [0] : t1mJ2 - tmJ2 %d\n", tableau_de_t[0]);
	tableau_de_t[1] = t1mJ1;
	//p printf("tableau de t [1] : t1mJ1 - t1J1 %d\n", tableau_de_t[1]);
	//calcul des t1iJ1 timJ2
	kboucle=indice;
	for (kboucle=2+indice; kboucle<nbmachines; kboucle++)
	{	
		t1iJ1=0; timJ2=0;
		if(j1!=n){
			for(mboucle=indice+1; mboucle<kboucle; mboucle++)
				{
					t1iJ1 += job[pnd->reste[j1]].p[mboucle]+job[pnd->reste[j1]].alpha[mboucle-1];
				}
				//p printf("j1 différent de n et t1%dJ1 = %d\n", kboucle, t1iJ1);
		}
		if(j2!=n){
			for(pboucle=kboucle-1; pboucle<nbmachines-1; pboucle++)
				{
					//printf("pboucle = %d\n",pboucle);
					timJ2 += job[pnd->reste[j2]].p[pboucle]+job[pnd->reste[j2]].alpha[pboucle];
				}
				//p printf("j2 différent de n et t%dmJ2 = %d\n", kboucle, timJ2);
		}
		tableau_de_t[kboucle]=	(t1iJ1+timJ2);
		//p printf("tableau_de_t[%d]=%d\n",kboucle, tableau_de_t[kboucle]);
	}

	

	//si j1 vaut n ou si j2 vaut n alors formule différente
	// n=omega = l'élement imaginaire

	
	// si j1 = j2 alors ca vaut 0
	if (j1==j2)
	{//p printf("j1=j2 dist=0 \n");
		dist=0;
	}

	else if (j1==n)
	{	//p printf("j1 = omega\n");
		dist=t1mJ2;
	}
	
	else if (j2==n)
	{	//p printf("j2 = omega\n");
		dist=0;
	}
	else 
	{
		dist=maxtableau(tableau_de_t,nbdet);
		//p printf("dist est le max du tableau de t : %d\n",dist);

	}
	
	//p printf("valeur de dist qu'on retourne : %d\n\n", dist);
	free(tableau_de_t);
	return dist;
}


/*int delta(int j1, int j2, typenoeud* pnd, typejobF3d *job, int n, int nbmachines)*/
int deltaancien(int j1, int j2, typenoeud* pnd, typejobF3d *job, int n, int nbmachines)
{

	int* tableau_de_t;
	int dist; //valeur retournée (max de tous les opération
	int nbdet; //nombre de t dans un calcul de delta = nombre de case dans le tableau tableau_de_t
	int iboucle, kboucle, mboucle, pboucle;
	dist=0;	

	int t1mJ1, t1mJ2, tmJ2, t1J1, t1iJ1, timJ2, t1J2; // ils seront toujours présents dans les calculs
	
	/*init*/
	t1mJ1=0; t1mJ2=0; tmJ2=0; t1J1=0; t1iJ1=0; timJ2=0; t1J2=0;
	nbdet = n*(nbmachines+1)/2;
	
	/* Allocations. */
	tableau_de_t    = (int *)calloc(nbdet,sizeof(int));

	/* Calculs des t1mJ1, t1mJ2, tmJ2, t1J1 */
	if (j1!=n)
	{
		for (iboucle=0 ; iboucle<nbmachines ; iboucle++)
		{
			t1mJ1 += job[pnd->reste[j1]].p[iboucle];
			
		}
		t1J1 = job[pnd->reste[j1]].p[0];
		//p printf("valeur de t1mJ1 : %d\n", t1mJ1);
		//p printf("valeur de t1J1 : %d\n", t1J1);
	}
	if (j2!=n)
	{
		for (iboucle=0 ; iboucle<nbmachines ; iboucle++)
		{
			t1mJ2 += job[pnd->reste[j2]].p[iboucle];
			
		}
		tmJ2 = job[pnd->reste[j2]].p[nbmachines-1];
		//p printf("valeur de t1mJ2 : %d\n", t1mJ2);
		//p printf("valeur de tmJ2 : %d\n", tmJ2);
	}

	//p printf("apres les calculs des t1mdsfh\n");
	/* Remplissage du tableau de t */
	tableau_de_t[0] = t1mJ2-tmJ2;
	//p printf("tableau de t [0] : t1mJ2 - tmJ2 %d\n", tableau_de_t[0]);
	tableau_de_t[1] = t1mJ1-t1J1;
	//p printf("tableau de t [1] : t1mJ1 - t1J1 %d\n", tableau_de_t[1]);
	//calcul des t1iJ1 timJ2
	kboucle=0;
	for (kboucle=2; kboucle<nbmachines; kboucle++)
	{	
		t1iJ1=0; timJ2=0;
		if(j1!=n){
			for(mboucle=0; mboucle<kboucle; mboucle++)
				{t1iJ1 += job[pnd->reste[j1]].p[mboucle];
				}
				//p printf("j1 différent de n et t1%dJ1 = %d\n", kboucle, t1iJ1);
		}
		if(j2!=n){
			for(pboucle=kboucle-1; pboucle<nbmachines; pboucle++)
				{
					//printf("pboucle = %d\n",pboucle);
					timJ2 += job[pnd->reste[j2]].p[pboucle];
				}
				//p printf("j2 différent de n et t%dmJ2 = %d\n", kboucle, timJ2);
		}
		tableau_de_t[kboucle]=	(t1iJ1+timJ2)-(t1J1+tmJ2);
		//p printf("tableau_de_t[%d]=%d\n",kboucle, tableau_de_t[kboucle]);
	}

	

	//si j1 vaut n ou si j2 vaut n alors formule différente
	// n=omega = l'élement imaginaire

	
	// si j1 = j2 alors ca vaut 0
	if (j1==j2)
	{//p printf("j1=j2 dist=0 \n");
		dist=0;
	}

	else if (j1==n)
	{	//p printf("j1 = omega\n");
		dist=(t1mJ2-tmJ2);
	}
	
	else if (j2==n)
	{	//p printf("j2 = omega\n");
		dist=0;
	}
	else 
	{
		dist=maxtableau(tableau_de_t,nbdet);
		//p printf("dist est le max du tableau de t : %d\n",dist);

	}
	
	//p printf("valeur de dist qu'on retourne : %d\n\n", dist);
	free(tableau_de_t);
	return dist;
}


void cdt(int *n, int *ordx, int *x, int *maxnd, int *inf, float *alpha, int *zeur, int *binf, int *fstar, int *lb0, int *lbc, int *nexp, int *nprobq, int *nass, int *active, int *lopt, float *spars, float *avson, int *err);


int appel_pvc(typenoeud *pnd,typejobF3d *job, int nbmachines, int njnp, int * seqpvc,int indice)
{

	/* Déclarations pour l'ATSP. */
	int n, ordx, maxnd, inf, zeur, lb0, lbc, err, lopt, active, nass, nprobq, nexp, binf;
	float alpha, avson, spars;
	int *fstar;
	int *x;
	int aboucle;

	/* Initialisations pour l'ATSP. */

	ordx = 123240;
	inf = 99999999;
	alpha = -1;
	maxnd = -1;
	zeur = -1;
	n=njnp+1;

/* Allocations. */
	x     = (int *)calloc(ordx,sizeof(int));


	
	//term  = (int *)calloc(nbmachines,sizeof(int));


// il faudra créer une fonction delta... en fonction du tableau tab de main.c... mis en paramètre de appel pvc !!!!


	//initialisation du tableau x pour un exemple à 5 noeuds
/* j1 et j2 des jobs*/
int iboucle;
int j1;	
int j2;
iboucle = 0;
j2=0;
while(j2<njnp+1)
{
	j1=0;
	while(j1<njnp+1)
	{	
		//printf("\nvaleur de l'indice de notre tableau x en ligne qui contient les distances%d\n", iboucle);
		//p printf("Valeur de j1 = %d, valeur de j2 = %d\n" , j1 ,j2);
		x[iboucle]=delta(j1, j2, pnd, job, njnp, nbmachines,indice);
		//p printf("valeur de x[iboucle] = %d\n\n ", x[iboucle]);
		j1++;
		iboucle++;
	}
	j2++;
}
	
	//printf("tableau des distance : \n");

	/*//VERIF for (aboucle=0;aboucle<((njnp+1)*(njnp+1));aboucle++)
	{
		printf(" %d", x[aboucle]);
	}*/



	fstar = (int *)calloc(n,sizeof(int));


	// APPEL A LA PROCEDURE FORTRAN
	cdt(&n, &ordx, x, &maxnd, &inf, &alpha, &zeur, &binf, fstar, &lb0, &lbc, &nexp, &nprobq, &nass, &active, &lopt, &spars, &avson, &err); //19 para

	int i;
		//on va calculer une borne sup ici en prenant le cbarre de la sequence optimale pour le pvc
	//VERIF printf("\nSequence optimale du pvc:\n");
	/*//VERIF for (i=0; i<=njnp; i++)
	{
		printf("%d->%d\n", i+1, fstar[i]);
		
		
	}*/

	//récup de la seq
	seqpvc[0] = fstar[njnp];

	for (i = 1 ; i < njnp ; i++)
	{
		seqpvc[i] = fstar[seqpvc[i-1]-1];
	}

	//printf("Sequence modifiee : ");
		/*for (i = 0 ; i < njnp ; i++)
		{
			printf("%d ",seqpvc[i]);
		}*/


	/*for (i=0; i<njnp; i++)
	{
		printf("seqpvc[%d] = %d\n", i , seqpvc[i]);
	}*/

	//printf("LBC %d\n ", lbc);
	//printf("valeur de l'erreur : %d\n",err);
	//printf("valeur de binf qui correspond à zstar dans le code fortran : %d\n",binf);
	//printf("valeur de zeur : %d\n",zeur);
	free(x);
	free(fstar);
	return binf;

}

