#include "headers.h"

// Wrapper function to initiate the recursive call and print all words
void afficherTousLesMots(struct Noeud *racine,int level )
{
    // Initialize the WordList structure
    struct WordList wordList;
    wordList.count = 0;

    char mot[MAX_WORDS];

    // Collect words from the tree
    collectWords(racine, mot, 0, &wordList);

    // Seed the random number generator with the current time
    srand((unsigned int)time(NULL));

 
    printAllWords(&wordList);
}
