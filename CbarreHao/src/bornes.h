#ifndef _BORNES_MIN_MAX_H
#define _BORNES_MIN_MAX_H

#include <stdlib.h>

int max2(int a, int b)
{
	return a>b?a:b;
}

// calcule le max d'un tableau
int maxTab(int* tab, int Dim)
{
	int m, i;
	m = -32768;
	for(i=0; i<Dim;i++)
		m = m>tab[i]?m:tab[i];
	return m;
}

// calcule le min d'un tableau
int minTab(int* tab, int Dim)
{
	int m, i;
	m = 32767;
	for(i=0; i<Dim; i++)
		m = m<tab[i]?m:tab[i];
	return m;
}

// Fait : Johnson sur un problème à 2 machines
// Fait : Johnson sur un problème à 2 machines avec une séquence partielle
// Fait : Mitten sur un problème à 2 machines avec une séquence partielle et délai minimaux
// Fait : Mitten sur un problème à n machines avec une séquence partielle et délai minimaux
// Fait : calculer la borne inférieure d'une séquence
// Fait : calculer la borne supérieure d'une séquence

// Décale d'un cran vers la droite tous les jobs à partir de indiceDeb
void DecalerSequenceADroite(int* Sequence, int indiceDeb, int NbCases)
{
	int Fin0, i;
	Fin0 = NbCases - 1;
	while(Sequence[Fin0]!=-1) Fin0--;
	for(i=Fin0; i>indiceDeb; i--)
		Sequence[i] = Sequence[i-1];
	Sequence[indiceDeb] = -1;
}

// Décale d'un cran vers la gauche tous les jobs jusqu'à indiceFin
void DecalerSequenceAGauche(int* Sequence, int indiceFin)
{
	int Deb0, i;
	Deb0 = 0;
	while(Sequence[Deb0]!=-1) Deb0++;
	for(i=Deb0; i<indiceFin; i++)
		Sequence[i]=  Sequence[i+1];
	Sequence[indiceFin] = -1;
}

// Insère un job dans une séquence selon l'algorithme de Mitten
void InsertJobInSequence(int** Pij, int** Alpha, int Job, int* Sequence, int NbJobsAPlacer, int m1, int m2)
{
	int i, AlphaJob, AlphaSeq, j;
	// Insère le job dans la séquence selon l'algo de Johnson
	if(Pij[Job][m1]<=Pij[Job][m2])
	{	// On place le job en début de liste
		for(i=0; i<NbJobsAPlacer; i++)
		{
			if(Sequence[i]==-1)
			{	// Aucun job n'est sur cette case, on insère donc le notre
				Sequence[i] = Job;
				break;
			}
			else
			{
				// On doit calculer les Alpha des deux jobs à comparer
				AlphaJob = Alpha[Job][m1];
				AlphaSeq = Alpha[Sequence[i]][m1];
				for(j=m1+1; j<m2; j++)
				{
					AlphaJob += Pij[Job][j] + Alpha[Job][j];
					AlphaSeq += Pij[Sequence[i]][j] + Alpha[Sequence[i]][j];
				}
				if((Pij[Sequence[i]][m1]+AlphaSeq) >= (Pij[Job][m1]+AlphaJob))
				{
					// Notre job est prioritaire sur celui de la case
					DecalerSequenceADroite(Sequence,i,NbJobsAPlacer);
					Sequence[i] = Job;
					break;
				}
			}			
		}
	}
	else
	{	// On place le job en fin de liste
		for(i=NbJobsAPlacer-1; i>=0 ;i--)
		{
			if(Sequence[i]==-1)
			{	// Aucun job n'est sur cette case, on insère donc le notre
				Sequence[i] = Job;
				break;
			}
			else
			{
				// On doit calculer les Alpha des deux jobs à comparer
				AlphaJob = Alpha[Job][m1];
				AlphaSeq = Alpha[Sequence[i]][m1];
				for(j=m1+1; j<m2; j++)
				{
					AlphaJob += Pij[Job][j] + Alpha[Job][j];
					AlphaSeq += Pij[Sequence[i]][j] + Alpha[Sequence[i]][j];
				}
				if((Pij[Sequence[i]][m2]+AlphaSeq) >= (Pij[Job][m2]+AlphaJob))
				{
					// Notre job est prioritaire sur celui de la case
					DecalerSequenceAGauche(Sequence,i);
					Sequence[i] = Job;
					break;
				}
			}
		}
	}
}

