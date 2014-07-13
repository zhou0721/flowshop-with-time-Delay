/**
 * Fm/delai min-max/Cmax
 *  PSE sur plusieurs fichiers.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <time.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>

#include "const.h"
#include "bornes.h"
#include "srcbornesup/PR1.h"
#include "srcbornesup/PR4.h"
#include "../include/pvcsimple.h"


/**
 * CONDITIONS DE CHOIX DES BORNES INFERIEURES
 */

int UsePVC3;
int UseRelaxFin;
int UseRelaxChargeFin;

// si l'intervalle entre délai min et délai max est supérieur à ce seuil, on n'utilise pas le PVC3
#define SEUIL_DELAI	9000

//ici structures

#include "atsp.h"
#include "simplexe.h"
#include "bsup.h"


/**
 * VARIABLES GLOBALES
 */

/* Fichiers de sortie (écriture des logs) et de résultats. */
FILE *sortie;

/* Fichiers de sortieexcel (écriture des excel) et de résultats. */
FILE *sortieexcel;
FILE* latex;
/* Nombre de travaux. */
int nbjobs;

/* Nombre de machines. */
int nbmachines;

/* Meilleur Cbarre */
int meilleurCbarre;

/* Compteurs. */
int compteur;
int cptnoeudsevalues;

/* Nombre de noeuds avant d'atteindre l'optimal */
int NoeudsAvantOpt;

/* Tableau pour la meilleure séquence trouvée. */
int meilleurSequence[NMAX];

/* Tableau des travaux du problème. */
typejobF3d job[NMAX];

/* Entier indiquant si les logs doivent être affichés ou non. */
int logger;

/* Entier indiquant si les résultats doivent être écrits dans un fichier LaTeX ou non. */
int output;

/* Entier indiquant si les résultats doivent être écrits dans un fichier excel ou non. */
int excel;

/*Statistiques pour les bornes*/	
// Compteurs qui s'incrémente quand l'heuristique est la meilleure, les 4 autres 'est quand l'heuristique est strictement la meilleure.

int cptpr1=0;
int cptpr2=0;
int cptpr4=0;
int cptpvc=0;

int cptsttpr1=0;
int cptsttpr2=0;
int cptsttpr4=0;
int cptsttpvc=0;



/*booléen pour le choix des bsup*/
int bsuppvc, bsuppr1, bsuppr4, bsuppr2; //booléen pour indiquer la borne sup que l'on souhaite utiliser.


/*CHOIX DE L'heuristique ou DES heuristiques*/
bsuppvc = 1;
bsuppr1 = 1;
bsuppr4 = 1;
bsuppr2 = 1;

double RPI_PR1 = 0;
double RPI_PR2 = 0;
double RPI_PR4 = 0;
double RPI_PVC = 0;
int cptRPI = 0;
/* Statistiques. */
double durees[NMAX];		/* Durées d'exécution des instances. */
double dureesbsup[NMAX];	/* Temps avant de trouver l'optimal dans les différentes instances. */
int nbnoeudsdev[NMAX];		/* Nombre total de noeuds développés. */
int nbnoeudseval[NMAX];		/* Nombre total de noeuds évalués. */
int ecartsbinf[NMAX];		/* Ecarts binf racine - optimal. */
int premierebinf;		/* Valeur de la borne inf au noeud racine. */
double chronobsup[ZMAX];	/* Dates auxquelles les bsup sont trouvées. */
int cptbsup;			/* Indice pour le tableau chronobsup. */

/**
 * PROCEDURES
 */



/**
 * int lecture_fichier(fichier, pnbjobs, job)
 *
 * Cette procédure permet la lecture du fichier de données.
 * Les données sont enregistrées dans le tableau job[].
 *
 * E:     numfic : numéro du fichier de données à ouvrir.
 *        pnbjob : pointeur sur le nombre de travaux du problème considéré.
 *        job    : tableau des travaux du problème.
 * PrecC: numfic > 0.
 * 
 * S:     Entier.
 * PostC: La lecture a réussi et la fonction renvoie 1, ou
 *        la lecture a échoué et la fonction renvoie 0.
 */

int lecture_fichier(fichier, job) /***************************************************************************************************************/
char *fichier;
typejobF3d *job;
{
	/* Variables de traitement. */

	/* Indices. */
	int i, j;
	/* Buffer pour lire les entiers. */
	int n;
	/* Fichier de données. */
	FILE *f;
	/* Booléen pour la lecture. */
	int lu;

	/* Ouverture du fichier. */

	f = fopen(fichier, "rt");

	if (f == NULL) return(0);

	/* Lecture. */

	fscanf(f, "%d", &nbmachines);
	fscanf(f, "%d", &nbjobs);

	for (j = 0 ; j < nbjobs ; j++)
	{
		/* Allocations. */
		job[j].p     = (int *)calloc(nbmachines, sizeof(int));
		job[j].alpha = (int *)calloc(nbmachines-1, sizeof(int));
		job[j].beta  = (int *)calloc(nbmachines-1, sizeof(int));
		job[j].a     = (int *)calloc(nbmachines-1, sizeof(int));
		job[j].b     = (int *)calloc(nbmachines, sizeof(int));	

		/* Renseignement des durées opératoires. */
		for (i = 0 ; i < nbmachines ; i++)
		{
			lu = fscanf(f, "%d", &n);
			if (lu == EOF) break;

			job[j].p[i] = n;
			//VERIF printf("job[%d].p[%d] = %d\n", j, i ,n);
		}
		/* Renseignement des alpha. */
		for (i = 0 ; i < nbmachines-1 ; i++)
		{
			lu = fscanf(f, "%d", &n);
			if (lu == EOF) break;

			job[j].alpha[i] = n;
		}

		if (lu == EOF) break;

		/* Renseignement des beta. */
		for (i = 0 ; i < nbmachines-1 ; i++)
		{
			lu = fscanf(f, "%d", &n);
			if (lu == EOF) break;

			job[j].beta[i] = n;
		}
		if (lu == EOF) break;
		


		/* Calcul des paramètres pour l'ATSP : simplexe. */
		calculparametres(job[j].a, job[j].b, job[j].alpha, job[j].beta, nbmachines, 0); //p qu'est-ce que ca fait ?! ou est cette fonction ?
	}

	/* Fermeture du fichier. */

	fclose(f);
	return(1);
}


/**
 * void copienoeud(pn1, pn2)
 *
 * Cette procédure permet de copier un noeud.
 *
 * E:     pn1 : noeud à copier.
 *        pn2 : noeud sur lequel copier pn1.
 * PreC:  pn1 et pn2 ne sont pas à NULL.
 * 
 * S:     Néant.
 * PostC: Le noeud est copié.
 */

