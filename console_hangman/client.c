#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <curl/curl.h>

#define PORT 10933
#define MAX_WORD_LENGTH 300

void printGameState(char gameState[200]);

void client_connect_and_play() {
    
    char server_ip[17];
    int server_port;
    int clientSocket;
    struct sockaddr_in serverAddr;

    // Create socket
    if ((clientSocket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }
    //printf("\n Enter game server ip : ");
    //scanf("%s",server_ip);
    //printf("\n Enter game server port : ");
   // scanf("%d",&server_port);

    // Configure server address struct
    
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = inet_addr("3.69.157.220");

    // Connect to the server
    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Error connecting to server");
        exit(EXIT_FAILURE);
    }


    // Receive initial game state from the server
    char gameState[MAX_WORD_LENGTH];
    ssize_t receivedBytes=recv(clientSocket, gameState, sizeof(gameState), 0);
      if (receivedBytes == -1) {
        perror("Error receiving initial game state");
        close(clientSocket);
        exit(EXIT_FAILURE);
    }
        gameState[receivedBytes] = '\0';  // Ensure null-termination

    char gameOverMsg[] = "\nGame Over! One of the players had guessed the secret word.\n";
    char message_win[]="\nCongratulations , You successfuly guessed the word and saved the Hangman!";
    char max_attempt_msg[]="\nGame Over: Sorry! Maximum number of attempts reached.\n";
    // Print initial game state
    printGameState(gameState);

    // Game loop
    while (1) {
        memset(gameState,0,strlen(gameState));

        // Get user's guessed letter
        printf("\nEnter your guess: ");
        char guess;
        scanf(" %c", &guess);

        // Send the guessed letter to the server
        send(clientSocket, &guess, sizeof(char), 0);
        sleep(1);// to avoid colision with other requests

        // Receive updated game state from the server
        receivedBytes= recv(clientSocket, gameState, MAX_WORD_LENGTH, 0); //number of received bytes 
        if (receivedBytes == -1) {
            perror("Error receiving updated game state");
            break;
        }
        gameState[receivedBytes] = '\0';  // Ensure null-termination TO AVOID PRINTING NON WANTED CHARACTERS (memory leaks)

        sleep(0.2);// to avoid colision with other requests

        
        // Print updated game state
        printGameState(gameState);

        // Check for game over message
        if (strstr(gameState,gameOverMsg) != NULL ||strstr(gameState,message_win)!=NULL|| strstr(gameState, max_attempt_msg) != NULL) {
            break; 
        }
    }

    // Close the client socket
    close(clientSocket);

}

void printGameState(char gameState[200]) {
   
    printf("\n%s", gameState);
}












































    //retrieving client ip
   /*  char *ip_address = get_public_ip();
    if (ip_address) {
        printf("Public IP address: %s\n", ip_address);
        //send(clientSocket, ip_address, strlen(ip_address) + 1, 0);

        free(ip_address); // Remember to free the allocated memory
    } else {
        fprintf(stderr, "Failed to retrieve public IP address\n");
    }
    */





/*

// Struct to hold response data from cURL
struct MemoryStruct {
    char *memory;
    size_t size;
};

// Write callback function to handle cURL response
static size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)userp;

    // Allocate memory or reallocate existing memory
    if (mem->memory == NULL) {
        mem->memory = (char *)malloc(realsize + 1);
    } else {
        mem->memory = (char *)realloc(mem->memory, mem->size + realsize + 1);
    }

    if (mem->memory == NULL) {
        fprintf(stderr, "Failed to allocate/reallocate memory\n");
        return 0;
    }

    // Copy received data to memory
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;

    return realsize;
}

// Function to retrieve the public IP address using cURL and return it as a string
char* get_public_ip() {
    CURL *curl = curl_easy_init();
    if (!curl) {
        fprintf(stderr, "Failed to initialize cURL\n");
        exit(EXIT_FAILURE);
    }

    struct MemoryStruct chunk;
    chunk.memory = NULL;
    chunk.size = 0;

    curl_easy_setopt(curl, CURLOPT_URL, "https://api.ipify.org");
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        fprintf(stderr, "cURL failed: %s\n", curl_easy_strerror(res));
        if (chunk.memory)
            free(chunk.memory);
        curl_easy_cleanup(curl);
        exit(EXIT_FAILURE);
    }

    curl_easy_cleanup(curl);

    // Null-terminate the string received from cURL
    if (chunk.memory)
        chunk.memory[chunk.size] = '\0';

    return chunk.memory;
}


*/
