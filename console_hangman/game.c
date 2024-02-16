#include "headers.h"
// Define the maximum number of levels
#define NUM_LEVELS 3








struct Player {
    char name[50];
    int score;
};


// Function to save the best score for a specific level
void saveBestScore(const char *playerName, int playerScore, int level) {
    struct Player bestPlayers[NUM_LEVELS];

    // Read existing best player information for all levels
    FILE *file = fopen("input_output_files/best_scoress.txt", "r");
    if (file != NULL) {
        for (int i = 0; i < NUM_LEVELS; i++) {
            fscanf(file, "%s %d", bestPlayers[i].name, &bestPlayers[i].score);
        }
        fclose(file);
    }

    // Compare scores and update if necessary for the specified level
    if (playerScore > bestPlayers[level - 1].score) {
        // Update the best score for the specified level
        snprintf(bestPlayers[level - 1].name, sizeof(bestPlayers[level - 1].name), "%s", playerName);
        bestPlayers[level - 1].score = playerScore;

        // Save the updated scores back to the file
        file = fopen("best_scoress.txt", "w");
        for (int i = 0; i < NUM_LEVELS; i++) {
            fprintf(file, "%s %d\n", bestPlayers[i].name, bestPlayers[i].score);
        }
        fclose(file);
    }
}

// Function to calculate the score based on the difficulty level
int calculateScore(int correctGuesses, int incorrectGuesses, int remainingTime, int difficulty) {
    int score = 0;

    // Adjust scoring based on difficulty level
    switch (difficulty) {
        case 1: // Easy
            score = correctGuesses * 10 - incorrectGuesses * 5 + remainingTime * 2;
            break;
        case 2: // Medium
            score = correctGuesses * 15 - incorrectGuesses * 8 + remainingTime * 3;
            break;
        case 3: // Hard
            score = correctGuesses * 20 - incorrectGuesses * 10 + remainingTime * 4;
            break;
        default:
            printf("Invalid difficulty level.\n");
            break;
    }

    return score > 0 ? score : 0;
}


void displayScoresForLevel(int level) {
    struct Player bestPlayers[NUM_LEVELS];

    // Read existing best player information for all levels
    FILE *file = fopen("best_scoress.txt", "r");
    if (file != NULL) {
        for (int i = 0; i < NUM_LEVELS; i++) {
            fscanf(file, "%s %d", bestPlayers[i].name, &bestPlayers[i].score);
        }
        fclose(file);

        // Display best score for the specified level
        printf("Best Scores for Level %d:\n", level);
        printf("----------------------------\n");
        printf("Player: %s\tScore: %d\n", bestPlayers[level - 1].name, bestPlayers[level - 1].score);
    } else {
        printf("Error opening the best scores file.\n");
    }
}
















// Function to collect words from the tree
void collectWords(struct Noeud *noeud, char mot[], int index, struct WordList *wordList)
{
    // Check if the node is not NULL
    if (noeud != NULL)
    {
        // Store the current letter in the word array
        mot[index] = noeud->lettre;

        // If the current letter is a word terminator ('#'), add the word to the word list
        if (noeud->lettre == '#')
        {
            mot[index] = '\0'; // Null-terminate the string
            strcpy(wordList->words[wordList->count], mot); // Copy the word to the word list
            wordList->count++; // Increment the word count in the list
        }

        // Recursively collect words from the left subtree
        collectWords(noeud->fils_gauche, mot, index + 1, wordList);

        // Recursively collect words from the right subtree
        collectWords(noeud->fils_droit, mot, index, wordList);
    }
}



// Function to search for a word in the trie
bool searchWord(struct Noeud *root, const char *word) {
    struct Noeud *currentNode = root; // Start from the root node
    int index = 0; // Index to iterate over characters in the word

    // Iterate until we reach the end of the word or a NULL node in the trie
    while (currentNode != NULL && word[index] != '\0') {
        // If the current node's letter matches the current character in the word
        if (currentNode->lettre == word[index]) {
            currentNode = currentNode->fils_gauche; // Move to the next node (left child)
            index++; // Move to the next character in the word
        } else {
            currentNode = currentNode->fils_droit; // Move to the next node (right child)
        }
    }

    // Check if we reached the end of the word and if the current node represents the end of a word
  
    return (currentNode != NULL && currentNode->lettre == '#' && word[index] == '\0');
}


































