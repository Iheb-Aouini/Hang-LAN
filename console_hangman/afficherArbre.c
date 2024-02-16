#include "headers.h"

void afficherArbre(struct Noeud *noeud)
{
    if (noeud != NULL)
    {
        printf("Noeud : %c\n", noeud->lettre);
        if (noeud->lettre != '#')
        {// to skip printing FG of # which is NULL always
            printf("FG de %c : ", noeud->lettre);
            afficherArbre(noeud->fils_gauche);
        }
        else{
            printf("\n"); //making new line every new word
        }

        if ((noeud->lettre != '\0') && (noeud->fils_droit != NULL))
        { // to skip empty node printing
            printf("FD de %c : ", noeud->lettre);
            afficherArbre(noeud->fils_droit);
        }
    }
   
}