// Calcule la borne inférieure du Cmax d'une séquence de jobs
int BorneInferieureRelaxFin(int NbJobsAPlacer, int NbEtages, int* Sequence, int* DatesDispo, int** Pij, int** Alpha, int m1, int m2)
{
	int *DatesD, *DatesTemp, i, j, result, *DelaiFin, AlphaJob;
	// Principe : on part des dates de dispos au plus tôt de chaque étage qu'on recopie
	// Puis pour chaque job, on modifie les dates de dispo au plus tôt de tous les étages
	// Puis, on passe au job suivant (inspiration à la tetris)

	DatesD = (int*)malloc(sizeof(int)*2);		// Dates de disponibilité en cours dans la séquence
	DatesTemp = (int*)malloc(sizeof(int)*2);		// Permet de faire la transition lors de l'ajout d'un job

	// Recopie initiale des dates de dispo pour pas flinguer celles de l'appel
	DatesD[0] = DatesDispo[m1];
	DatesD[1] = DatesDispo[m2];

	for(i=0; i<NbJobsAPlacer; i++)
	{
		// Pour l'étage m1, c'est la date de dispo + le temps opératoire
		DatesTemp[0] = DatesD[0] + Pij[Sequence[i]][m1];

		// Pour l'étage m2, cf rapport
		AlphaJob = Alpha[Sequence[i]][m1];
		for(j=m1+1; j<m2;j++)
			AlphaJob += Pij[Sequence[i]][j] + Alpha[Sequence[i]][j];
		DatesTemp[1] = max2(DatesD[1]+Pij[Sequence[i]][m2],DatesD[0]+Pij[Sequence[i]][m1]+AlphaJob+Pij[Sequence[i]][m2]);

		// Toutes les nouvelles dates de dispo ont été calculées, on recopie pour passer au job suivant
		DatesD[0] = DatesTemp[0];
		DatesD[1] = DatesTemp[1];
	}

	result = DatesD[1];
	free(DatesD);
	free(DatesTemp);

	// Partie 2 : on ajoute le temps minimum pour atteindre la dernière machine
	DelaiFin = (int*)malloc(sizeof(int)*NbJobsAPlacer);
	for(i=0; i<NbJobsAPlacer; i++)
	{
		DelaiFin[i] = 0;
		if(m2<NbEtages-1)
		{
			DelaiFin[i] = Alpha[Sequence[i]][m2];
			for(j=m2+1; j<NbEtages-1; j++)
				DelaiFin[i] += Pij[Sequence[i]][j] + Alpha[Sequence[i]][j];
			DelaiFin[i] += Pij[Sequence[i]][NbEtages-1];
		}
	}
	result += minTab(DelaiFin,NbJobsAPlacer);
	free(DelaiFin);

	return result;
}