void copienoeud(pn1, pn2) /********************************************************************************************************************************/
typenoeud *pn1,*pn2;
{
	int i;
	pn2->niveau = pn1->niveau;

	for (i = 0 ; i < pn1->niveau ; i++)
	{
		pn2->seqpar[i] = pn1->seqpar[i];
	}

	pn2->binf = pn1->binf;
	pn2->cbarre = pn1->cbarre;
	for (i = 0 ; i < nbmachines ; i++)
	{
		pn2->d[i] = pn1->d[i];
	}

	for(i = 0 ; i < nbjobs-(pn1->niveau) ; i++)
	{
		pn2->reste[i] = pn1->reste[i];
	}
}


/**
 * void miseajourmeilleur(bs, pnd, sigma)
 *
 * Cette procédure permet de mettre à jour éventuellement la borne sup et de sauvegarder la séquence.
 *
 * E:     bs    : nouvelle borne sup.
 *        pnd   : noeud correspondant.
 *        sigma : sequence correspondante.
 * PreC:  sigma et pnd ne sont pas à NULL.
 * 
 * S:     Néant.
 * PostC: Les paramètres d'entrée sont sauvegardés et la borne sup est améliorée.
 */

void miseajourmeilleurCbarre(bs, pnd, sigma) /******************************************************************************************************************/
int bs, *sigma;
typenoeud *pnd;
{
	int i;

	if (bs < meilleurCbarre)
	{
		NoeudsAvantOpt = cptnoeudsevalues;
		chronobsup[cptbsup] = clock();
		cptbsup++;
		meilleurCbarre = bs;
		for (i = 0 ; i < (pnd->niveau) ; i++)        meilleurSequence[i]               = (pnd->seqpar)[i]+1;
		for (i = 0 ; i < nbjobs-(pnd->niveau) ; i++) meilleurSequence[i+(pnd->niveau)] = (pnd->reste)[sigma[i]-1]+1;
	}
}

void miseajourmeilleurRelax(int bs, typenoeud* pnd, int* sigma) /******************************************************************************************************************/
{
	int i;

	if (bs < meilleurCbarre)
	{
		NoeudsAvantOpt = compteur;
		chronobsup[cptbsup] = clock();
		cptbsup++;
		meilleurCbarre = bs;
		for (i = 0 ; i < nbjobs ; i++) meilleurSequence[i] = sigma[i]+1;
	}
}

/*************** TRI A BULLE **************/
#define TRUE 1
#define FALSE 0

void tri_a_bulle(int* t,int n)
{ 
	int j =0;
	int tmp =0;
	int en_desordre =1;
	while (en_desordre) 
	{
		en_desordre = FALSE; 
		for(j =0; j < n-1; j++)
		{  
			if (t[j] > t[j+1])
			{
				tmp = t[j+1];
				t[j+1] = t[j];
				t[j] = tmp;
				en_desordre = TRUE;
			}
		}
	}
}

/************************************************/

/*
*calculcbarre calcule le cbarre de la séquence ordonnancée "sequence" d'une noeud avec un profil de machines donné "datesDispo"
*
 cbarre = cbarre d'origine avant d'ordonnancer la séquence
*
*/


int calculcbarre(int* sequence, int * datesDispo, int njnp, int cbarre)
{
	
	int mycbarre=cbarre;
	int sigma[1]; //sigma est le singleton du job ou l'on va recalculer les dates de dispo
	int iboucle, currentJob, jboucle;
	int * datesD;
	int **p;


	if ( (p = (int**)malloc(nbjobs*sizeof(int*))) == NULL){printf("Pb malloc dates dispo p dans calculcbarre\n");}
	for(currentJob=0;  currentJob<nbjobs; currentJob++)
	{
		p[currentJob] = job[currentJob].p;
	}

	for(iboucle=0; iboucle<njnp; iboucle++)
	{
		sigma[0] = sequence[iboucle];
		datesD = GetDatesDisponibilite(1,nbmachines,sigma,datesDispo,p);
		datesDispo = datesD;

		mycbarre += datesD[nbmachines-1];

		
	}
	
	free(p);

	return(mycbarre);

}

void print(int *res, int n)
{
    int i;
    for(i=0;i<n;i++)
	{
	    printf("%d, ",res[i]);
	}
	printf("coucouprint\n");
}

/*BORNE SUPERIEUR
PR4*/
void bspr4(int nbmachines, int njnp, typenoeud * pnd, int* tableauseq)
{
        int n=njnp;
        int m=nbmachines;

        char buffer[1024], *tmp;
	int i,j;

        int M[m][n];

	if (tableauseq== NULL) {printf("erreur d'allocation bspr4");}

	//init de la matrice M : 
	for (i=0;i<m;i++)
	{
		for (j=0; j<n; j++)
		{
			M[i][j] = job[pnd->reste[j]].p[i];

		}
	}



    //init des paramètres : x= le nombre de séquence que l'on souhaite retourner, d=1 (cf article de recherche)
    int x=15;
    int d=1;
    int * ptmp;
    int * res;


	/*M la matrice des job*/
	res=PR4(*M, m, n, x, d);
	ptmp=res;

	for(i=0;i<n;i++)
	{
		tableauseq[i]=*(ptmp+i);
	}
	for(i=0;i<n;i++)
	{
		tableauseq[i]=pnd->reste[tableauseq[i]];
	}
	free(res);

}

/*BORNE SUPERIEUR
PR1*/
void bspr1(int nbmachines, int njnp, typenoeud * pnd, int* tableauseq)
{
        int n=njnp;
        int m=nbmachines;

        char buffer[1024], *tmp;
	int i,j;

        int M[m][n];

	if (tableauseq== NULL) {printf("erreur d'allocation bspr1");}

	//init de la matrice M : 
	for (i=0;i<m;i++)
	{
		for (j=0; j<n; j++)
		{
			M[i][j] = job[pnd->reste[j]].p[i];
			
		}
	}


    int x=15;
    int d=1;
    int * ptmp;
    int * res;


	/*M la matrice des job*/
	res=PR1(*M, m, n, x, d);
	ptmp=res;
	
	for(i=0;i<n;i++)
	{
		tableauseq[i]=*(ptmp+i);
	}
	for(i=0;i<n;i++)
	{
		tableauseq[i]=pnd->reste[tableauseq[i]];
	}
	free(res);

}

/*BORNE SUPERIEUR
PR2*/
void bspr2(int nbmachines, int njnp, typenoeud * pnd, int* tableauseq)
{
        int n=njnp;
        int m=nbmachines;

        char buffer[1024], *tmp;
	int i,j;

        int M[m][n];

	if (tableauseq== NULL) {printf("erreur d'allocation bspr1");}

	//init de la matrice M : 
	for (i=0;i<m;i++)
	{
		for (j=0; j<n; j++)
		{
			M[i][j] = job[pnd->reste[j]].p[i];
		}
	}

    int x=15;
    int d=1;
    int * ptmp;
    int * res;


	/*M la matrice des job*/
	res=PR2(*M, m, n, x, d);
	ptmp=res;

	for(i=0;i<n;i++)
	{
		tableauseq[i]=*(ptmp+i);
	}
	for(i=0;i<n;i++)
	{
		tableauseq[i]=pnd->reste[tableauseq[i]];
	}
	free(res);

}