char *chooseRandomWord(const char *filename, int level) {
   FILE *file = fopen("input_output_files/dictionnaire.txt", "r");
    if (file == NULL) {
        printf("Error opening file %s\n", filename);
        exit(1);
    }

    char word[100]; // Adjust the size according to your needs
    int validCount = 0;
    char *chosenWord = NULL;

    while (fscanf(file, "%s", word) == 1) {
        int wordLength = strlen(word);

        if ((level == 1 && wordLength >= 1 && wordLength <= 4) ||
            (level == 2 && wordLength >= 5 && wordLength <= 6) ||
            (level == 3 && wordLength >= 7)) {
            validCount++;
            // Choose this word with probability 1/validCount
            if (rand() % validCount == 0) {
                chosenWord = strdup(word);
            }
        }
    }

    fclose(file);
    return chosenWord; // Return the chosen word
}

// Function to print all collected words
void printAllWords(struct WordList *wordList)
{
    printf("All words in the list:\n");
    for (int i = 0; i < wordList->count; i++)
    {
        printf("%s\n", wordList->words[i]);
    }
}



int getRandomNumber(int min, int max) {
    return rand() % (max - min + 1) + min;
}



// Function to play Hangman with a specified word and parameters
void playHangman(const char *selectedWord, int maxAttempts, int maxIncorrectAttempts, int timeLimitSeconds, int difficulty) {
    int wordLength = strlen(selectedWord);
    char guessedWord[wordLength + 1];
    strcpy(guessedWord, selectedWord);

    // Initialize the guessedWord with underscores
    for (int i = 0; i < wordLength; i++) {
        if (guessedWord[i] != '#')
            guessedWord[i] = '*';
    }

    int attempts = maxAttempts; // Number of allowed incorrect attempts
    char guessedLetters[26] = {0}; // Array to track guessed letters
    int correctGuesses = 0;
    int incorrectGuesses = 0;
    int helpUsed = 0; // Counter for help option

    time_t startTime = time(NULL);
    int remainingTime;  // Declare remainingTime outside the loop

    printf("You have %d attempts and %d seconds to guess the word.\n", attempts, timeLimitSeconds);

    while (attempts > 0 && strcmp(guessedWord, selectedWord) != 0) {
        // Display the word with '*' instead of '#' for better visibility
        printf("\nTry to guess the word: ");
        for (int i = 0; i < wordLength; i++) {
            if (selectedWord[i] != '#')
                printf("%c", guessedWord[i]);
            else
                printf(" ");
        }

        printf("\nGuessed letters: ");
        for (int i = 0; i < 26; i++) {
            if (guessedLetters[i])
                printf("%c ", 'a' + i);
        }

        // Display remaining time
        time_t elapsedTime = time(NULL) - startTime;
        remainingTime = timeLimitSeconds - (int)elapsedTime;
        printf("\nTime remaining: %d seconds\n", remainingTime);

        if (difficulty == 3 && helpUsed < 3) {
            printf("Enter a letter or use help (1): ");
        } else {
            printf("Enter a letter: ");
        }

        char guess;
        scanf(" %c", &guess);

        if (difficulty == 3 && guess == '1' && helpUsed < 3 && remainingTime > 0) {
            // Display a random letter as help
            int hiddenIndex = getRandomNumber(0, wordLength - 1);
            while (guessedWord[hiddenIndex] != '*') {
                hiddenIndex = getRandomNumber(0, wordLength - 1);
            }

            guessedWord[hiddenIndex] = selectedWord[hiddenIndex];
            helpUsed++;

            printf("Help used! A letter has been revealed.\n");
            continue;
        }

        // Validate the input
        if ((guess < 'a' || guess > 'z') || guessedLetters[guess - 'a']) {
            printf("Invalid or repeated guess. Please enter a valid letter.\n");
            // Beep on incorrect guess
            printf("\a");
            continue;
        }

        guessedLetters[guess - 'a'] = 1; // Mark the letter as guessed

        int correctGuess = 0;

        for (int i = 0; i < wordLength; i++) {
            if (selectedWord[i] == guess) {
                guessedWord[i] = guess;
                correctGuess = 1;
            }
        }

        if (!correctGuess) {
            // Draw hangman figure before decrementing attempts
            dessinerPendu(maxIncorrectAttempts - attempts);

            attempts--;
            incorrectGuesses++;
            printf("Incorrect guess! Attempts left: %d\n", attempts);
            printf("\a");
        } else {
            correctGuesses++;
            printf("Good guess!\n");
        }

        // Check the time elapsed
        elapsedTime = time(NULL) - startTime;
        remainingTime = timeLimitSeconds - (int)elapsedTime;
        if (remainingTime <= 0) {
            printf("\nTime is up! Game over.\n");
            break;
        }
    }

    // Display the final state of the word
    printf("\nTry to guess the word: %s\n", guessedWord);

    if (strcmp(guessedWord, selectedWord) == 0) {
        printf("Congratulations! You guessed the word: %s\n", selectedWord);
        system("paplay ressources/output.wav");
    } else {
        printf("Sorry, you ran out of attempts. The correct word was: %s\n", selectedWord);
        system("paplay ressources/sound.oga"); 
    }

    // Display scores
   // Calculate player score
        int playerScore = calculateScore(correctGuesses, incorrectGuesses, remainingTime, difficulty);

   
    printf("Your score: %d\n", playerScore);
    
    // Display correct and incorrect guess counts
    printf("Correct Guesses: %d\n", correctGuesses);
    printf("Incorrect Guesses: %d\n", incorrectGuesses);

    // Save the best score to a file
    char playerName[50];
    printf("Enter your name: ");
    scanf("%s", playerName);
    saveBestScore(playerName, playerScore, difficulty);
}