// Calcule la borne inférieure du Cmax d'une séquence de jobs
int BorneInferieureRelaxChargeFin(int NbJobsAPlacer, int NbEtages, int* Sequence, int* DatesDispo, int** Pij, int** Alpha, int m1, int m2)
{
	// Principe : on part des dates de dispos au plus tôt de chaque étage qu'on recopie
	// Puis pour chaque job, on modifie les dates de dispo au plus tôt de tous les étages
	// Puis, on passe au job suivant (inspiration à la tetris)

	int *DatesD, *DatesTemp, i, j, AlphaJob, *DelaiFin, result;
	DatesD = (int*)malloc(sizeof(int)*2);		// Dates de disponibilité en cours dans la séquence
	DatesTemp = (int*)malloc(sizeof(int)*2);		// Permet de faire la transition lors de l'ajout d'un job

	// Recopie initiale des dates de dispo pour pas flinguer celles de l'appel
	DatesD[0] = DatesDispo[m1];
	DatesD[1] = DatesDispo[m2];

	for(i=0; i<NbJobsAPlacer; i++)
	{
		// Pour l'étage m1, c'est la date de dispo + le temps opératoire
		DatesTemp[0] = DatesD[0] + Pij[Sequence[i]][m1];

		// Pour l'étage m2, cf rapport
		AlphaJob = Alpha[Sequence[i]][m1];
		for(j=m1+1; j<m2;j++)
			AlphaJob += Pij[Sequence[i]][j] + Alpha[Sequence[i]][j];
		DatesTemp[1] = max2(DatesD[1]+Pij[Sequence[i]][m2],DatesD[0]+Pij[Sequence[i]][m1]+AlphaJob+Pij[Sequence[i]][m2]);

		// Toutes les nouvelles dates de dispo ont été calculées, on recopie pour passer au job suivant
		DatesD[0] = DatesTemp[0];
		DatesD[1] = DatesTemp[1];
	}

	result = DatesD[1];
	free(DatesD);
	free(DatesTemp);

	// Partie 2 : On ajoute le délai jusqu'au dernier étage
	DelaiFin = (int*)malloc(sizeof(int)*NbJobsAPlacer);
	for(i=0; i<NbJobsAPlacer; i++)
	{
		DelaiFin[i] = 0;
		if(m2<NbEtages-1)
		{
			DelaiFin[i] = Alpha[Sequence[i]][m2];
			for(j=m2+1; j<NbEtages-1; j++)
				DelaiFin[i] += Pij[Sequence[i]][j] + Alpha[Sequence[i]][j];
			DelaiFin[i] += Pij[Sequence[i]][NbEtages-1];
		}
	}
	result += minTab(DelaiFin,NbJobsAPlacer);
	free(DelaiFin);

	return result;
}