/*
*	la fonction borneinfetsuppauline calcule la borne inférieur et supérieure d'un noeud.
*	Seule la valeur de la borne inf est renvoyée.
*	La fonction est la meme que l'originale elle ne prend pas en compte les alpha et beta et elle fait appel aux fonctions de calcul de borne inf et sup   *	différentes
*/

/**
 * int borneinfetsuppauline(pnd, numfils, borninfpere)
 *
 * Cette procédure permet de déterminer les bornes inférieures et supérieures du noeud passé en argument.
 * Si plusieurs bornes sont calculées, seules les meilleures sont sauvegardées.
 *
 * E:     pnd         : pointeur sur le noeud considéré.
 *        numfils     : numéro du fils.
 *        borninfpere : borne inférieure du père.
 * PrecC: pnd n'est pas NULL.
 * 
 * S:     Entier.
 * PostC: Les bornes inférieures et supérieures sont calculées, la valeur de la borne inférieure est renvoyée.
 */

int borneinfetsuppauline(pnd, numfils, borninfpere) /************************************************************************************************************/
typenoeud *pnd;
int numfils;
{


	
	/* Variables de traitement. */

	/* Indices. */
	int i, j, currentJob;
	/* Bornes. */
	int borninf, biPVC, bs, bsneh;
	/* Nombre de travaux restant. */
	int njnp;
	/* Séquence. */
	int seqpvc[NMAX]; // la sequence du reste générée par le pvc
	/* Paramètres pour le no-wait. */
	//int **pPVC, **pRelax;
	//int **dist;
	/* Paramètres pour la borne supérieure */
	int  *Sequence;//, **resultBornesRelaxFin, **resultBornesRelaxChargeFin; //**alpha, **beta,
	//int *d;
	
	int bs_pvc, bs_pr1, bs_pr2, bs_pr4; // pour stocké la valeur des bornes sup
	int tab_bs[4];//tableau de bornesup

	/*parametres pour la borne inférieure*/
	int constanteT;
	int resultSmith;
	int resultpvc;
	int jobi; //indice
	int* tabatrier; //pour Smith


		/* Initialisations. */

	bs_pvc=2147483647;
	bs_pr1=2147483647; 
	bs_pr2=2147483647;
	bs_pr4=2147483647;

	njnp = nbjobs - (pnd->niveau);

	borninf = 0;
	for(i=0;i<nbjobs;i++) //init seqpvc
	{
		seqpvc[i]=0;
	}
		/***********************/

	 	/*CAS NOEUD FEUILLE*/

	if (njnp == 0) // si c'est un noeud feuille
	{
		borninf = pnd->cbarre; // la borne inférieur est le cbarre de la seq du noeud :)
		bs      = pnd->cbarre; // la borne sup pareil
		//VERIF printf("valeur borninf : %d , valeur bs : %d\n ", borninf, bs);

		(pnd->binf) = borninf;
		miseajourmeilleurCbarre(bs, pnd, seqpvc);
		return(borninf); //fin de la fonction quand c'est un noeud feuille.
	}

	//Réponse à : premierebinf = borneinfetsuppauline(pilenoeud + 0, 1);
	//njnp est le nombre de job dans reste

	if (logger) fprintf(sortie, "\n\nEtage %d, Noeud %d. *********************\n\n", nbjobs - njnp + 1, numfils);

	if (logger)
	{
		fprintf(sortie, "La sequence partielle contient %d element(s).\t", nbjobs - njnp);
		if (nbjobs - njnp > 0)
		{
			fprintf(sortie, "Ordonnancement partiel : ");
			for (i = 0 ; i < nbjobs - njnp ; i++) fprintf(sortie, "%d ", pnd->seqpar[i]+1);
			fprintf(sortie, "\n");
		}
		fprintf(sortie, "Il reste donc %d element(s) a ordonnancer.\t", njnp);
		if (nbjobs > 0)
		{
			fprintf(sortie, "Reste a ordonnancer : ");
			for (i = 0 ; i < njnp ; i++) fprintf(sortie, "%d ", pnd->reste[i]+1);
			fprintf(sortie, "\n");
		}
		fprintf(sortie, "Dates de disponibilite des machines : ");
		for (i = 0 ; i < nbmachines ; i++) fprintf(sortie, "%d ; ", pnd->d[i]);
		fprintf(sortie, "\n\n");
	}


	 	/*CAS NOEUD NON FEUILLE*/
	if (njnp > 1) 
	{
	/*
	*			CALCUL BORNE INF
	***********************************************************************************************/

				/* La constante Tnbmachines : constanteT */

		constanteT=0;
		for (jobi=0; jobi<njnp; jobi++)
		{
			constanteT += job[pnd->reste[jobi]].p[nbmachines-1];
			//VERIF printf("pnd->rest[%d].p[%d] = %d\n",jobi, nbmachines-1, job[pnd->reste[jobi]].p[nbmachines-1] );
		}





				/* le résultat de la regle de Smith : resultSmith */

		// trier les travaux de la premiere colonne par pi croissants : 
		// tri à bulle 
		//on créé le tableau tabatrier qui sera que la premiere colonne de notre tableau tab

		resultSmith = 0;
		if( (tabatrier = (int *)malloc(njnp*sizeof(int))) == NULL){printf("Pb malloc tabatrier\n");}

		for(jobi=0;jobi<njnp;jobi++)
		{
			tabatrier[jobi]= job[pnd->reste[jobi]].p[0]; //on met les durées op de tous les jobs sur la premiere machine dans le tableau tabatrier
		}

		tri_a_bulle(tabatrier,njnp);// tabatrier est maintenant trié !
		for(jobi=1;jobi<njnp;jobi++)
		{
			resultSmith += (njnp-jobi)*tabatrier[jobi-1];
		}
		free(tabatrier);





				/* Le résultat du pvc : resultpvc */
		
		resultpvc = appel_pvc(pnd,job,nbmachines, njnp, seqpvc);
      		


		/*BORNE INF*/
		/*MISE À JOUR DE LA BORNE INF
		****************************************************************************************/

		if(pnd->niveau == 0)//si noeud racine
		{	
			borninf = constanteT + resultSmith + resultpvc ; 
		}
		else //si noeud pas racine
		{
			borninf = pnd->cbarre + (njnp * (pnd->d[0])) + constanteT + resultSmith + resultpvc ; 
		}
	}



	
	/* 	CALCUL SEQ ORDONANCE SELON HEURISTIQUE VOULUE
	*********************************************************************************************/

	if ( (Sequence = (int*) malloc(nbjobs*sizeof(int))) == NULL){printf("Pb malloc Sequence\n");}


	for(i=0; i<(nbjobs-njnp); i++) 
	{
		Sequence[i] = pnd->seqpar[i]; // on remplit le début de la seq par les jobs déja ordonnancés
	}
	
	int * ordorestepvc; 
	int * ordorestepr1; 
	int * ordorestepr4; 
	int * ordorestepr2;
	if ( (ordorestepvc = (int*) malloc(njnp*sizeof(int))) == NULL){printf("Pb malloc ordorestepvc\n");}
	if ( (ordorestepr1 = (int*) malloc(njnp*sizeof(int))) == NULL){printf("Pb malloc ordorestepr1\n");}
	if ( (ordorestepr4 = (int*) malloc(njnp*sizeof(int))) == NULL){printf("Pb malloc ordorestepr4\n");}
	if ( (ordorestepr2 = (int*) malloc(njnp*sizeof(int))) == NULL){printf("Pb malloc ordorestepr2\n");}

	int * ordoreste; 
	if ( (ordoreste = (int*) malloc(njnp*sizeof(int))) == NULL){printf("Pb malloc ordorest\n");}


	if (njnp==1)// njnp=1: il reste qu'un seul job à placer
	{
		Sequence[nbjobs-njnp] = pnd->reste[0]; //ajout du reste à la sequence totale
		ordoreste[0] = pnd->reste[0];
		pnd->bsup = calculcbarre(ordoreste, pnd->d, njnp, pnd->cbarre);

	}
	else //noeud au milieu de l'arborescence
	{
		/*Calcul des séquences en fonction des heuristiques choisies et calcule du cbarre de cette séquence pour la borne supérieure*/
		bs_pvc=2147483647;
		if (bsuppvc == 1)
		{
			j=0;
			for(i=0; i<njnp; i++) 
			{
				ordorestepvc[i] = pnd->reste[seqpvc[j]-1];
				j++;
			}
			bs_pvc = calculcbarre(ordorestepvc, pnd->d, njnp, pnd->cbarre);
			if(pnd->niveau == 0) printf(" Borne sup pvc = %d\n", bs_pvc);
			if (logger) fprintf(sortie, "Borne sup pvc = %d ", bs_pvc);
		}
		bs_pr1=2147483647;
		if ((bsuppr1 == 1)&&/* modif CL*/(njnp>2))
		{
			bspr1(nbmachines, njnp, pnd, ordorestepr1);
			bs_pr1 = calculcbarre(ordorestepr1, pnd->d, njnp, pnd->cbarre);
			if(pnd->niveau == 0) printf(" Borne sup pr1 = %d\n", bs_pr1);
			if (logger) fprintf(sortie, "Borne sup pr1 = %d ", bs_pr1);
		}
		bs_pr4=2147483647;
		if ((bsuppr4 ==1)&&/* modif CL*/(njnp>2))
		{
			bspr4(nbmachines, njnp, pnd, ordorestepr4);
			bs_pr4 = calculcbarre(ordorestepr4, pnd->d, njnp, pnd->cbarre);
			if(pnd->niveau == 0) printf(" Borne sup pr4 = %d\n", bs_pr4);
			if (logger) fprintf(sortie, "Borne sup pr4 = %d ", bs_pr4);
		}
		bs_pr2=2147483647;
		if ((bsuppr2 ==1)&&/* modif CL*/(njnp>2))
		{
			bspr2(nbmachines, njnp, pnd, ordorestepr2);
			bs_pr2 = calculcbarre(ordorestepr2, pnd->d, njnp, pnd->cbarre);
			if(pnd->niveau == 0) printf(" Borne sup pr2 = %d\n", bs_pr2);
			if (logger) fprintf(sortie, "Borne sup pr2 = %d ", bs_pr2);
		}


		//création du tableau de bornesup associé.
		tab_bs[0]=bs_pvc;
		tab_bs[1]=bs_pr1;
		tab_bs[2]=bs_pr2;
		tab_bs[3]=bs_pr4;

		pnd->bsup = mintableau(tab_bs,4); //la valeur de la borne supérieur est la plus petite valeur du tableau de bornesup
		
		cptRPI++;
		RPI_PR1 = RPI_PR1 + 100*(bs_pr1 - pnd->bsup)/(pnd->bsup);
		RPI_PR2 = RPI_PR2 + 100*(bs_pr2 - pnd->bsup)/(pnd->bsup);
		RPI_PR4 = RPI_PR4 + 100*(bs_pr4 - pnd->bsup)/(pnd->bsup);
		RPI_PVC = RPI_PVC + 100*(bs_pvc - pnd->bsup)/(pnd->bsup);

		j=0;
		if(pnd->bsup == bs_pr1)
		{	//mettre ici un printf dans les log genre on a pris l'heuristique pr1
			for(i=(nbjobs-njnp); i<nbjobs; i++) 
			{
				Sequence[i] = ordorestepr1[j]; // ajout du reste à la seq
				j++;
			}
		}
		else if(pnd->bsup == bs_pr2)
		{	//mettre ici un printf dans les log genre on a pris l'heuristique pr2
			for(i=(nbjobs-njnp); i<nbjobs; i++) 
			{
				Sequence[i] = ordorestepr2[j]; // ajout du reste à la seq
				j++;
			}
		}
		else if(pnd->bsup == bs_pr4)
		{	//mettre ici un printf dans les log genre on a pris l'heuristique pr4
			for(i=(nbjobs-njnp); i<nbjobs; i++) 
			{
				Sequence[i] = ordorestepr4[j]; // ajout du reste à la seq
				j++;
			}
		}
		else if(pnd->bsup == bs_pvc)
		{	//mettre ici un printf dans les log genre on a pris l'heuristique pvc
			for(i=(nbjobs-njnp); i<nbjobs; i++) 
			{
				Sequence[i] = ordorestepvc[j]; // ajout du reste à la seq
				j++;
			}
		}
		/* COMPTEURS DES BORNES SUP */
		
		if(((pnd->bsup)==bs_pr1) && ((pnd->bsup)!=bs_pvc) && ((pnd->bsup)!=bs_pr2) && ((pnd->bsup)!=bs_pr4)) {cptsttpr1++;}
		else if(pnd->bsup == bs_pr2 && pnd->bsup!=bs_pvc && pnd->bsup!=bs_pr1 && pnd->bsup!=bs_pr4) {cptsttpr2++;}
		else if(pnd->bsup == bs_pr4 && pnd->bsup!=bs_pvc && pnd->bsup!=bs_pr2 && pnd->bsup!=bs_pr1) {cptsttpr4++;}
		else if(pnd->bsup == bs_pvc && pnd->bsup!=bs_pr4 && pnd->bsup!=bs_pr2 && pnd->bsup!=bs_pr1) {cptsttpvc++;}
		else if(pnd->bsup == bs_pr1 && pnd->bsup==bs_pvc && pnd->bsup!=bs_pr2 && pnd->bsup!=bs_pr4) {cptpr1++; cptpvc++;}
		else if(pnd->bsup == bs_pr2 && pnd->bsup==bs_pvc && pnd->bsup!=bs_pr2 && pnd->bsup!=bs_pr4) {cptpr2++; cptpvc++;}
		else if(pnd->bsup == bs_pr4 && pnd->bsup==bs_pvc && pnd->bsup!=bs_pr2 && pnd->bsup!=bs_pr4) {cptpr4++; cptpvc++;}
		else if(pnd->bsup == bs_pr1 && pnd->bsup==bs_pr2 && pnd->bsup!=bs_pvc && pnd->bsup!=bs_pr4) {cptpr1++; cptpr2++;}
		else if(pnd->bsup == bs_pr1 && pnd->bsup==bs_pr4 && pnd->bsup!=bs_pvc && pnd->bsup!=bs_pr2) {cptpr1++; cptpr4++;}
		else if(pnd->bsup == bs_pr4 && pnd->bsup==bs_pr2 && pnd->bsup!=bs_pvc && pnd->bsup!=bs_pr1) {cptpr4++; cptpr2++;}
		else if(pnd->bsup == bs_pvc && pnd->bsup==bs_pr1 && pnd->bsup==bs_pr2 && pnd->bsup!=bs_pr4) {cptpr1++; cptpr2++; cptpvc++;}
		else if(pnd->bsup == bs_pvc && pnd->bsup==bs_pr1 && pnd->bsup==bs_pr4 && pnd->bsup!=bs_pr2) {cptpr1++; cptpr4++; cptpvc++;}
		else if(pnd->bsup == bs_pvc && pnd->bsup==bs_pr4 && pnd->bsup==bs_pr2 && pnd->bsup!=bs_pr1) {cptpr4++; cptpr2++; cptpvc++;}
		else if(pnd->bsup == bs_pr4 && pnd->bsup==bs_pr1 && pnd->bsup==bs_pr2 && pnd->bsup!=bs_pvc) {cptpr1++; cptpr2++; cptpr4++;}
		else if(pnd->bsup == bs_pr4 && pnd->bsup==bs_pr1 && pnd->bsup==bs_pr2 && pnd->bsup==bs_pvc) {cptpr1++; cptpr2++; cptpr4++; cptpvc++;}

	
	}

	//Affichage de la sequence

	//VERIF for (i=0; i<nbjobs; i++) printf("Sequence : %d\n", Sequence[i]); //la sequence du cbarre à calculé


	//p for(i=0; i<njnp; i++) printf("seq_rest ordo pvc : %d\n", Sequenceresteordopvc[i]);



	/*
	*		CALCUL BORNE SUP = Cbarre associé à la sequence
	***********************************************************************/	



		if (njnp==1)// njnp=1: il reste qu'un seul job à placer
	{
		borninf=pnd->bsup;
		if (logger) fprintf(sortie, "Pas de borne inf a calculer ; Bsup = %d\n", pnd->bsup);
		//VERIF printf("borninf de ce fils : %d\n\n",borninf);
	}
	
	if(pnd->niveau == 0)//si noeud racine on met la bsup trouvé à meilleur cbarre et on met la meilleurSequence à jour
	{
		meilleurCbarre = pnd->bsup;
		printf("\n  bs au noeud racine : %d\n",pnd->bsup);
		for(i=0;i<nbjobs;i++)
		{
			meilleurSequence[i] = Sequence[i];
		}
		
	}
	else
	{
		if (pnd->bsup<meilleurCbarre)
		{
			meilleurCbarre=pnd->bsup;
			NoeudsAvantOpt = cptnoeudsevalues;
			for(i=0;i<nbjobs;i++)
			{
				meilleurSequence[i] = Sequence[i];
			}
		}
	}
	free(Sequence);
	free(ordoreste);
	free(ordorestepvc);
	free(ordorestepr1);
	free(ordorestepr2);
	free(ordorestepr4);
			/*MISE A JOUR DE LA BORNE INF*/
	(pnd->binf) = borninf;

	if (logger) fprintf(sortie, "Binf = %d ;Meilleur Cbarre = %d\n", borninf, meilleurCbarre);
	/*		RETOUR BORNINF              */
	return(borninf);
	
}





