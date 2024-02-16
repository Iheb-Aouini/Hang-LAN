#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "headers.h"
#include <signal.h>

#define PORT 52631
#define MAX_PENDING_CONNECTIONS 5
#define MAX_WORD_LENGTH 300

// use ngrok tcp 192.168.1.21:52631 to make it publicly accessible (port forwarding)
// ncat 3.125.102.39 17921
int level;
int gameOver = 0;                                         // Global flag indicating if the game is over
pthread_mutex_t gameOverLock = PTHREAD_MUTEX_INITIALIZER; // Mutex to protect access to the gameOver flag

void *handleClient(void *arg);
void playHangman_server_side(struct ClientData *clientData);

// Function to check if a character is a valid letter
int isValidLetter(char c)
{
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}

void run_server_handle_game_logic_and_clients(struct Noeud *racine)
{
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
    pthread_t threadID;

    // Create socket
    if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    // Configure server address struct
    serverAddr.sin_family = AF_INET; // IPV4
    serverAddr.sin_port = htons(PORT);

    // char* external_ip = "192.168.1.21";

    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY); // set it to any available interfaces (wifi , ethernet, lo ....)
                                                    // serverAddr.sin_addr.s_addr = inet_addr(external_ip);

    // Bind the socket
    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
    {
        perror("Error binding socket");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(serverSocket, MAX_PENDING_CONNECTIONS) == -1)
    {
        perror("Error listening for connections");
        exit(EXIT_FAILURE);
    }

    printf("Hangman server is running on port %d...\n", PORT);
    int status = 0;

    // char secret_word[MAX_WORD_LENGTH]; //set secret word manualy
    // printf("Enter the word for hangman: ");
    // scanf("%s", secret_word);

    printf("              |  Level  |                                          |\n");
    printf("              ------------------------------------------------------\n");
    printf("              |    1    |               Easy                       |\n");
    printf("              |    2    |               Medium                     |\n");
    printf("              |    3    |               Hard                       |\n");
    printf("              ------------------------------------------------------\n");
    printf("\n");
    printf("	          Select the level: ");
    scanf("%d", &level);
    // Initialize the WordList structure
    struct WordList wordList;
    wordList.count = 0;

    char mot[MAX_WORD_LENGTH];

    // Collect words from the tree
    collectWords(racine, mot, 0, &wordList);
    // select random word from wordlist based on the level
    srand((unsigned int)time(NULL));
    char *secret_word = chooseRandomWord(&wordList, level);
    printf("\n Random word selected : %s \n", secret_word);
    while (1)
    {
        // Accept a connection
        if ((clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &clientAddrLen)) == -1)
        {
            perror("Error accepting connection");
            continue;
        }

        char clientIP[INET_ADDRSTRLEN];                                      // Declaration of clientIP
        inet_ntop(AF_INET, &clientAddr.sin_addr, clientIP, INET_ADDRSTRLEN); // extracting client IP(bug requires fix )

        printf("\nClient connected from: %s\n", clientIP);

        // Prepare client data
        struct ClientData *clientData = (struct ClientData *)malloc(sizeof(struct ClientData));
        clientData->clientSocket = clientSocket;
        if (level == 1) // easy
        {
            clientData->attempts = 10;
        }
        else if (level == 2) // medium
        {
            clientData->attempts = 8;
        }
        else if (level == 3) // hard
        {
            clientData->attempts = 7;
        }


        pthread_mutex_lock(&gameOverLock); // start secion critique
        if (gameOver == 1) //game over
        {
            printf("\nOne of the player won the game , updating a new secret word ....\n");
            
            printf("              |  Level  |                                          |\n");
            printf("              ------------------------------------------------------\n");
            printf("              |    1    |               Easy                       |\n");
            printf("              |    2    |               Medium                     |\n");
            printf("              |    3    |               Hard                       |\n");
            printf("              ------------------------------------------------------\n");
            printf("\n");
            printf("	          Select the level: ");
            scanf("%d", &level);

            // Seed the random number generator with the current time
            srand((unsigned int)time(NULL));
            secret_word = chooseRandomWord(&wordList, level);
            printf("\n Random word selected : %s \n", secret_word);
            strcpy(clientData->word, secret_word); //updating secret word

            gameOver = 0; //reseting gameover to 0 to allow other players to play
        }
        else
        {
            // sets same secret word 
            strcpy(clientData->word, secret_word);
        }

        pthread_mutex_unlock(&gameOverLock); //ends section critique

        // Initialize guessed word with underscores
        int wordLength = strlen(clientData->word);
        for (int i = 0; i < wordLength; ++i)
        {
            clientData->guessedWord[i] = '_';
        }
        clientData->guessedWord[wordLength] = '\0';

        // Create a new thread to handle the client
        if (pthread_create(&threadID, NULL, handleClient, (void *)clientData) != 0)
        {
            perror("Error creating thread");
            close(clientSocket);
            free(clientData);
            continue;
        }

        // Detach the thread to avoid memory leaks
        pthread_detach(threadID);
    }

    // Close the server socket
    close(serverSocket);
}