// Calcule la borne supérieure du Cmax d'une séquence de jobs
int BorneSuperieure(int NbJobsAPlacer, int NbEtages, int* Sequence, int* DatesDispo, int** Pij, int** Alpha, int** Beta)
{
	int *DatesD, *DatesTemp, *ContrTemp, i, j, k, l, result;
	// Principe : on part des dates de dispos au plus tôt de chaque étage qu'on recopie
	// Puis pour chaque job, on modifie les dates de dispo au plus tôt de tous les étages
	// Puis, on passe au job suivant (inspiration à la tetris)

	DatesD = (int*) malloc(sizeof(int)*NbEtages);		// Dates de disponibilité en cours dans la séquence
	DatesTemp = (int*)malloc(sizeof(int)*NbEtages);		// Permet de faire la transition lors de l'ajout d'un job
	ContrTemp = (int*)malloc(sizeof(int)*NbEtages);		// Utilisé pour calculer les contraintes de chaque étage pour un job

	// Recopie initiale des dates de dispo pour pas flinguer celles de l'appel
	for(i=0; i<NbEtages; i++) DatesD[i] = DatesDispo[i];

	for(i=0; i<NbJobsAPlacer; i++)
	{
		// Pour l'étage 1
			// Contrainte de l'étage lui-même
			ContrTemp[0] = DatesD[0] + Pij[Sequence[i]][0];
			// Contraintes des étages suivants
			for(j=1; j<NbEtages; j++)
			{
				ContrTemp[j] = DatesD[j];
				for(k=j-1; k>0; k--)
					ContrTemp[j] -= Beta[Sequence[i]][k] + Pij[Sequence[i]][k];
				ContrTemp[j] -= Beta[Sequence[i]][0];
			}
			// On récupère le max de ces contraintes
			DatesTemp[0] = maxTab(ContrTemp,NbEtages);


		// Pour les autres étages, on se base sur l'étage précédent
		// et on suit la formule indiquée dans mon rapport (trop long pour être mise ici)
		for(j=1; j<NbEtages; j++)
		{
			// Contraintes des étages précédents
			for(k=0; k<j; k++)
			{
				ContrTemp[k] = DatesD[k];
				for(l=k; l<j; l++)
					ContrTemp[k] += Pij[Sequence[i]][l] + Alpha[Sequence[i]][l];
				ContrTemp[k] += Pij[Sequence[i]][j];
			}

			// Contrainte de l'étage lui-même
			ContrTemp[j] = DatesD[j]+Pij[Sequence[i]][j];

			// Contraintes des étages suivants
			for(k=j+1; k<NbEtages; k++)
			{
				ContrTemp[k] = DatesD[k];
				for(l=k-1; l>j; l--)
					ContrTemp[k] -= Pij[Sequence[i]][l] + Beta[Sequence[i]][l];
				ContrTemp[k] -= Beta[Sequence[i]][j];
			}

			// On récupère le max des contraintes
			DatesTemp[j] = maxTab(ContrTemp,NbEtages);
		}


		// Pour le dernier étage
			// Contrainte de l'étage lui-même
			ContrTemp[NbEtages-1] = DatesD[NbEtages-1];

			// Contraintes des étages précédents
			for(j=0; j<NbEtages-1; j++)
			{
				ContrTemp[j] = DatesD[j];
				for(k=j; k<NbEtages-1; k++)
					ContrTemp[j] += Pij[Sequence[i]][k] + Alpha[Sequence[i]][k];
				ContrTemp[j] += Pij[Sequence[i]][NbEtages-1];
			}

			// On récupère le max des contraintes
			ContrTemp[NbEtages-1] = maxTab(ContrTemp,NbEtages);


		// Toutes les nouvelles dates de dispo ont été calculées, on recopie pour passer au job suivant
		for(j=0; j<NbEtages; j++)
			DatesD[j] = DatesTemp[j];
	}

	result = DatesD[NbEtages-1];
	free(DatesD);
	free(DatesTemp);
	free(ContrTemp);
	return result;
}
/*
int * GetDatesDisponibilitenowait(int NbJobsAPlacer, int NbEtages, int* Sequence, int* DatesDispo, int** Pij)
{
	
    	int *DatesTemp, i, j, k, l, varmaxi,*DatesD,max;
	// Principe : on part des dates de dispos au plus tôt de chaque étage qu'on recopie
	// Puis pour chaque job, on modifie les dates de dispo au plus tôt de tous les étages
	// Puis, on passe au job suivant (inspiration à la tetris)
	varmaxi =0;
	//DatesD = (int*) malloc(sizeof(int)*NbEtages);		// Dates de disponibilité en cours dans la séquence
	DatesTemp = (int*)malloc(sizeof(int)*NbEtages);		// Permet de faire la transition lors de l'ajout d'un job
	DatesD = (int*)malloc(sizeof(int)*NbEtages);
	if (DatesTemp == 0) printf("Prbleme DatesTemp vaut 0 dans GetDatesDisponibilite \n");
        // if (DatesD == 0) printf("Prbleme DatesD vaut 0 dans GetDatesDisponibilite \n");

	// Recopie initiale des dates de dispo pour pas flinguer celles de l'appel
	for(i=0; i<NbEtages; i++) {
	DatesD[i] = DatesDispo[i];
	DatesTemp[i]=DatesDispo[i];
	//printf("datesDispo[%d]=%d\n",i,DatesDispo[i]);
	} //printf("datesTemp[%d]=%d\n",i,DatesTemp[i]); }
	
	for(i=0; i<NbJobsAPlacer; i++)
	{
        DatesD[0] = DatesD[0] + Pij[Sequence[i]][0];
        for(j=1; j<NbEtages; j++)
        {
	  //  printf("pij[%d][%d]=%d\n",i,j,Pij[Sequence[i]][j]);
            DatesD[j] = DatesD[j-1] + Pij[Sequence[i]][j];
	    //DatesTemp[j]=DatesD[j-1] + Pij[Sequence[i]][j];
	    
        }
        max=0;
        for(j=0; j<NbEtages; j++)
        {
	//    printf("DatesD[%d]=%d\n",j,DatesD[j]);
            if((DatesTemp[j]-(DatesD[j]-Pij[Sequence[i]][j]))>max)
            {
                max=DatesTemp[j]-DatesD[j]+Pij[Sequence[i]][j];
            }
        }
	//printf("max=%d\n",max);
        for(j=0; j<NbEtages; j++)
        {
	    
            DatesTemp[j] = DatesD[j]+max;
	 //   printf("DatesTemp[%d]=%d\n",j,DatesTemp[j]);
	    
        }
	}
	return DatesTemp;
}
*/

