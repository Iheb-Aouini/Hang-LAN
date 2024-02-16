#include "headers.h"

struct Noeud *creerNoeud(char lettre)
{
    struct Noeud *nouveau_noeud = (struct Noeud *)malloc(sizeof(struct Noeud));
    nouveau_noeud->lettre = lettre;
    nouveau_noeud->fils_gauche = NULL;
    nouveau_noeud->fils_droit = NULL;
    nouveau_noeud->parent = NULL;
    return nouveau_noeud;
}
void lireMotsDepuisFichier(struct Noeud *racine, const char *nomFichier)
{
    FILE *fichier = fopen(nomFichier, "r");
    if (fichier != NULL)
    {
        char mot[50];

        while (fgets(mot, sizeof(mot), fichier) != NULL)
        {
            strtok(mot, "\n");
            insererMot_arbre(racine, mot);
        }
        fclose(fichier);
    }
    else
    {
        printf("Erreur lors de l'ouverture du fichier.\n");
    }
}

void insererMot_arbre(struct Noeud *racine, char *mot)
{
    int i = 0;
    int lettre_presente = 0;
    struct Noeud *noeud_actuel = racine;

    while (i < strlen(mot))
    {
        if ((noeud_actuel->lettre == '\0') && (noeud_actuel->fils_droit == NULL) && (noeud_actuel->fils_gauche == NULL))
        {
            //if node is empty then insert in it 
            noeud_actuel->lettre = mot[i];
            noeud_actuel->fils_gauche = creerNoeud('\0');
            noeud_actuel->fils_gauche->parent = noeud_actuel;
        }

        else // node is not empty 
        {
            struct Noeud *tmp_noeud = noeud_actuel;

            lettre_presente = 0;

            if (noeud_actuel->lettre == mot[i])
            {
                lettre_presente = 1;  //check if the letter exists in this current node if yes then ignore it and move on 
            }
            while ((noeud_actuel->fils_droit != NULL) && (lettre_presente == 0))
            {//check if the letter exists in the others nodes if yes then ignore it and move on, else lettre will be inserted later
                noeud_actuel = noeud_actuel->fils_droit;
                if (noeud_actuel->lettre == mot[i])
                {
                    lettre_presente = 1;
                }
            }

            if (lettre_presente == 0) // inserting the letter cause its not found
            {
                int check_ordre_lettre = 0; //checks asci order of letter to insert it in the correct position (arbre de recherche)
                if ((tmp_noeud->lettre > mot[i]) && (tmp_noeud->lettre != '#'))
                {
                    check_ordre_lettre = 1;
                }
                while ((tmp_noeud->fils_droit != NULL) && (check_ordre_lettre == 0))
                {
                    tmp_noeud = tmp_noeud->fils_droit;
                    if ((tmp_noeud->lettre > mot[i]) && (tmp_noeud->lettre != '#'))
                    {
                        //if a letter node is bigger than the letter then insert it in current node  else insert after it 
                        // printf("found a node bigger than me");
                        check_ordre_lettre = 1;
                    }
                }

                if (check_ordre_lettre == 0)
                {//insert it on fils droit 
                    noeud_actuel->fils_droit = creerNoeud(mot[i]);
                    noeud_actuel->fils_droit->parent = noeud_actuel;
                    noeud_actuel = noeud_actuel->fils_droit;
                    noeud_actuel->fils_gauche = creerNoeud('\0');
                    noeud_actuel->fils_gauche->parent = noeud_actuel;
                }
                else
                {
                    //insert it in current node place 
                    struct Noeud *new_node = creerNoeud('\0');
                    new_node->lettre = tmp_noeud->lettre;
                    new_node->fils_droit = tmp_noeud->fils_droit;
                    new_node->fils_gauche = tmp_noeud->fils_gauche;
                    new_node->fils_gauche->parent = new_node;

                    tmp_noeud->lettre = mot[i];
                    tmp_noeud->fils_droit = new_node;
                    tmp_noeud->fils_droit->parent = tmp_noeud;
                    tmp_noeud->fils_gauche = creerNoeud('\0');
                    tmp_noeud->fils_gauche->parent = tmp_noeud;
                    noeud_actuel = tmp_noeud;
                }
            }
        }
        noeud_actuel = noeud_actuel->fils_gauche;
        i++;
    } // end while

    noeud_actuel = noeud_actuel->parent;
    if (noeud_actuel->fils_gauche->lettre == '\0')
    {//marking the end of words with #
        noeud_actuel->fils_gauche = creerNoeud('#');
        noeud_actuel->fils_gauche->parent = noeud_actuel;
    }
    else
    {//if fils gauche not empty , executed in cases of words like 'test' and 'testing' (that shares first part (test))
        struct Noeud *tmp;
        tmp = noeud_actuel->fils_gauche;
        noeud_actuel->fils_gauche = creerNoeud('#');
        noeud_actuel->fils_gauche->parent = noeud_actuel;

        noeud_actuel->fils_gauche->fils_droit = tmp;
        noeud_actuel->fils_gauche->fils_droit->parent = noeud_actuel->fils_gauche;
    }
}