/**
 * int developper(pilenoeud, pile, apile)
 *
 * Cette procédure permet de développer et de classer les fils d'un noeud.
 *
 * E:     pilenoeud : pile des noeuds de la PSE.
 *        pile      : indice dans la pile.
 *        apile     : ?
 * PreC:  pilenoeud n'est pas à NULL.
 * 
 * S:     Entier.
 * PostC: La procédure renvoie -1 en cas de coupe, 0 pour une feuille et
 *        1 pour un noeud ordinaire.
 */

int developper(pilenoeud, pile, apile) /*******************************************************************************************************************/
int pile,*apile;
typenoeud *pilenoeud;
{
	/* Variables de traitement. */

	/* Indices. */
	int i, j, k, l, r, iboucle;
	int sigma[NMAX], h, jmin, jpile, numfils, somme;
	/* Tableau des fils générés. */
	typenoeud fils[NMAX];
	int currentM, currentJob;
	int **p, *datesD;



	//VERIF printf("début du noeud : cbarre du noeud = %d  ",pilenoeud[pile].cbarre);		
	h = pilenoeud[pile].niveau; //h = niveau du noeud oui
	//VERIF printf("\n\nniveau du noeud : %d\n", h);

	/* Cas d'une feuille. */

	if (h == nbjobs)
	{
		//VERIF printf("noeud feuille\n");
		miseajourmeilleurCbarre(pilenoeud[pile].d[nbmachines-1], pilenoeud+pile, sigma); // Sigma ne sert à rien.
		*apile = pile;
		return(0);
	}

	/* Cas d'une coupe. */

	if (pilenoeud[pile].binf >= meilleurCbarre)
	{	//VERIF printf("coucoupe\n");
		*apile = pile;
		return(-1);
	}

	/* Calcul des fils. */

	compteur++;

	for (i = 0 ; i < nbjobs-h ; i++)//pour tous les fils
	{
		cptnoeudsevalues++;

		//VERIF printf("-----------------------------------\n");
		//printf("1 cbarre de la seq part du fils : %d\n", fils[i].cbarre);

		numfils = pilenoeud[pile].reste[i]; //numfils sont les indices des job du reste du noeud pere
		//VERIF printf("numéro du fils = %d qui est aussi pilenoeud[%d].reste[%d] qui est aussi sigma[%d]\n", numfils, pile, i, i);
		sigma[i] = numfils;//on met le numéro du fils dans sigma en effet la sequence partielle sera le numéro du fils ?! ca marche pour le 1er niveau en tt cas
		fils[i].niveau = h+1;

		for(j = 0 ; j < h ; j++)
		{
			fils[i].seqpar[j] = pilenoeud[pile].seqpar[j]; //On remplit la sequence partielle de ce quil y avait dans le pere
		}

		fils[i].seqpar[h] = numfils; // on rajoute le nouveau job qui est le numfils

		
		
		//VERIF printf("affichage de la sequence partielle\n");
		/*VERIF for(iboucle=0;iboucle<h+1;iboucle++)
		{
			printf("LA seqpar[%d] = %d\n", iboucle, fils[i].seqpar[iboucle]);
		}  VERIF*/



		/* Calcul des dates de dispo. */

		if( (p = (int**)malloc(nbjobs*sizeof(int*))) == NULL){printf("Pb malloc dates dispo p dans développer\n");}


		for(currentJob=0;  currentJob<nbjobs; currentJob++)
		{
			p[currentJob] = job[currentJob].p;

		}

		for(currentM=0;currentM<nbmachines;currentM++) fils[i].d[currentM] = pilenoeud[pile].d[currentM]; //on recopie les dates de dispo du noeud père
		datesD = GetDatesDisponibilite(1,nbmachines,fils[i].seqpar+h,fils[i].d,p);

		//VERIF for(currentM=0;currentM<nbmachines;currentM++) printf("dates dispo de la machine %d = %d\n", currentM, datesD[currentM]);

		for(currentM=0;currentM<nbmachines;currentM++) fils[i].d[currentM] = datesD[currentM];

		//calcul du cbarre des fils

		fils[i].cbarre = (pilenoeud[pile].cbarre) + datesD[nbmachines-1];
		//VERIF printf("cbarre du noeud = %d  ",pilenoeud[pile].cbarre);		
		//VERIF printf("cbarre de la seq part du fils : %d\n", fils[i].cbarre);

		free(p);
		free(datesD);

		for(j = 0 ; j < i ; j++)          fils[i].reste[j] = pilenoeud[pile].reste[j];
		for(j = i ; j < nbjobs-1-h ; j++) fils[i].reste[j] = pilenoeud[pile].reste[j+1];	
		/*//VERIF 		for(iboucle=0;iboucle<nbjobs-h-1;iboucle++)
		{
			printf("rest[%d] = %d\n", iboucle, fils[i].reste[iboucle]);
		}*/
		
		//pour chaque noeud fils on calcule la borneinf et sup
		borneinfetsuppauline(fils+i, i+1, pilenoeud[pile].binf);
		//VERIF printf("cbarre de la seq part du fils : %d\n", fils[i].cbarre);


	}

	/* Tri des fils : plus grand d'abord ie ordre décroissant. */
	/*   A borne inf égale, meilleure bsup d'abord. */
	//ici on incrémente la pile pour qu'elle continu de developper des noeud fils ... car sinon la pile est à 0 et s'arrête apres les fils du noeud racine.
	//ca remet pile à 1

	/*//VERIF for(j = 0 ; j < nbjobs-h ; j++)
	{
		printf("borne inf de fils %d = %d\n", j, fils[j].binf);
		printf("borne sup de fils %d = %d\n", j, fils[j].bsup);
	}*/
        for (i = 0 , jpile = 0 ; i < nbjobs-h ; i++)
	{
		jmin = 0;

		for(j = 1 ; j < nbjobs-h ; j++)
		{
			if (fils[j].binf > fils[jmin].binf)
			{
				jmin = j;
			}
			else if (fils[j].binf == fils[jmin].binf)
				{
					if (fils[j].bsup < fils[jmin].bsup)
						{jmin = j;}
				}
		}
			//ca remet pile à 1
		//VERIF printf("jmin = %d ", jmin);
		if (fils[jmin].binf < meilleurCbarre)
		{
			copienoeud(fils+jmin, pilenoeud+(pile+jpile));
//			pilenoeud[jpile].cbarre = fils[jmin].cbarre; //le noeud va changer on met actualise le cbarre du noeud
                        //VERIF printf("cbarre pilenoeud : %d\n", pilenoeud[pile+jpile].cbarre);
			jpile++;
		}
		//VERIF else printf("ce noeud est coupé \n");

		fils[jmin].binf = 0; // HIGHVALUE;
	}

	*apile = pile+jpile;

	return(1);
}


