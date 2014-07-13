#include "PR1.h"
#include "PR4.h"

int Affichage(TypeCellule* L)
{
    TypeCellule *p;
    p=L;
    int Cm=0;
    while(p!=NULL)
    {
        printf(" %d ",p->job[0]);
        Cm=Cm + p->job[1];
        printf("finish time : %d\n", p->job[1]);
        p=p->suivant;

    }
    //puts("");
    printf("Cm pour cette sequence est: %d",Cm);
    return Cm;
}

TypeCellule * Append(TypeCellule *L, int jobnumero, int cm)
{
    TypeCellule *p, *nouveau;
    nouveau=(TypeCellule*)malloc(sizeof(TypeCellule));
    nouveau->job[0]=jobnumero;
    nouveau->job[1]=cm;
    nouveau->suivant=NULL;
    if(L==NULL) L=nouveau;
    else
    {
        for(p=L; p->suivant!=NULL;p=p->suivant){}
        p->suivant=nouveau;
    }
    return L;
}

TypeCellule *Supprime(TypeCellule *L,int jobnumero)
{
    TypeCellule *p,*suivant,*pL;
    p=L;
    if(p==NULL)
    return L;
    if(p->job[0]==jobnumero)
    {
        pL=p->suivant;
        free(p);
        return pL;
    }
    suivant=p->suivant;
    while(suivant->suivant!=NULL&&suivant->job[0]!=jobnumero)
    {
        p=p->suivant;
        suivant=p->suivant;
    }
    if(suivant->job[0]==jobnumero)
    {
        p->suivant=suivant->suivant;
        free(suivant);
   }
    return L;
}
void Liveration(TypeCellule **pL)
{
    TypeCellule *p;
    while(*pL!=NULL)
    {
        p=*pL;
        *pL=(*pL)->suivant;
        free(p);
    }
    *pL=NULL;
}

TypeCellule * InsereElement(TypeCellule *L, int jobnumero, int xi)
{
    TypeCellule *p, *nouveau, *suivant;
    nouveau=(TypeCellule*)malloc(sizeof(TypeCellule));
    nouveau->job[0]=jobnumero;
    nouveau->job[1]=xi;
    nouveau->suivant=NULL;
    
    if(L==NULL)
		return nouveau;
		
    p=L;
    suivant=p->suivant;
    if(p->job[1]<=xi)
    {
        if(suivant==NULL)
        {
            p->suivant=nouveau;
            return L;
        }

    }
    else
    {
        nouveau->suivant=p;
        return nouveau;
    }
    
    while(suivant->suivant!=NULL&&suivant->job[1]<=xi)
    {
        p=p->suivant;
        suivant=p->suivant;
    }
    
    if(suivant->job[1]>xi)
    {
        p->suivant=nouveau;
        nouveau->suivant=suivant;
    }
    else
		suivant->suivant=nouveau;
		
    return L;
}
TypeCellule *Tri (TypeCellule *L)
{
   TypeCellule *p,*Listetriee=NULL;
   p=L;
   while(p!=NULL)
   {
       Listetriee=InsereElement(Listetriee,p->job[0],p->job[1]);
       p=p->suivant;
   }
   return Listetriee;
}

