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

void cdt(int *n, int *ordx, int *x, int *maxnd, int *inf, float *alpha, int *zeur, int *binf, int *fstar, int *lb0, int *lbc, int *nexp, int *nprobq, int *nass, int *active, int *lopt, float *spars, float *avson, int *err);


int max(a, b)
int a, b;
{
	if (a > b) return a;
	else return b;
}
/*
inline int max(a, b)
int a, b;
{
	(a < b) ? return b : return a;
}
*/
int atsp(nbj, p, dist, sequence, d, nbmachines, log)
int nbj, **p, **dist, *sequence, *d, nbmachines;
FILE* log;
{
	/* Variables de traitement. */

	int i, j, k, binf, ii, jj, somme;
	int term1, term2, term3;
	int *term;
	int modificateur, modiftotal;
	int *x, *fstar;
	/* Pour l'ATSP. */
	int n, ordx, maxnd, inf, zeur, lb0, lbc, err, lopt, active, nass, nprobq, nexp;
	float alpha, avson, spars;

	/* Initialisations pour l'ATSP. */

	n = nbj+1;
	ordx = 10000;
	inf = 99999999;
	alpha = -1;
	maxnd = -1;
	zeur = -1;
	if(log)printf("nbj=%d\n", nbj);

	/* Allocations. */
	x     = (int *)calloc(ordx,sizeof(int));
	fstar = (int *)calloc(n,sizeof(int));
	term  = (int *)calloc(nbmachines,sizeof(int));

	/* Modification des durées opératoires afin qu'elles soient toutes positives. */

	if (log) printf("ATSP\n");

	modiftotal = 0;

	for (i = 0 ; i <= nbj ; i++)
	{
		modificateur = 0;

		for (j = 0 ; j < nbmachines ; j++)
		{
			if (p[i][j] + modificateur < 0)
			{
				modificateur = - p[i][j];
			}
		}

		for (j = 0 ; j < nbmachines-1 ; j++)
		{
			p[i][j]    += modificateur;
			dist[i][j] -= modificateur;
		}
		p[i][nbmachines-1] += modificateur;

		modiftotal += modificateur;
	}

	/* Distances. */

	i = 0;
	for (j = 0 ; j <= nbj ; j++)
	{
		for (k = 0 ; k <= nbj ; k++)
		{
			if (k != j)
			{
				term[0] = p[k][0];
//				term[0] = p[j][0];
				for (ii = 1 ; ii < nbmachines ; ii++)
				{
					term[ii] = 0;

					for (jj = 1 ; jj <= ii ; jj++)
					{
						term[ii] += p[j][jj] + dist[j][jj-1] - p[k][jj-1] - dist[k][jj-1];
					}
				}

/*				term1 = ado[k];
				term2 = bdo[j] + dista[j] - ado[k] - dista[k];
				term3 = bdo[j] + dista[j] + cdo[j] + distb[j] - ado[k] - dista[k] - bdo[k] - distb[k];
*/
				x[i] = 0;
				for (ii = 1 ; ii < nbmachines ; ii++)
				{				
					x[i] = max(x[i], term[ii]);
				}
				x[i] += term[0];

				if (log) printf("%d", x[i]);
			}
			else
			{
				x[i] = HIGHVALUE;
				if (log) printf("%d", HIGHVALUE);
			}
			
			if (k == nbj)
			{
				i = j+1;
				if (log) printf("\n");
			}
			else
			{
				i += (nbj+1);
				if (log) printf("\t");
			}
		}
	}

//	char input = (char)getchar();

	/* Exécution de l'ATSP. */

	cdt(&n, &ordx, x, &maxnd, &inf, &alpha, &zeur, &binf, fstar, &lb0, &lbc, &nexp, &nprobq, &nass, &active, &lopt, &spars, &avson, &err);

	if (log)
	{
		printf("Sequence renvoyee par l'ATSP : ");
		for (i = 0 ; i <= nbj ; i++)
		{
			printf("%d ",fstar[i]);
		}
		printf("\n");
	}

	/* Récupération de la séquence. */

	sequence[0] = fstar[nbj];

	for (i = 1 ; i < nbj ; i++)
	{
		sequence[i] = fstar[sequence[i-1]-1];
	}

	if (log)
	{
		printf("Sequence modifiee : ");
		for (i = 0 ; i < nbj ; i++)
		{
			printf("%d ",sequence[i]);
		}
		printf("\n");
	}

	/* La borne inférieure est retournée. */

	if (log) fprintf(log,"%d\n", binf);
	if (log) fprintf(log,"%d\n", binf - modiftotal);

	binf = binf - modiftotal;

/*	modiftotal = 0;

	for (i = 0 ; i < nbmachines ; i++)
	{
		somme = 0;
		for (j = 0 ; j < i ; j++)
		{
			somme += - p[sequence[0]-1][j-1] - dist[sequence[0]-1][j-1];
		}
		modiftotal = max(modiftotal, d[i] + somme);
	}

	binf += modiftotal;*/




/*	binf = binf - modiftotal + max(d1, d2 - (ado[sequence[0]-1] + dista[sequence[0]-1]),
		d3 - (ado[sequence[0]-1] + dista[sequence[0]-1] + bdo[sequence[0]-1] + distb[sequence[0]-1]));
*/
	/* Désallocations. */

	free(fstar);
	free(x);
	free(term);

	return binf; /*- modiftotal;*/
}

int monmin(x,y)
int x,y;
{ 
	return (x<y ? x : y);
}

int monmax(x,y)
int x,y;
{ 
	return (x>y ? x : y);
}

