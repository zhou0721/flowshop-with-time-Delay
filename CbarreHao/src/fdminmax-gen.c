/**
 * Générateur de données pour le F3|delai min-max|Cmax.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>

#include "../include/const.h"


/**
 * VARIABLES GLOBALES
 */

/* Fichier de sortie et chemin (écriture des résultats). */
FILE *sortie;
char cheminfichier[100];

/* Nombre de machines et de travaux. */
int nbmachines, nbtravaux;

/* Nombre de jeux de données. */
int nbjeux;

/* Graine pour les valeurs aléatoires. */
int seed;


/**
 * PROCEDURES
 */

/**
 * int main(int argc, char **argv)
 *
 * Procédure principale du programme. Lance la génération des données et écrit les résultats dans le fichier de sortie.
 * Usage : generateur <nbmachines> <nbtravaux> <minp> <maxp> <minalpha> <maxalpha> <minbeta> <maxbeta> <nbjeux> <chemin>
 *
 * E:     Néant.
 * PrecC: Néant.
 * 
 * S:     Entier.
 * PostC: La procédure renvoie 1 si tout se passe bien.
 */

int main(int argc, char **argv)
{
	/* Déclaration des variables. */

	int i, j, k, p;
	char nomfichier[100];
	char buf[100];
	/* Intervalle pour les durées opératoires. */
	int minp, maxp;
	/* Intervalles pour les délais min-max. */
	int minalpha, maxalpha, minbeta, maxbeta;
	int alpha, beta[100];
	/* Arguments. */
	int heterogene, machineload[100], tlm[100], tlt[100];

	/* Initialisations. */

	for (i = 0 ; i < 100 ; i++)
	{
		machineload[i] = 0;
		tlm[i] = 1;
		tlt[i] = 1;
	}
	heterogene = 0;

	if (strcmp(argv[1], "--help") == 0)
	{
		printf("L'application « fdminmax-gen » génère un ou plusieurs jeux d'essais pour le\n");
		printf("programme « fdminmax » dans le dossier specifié.\n\n");

		printf("Usage: fdminmax-gen <nbm> <nbt> <minp> <maxp> <minalpha> <maxalpha> <minbeta>\n");
		printf("         <maxbeta> <nbjeux> <chemin> [OPTION]...\n\n");

		printf("Exemples:\n");
		printf("  fdminmax-gen 5 12 20 50 0 0 0 7 10 data/benchmarks/classe04\n");
		printf("    # Génère dans le dossier « data/benchmarks/classe04 » 10 jeux de données\n");
		printf("    # avec 5 machines et 12 travaux (dont les durees operatoires sont compri-\n");
		printf("    # ses entre 20 et 50,  les décalages minimum égaux à 0  et les décalages\n");
		printf("    # maximum compris entre 0 et 7).\n");
		printf("  fdminmax-gen 5 12 20 50 0 0 0 7 10 data/benchmarks/classe04 -h -tlm 2 1 3\n");
		printf("            -l 3 1 3 5 -tlt 6 1 3 8 9\n");
		printf("    # Idem. On spécifie en plus que les durées opératoires sont hétérogènes,\n");
		printf("    # qu'on  ne tient compte des  décalages que pour les  machines 1 et 3 et\n");
		printf("    # et les travaux 1, 3, 8  et 9, et que les  machines 1, 3 et 5 sont 75 %\n");
		printf("    # plus chargées.\n\n");

		printf("Les 10 premiers arguments sont obligatoires, les suivants sont optionnels.\n\n");

		printf("Arguments obligatoires:\n");
		printf("  <nbm>            nombre de machines\n");
		printf("  <nbt>            nombre de travaux\n");
		printf("  <minp>           valeur minimale des durées opératoires\n");
		printf("  <maxp>           valeur maximale des durées opératoires\n");
		printf("  <minalpha>       valeur minimale pour alpha\n");
		printf("  <maxalpha>       valeur maximale pour alpha\n");
		printf("  <minbeta>        valeur minimale pour beta\n");
		printf("  <maxbeta>        valeur maximale pour beta\n");
		printf("  <nbjeux>         nombre de fichiers à générer\n");
		printf("  <chemin>         chemin du répertoire de stockage\n\n");

		printf("Arguments optionnels:\n");
		printf("  -h               les durées opératoires de la  deuxième moitié des travaux\n");
		printf("                   sont tirées entre minp et 2*maxp\n");
		printf("  -l <nb> <machine1> <machine2> ... <machinenb>\n");
		printf("                   les machines spécifiées seront  plus chargées, les durées\n");
		printf("                   opératoires de ces dernières  seront multipliées par 75%.\n");
		printf("                   « nb » est le nombre de machines concernées\n");
		printf("  -tlm <nb> <machine1> <machine2> ... <machinenb>\n");
		printf("                   on ne  tient compte des  décalages que pour  les machines\n");
		printf("                   spécifiées. « nb » est le nombre de machines concernées\n");
		printf("  -tlt <nb> <travail1> <travail2> ... <travailnb>\n");
		printf("                   on ne tient compte des décalages que pour les travaux spé-\n");
		printf("                   cifiés. « nb » est le nombre de travaux concernées\n\n");

		printf("Rapporter toutes anomalies à <vincent.augusto@gmail.com>.\n");
		exit(0);
	}

	/* Récupération des arguments. */
	nbmachines = atoi(argv[1]);
	nbtravaux  = atoi(argv[2]);
	minp       = atoi(argv[3]);
	maxp       = atoi(argv[4]);
	minalpha   = atoi(argv[5]);
	maxalpha   = atoi(argv[6]);
	minbeta    = atoi(argv[7]);
	maxbeta    = atoi(argv[8]);
	nbjeux     = atoi(argv[9]);

	for (i = 11 ; i < argc ; i++)
	{
		if (strcmp(argv[i], "-h") == 0)
		{
			heterogene = 1;
		}
		if (strcmp(argv[i], "-l") == 0)
		{
			for (j = i+2 ; j < i+2+atoi(argv[i+1]) ; j++)
			{
				machineload[atoi(argv[j])-1] = 1;
			}
		}
		if (strcmp(argv[i], "-tlm") == 0)
		{
			for (j = 0 ; j < 100 ; j++)
			{
				tlm[j] = 0;
			}
			for (j = i+2 ; j < i+2+atoi(argv[i+1]) ; j++)
			{
				tlm[atoi(argv[j])-1] = 1;
			}
		}
		if (strcmp(argv[i], "-tlt") == 0)
		{
			for (j = 0 ; j < 100 ; j++)
			{
				tlt[j] = 0;
			}
			for (j = i+2 ; j < i+2+atoi(argv[i+1]) ; j++)
			{
				tlt[atoi(argv[j])-1] = 1;
			}
		}
	}

	strcpy(cheminfichier, argv[10]);
	strcat(cheminfichier, "/fd_");

	/* Générateur aléatoire. */
	//seed = 10000;
	srand(time(0));

	/* Génération des fichiers. */

	for (i = 0 ; i < nbjeux ; i++)
	{
		sprintf(buf, "%d", i+1);

		strcpy(nomfichier, cheminfichier);
		strcat(nomfichier, buf);

		sortie = fopen(nomfichier, "wt");

		fprintf(sortie, "%d\n", nbmachines);
		fprintf(sortie, "%d\n", nbtravaux);

		for (j = 0 ; j < nbtravaux ; j++)
		{
			for (k = 0 ; k < nbmachines ; k++)
			{
				p = (int)( (maxp-minp) * ((double)rand()/((double)(RAND_MAX)+(double)(1))) ) + minp;

				if (heterogene)	/* Les durées opératoires de la 2e moitié des travaux sont tirées entre minp et 2maxp. */
				{
					if (j >= nbtravaux/2)
					{
						p = (int)( (2*maxp-minp) * ((double)rand()/((double)(RAND_MAX)+(double)(1))) ) + minp;
					}
				}

				if (machineload[k])	/* La machine k est 75% plus chargée. */
				{
					p = p*0.75;

				}

				fprintf(sortie, "%d\t", p);
			}
			for (k = 0 ; k < nbmachines-1 ; k++)
			{
				alpha   = (int)( (maxalpha-minalpha) * ((double)rand()/((double)(RAND_MAX)+(double)(1))) ) + minalpha;
				beta[k] = (int)( (maxbeta-minbeta) * ((double)rand()/((double)(RAND_MAX) + (double)(1))) ) + minbeta;
				if (beta[k] < alpha)
				{
					alpha = beta[k];
				}

				if (!tlm[k])	/* On ne tient pas compte des time lags pour la machine k. */
				{
					alpha   = 0;
					beta[k] = 10000;
				}

				if (!tlt[j])	/* On ne tient pas compte des time lags pour le travail j. */
				{
					alpha   = 0;
					beta[k] = 10000;
				}

				fprintf(sortie, "%d\t", alpha);
			}
			for (k = 0 ; k < nbmachines-1 ; k++)
			{
				fprintf(sortie, "%d\t", beta[k]);
			}
			fprintf(sortie, "\n");
		}

		printf("Fichier genere : %s.\n", nomfichier);

		close(sortie);
	}

}