int* GetDatesDisponibilite(int NbJobsAPlacer, int NbEtages, int* Sequence, int* DatesDispo, int** Pij, int ** delaismin,int ** delaismax) //1,nbmachines,sigma,datesD,p);
{
	int *DatesTemp, i, j, k, l, varmaxi,ptemp,*DatesD;
	// Principe : on part des dates de dispos au plus tôt de chaque étage qu'on recopie
	// Puis pour chaque job, on modifie les dates de dispo au plus tôt de tous les étages
	// Puis, on passe au job suivant (inspiration à la tetris)
	int table[NbEtages];
	varmaxi =0;
	DatesD = (int*) malloc(sizeof(int)*NbEtages);		// Dates de disponibilité en cours dans la séquence
	DatesTemp = (int*)malloc(sizeof(int)*NbEtages);		// Permet de faire la transition lors de l'ajout d'un job
	if (DatesTemp == 0) printf("Prbleme DatesTemp vaut 0 dans GetDatesDisponibilite \n");
       	if (DatesD == 0) printf("Prbleme DatesD vaut 0 dans GetDatesDisponibilite \n");

	// Recopie initiale des dates de dispo pour pas flinguer celles de l'appel
	for(i=0; i<NbEtages; i++) {DatesTemp[i] = DatesDispo[i];DatesD[i] = DatesDispo[i];} //printf("datesTemp[%d]=%d\n",i,DatesTemp[i]); }
	
	for(i=0; i<NbJobsAPlacer; i++)
	{
		//printf("Sequence[%d]=%d\n",i,Sequence[i]);
		//printf("datedispo 1 %d\n",i);
		// Pour l'étage 1
		// Contrainte de l'étage lui-même
		//printf("datestemp[0] = %d\n PijSeqi0 = %d\n",DatesTemp[0], Pij[Sequence[i]][0]);
		//DatesTemp[0] = DatesTemp[0] + Pij[Sequence[i]][0];

		for(j=0;j<NbEtages;j++)
		{
			DatesD[j]=DatesTemp[j];
		}

		
		//dates[i][0][0] = dates[i-1][0][1];
		//dates[i][0][1] = dates[i][0][0] + job[sequence[i]].p[0];
		DatesTemp[0] = DatesTemp[0]+Pij[Sequence[i]][0];

		for(k=1; k<NbEtages; k++)
		{
			DatesTemp[k] =  max2(DatesD[k],DatesTemp[k-1]+delaismin[Sequence[i]][k-1])+Pij[Sequence[i]][k];
		}

		for(k= NbEtages-2 ; k>=0; k--)
		{
			if(DatesTemp[k+1] > (DatesTemp[k]+delaismax[Sequence[i]][k]+Pij[Sequence[i]][k+1]))
			{
				DatesTemp[k]=DatesTemp[k+1]-delaismax[Sequence[i]][k]-Pij[Sequence[i]][k+1];
			}
		}	
			// printf("ici III\n");
			// Contraintes des étages suivants
		free(DatesD);
			
	}
		
	return DatesTemp;
}

