
extern int nbmachines;

/**
 * int cmaxF3d(nbj, job, c1ini, c2ini, c3ini, sigma, liste)
 *
 * Cette procédure permet le calcul d'une borne supérieure.
 *
 * E:     nbj   : nombre de travaux dans l'ordonnancement.
 *        job   : travaux de l'ordonnancement.
 *        c1ini : date de dispo de la machine 1.
 *        c2ini : resp. 2.
 *        c3ini : resp. 3.
 *        sigma : séquence donnée lors du calcul de la borne inférieure.
 *        liste : liste de travaux non ordonnancés.
 * PrecC: nbj > 0 et job non vide et sigma non vide.
 * 
 * S:     Entier.
 * PostC: Le calcul de la borne supérieure a réussi et la fonction renvoie le borne en question.
 */

int cmaxF3d(nbj, job, cini, sigma, liste) /***************************************************************************************************************/
int nbj, *cini, *sigma, *liste;
typejobF3d *job;
{
	/* Variables de traitement. */
	int i, j, k, l, r, somme;
	int *c, bsup;

	/* Calcul du F3|delai|Cmax sur j -> bsup */

	c = (int *)calloc(nbmachines, sizeof(int));

	for (i = 0 ; i < nbmachines ; i++)
	{
		c[i] = cini[i];
	}

	for (i = 0 ; i < nbj ; i++)
	{
		j = liste[sigma[i]-1];

		c[0] = monmax(c[0] + job[j].p[0] , c[1] - job[j].beta[0]);

		for (k = 2 ; k < nbmachines ; k++)
		{
			somme = - job[j].beta[0];;
			for (l = 1 ; l < k ; l++)
			{
				somme += - job[j].p[l] - job[j].beta[l];
			}

			c[0] = monmax(c[0], c[k] + somme);
		}
//		printf("c1 = %d\t", c[0]);

		for (r = 1 ; r < nbmachines-1 ; r++)
		{
			c[r] = monmax(c[r-1] + job[j].alpha[r-1] + job[j].p[r], c[r] + job[j].p[r]);
			c[r] = monmax(c[r], c[r+1] - job[j].beta[r]);
			for (k = r+1 ; k < nbmachines ; k++)
			{
				somme = -job[j].beta[r];
				for (l = r ; l < k ; l++)
				{
					somme += - job[j].p[l+1] - job[j].beta[l+1];
				}

				c[r] = monmax(c[r], c[k] + somme);
			}
//			printf("c%d = %d\t", r+1,c[r]);
		}
//		printf("\n");

		c[nbmachines-1] = monmax(c[nbmachines-2]+job[j].alpha[nbmachines-2]+job[j].p[nbmachines-1],
					 c[nbmachines-1]+job[j].p[nbmachines-1]);

/*		c1 = monmax( monmax(c1 + job[j].p[0] , c2 - job[j].beta[0]) , c3 - job[j].beta[0] - job[j].p[1] - job[j].beta[1]);
		c2 = monmax( monmax(c1 + job[j].alpha[0] , c2) + job[j].p[1] , c3 - job[j].beta[1]);
		c3 = monmax(c3,c2 + job[j].alpha[1]) + job[j].p[2];*/
//		printf("c1 = %d\tc2 = %d\tc3 = %d\n", c1, c2, c3);
	}

	bsup = c[nbmachines-1];
	free(c);

	return(bsup);
}


/**
 * int neh(nbj, job, c1ini, c2ini, c3ini, sigma, liste)
 *
 * Cette procédure permet le calcul d'une borne supérieure suivant l'heuristique NEH.
 *
 * E:     nbj   : nombre de travaux dans l'ordonnancement.
 *        job   : travaux de l'ordonnancement.
 *        c1ini : date de dispo de la machine 1.
 *        c2ini : resp. 2.
 *        c3ini : resp. 3.
 *        sigma : séquence donnée lors du calcul de la borne inférieure.
 *        liste : liste de travaux non ordonnancés.
 * PrecC: nbj > 0 et job non vide et sigma non vide.
 * 
 * S:     Entier.
 * PostC: Le calcul de la borne supérieure a réussi et la fonction renvoie le borne en question.
 */

