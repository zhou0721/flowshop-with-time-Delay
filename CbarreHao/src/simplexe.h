#include <stdlib.h>
#include <math.h>
#include <glpk.h>

int calculparametres(int* a, int* b, int* alpha, int* beta, int nbmachines, int log)
{
	/* Variables de traitement. */

	LPX *lp;
	int i, j, k, cpt, somme, nblignes, ligne;
	int ia[1+10000], ja[1+10000];
	double ar[1+10000], Z;
	char buf[50];

	lp = lpx_create_prob();
	lpx_set_prob_name(lp, "calculparametres");
	lpx_set_obj_dir(lp, LPX_MAX);

	lpx_set_int_parm(lp, LPX_K_MSGLEV, 0);

	/* Lignes. */

	lpx_add_rows(lp, nbmachines*nbmachines - (2*nbmachines-1));
	cpt = 1;
	/* Au-dessus de la diagonale. */
	for (i = 0 ; i < nbmachines-1 ; i++)
	{
		for (j = 0 ; j <= i ; j++)
		{
			somme = 0;

			for (k = j ; k <= nbmachines-i+j-2 ; k++)
			{
				somme += alpha[k];
			}

			sprintf(buf, "%d", cpt);
			lpx_set_row_name(lp, cpt, buf);
			lpx_set_row_bnds(lp, cpt, LPX_UP, 0, somme);
			cpt++;
		}
	}
	/* En-dessous de la diagonale. */
	for (i = 0 ; i < nbmachines-2 ; i++)
	{
		for (j = 0 ; j <= i ; j++)
		{
			somme = 0;

			for (k = j ; k <= nbmachines-i+j-2 ; k++)
			{
				somme += -beta[k];
			}

			sprintf(buf, "%d", cpt);
			lpx_set_row_name(lp, cpt, buf);
			lpx_set_row_bnds(lp, cpt, LPX_UP, 0, somme);
			cpt++;
		}
	}

	/* Colonnes. */

	lpx_add_cols(lp, 2*nbmachines-1);
	for (i = 0 ; i < nbmachines-1 ; i++)
	{
		sprintf(buf, "%d", i+1);
		strcat(buf, "a");
		lpx_set_col_name(lp, i+1, buf);
		lpx_set_col_bnds(lp, i+1, LPX_LO, beta[i], 0);
		lpx_set_obj_coef(lp, i+1, 0);
	}
	for (i = 0 ; i < nbmachines ; i++)
	{
		sprintf(buf, "%d", i+1);
		strcat(buf, "b");
		lpx_set_col_name(lp, nbmachines+i, buf);
		lpx_set_col_bnds(lp, nbmachines+i, LPX_LO, 0, 0);
		lpx_set_obj_coef(lp, nbmachines+i, -1);
	}

	/* Coefficients. */

	cpt   = 1;
	ligne = 1;
	/* Au-dessus de la diagonale. */
	for (i = 0 ; i < nbmachines-1 ; i++)
	{
		for (j = 0 ; j <= i ; j++)
		{
			for (k = j ; k <= nbmachines-i+j-2 ; k++)
			{
				ia[cpt] = ligne, ja[cpt] = k+1, ar[cpt] = 1;
//				printf("%d : %d,%d\t1\n",cpt,i+j+1,k+1);
				cpt++;
			}
			for (k = j ; k <= nbmachines-i+j-1 ; k++)
			{
				ia[cpt] = ligne, ja[cpt] = k+nbmachines, ar[cpt] = -1;
//				printf("%d : %d,%d\t-1\n",cpt,i+j+1,k+nbmachines);
				cpt++;
			}
			ligne++;
		}
	}

//	nblignes = ((nbmachines-1)*nbmachines)/2;
	/* En-dessous de la diagonale. */
	for (i = 0 ; i < nbmachines-2 ; i++)
	{
		for (j = 0 ; j <= i ; j++)
		{
			for (k = j ; k <= nbmachines-i+j-2 ; k++)
			{
				ia[cpt] = ligne, ja[cpt] = k+1, ar[cpt] = -1;
//				printf("%d : %d,%d\t-1\n",cpt,i+j+1,k+1);
				cpt++;
			}
			for (k = j+1 ; k <= nbmachines-i+j-2 ; k++)
			{
				ia[cpt] = ligne, ja[cpt] = k+nbmachines, ar[cpt] = 1;
//				printf("%d : %d,%d\t1\n",cpt,i+j+1,k+nbmachines);
				cpt++;
			}
			ligne++;
		}
	}

	lpx_load_matrix(lp, cpt-1, ia, ja, ar);
	lpx_simplex(lp);

	Z = lpx_get_obj_val(lp);

	if (log) printf("%f %f %f %f %f\n", lpx_get_col_prim(lp, 1), lpx_get_col_prim(lp, 2), lpx_get_col_prim(lp, 3), lpx_get_col_prim(lp, 4), lpx_get_col_prim(lp, 5));

	cpt = 1;
	for (i = 0 ; i < nbmachines-1 ; i++)
	{
		a[i] = lpx_get_col_prim(lp, cpt);
		cpt++;
	}
	for (i = 0 ; i < nbmachines ; i++)
	{
		b[i] = lpx_get_col_prim(lp, cpt);
		cpt++;
	}

	lpx_delete_prob(lp);

	return 1;
}
