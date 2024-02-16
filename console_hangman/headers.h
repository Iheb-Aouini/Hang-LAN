// headers.h

#ifndef HEADERS_H
#define HEADERS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#define MAX_WORDS 120
#define MAX_WORD_LENGTH 50

struct Noeud {
    char lettre;
    struct Noeud *fils_gauche;
    struct Noeud *fils_droit;
    struct Noeud *parent;
};

struct ClientData
{
    int clientSocket;
    char word[MAX_WORD_LENGTH];
    char guessedWord[MAX_WORD_LENGTH];
    int attempts;
} ;


struct WordList {
    char words[MAX_WORDS][MAX_WORDS];
    int count;
};

struct Noeud *creerNoeud(char lettre);
void afficherArbre(struct Noeud *noeud);
void lireMotsDepuisFichier(struct Noeud *racine, const char *nomFichier);
void insererMot_arbre(struct Noeud *racine, char *mot);
void insererMot_dictionnaire(char *mot);
void Rechercher_mot(struct Noeud *racine, char *mot);
void supprimerMot(struct Noeud *racine, char *mot);
void supprimerMotEntier(struct Noeud *noeud);
void afficherMots(struct Noeud *noeud, char mot[], int index);
void collectWords(struct Noeud *noeud, char mot[], int index, struct WordList *wordList);
char *chooseRandomWord(const char *filename, int level);

void printAllWords(struct WordList *wordList);
void afficherTousLesMots(struct Noeud *racine, int level);
void playHangman(const char *selectedWord, int maxAttempts, int maxIncorrectAttempts, int timeLimitSeconds, int difficulty);
void dessinerPendu(int incorrectAttempts);
void client_connect_and_play();
void run_server_handle_game_logic_and_clients(struct Noeud *racine);
void printGameState(char gameState[200]);
void *handleClient(void *arg);
void playHangman_server_side(struct ClientData *clientData);
void displayBestScores(int level);


#endif