int neh(nbj, job, cini, sigma, liste) /****************************************************************************************************/
int nbj, *cini, *sigma, *liste;
typejobF3d *job;
{
	/* Variables de traitement. */
	int i, j, k, l, m, c1, c2, c3, cpt, *c, ii, jj, kk, ll, somme, r;
	int ordopartiel[NMAX], ordopartielmodif[NMAX], meilleurordo[NMAX], meilleurcmax;

	c = (int *)calloc(nbmachines, sizeof(int));

	/**
	 * Exécution de l'heuristique.
	 *   On considère que le classement des travaux donné par la borne inférieure est valide.
	 */

//	printf("c1 = %d\tc2 = %d\tc3 = %d\n", c1, c2, c3);

	/* Placement du premier travail. */

	ordopartiel[0] = liste[sigma[0]-1];

//	printf("%d - ", ordopartiel[0]);

	/* Cette première boucle parcourt la liste de travaux triées. */

	for (i = 1 ; i < nbj ; i++)
	{
		meilleurcmax = 32000;

		j = liste[sigma[i]-1];

		/* Cette deuxième boucle permet le test du travail suivant à toutes les positions. */

		for (k = 0 ; k <= i ; k++)
		{
			/* Création de l'ordo partiel modifié (par insertion du nouveau travail à sa place). */

			cpt = 0;
			for (l = 0 ; l < k ; l++) { ordopartielmodif[l] = ordopartiel[l]; cpt++; }
			ordopartielmodif[cpt] = j;
			for (l = cpt+1 ; l <= i ; l++) { ordopartielmodif[l] = ordopartiel[cpt]; cpt++; }

//			for (l = 0 ; l <= i ; l++) printf("%d ; ", ordopartielmodif[l]);

			/* Calcul du Cmax associé à cet ordo partiel modifié. */

			for (ii = 0 ; ii < nbmachines ; ii++)
			{
				c[ii] = cini[ii];
			}

//			char input = (char)getchar();
			for (ii = 0 ; ii <= i ; ii++)
			{
				jj = ordopartielmodif[ii];

				c[0] = monmax(c[0] + job[jj].p[0] , c[1] - job[jj].beta[0]);

				for (kk = 2 ; kk < nbmachines ; kk++)
				{
					somme = - job[jj].beta[0];;
					for (ll = 1 ; ll < kk ; ll++)
					{
						somme += - job[jj].p[ll] - job[jj].beta[ll];
					}
		
					c[0] = monmax(c[0], c[kk] + somme);
				}
//				printf("c1 = %d\t", c[0]);

				for (r = 1 ; r < nbmachines-1 ; r++)
				{
					c[r] = monmax(c[r-1] + job[jj].alpha[r-1] + job[jj].p[r], c[r] + job[jj].p[r]);
					c[r] = monmax(c[r], c[r+1] - job[jj].beta[r]);
					for (kk = r+1 ; kk < nbmachines ; kk++)
					{
						somme = -job[jj].beta[r];
						for (ll = r+1 ; ll < kk ; ll++)
						{
							somme += - job[jj].p[ll] - job[jj].beta[ll];
						}
		
						c[r] = monmax(c[r], c[kk] + somme);
					}
//					printf("c%d = %d\t", r+1,c[r]);
				}
//				printf("\n");

				c[nbmachines-1] = monmax(c[nbmachines-2]+job[jj].alpha[nbmachines-2]+job[jj].p[nbmachines-1],
							 c[nbmachines-1]+job[jj].p[nbmachines-1]);
			}


			/*	c1 = monmax(monmax(c1 + job[m].p[0] , c2 - job[m].beta[0]) , c3 - job[m].beta[0] - job[m].p[1] - job[m].beta[1]);
				c2 = monmax(monmax(c1 + job[m].alpha[0] , c2) + job[m].p[1] , c3 - job[m].beta[1]);
				c3 = monmax(c3,c2 + job[m].alpha[1]) + job[m].p[2];
*/

//			printf("%d", c3);

			if (c[nbmachines-1] < meilleurcmax)
			{
				meilleurcmax = c[nbmachines-1];
				for (l = 0 ; l <= i ; l++)
				{
					meilleurordo[l] = ordopartielmodif[l];
				}
			}
		}

		/* MAJ de l'ordo partiel. */

		for (k = 0 ; k <= i ; k++)
		{
			ordopartiel[k] = meilleurordo[k];
		}

	}

	free(c);

	return(meilleurcmax);
}