/*
int* GetDatesDisponibilite(int NbJobsAPlacer, int NbEtages, int* Sequence, int* DatesDispo, int** Pij, int ** delaismin,int ** delaismax) //1,nbmachines,sigma,datesD,p);
{
	
	int *DatesTemp, i, j, k, l, varmaxi,ptemp,*DatesD;
	// Principe : on part des dates de dispos au plus tôt de chaque étage qu'on recopie
	// Puis pour chaque job, on modifie les dates de dispo au plus tôt de tous les étages
	// Puis, on passe au job suivant (inspiration à la tetris)
	int table[NbEtages];
	varmaxi =0;
	DatesD = (int*) malloc(sizeof(int)*NbEtages);		// Dates de disponibilité en cours dans la séquence
	DatesTemp = (int*)malloc(sizeof(int)*NbEtages);		// Permet de faire la transition lors de l'ajout d'un job
	if (DatesTemp == 0) printf("Prbleme DatesTemp vaut 0 dans GetDatesDisponibilite \n");
       	if (DatesD == 0) printf("Prbleme DatesD vaut 0 dans GetDatesDisponibilite \n");

	// Recopie initiale des dates de dispo pour pas flinguer celles de l'appel
	for(i=0; i<NbEtages; i++) {DatesTemp[i] = DatesDispo[i];DatesD[i] = DatesDispo[i];} //printf("datesTemp[%d]=%d\n",i,DatesTemp[i]); }
	
	for(i=0; i<NbJobsAPlacer; i++)
	{
		//printf("Sequence[%d]=%d\n",i,Sequence[i]);
		//printf("datedispo 1 %d\n",i);
		// Pour l'étage 1
		// Contrainte de l'étage lui-même
		//printf("datestemp[0] = %d\n PijSeqi0 = %d\n",DatesTemp[0], Pij[Sequence[i]][0]);
		//DatesTemp[0] = DatesTemp[0] + Pij[Sequence[i]][0];
		for(j=0;j<NbEtages;j++)
		{
			DatesD[j]=DatesTemp[j];
		}
		for(j=0; j<NbEtages;j++)
		{
			
			for(k=0;k<NbEtages;k++)
			{
				table[k]=0;
				ptemp=0;
				
				if(k<j)
				{
					for(l=k;l<j;l++)
					{
						ptemp+=Pij[Sequence[i]][l]+delaismin[Sequence[i]][l];
					}	
					table[k]=DatesD[k]+Pij[Sequence[i]][j]+ptemp;
					//printf("111%d\n",table[k]);
					//printf("111k<J,table[%d][%d][%d]=%d=%d+%d+%d\n",i,j,k,table[k],DatesD[k],Pij[i][j],ptemp);
				}
				else if(k==j)
				{
					table[k]=DatesD[k]+Pij[Sequence[i]][j];
					//printf("222%d\n",table[k]);
					//printf("222k=J,table[%d][%d][%d]=%d=%d+%d\n",i,j,k,table[k],DatesD[k],Pij[i][j]);
				}
				else if(k>j)
				{
					for(l=j;l<k-1;l++)
					{
						ptemp+=Pij[Sequence[i]][l]+delaismax[Sequence[i]][l];
					}
					table[k]=DatesD[k]-delaismax[Sequence[i]][j]-ptemp;
					//printf("333%d\n",table[k]);
					//printf("333k>J,table[%d][%d][%d]=%d=%d-%d-%d\n",i,j,k,table[k],DatesD[k],delaismax[i][j],ptemp);
				}
					
					//printf("444table[%d][%d][%d]=%d\n",i,j,k,table[k]);
			}
			DatesTemp[j]=maxTab(table,NbEtages);
				//printf("DatesTemp[%d]=%d\n",j,DatesTemp[j]);
				
		}
			// printf("ici III\n");
			// Contraintes des étages suivants
	}
	
		
	return DatesTemp;
}*/
/*
int* GetDatesDisponibilite(int NbJobsAPlacer, int NbEtages, int* Sequence, int* DatesDispo, int** Pij)//1,nbmachines,sigma,datesD,p);
{
	int *DatesTemp, i, j, k, l, varmaxi;//*DatesD,
	// Principe : on part des dates de dispos au plus tôt de chaque étage qu'on recopie
	// Puis pour chaque job, on modifie les dates de dispo au plus tôt de tous les étages
	// Puis, on passe au job suivant (inspiration à la tetris)
	varmaxi =0;
	//DatesD = (int*) malloc(sizeof(int)*NbEtages);		// Dates de disponibilité en cours dans la séquence
	DatesTemp = (int*)malloc(sizeof(int)*NbEtages);		// Permet de faire la transition lors de l'ajout d'un job
	if (DatesTemp == 0) printf("Prbleme DatesTemp vaut 0 dans GetDatesDisponibilite \n");
       // if (DatesD == 0) printf("Prbleme DatesD vaut 0 dans GetDatesDisponibilite \n");

	// Recopie initiale des dates de dispo pour pas flinguer celles de l'appel
	for(i=0; i<NbEtages; i++) {DatesTemp[i] = DatesDispo[i];} //printf("datesTemp[%d]=%d\n",i,DatesTemp[i]); }

	for(i=0; i<NbJobsAPlacer; i++)
	{//printf("Sequence[%d]=%d\n",i,Sequence[i]);
	//printf("datedispo 1 %d\n",i);
		// Pour l'étage 1
			// Contrainte de l'étage lui-même
			//printf("datestemp[0] = %d\n PijSeqi0 = %d\n",DatesTemp[0], Pij[Sequence[i]][0]);
			DatesTemp[0] = DatesTemp[0] + Pij[Sequence[i]][0];
			//printf("ici III\n");
			// Contraintes des étages suivants
			for(j=1; j<NbEtages; j++)
			{//printf("ici boucle %d\n",j);
				varmaxi =  max2(DatesTemp[j-1],DatesTemp[j]);
				DatesTemp[j] = varmaxi + Pij[Sequence[i]][j];
			}
	}

	return DatesTemp;
}*/
// Calcule les bornes supérieures et inférieures pour un couple de machines donné
int** MittenRelaxFin(int NbJobsAPlacer, int NbEtages, int* JobsAPlacer, int* DatesDispo, int** Pij, int** Alpha, int** Beta, int m1, int m2)
{
	int *Sequence, i, **result;
	// On initialise une séquence vide
	Sequence = (int*)malloc(sizeof(int)*NbJobsAPlacer);
	for(i=0; i<NbJobsAPlacer; i++) Sequence[i] = -1;
	
	// On place chaque job dans la séquence
	for (i=0; i<NbJobsAPlacer; i++)
	{
		InsertJobInSequence(Pij,Alpha,JobsAPlacer[i],Sequence,NbJobsAPlacer,m1,m2);
	}

	// Création du tableau stockant les résultats
	result = (int**)malloc(sizeof(int*)*2);
	result[0] = (int*) malloc(sizeof(int)*2);
		// result[0][1]   => borne inférieure
		// result[1][2]   => borne supérieure
		// result[1][...] => séquence

	result[0][0] = BorneInferieureRelaxFin(NbJobsAPlacer,NbEtages,Sequence,DatesDispo,Pij,Alpha,m1,m2);

	result[0][1] = BorneSuperieure(NbJobsAPlacer,NbEtages,Sequence,DatesDispo,Pij,Alpha,Beta);

	result[1] = Sequence;

	return result;
}