void insererMot_dictionnaire(char *mot)
{
    FILE *fichier = fopen("input_output_files/dictionnaire.txt", "a");
    if (fichier != NULL)
    {
        fprintf(fichier, "%s\n", mot);
        fclose(fichier);
    }
    else
    {
        printf("Error opening the file.\n");
    }
}

void Rechercher_mot(struct Noeud *racine, char *mot)
{
    int i = 0;
    bool lettre_trouve = true;
    struct Noeud *noeud_actuel = racine;

    while (i < strlen(mot) && lettre_trouve)
    {
        while (noeud_actuel != NULL)
        {
            if (noeud_actuel->lettre == mot[i])
            {
                lettre_trouve = true;
                printf("ok %c \n", noeud_actuel->lettre);
                break;
            }
            else
            {
                lettre_trouve = false;
                printf("down %c \n", noeud_actuel->lettre);
                noeud_actuel = noeud_actuel->fils_droit;
            }
        }

        if (lettre_trouve)
        {
            noeud_actuel = noeud_actuel->fils_gauche;
            i++;
        }
    }

    if ((i == strlen(mot) && lettre_trouve) && (noeud_actuel->lettre == '#'))
    {
        printf("\n Le mot : %s existe dans l'arbre \n", mot);
    }
    else
    {
        printf("\nLe mot : %s non trouvé 404 \n", mot);
    }
}
void supprimerMot(struct Noeud *racine, char *mot)
{

    struct Noeud *noeud_actuel = racine;

    // Find the node corresponding to the last character of the word
    for (int i = 0; i < strlen(mot); i++)
    {
        while (noeud_actuel != NULL)
        {
            if (noeud_actuel->lettre == mot[i])
            {
                if (i == strlen(mot) - 1)
                {
                    // Last character of the word
                    // Delete the entire word
                    supprimerMotEntier(noeud_actuel);
                    printf("Le mot \"%s\" a été supprimé de l'arbre.\n", mot);
                }

                // Move to the next level
                noeud_actuel = noeud_actuel->fils_gauche;
                break;
            }
            else
            {
                // Move to the next sibling
                noeud_actuel = noeud_actuel->fils_droit;
            }
        }

        if (noeud_actuel == NULL)
        {
            // The word or part of the word is not found
            printf("Le mot \"%s\" n'existe pas dans l'arbre.\n", mot);
            return;
        }
    }
    // Supprimer le mot du fichier dictionnaire.txt
    FILE *fichier = fopen("input_output_files/dictionnaire.txt", "r");
    if (fichier != NULL)
    {
        char buffer[100];
        FILE *nouveau_fichier = fopen("input_output_files/temp.txt", "w");

        int motTrouve = 0;

        while (fgets(buffer, sizeof(buffer), fichier) != NULL)
        {
            // Comparer le mot dans le fichier avec le mot Ã  supprimer
            if (strncmp(buffer, mot, strlen(mot)) == 0)
            {
                motTrouve = 1;
            }
            else
            {
                fprintf(nouveau_fichier, "%s", buffer);
            }
        }

        fclose(fichier);
        fclose(nouveau_fichier);
        remove("input_output_files/dictionnaire.txt");
        rename("input_output_files/temp.txt", "input_output_files/dictionnaire.txt");

        // Si le mot n'a pas Ã©tÃ© trouvÃ© dans le fichier, afficher un message
        if (!motTrouve)
        {
            printf("Le mot n'existe pas dans le dictionnaire.\n");
            return;
        }
    }
    else
    {
        printf("Erreur lors de l'ouverture du fichier.\n");
        return;
    }
}
void supprimerMotEntier(struct Noeud *noeud)
{
    if (noeud != NULL)
    {
        // Recursively delete the entire word
        supprimerMotEntier(noeud->fils_gauche);
        supprimerMotEntier(noeud->fils_droit);

        // Update parent's pointers to remove the node
        if (noeud->parent != NULL)
        {
            if (noeud->parent->fils_gauche == noeud)
            {
                noeud->parent->fils_gauche = NULL;
            }
            else
            {
                noeud->parent->fils_droit = NULL;
            }

            free(noeud); // Free memory associated with the deleted node
        }
        else
        {
            // If the node has no parent, it is the root
            // Set the root to NULL
            free(noeud);
        }
    }
}

void afficherMots(struct Noeud *noeud, char mot[], int index)
{
    if (noeud != NULL)
    {
        mot[index] = noeud->lettre;

        if (noeud->lettre == '#')
        {
            mot[index + 1] = '\0'; // Null-terminate the string
            printf("%s\n", mot);
        }

        afficherMots(noeud->fils_gauche, mot, index + 1);
        afficherMots(noeud->fils_droit, mot, index);
    }
}
