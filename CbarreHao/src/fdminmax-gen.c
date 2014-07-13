/**
 * G�n�rateur de donn�es pour le F3|delai min-max|Cmax.
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

/* Fichier de sortie et chemin (�criture des r�sultats). */
FILE *sortie;
char cheminfichier[100];

/* Nombre de machines et de travaux. */
int nbmachines, nbtravaux;

/* Nombre de jeux de donn�es. */
int nbjeux;

/* Graine pour les valeurs al�atoires. */
int seed;


/**
 * PROCEDURES
 */

/**
 * int main(int argc, char **argv)
 *
 * Proc�dure principale du programme. Lance la g�n�ration des donn�es et �crit les r�sultats dans le fichier de sortie.
 * Usage : generateur <nbmachines> <nbtravaux> <minp> <maxp> <minalpha> <maxalpha> <minbeta> <maxbeta> <nbjeux> <chemin>
 *
 * E:     N�ant.
 * PrecC: N�ant.
 * 
 * S:     Entier.
 * PostC: La proc�dure renvoie 1 si tout se passe bien.
 */

int main(int argc, char **argv)
{
	/* D�claration des variables. */

	int i, j, k, p;
	char nomfichier[100];
	char buf[100];
	/* Intervalle pour les dur�es op�ratoires. */
	int minp, maxp;
	/* Intervalles pour les d�lais min-max. */
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
		printf("L'application � fdminmax-gen � g�n�re un ou plusieurs jeux d'essais pour le\n");
		printf("programme � fdminmax � dans le dossier specifi�.\n\n");

		printf("Usage: fdminmax-gen <nbm> <nbt> <minp> <maxp> <minalpha> <maxalpha> <minbeta>\n");
		printf("         <maxbeta> <nbjeux> <chemin> [OPTION]...\n\n");

		printf("Exemples:\n");
		printf("  fdminmax-gen 5 12 20 50 0 0 0 7 10 data/benchmarks/classe04\n");
		printf("    # G�n�re dans le dossier � data/benchmarks/classe04 � 10 jeux de donn�es\n");
		printf("    # avec 5 machines et 12 travaux (dont les durees operatoires sont compri-\n");
		printf("    # ses entre 20 et 50,  les d�calages minimum �gaux � 0  et les d�calages\n");
		printf("    # maximum compris entre 0 et 7).\n");
		printf("  fdminmax-gen 5 12 20 50 0 0 0 7 10 data/benchmarks/classe04 -h -tlm 2 1 3\n");
		printf("            -l 3 1 3 5 -tlt 6 1 3 8 9\n");
		printf("    # Idem. On sp�cifie en plus que les dur�es op�ratoires sont h�t�rog�nes,\n");
		printf("    # qu'on  ne tient compte des  d�calages que pour les  machines 1 et 3 et\n");
		printf("    # et les travaux 1, 3, 8  et 9, et que les  machines 1, 3 et 5 sont 75 %\n");
		printf("    # plus charg�es.\n\n");

		printf("Les 10 premiers arguments sont obligatoires, les suivants sont optionnels.\n\n");

		printf("Arguments obligatoires:\n");
		printf("  <nbm>            nombre de machines\n");
		printf("  <nbt>            nombre de travaux\n");
		printf("  <minp>           valeur minimale des dur�es op�ratoires\n");
		printf("  <maxp>           valeur maximale des dur�es op�ratoires\n");
		printf("  <minalpha>       valeur minimale pour alpha\n");
		printf("  <maxalpha>       valeur maximale pour alpha\n");
		printf("  <minbeta>        valeur minimale pour beta\n");
		printf("  <maxbeta>        valeur maximale pour beta\n");
		printf("  <nbjeux>         nombre de fichiers � g�n�rer\n");
		printf("  <chemin>         chemin du r�pertoire de stockage\n\n");

		printf("Arguments optionnels:\n");
		printf("  -h               les dur�es op�ratoires de la  deuxi�me moiti� des travaux\n");
		printf("                   sont tir�es entre minp et 2*maxp\n");
		printf("  -l <nb> <machine1> <machine2> ... <machinenb>\n");
		printf("                   les machines sp�cifi�es seront  plus charg�es, les dur�es\n");
		printf("                   op�ratoires de ces derni�res  seront multipli�es par 75%.\n");
		printf("                   � nb � est le nombre de machines concern�es\n");
		printf("  -tlm <nb> <machine1> <machine2> ... <machinenb>\n");
		printf("                   on ne  tient compte des  d�calages que pour  les machines\n");
		printf("                   sp�cifi�es. � nb � est le nombre de machines concern�es\n");
		printf("  -tlt <nb> <travail1> <travail2> ... <travailnb>\n");
		printf("                   on ne tient compte des d�calages que pour les travaux sp�-\n");
		printf("                   cifi�s. � nb � est le nombre de travaux concern�es\n\n");

		printf("Rapporter toutes anomalies � <vincent.augusto@gmail.com>.\n");
		exit(0);
	}

	/* R�cup�ration des arguments. */
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

	/* G�n�rateur al�atoire. */
	//seed = 10000;
	srand(time(0));

	/* G�n�ration des fichiers. */

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

				if (heterogene)	/* Les dur�es op�ratoires de la 2e moiti� des travaux sont tir�es entre minp et 2maxp. */
				{
					if (j >= nbtravaux/2)
					{
						p = (int)( (2*maxp-minp) * ((double)rand()/((double)(RAND_MAX)+(double)(1))) ) + minp;
					}
				}

				if (machineload[k])	/* La machine k est 75% plus charg�e. */
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