// Wrapper function to initiate the game with a randomly chosen word and difficulty level
void playHangmanWithRandomWordAndLevel(struct Noeud *racine, int level) {
    // Initialize the WordList structure
    struct WordList wordList;
    wordList.count = 0;

    // Seed the random number generator with the current time
    srand((unsigned int)time(NULL));

    // Determine maximum attempts, incorrect attempts, and time limit based on difficulty level
    int maxAttempts, maxIncorrectAttempts, timeLimitSeconds;

    switch (level) {
        case 1: // Easy
            maxAttempts = 10;
            maxIncorrectAttempts = 7;// variable to customse 
            timeLimitSeconds = 120; // 2 minutes
            break;
        case 2: // Medium
            maxAttempts = 8;
            maxIncorrectAttempts = 7;
            timeLimitSeconds = 60; // 1 minute
            break;
        case 3: // Hard
            maxAttempts = 7;
            maxIncorrectAttempts = 7;
            timeLimitSeconds = 40; // 40 seconds
            break;
        default:
            printf("Invalid difficulty level.\n");
            return;
    }

 
    // Choose a random word
    char *randomWord = chooseRandomWord("input_output_files/dictionnaire.txt", level);

    // Play Hangman with the chosen word, difficulty level, and time limit
    if (randomWord != NULL) {
        playHangman(randomWord, maxAttempts, maxIncorrectAttempts, timeLimitSeconds, level);

    } else {
        printf("No words found in the specified level and length range.\n");
    }
}




// Function to draw the hangman figure based on the number of incorrect attempts
void dessinerPendu(int incorrectAttempts)
{
    switch (incorrectAttempts)
    {

    case 1:
        printf("  +---+\n");
        printf("  |   |\n");
        printf("  O   |\n");
        printf("      |\n");
        printf("      |\n");
        break;
    case 2:
        printf("  +---+\n");
        printf("  |   |\n");
        printf("  O   |\n");
        printf("  |   |\n");
        printf("      |\n");
        break;
    case 3:
        printf("  +---+\n");
        printf("  |   |\n");
        printf("  O   |\n");
        printf(" /|   |\n");
        printf("      |\n");
        break;
    case 4:
        printf("  +---+\n");
        printf("  |   |\n");
        printf("  O   |\n");
        printf(" /|\\  |\n");
        printf("      |\n");
        break;
    case 5:
        printf("  +---+\n");
        printf("  |   |\n");
        printf("  O   |\n");
        printf(" /|\\  |\n");
        printf(" /    |\n");
        break;
    case 6:
        printf("  +---+\n");
        printf("  |   |\n");
        printf("  O   |\n");
        printf(" /|\\  |\n");
        printf(" / \\  |\n");
        break;
    default:
        break;
    }
}