void *handleClient(void *arg)
{
    struct ClientData *clientData = (struct ClientData *)arg;

    // Handle game logic in a separate function
    playHangman_server_side(clientData);

    // Free memory allocated for client data
    free(clientData);

    // Close the connection after the game
    close(clientData->clientSocket);
    printf("\nClient disconnected\n");

    return NULL;
}







void playHangman_server_side(struct ClientData *clientData)
{
    // Acquire the lock before changing the localgameOver flag
    pthread_mutex_lock(&gameOverLock);
    int localGameOver = gameOver;
    pthread_mutex_unlock(&gameOverLock);
    char gameOverMsg[] = "\nGame Over! One of the players had guessed the secret word.\n";
    char invalid_msg[] = "Invalid guess please enter a Letter only";
    if (localGameOver)
    {
        // If the game is already over, send a message to the client and return
        send(clientData->clientSocket, gameOverMsg, strlen(gameOver), 0);
        return;
    }

    // Send initial game state to the client (sends for exemple : ------)
    send(clientData->clientSocket, clientData->guessedWord, strlen(clientData->guessedWord), 0);

    // Game loop
    while (clientData->attempts > 0)
    {
        pthread_mutex_lock(&gameOverLock); //semaphore to protect it while assigning it 
        int localGameOver = gameOver; // local gamer over is only for currect client not global like gameover for all client
        pthread_mutex_unlock(&gameOverLock);
        if (localGameOver)
        { //game is over , send game over message
            send(clientData->clientSocket, gameOverMsg, strlen(gameOverMsg), 0);

            break;
        }
        // Receive guessed letter from client
        char guess;

        recv(clientData->clientSocket, &guess, sizeof(char), 0);
        // Validate the received character to prevent injection
        if (!isValidLetter(guess)) // checks if its not alphabetic character
        {
            send(clientData->clientSocket, invalid_msg, strlen(invalid_msg), 0);

            continue; // Skip processing invalid input
        }
        // Check if the guessed letter is in the word
        int correctGuess = 0;
        for (int i = 0; i < strlen(clientData->word); ++i)
        {
            if (clientData->word[i] == guess)
            {
                clientData->guessedWord[i] = guess;
                correctGuess = 1;
            }
        }

       
        // Acquire the lock before checking the gameOver flag
        pthread_mutex_lock(&gameOverLock);
        localGameOver = gameOver;
        pthread_mutex_unlock(&gameOverLock);

        // Check if the word is completely guessed
        if (strcmp(clientData->guessedWord, clientData->word) == 0)
        {
            // Set the global gameOver flag
            pthread_mutex_lock(&gameOverLock);
            gameOver = 1;
            pthread_mutex_unlock(&gameOverLock);
            char message_win[] = "\nCongratulations , You successfuly guessed the word and saved the Hangman!";
            // Notify current client that he wins , and all other clients about game over
            send(clientData->clientSocket, message_win, strlen(message_win), 0);
            break;
        }

        // Check if the guessed letter was incorrect
        char attemptsMsg[70];
        memset(attemptsMsg, 0, strlen(attemptsMsg));
        if (!correctGuess)
        {
            clientData->attempts--;
            snprintf(attemptsMsg, sizeof(attemptsMsg), "\nWrong guess ,Attempts remaining: %d\n", clientData->attempts);
        }
        else
        {
            snprintf(attemptsMsg, sizeof(attemptsMsg), "\nYou guessed the letter correctly \n Attempts remaining: %d\n", clientData->attempts);
        } // attempts remaining : !

        send(clientData->clientSocket, attemptsMsg, strlen(attemptsMsg), 0);
         // Send updated game state to the client 
        send(clientData->clientSocket, clientData->guessedWord, strlen(clientData->guessedWord), 0);

        // Acquire lock before changing localgameOver 
        pthread_mutex_lock(&gameOverLock);
        localGameOver = gameOver;
        pthread_mutex_unlock(&gameOverLock);

        // Check if the player has run out of attempts
        if (clientData->attempts == 0)
        {
            localGameOver=1;
            // Notify all clients about game over
            char max_attempt_msg[] = "\nGame Over: Sorry! Maximum number of attempts reached.\n";
            send(clientData->clientSocket, max_attempt_msg, strlen(max_attempt_msg), 0);
            break;
        }
    }
}