int neh2(nbj, job, cini, sigma, liste) /****************************************************************************************************/
int nbj, *cini, *sigma, *liste;
typejobF3d *job;
{
	/* Variables de traitement. */
	int i, j, k, l, m, c1, c2, c3, cpt, r, ii, jj, kk, ll, *c, somme;
	int ordopartiel[NMAX], ordopartielmodif[NMAX], meilleurordo[NMAX], meilleurcmax;

	int sommepi[NMAX], liste2[NMAX];

	c = (int *)calloc(nbmachines, sizeof(int));

	/**
	 * Classement des travaux.
	 */

	for (i = 0 ; i < nbj ; i++)
	{
		sommepi[i] = 0;
		for (j = 0 ; j < nbmachines ; j++)
		{
			sommepi[i] += job[liste[sigma[i]-1]].p[j];
		}
	}

	for (i = 0 ; i < nbj ; i++)
	{
		m = 0;

		for(j = 0 ; j < nbj ; j++)
		{
			if (sommepi[j] <= sommepi[m])
			{
				m = j;
			}
		}

		sommepi[m] = HIGHVALUE;
		liste2[i] = m;
	}

	/**
	 * Exécution de l'heuristique.
	 */

//	printf("c1 = %d\tc2 = %d\tc3 = %d\n", c1, c2, c3);

	/* Placement du premier travail. */

	ordopartiel[0] = liste2[0];

//	printf("%d - ", ordopartiel[0]);

	/* Cette première boucle parcourt la liste de travaux triées. */

	for (i = 1 ; i < nbj ; i++)
	{
		meilleurcmax = 32000;

		j = liste2[i];

		/* Cette deuxième boucle permet le test du travail suivant à toutes les positions. */

		for (k = 0 ; k <= i ; k++)
		{
			/* Création de l'ordo partiel modifié (par insertion du nouveau travail à sa place). */

			cpt = 0;
			for (l = 0 ; l < k ; l++) { ordopartielmodif[l] = ordopartiel[l]; cpt++; }
			ordopartielmodif[cpt] = j;
			for (l = cpt+1 ; l <= i ; l++) { ordopartielmodif[l] = ordopartiel[cpt]; cpt++; }

//			char input = (char)getchar();
//			for (l = 0 ; l <= i ; l++) printf("%d ; ", ordopartielmodif[l]);

			/* Calcul du Cmax associé à cet ordo partiel modifié. */

			for (ii = 0 ; ii < nbmachines ; ii++)
			{
				c[ii] = cini[ii];
//				printf("%d\n", c[ii]);
			}


			for (ii = 0 ; ii <= i ; ii++)
			{
				jj = ordopartielmodif[ii];

				/* c1 = Max (c1 + p1, c2 - beta1, c3 - beta2 - p2 - beta1, etc...) */

				c[0] = monmax(c[0] + job[jj].p[0] , c[1] - job[jj].beta[0]);

				for (kk = 2 ; kk < nbmachines ; kk++)
				{
					somme = - job[jj].beta[0];
					for (ll = 1 ; ll < kk ; ll++)
					{
						somme += - job[jj].p[ll] - job[jj].beta[ll];
					}
		
					c[0] = monmax(c[0], c[kk] + somme);
				}
//				printf("c1 = %d\t", c[0]);

				for (r = 1 ; r < nbmachines-1 ; r++)
				{
					c[r] = monmax(c[r-1] + job[jj].alpha[r-1] + job[jj].p[r], c[r] + job[jj].p[r]);
					c[r] = monmax(c[r], c[r+1] - job[jj].beta[r]);
					for (kk = r+1 ; kk < nbmachines ; kk++)
					{
						somme = -job[jj].beta[r];
						for (ll = r+1 ; ll < kk ; ll++)
						{
							somme += - job[jj].p[ll] - job[jj].beta[ll];
						}
		
						c[r] = monmax(c[r], c[kk] + somme);
					}
//					printf("c%d = %d\t", r+1,c[r]);
				}
//				printf("\n");

				c[nbmachines-1] = monmax(c[nbmachines-2]+job[jj].alpha[nbmachines-2]+job[jj].p[nbmachines-1],
							 c[nbmachines-1]+job[jj].p[nbmachines-1]);
			}

/*			c1 = c1ini;
			c2 = c2ini;
			c3 = c3ini;

			for (l = 0 ; l <= i ; l++)
			{
				m = ordopartielmodif[l];
				c1 = monmax(monmax(c1 + job[m].p[0] , c2 - job[m].beta[0]) , c3 - job[m].beta[0] - job[m].p[1] - job[m].beta[1]);
				c2 = monmax(monmax(c1 + job[m].alpha[0] , c2) + job[m].p[1] , c3 - job[m].beta[1]);
				c3 = monmax(c3,c2 + job[m].alpha[1]) + job[m].p[2];
			}
*/
//			printf("%d\n", c[nbmachines-1]);

			if (c[nbmachines-1] < meilleurcmax)
			{
				meilleurcmax = c[nbmachines-1];
				for (l = 0 ; l <= i ; l++)
				{
					meilleurordo[l] = ordopartielmodif[l];
				}
			}
		}

		/* MAJ de l'ordo partiel. */

		for (k = 0 ; k <= i ; k++)
		{
			ordopartiel[k] = meilleurordo[k];
		}

	}

	free(c);

	return(meilleurcmax);
}



