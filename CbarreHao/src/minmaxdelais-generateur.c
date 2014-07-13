/**
 * Générateur de données pour le F3 || permu,Snsd || Cbarre.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/**
 * PROCEDURES
 */

/**
 * int main(int argc, char **argv)
 *
 * Procédure principale du programme. Lance la génération des données et écrit les résultats dans le fichier de sortie.
 * Usage : generateur <nbmachines> <nbtravaux> <minp> <maxp> <mins> <maxs> <nbjeux> <chemin>
 *
 * E:     Néant.
 * PrecC: Néant.
 *
 * S:     Entier.
 * PostC: La procédure renvoie 0 si tout se passe bien. Sinon -1
 */

int main(int argc, char **argv){
	/* Déclaration des variables. */

	int i, j, p, s, iboucle;
	char buf[100];
	/* Intervalle pour les durées opératoires. */
	int minp, maxp;
	/* Intervalles pour les durées de delai minimal */
	int minalpha, maxalpha;
	/* Intervalles pour les durées de delai maximal */
	int minbeta, maxbeta;
	/* Fichier de sortie et chemin (écriture des résultats). */
	FILE *sortie;
	char cheminfichier[100],nomfichier[100];
	/* Nombre de machines et de travaux. */
	int nbmachines, nbtravaux;
	/* Nombre de jeux de données. */
	int nbjeux;

	if (argc == 1 || (argc == 2 && strcmp(argv[1], "--help") == 0))
	{
		printf("\nL'application « snsd-generateur » génère un ou plusieurs jeux d'essais ");
		printf("dans le dossier specifié.\n\n");

		printf("Usage: snsd-generateur <nbm> <nbt> <minp> <maxp> <mins> ");
		printf("<maxs> <nbjeux> <chemin>\n\n");

		printf("Arguments obligatoires:\n");
		printf("  <nbm>            nombre de machines\n");
		printf("  <nbt>            nombre de travaux\n");
		printf("  <minp>           valeur minimale des durées opératoires\n");
		printf("  <maxp>           valeur maximale des durées opératoires\n");
		printf("  <minalpha>       valeur minimale pour le délai minimal\n");
		printf("  <maxalpha>       valeur maximale pour le délai minimal\n");
		printf("  <minbeta>       valeur minimale pour le délai maximal\n");
		printf("  <maxbeta>       valeur maximale pour le délai maximal\n");
		printf("  <nbjeux>         nombre de fichiers à générer\n");
		printf("  <chemin>         chemin du répertoire de stockage\n\n");

		printf("Format de fichier généré:\n");
		printf("  nbm\n  nbt\n  p   p   p   s   s   s\n  p   p   p   s   s   s\n");

		printf("Exemples:\n");
		printf("  snsd-generateur 5 12 20 50 0 7 10 data\n");
		printf("    # Génère dans le dossier « data » 10 jeux de données\n");
		printf("    # avec 5 machines et 12 travaux (dont les durées operatoires sont comprises\n");
		printf("    # entre 20 et 50, les temps de montage sont comprises entre 0 et 7).\n");

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

	strcpy(cheminfichier, argv[10]);
	strcat(cheminfichier, "/fd_");

	/* Générateur aléatoire. */
	srand(time(0));

	/* Génération des fichiers. */
	for (iboucle = 0 ; iboucle < nbjeux ; iboucle++)
	{
		sprintf(buf, "%d", iboucle+1);

		strcpy(nomfichier, cheminfichier);
		strcat(nomfichier, buf);

		sortie = fopen(nomfichier, "wt");
		if(sortie == NULL)
		{
			printf("Erreur ouverture de fichier %s\n",nomfichier);
			return -1;
		}

		fprintf(sortie, "%d\n", nbmachines);
		fprintf(sortie, "%d\n", nbtravaux);

		for (j = 0 ; j < nbtravaux ; j++)
		{
			for (i = 0 ; i < nbmachines ; i++)
			{
				p = (int)( (maxp-minp) * ((double)rand()/((double)(RAND_MAX)+(double)(1))) ) + minp;

				fprintf(sortie, "%d\t",p);
			}
			for (i = 0 ; i < nbmachines-1 ; i++)
			{
				s = (int)( (maxalpha-minalpha) * ((double)rand()/((double)(RAND_MAX)+(double)(1))) ) + minalpha;

				fprintf(sortie, "%d\t",s);
			}
			for (i = 0 ; i < nbmachines-1 ; i++)
			{
				s = (int)( (maxbeta-minbeta) * ((double)rand()/((double)(RAND_MAX)+(double)(1))) ) + minbeta;

				fprintf(sortie, "%d\t",s);
			}
			fprintf(sortie, "\n");
		}

		printf("Fichier généré : %s\n", nomfichier);
		close(sortie);
	}

	return 0;
}