// Calcule les bornes supérieures et inférieures pour un couple de machines donné
int** MittenRelaxChargeFin(int NbJobsAPlacer, int NbEtages, int* JobsAPlacer, int* DatesDispo, int** Pij, int** Alpha, int** Beta, int m1, int m2)
{
	int *Sequence, i, **result;

	// On initialise une séquence vide
	Sequence = (int*)malloc(sizeof(int)*NbJobsAPlacer);
	for(i=0; i<NbJobsAPlacer; i++) Sequence[i] = -1;
	
	// On place chaque job dans la séquence
	for (i=0; i<NbJobsAPlacer; i++)
	{
		InsertJobInSequence(Pij,Alpha,JobsAPlacer[i],Sequence,NbJobsAPlacer,m1,m2);
	}

	// Création du tableau stockant les résultats
	result = (int**)malloc(sizeof(int*)*2);
	result[0] = (int*)malloc(sizeof(int)*2);
		// result[0][1]   => borne inférieure
		// result[1][2]   => borne supérieure
		// result[1][...] => séquence

	result[0][0] = BorneInferieureRelaxChargeFin(NbJobsAPlacer,NbEtages,Sequence,DatesDispo,Pij,Alpha,m1,m2);

	result[0][1] = BorneSuperieure(NbJobsAPlacer,NbEtages,Sequence,DatesDispo,Pij,Alpha,Beta);

	result[1] = Sequence;

	return result;
}