/**
 * int pse(fichier)
 *
 * Procédure de contrôle de la PSE.
 *
 * E:     fichier : nom du jeu de données.
 *        n       : indice du jeu de données.
 * PrecC: fichier n'est pas à NULL.
 * 
 * S:     Entier.
 * PostC: La procédure renvoie 0 si tout se passe bien, -1 sinon.
 */

int pse(fichier, n) /**************************************************************************************************************************************/
char *fichier;
int n;
{
	/* Variables de traitement. */

	/* Indice. */
	int i, j;
	/* Variables pour le chronomètre. */
	double start, finish, duration; 
	/* Taille de la pile. */
	int pile;
	/* Pile de la PSE. */
	typenoeud pilenoeud[ZMAX];

	/* Lecture du fichier en cours. */

	if (!lecture_fichier(fichier, job)) 
	{
		return(-1);
	}
	else
	{
		compteur         = 0;
		cptnoeudsevalues = 0;
		cptbsup          = 0;


		/* Calculs sur le noeud racine. : initialisation */

		pilenoeud[0].niveau = 0;

		for (i = 0 ; i < nbmachines ; i++)
		{
			pilenoeud[0].d[i] = 0;
		}
		for (i = 0 ; i < nbjobs ; i++)
		{
			(pilenoeud[0].reste)[i] = i;
		}
		pilenoeud[0].cbarre=0;

		start = clock();

		meilleurCbarre = HIGHVALUE;
		//calcul de la premiere borne inf au noeud racine
		premierebinf = borneinfetsuppauline(pilenoeud + 0, 1,0);

			printf("  binf du noeud racine : %d (%d).\n  Durée d'execution : %.2f.\n", premierebinf, compteur, duration);
			printf("  Ordo :");
			for (i = 0 ; i < nbjobs ; i++) printf("%d ", meilleurSequence[i]); //p Mettre à jour ça

		if (logger) fprintf(sortie, "%4d\t%4d\t", pilenoeud[0].binf, meilleurCbarre);

		pile = 1;
		i = 0;
		//créé les fils d'un noeud
		// Suite de la PSE : calcul des fils

		while((pile < ZMAX) && (pile > 0))
		{
			pile--;
			//VERIF printf("PSE : cbarre pilenoeud[%d] : %d\n", pile, pilenoeud[pile].cbarre);

		/*VERIF	for(i=0;i<((pilenoeud[pile].niveau));i++)
		{
			printf("seqpar[%d] = %d\n", i, pilenoeud[pile].seqpar[i]);
		}
			for(i=0;i<(nbjobs-(pilenoeud[pile].niveau));i++)
		{
			printf("reste[%d] = %d\n", i, pilenoeud[pile].reste[i]);
		}*/

			
			i = developper(pilenoeud,pile,&pile); //suite de la PSE en elle-même
		}

		if (pile>=ZMAX)
		{
			if (logger) fprintf(sortie,"%d\t%4d\t%4d\n",0,meilleurCbarre,compteur);
			printf("Pile pleine %4d %4d!\n",meilleurCbarre,compteur);
		}
		else
		{
			finish = clock();
			duration = (double)(finish - start) / CLOCKS_PER_SEC;
			j = meilleurCbarre - premierebinf;
			if (j < 0) j = 0;

			printf("  Optimal : %d (%d).\n  Durée d'execution : %.2f.\n", meilleurCbarre, compteur, duration);
			printf("  Ordo : ");
			for (i = 0 ; i < nbjobs ; i++) printf("%d ", meilleurSequence[i]);
			printf("\n");
			printf("  Durée avant de trouver l'optimal : %.2f.\n", (double)(chronobsup[cptbsup-1]-start)/CLOCKS_PER_SEC);
			printf("  Ecart entre la première borne inf et l'optimal : %d.\n", j);
			printf("  Nombre de noeuds avant l'optimal : %d.\n", NoeudsAvantOpt);
			printf("  Nombre de noeuds total : %d\n", cptnoeudsevalues);
			printf(" Compteurs : \n");
			if(bsuppr1=1) printf("  cptsttpr1 : %d\n", cptsttpr1);
			if(bsuppr2=1) printf("  cptsttpr2 : %d\n", cptsttpr2);
			if(bsuppr4=1) printf("  cptsttpr4 : %d\n", cptsttpr4);
			if(bsuppvc=1) printf("  cptsttpvc : %d\n", cptsttpvc);
			if(bsuppr1=1) printf("  cptpr1 : %d\n", cptpr1);
			if(bsuppr2=1) printf("  cptpr2 : %d\n", cptpr2);
			if(bsuppr4=1) printf("  cptpr4 : %d\n", cptpr4);
			if(bsuppvc=1) printf("  cptpvc : %d\n", cptpvc);
			if (logger)
			{
				fprintf(sortie,"%d\t%4d\t%4d\n",1,meilleurCbarre,compteur);
				fprintf(sortie, "\n\n***************************************\n\nFin de la PSE.");
				fprintf(sortie, "  Optimal : %d (%d).\n  Durée d'execution : %4f.\n",meilleurCbarre,compteur,duration);
				fprintf(sortie, "  Ordo : ");
				for (i = 0 ; i < nbjobs ; i++) fprintf(sortie, "%d ", meilleurSequence[i]);
				fprintf(sortie, "\n");
				fprintf(sortie, "  Durée avant de trouver l'optimal : %.2f.\n", (double)(chronobsup[cptbsup-1]-start)/CLOCKS_PER_SEC);
				fprintf(sortie, "  Ecart entre la première borne inf et l'optimal : %d.\n", j);
			}

			if (excel)
			{
				printf("PreBinf:%d\n", premierebinf);
				fprintf(sortieexcel,"V_f_A:,%d,%d,%4f,%d,%d,%d,%d,\n",  meilleurCbarre, premierebinf, duration,compteur,cptnoeudsevalues,NoeudsAvantOpt,j);
				printf("V_f_A:,%d,%4f,%d,%d,%d,%d\n",  meilleurCbarre,duration,compteur,cptnoeudsevalues,NoeudsAvantOpt,j);
			}

			durees[n]       = duration;
			dureesbsup[n]   = (double)(chronobsup[cptbsup-1]-start)/CLOCKS_PER_SEC;
			nbnoeudsdev[n]  = compteur;
			nbnoeudseval[n] = cptnoeudsevalues;
			ecartsbinf[n]   = j;
		}

		return(0);
	}
}