int** BorneInfEtSupRelaxFin(int NbJobsAPlacer, int NbEtages, int* JobsAPlacer, int* DatesDispo, int** Pij, int** Alpha, int **Beta)
{
	int NbCombi, m1, m2, CurrentCombi, ***Results, **result, i;
	NbCombi = 0;
	// On calcule le nombre de combinaisons possibles
	for (m1 = 0; m1 < NbEtages-1; m1++) // m1 et m2 permettent d'énumérer tous les flowshops extractibles
		for (m2 = m1+1; m2 < NbEtages; m2++)
	{
		NbCombi++;
	}

	// On crée un tableau pour stocker les résultats
	Results = (int***) malloc(sizeof(int**)*NbCombi);

	CurrentCombi = 0;
	for (m1 = 0; m1 < NbEtages-1; m1++) // m1 et m2 permettent d'énumérer tous les flowshops extractibles
		for (m2 = m1+1; m2 < NbEtages; m2++)
	{
		// On calcule les bornes pour chaque couple et on les récupère
		Results[CurrentCombi] = MittenRelaxFin(NbJobsAPlacer,NbEtages,JobsAPlacer,DatesDispo,Pij,Alpha,Beta,m1,m2);
		CurrentCombi++;
	}

	// Création du tableau stockant les résultats
	result = (int**) malloc(sizeof(int*)*2);
	result[0] = (int*) malloc(sizeof(int)*2);
		// result[0] => borne inférieure
		// result[1] => borne supérieure

	// La borne inférieure du noeud est le max des bornes inférieures
	// La borne supérieure du noeud est le min des bornes supérieures
	result[0][0] = -32768;
	result[0][1] = 32767;
	for(i=0; i<NbCombi; i++)
	{
		result[0][0] = result[0][0]>Results[i][0][0]?result[0][0]:Results[i][0][0];
		if(result[0][1]>Results[i][0][1])
		{
			result[0][1] = Results[i][0][1];
			result[1] = Results[i][1];
		}
	}

	// On supprime les variables temporaires du tas
	for(i=0; i<NbCombi; i++)
	{
		free(Results[i][0]);
		if(Results[i][1]!=result[1]) free(Results[i][1]);
		free(Results[i]);
	}
	free(Results);

	// Et on renvoie le résultat
	return result;
}

int** BorneInfEtSupRelaxChargeFin(int NbJobsAPlacer, int NbEtages, int* JobsAPlacer, int* DatesDispo, int** Pij, int** Alpha, int **Beta)
{

	int *Charge, i, j, m1, m2, m;

	// On calcule les charges de chaque machine
	Charge = (int*)malloc(sizeof(int)*NbEtages);
	for(i=0; i<NbEtages; i++)
	{
		Charge[i] = 0;
		for(j=0; j<NbJobsAPlacer; j++) Charge[i] += Pij[JobsAPlacer[j]][i];
	}

	// On prend l'étage le plus chargé
	m = -32768;
	for(i=0; i<NbEtages; i++)
		if(m<Charge[i])
		{
			m = Charge[i];
			m1 = i;
		}

	// On prend le second étage le plus chargé
	m = -32768;
	for(i=0; i<NbEtages; i++)
		if((m<Charge[i]) && (i!=m1))
		{
			m = Charge[i];
			m2 = i;
		}

	free(Charge);
	if(m1<m2)
		return MittenRelaxChargeFin(NbJobsAPlacer,NbEtages,JobsAPlacer,DatesDispo,Pij,Alpha,Beta,m1,m2);
	else
		return MittenRelaxChargeFin(NbJobsAPlacer,NbEtages,JobsAPlacer,DatesDispo,Pij,Alpha,Beta,m2,m1);
}

#endif