/**
 * int main()
 *
 * Procédure principale du programme. Lance la PSE et écrit les résultats dans le fichier de sortie.
 *
 * E:     Néant.
 * PrecC: Néant.
 * 
 * S:     Entier.
 * PostC: La procédure renvoie 1 si tout se passe bien.
 */

int main(int argc, char **argv) /**************************************************************************************************************************/
{
	/* Variables de traitement. */

	/* Indice. */
	int i, j;
	/* Booléens pour les arguments. */
	int argm;
	/* Variables pour les fichiers. */
	struct dirent *lecture;
	DIR *rep;
	
	char dossier[LENGTHMAX];
	char fichier[LENGTHMAX];
	char dossierlog[LENGTHMAX];
	char fichierlog[LENGTHMAX];
	char dossierout[LENGTHMAX];
	char fichierout[LENGTHMAX];
	char dossierexcel[LENGTHMAX];
	char fichierexcel[LENGTHMAX];
	/* Moyennes. */
	double moyennetemps, moyennetempsbsup, moyennenoeudsdev, moyennenoeudseval, moyenneecarts;

	/* Initialisations. */

	logger = 0;
	output = 0;
	argm   = 0;
	excel  = 0;
	
	// by default it must be saved in excel files
	output = 1;
	strcpy(dossierout, "../excel/");
	/* Arguments. */

	if (strcmp(argv[1], "--help") == 0)
	{
		printf("L'application « fdminmax » exécute la PSE résolvant le problème d'ordonnance-\n");
		printf("ment « Fm|délais min-max|Cmax ».\n\n");

		printf("Usage: fdminmax <-f|-m> <chemin> [OPTION]...\n\n");

		printf("Exemples:\n");
		printf("  fdminmax -f data/data10_10/fd_1\n");
		printf("    # Lance la PSE sur le fichier spécifié en argument.\n");
		printf("  fdminmax -m data/benchmarks/classe07 -l log\n");
		printf("    # Lance la PSE sur le groupe de  fichiers situé dans le dossier spécifié\n");
		printf("    # en argument.  On précise ici de plus que les logs d'exécutions doivent\n");
		printf("    # être placés dans le dossier « log ».\n\n");

		printf("Les 2 premiers arguments sont obligatoires, les suivants sont optionnels.\n\n");

		printf("Arguments obligatoires:\n");
		printf("  <-f|-m>         précise si le programme va  s'exécuter sur un seul ou plu-\n");
		printf("                  sieurs fichiers\n");
		printf("  <chemin>        chemin du jeu de données ou du dossier de jeux de données.\n\n");

		printf("Argument optionnel:\n");
		printf("  -l <chemin>     Un fichier par jeu de données  sera généré dans le dossier\n");
		printf("                  spécifié en argument. Ces derniers permettront de retracer\n");
		printf("                  le cheminement de la PSE dans l'arbre de recherche.\n");

		printf("\nRapporter toutes anomalies à <vincent.augusto@gmail.com>.\n");
		exit(0);
	}

	for (i = 0 ; i < argc ; i++)
	{
		if (strcmp(argv[i], "-f") == 0)
		{
			argm = 0;
			strcpy(fichier, argv[i+1]); 
		}
		if (strcmp(argv[i], "-m") == 0)
		{
			argm = 1;
			strcpy(dossier, argv[i+1]); 
		}
		if (strcmp(argv[i], "-l") == 0)
		{
			logger = 1;
			strcpy(dossierlog, argv[i+1]);
		}
		if (strcmp(argv[i], "-o") == 0)
		{
			output = 1;
			strcpy(dossierout, argv[i+1]);
		}
		if (strcmp(argv[i], "-e") == 0)
		{
			excel = 1;
			strcpy(dossierexcel, argv[i+1]);
		}
	}

	/* Affichage. */

	printf("***************************************\n");
	printf("* Resolution du Fm|delai min-max|Cmax *\n");
	printf("***************************************\n\n");

	if (argm)		/* Mode fichiers multiples. */
	{
		rep = opendir(dossier);

		i = 0;

		while (lecture = readdir(rep))
		{
			if ((strcmp(lecture->d_name, ".") != 0) && (strcmp(lecture->d_name, "..") != 0))
			{
				strcpy(fichier, dossier);
				strcat(fichier, "/");
				strcat(fichier, lecture->d_name);

				if (logger)
				{
					strcpy(fichierlog, dossierlog);
					FILE * i = fopen(fichierlog,"rt");	
					if(i == NULL)
						mkdir("log",S_IRWXU);
					else
						fclose(i);
					strcat(fichierlog, "/fd-out-");
					strcat(fichierlog, lecture->d_name);
					sortie = fopen(fichierlog, "wt");
				}

				if (output)
				{
					strcpy(fichierout, dossierout);
					FILE * i = fopen(fichierout,"rt");	
					if(i == NULL)
						mkdir("latex",S_IRWXU);
					else
						fclose(i);
					strcat(fichierout, "/fd-out-");
					strcat(fichierout, lecture->d_name);
					strcat(fichierout, ".tex");
					latex = fopen(fichierout, "wt");
				}

				if (excel)
				{	
					strcpy(fichierexcel, dossierexcel);
					FILE * ii = fopen(fichierexcel,"rt");	
					if(ii == NULL)
						mkdir("excel",S_IRWXU);
					else
						fclose(ii);
					strcat(fichierexcel, "/fd-out-");
					strcat(fichierexcel, dossier+strlen(dossier)-8);
					strcat(fichierexcel, ".csv");
					sortieexcel = fopen(fichierexcel, "at");
					if(i == 0)
					{
						fprintf(sortieexcel,"Nom_C,Nom_F,Nom_M,OPT,P_Binf,Duree,N_Dev,N_Eva,N_A_OPT,Ecart\n");
					}
					fprintf(sortieexcel,"Classe%c%c, %s, ",dossier[strlen(dossier)-2],dossier[strlen(dossier)-1], lecture->d_name );
				}

				printf("Fichier %s :\n", fichier);

				if (pse(fichier, i) == -1)
				{
					printf("Erreur de lecture.\n");
				}

				
				if (logger) fclose(sortie);
				if (excel) fclose(sortieexcel);

				i++;
			}
		}
		closedir(rep);
		if (output) fclose(latex);
		/* Statistiques. */

		moyennetemps      = 0;
		moyennetempsbsup  = 0;
		moyennenoeudsdev  = 0;
		moyennenoeudseval = 0;
		moyenneecarts     = 0;
		for (j = 0 ; j < i ; j++)
		{
			moyennetemps      += durees[j];
			moyennetempsbsup  += dureesbsup[j];
			moyennenoeudsdev  += nbnoeudsdev[j];
			moyennenoeudseval += nbnoeudseval[j];
			moyenneecarts     += ecartsbinf[j];
		}

		if(logger)
		{
			strcpy(fichierlog, dossierlog);
			strcat(fichierlog, "/fd-out-");
			strcat(fichierlog, "fin");
			sortie = fopen(fichierlog, "wt");
		}

		printf("\nTemps moyen d'exécution (s) :\t\t%.2f", moyennetemps / i);
		if (logger) fprintf(sortie, "\n  Temps moyen d'exécution (s) :\t\t%.2f", moyennetemps / i);

		printf("\nTemps moyen avant de trouver l'optimal (s) :\t\t%.2f", moyennetempsbsup / i);
		if (logger) fprintf(sortie, "\n  Temps moyen avant de trouver l'optimal (s) :\t\t%.2f", moyennetempsbsup / i);

		printf("\nNombre de noeuds moyen développés :\t%d", (int)rint(moyennenoeudsdev / i));
		if (logger) fprintf(sortie, "\n  Nombre de noeuds moyen développés :\t%d", (int)rint(moyennenoeudsdev / i));

		printf("\nNombre de noeuds moyen évalués :\t%d\n", (int)rint(moyennenoeudseval / i));
		if (logger) fprintf(sortie, "\n  Nombre de noeuds moyen évalués :\t%d\n", (int)rint(moyennenoeudseval / i));

		printf("\nEcart moyen première binf - optimal :\t%d\n", (int)rint(moyenneecarts / i));
		if (logger) fprintf(sortie, "\n  Ecart moyen première binf - optimal :\t%d\n", (int)rint(moyenneecarts / i));
		
		if(logger) fclose(sortie);
	}
	else			/* Mode fichier unique. */
	{
		printf("Mode FICHIER UNIQUE\n");
		
		if (logger)
		{printf("sortie logger\n");
			strcpy(fichierlog, dossierlog);
			strcat(fichierlog, "/fd-out");
			sortie = fopen(fichierlog, "wt");
		}

		if (output)
		{
			strcpy(fichierout, dossierout);
			strcat(fichierout, "fd-out");
			strcat(fichierout, ".csv");
			latex = fopen(fichierout, "wt");
			
			
			fprintf(latex,"File;Machine;Job;Opt;1st_LB;Gap_LB;Gap_LB%;RPI_TSP;RPI_PR1;RPI_PR2;RPI_PR4;Opt.Seq;\n");
		}
		
		if (excel)
		{	printf("sortie excel\n");		
			strcpy(fichierexcel, dossierexcel);
			strcat(fichierexcel, "/fd-out");
			strcat(fichierexcel, ".csv");
			sortieexcel = fopen(fichierexcel, "at");
		}	
	
		if (pse(fichier, 0) == -1) //C'est ici que le programme s'exécute lors d'un fichier unique!!!
		{
			printf("Erreur de lecture.\n");
		}

		if (output) fclose(latex);
		if (logger) fclose(sortie);
		if (excel)  fclose(sortieexcel);
	}

	return(1);
}
