#include "headers.h"

int main()
{
    #ifdef _WIN32
        system("cls");
#else
        system("clear");
#endif
    srand((unsigned int)time(NULL));

    char motSecret[50];
    int niveau;
    int optionx;

    struct Noeud *racine = creerNoeud('\0');
    lireMotsDepuisFichier(racine, "input_output_files/dictionnaire.txt");
    int choix;
    int level;
    char mot[50];

    unsigned int seed = (unsigned int)time(NULL);
    srand(seed);

    printf("\n");
    printf("                               Welcome to Hangman Game!          \n");
    printf("\n");
    printf("\n");

    while (1)
    {

        // Display the menu in a more aesthetically pleasing format
        printf("              ------------------------------------------------------\n");
        printf("              |                 Main Menu                          |\n");
        printf("              ------------------------------------------------------\n");
        printf("              |  Option |            Operation                     |\n");
        printf("              ------------------------------------------------------\n");
        printf("              |    1    |    Insert a word                         |\n");
        printf("              |    2    |    Display the tree                      |\n");
        printf("              |    3    |    Search for a word                     |\n");
        printf("              |    4    |    Display all words in the tree         |\n");
        printf("              |    5    |    Delete a word                         |\n");
        printf("              |    6    |    Play game : Single Player             |\n");
        printf("              |    7    |    Play game : MultiPlayer (LAN)         |\n");
        printf("              |    8    |    Quit                                  |\n");
        printf("              ------------------------------------------------------\n");

        // User input
        printf("\n");
        printf("    Choose: ");
        scanf("%d", &choix);

        // Handle user choices based on the selected option
        switch (choix)
        {
        case 1:
            // Insert a word
            printf("Enter the word: ");
            scanf("%s", mot);
            insererMot_arbre(racine, mot);
            insererMot_dictionnaire(mot);

            break;

        case 2:
            // Display the tree
            racine = creerNoeud('\0');
            lireMotsDepuisFichier(racine, "input_output_files/dictionnaire.txt");
            if (racine->lettre != '\0')
                printf("The letter of the root is: %c\n", racine->lettre);
            else
                printf("The root has no letter.\n");
            printf("Displaying the tree:\n");
            afficherArbre(racine);
            break;

        case 3:
            // Search for a word
            printf("Enter the word to search: ");
            scanf("%s", mot);
            Rechercher_mot(racine, mot);
            break;

        case 4:
            // Display all words in the tree
            afficherTousLesMots(racine, 1);
            break;

        case 5:
            // Delete a word
            printf("Enter the word to delete: ");
            scanf("%s", mot);
            supprimerMot(racine, mot);
            break;

 case 6:
    // Play game or display scores
    while (1) {
        // Clear the terminal
        #ifdef _WIN32
        system("cls");
        #else
        system("clear");
        #endif

        printf("              ------------------------------------------------------\n");
        printf("              |              Welcome to Hangman!                   |\n");
        printf("              ------------------------------------------------------\n");
        printf("              |  Level  |                                          |\n");
        printf("              ------------------------------------------------------\n");
        printf("              |    1    |               Easy                       |\n");
        printf("              |    2    |               Medium                     |\n");
        printf("              |    3    |               Hard                       |\n");
        printf("              |    4    |          Display Scores                   |\n");
        printf("              ------------------------------------------------------\n");
        printf("\n");
        printf("    Enter the option: ");
        scanf("%d", &optionx);

        if (optionx == 4) {
            // Display scores for a specific level
        printf("	     +-------------------------------------+\n");
	printf("	     | Choose the level to display scores: |\n");
	printf("	     +-------------------------------------+\n");
	printf("	     |  Level  | Difficulty                |\n");
	printf("	     +-------------------------------------+\n");
	printf("	     |    1    |     Easy                  |\n");
	printf("	     |    2    |    Medium                 |\n");
	printf("             |    3    |     Hard                  |\n");
	printf("             +-------------------------------------+\n");
        printf("	Enter your choice: ");

            int subOption;
            scanf("%d", &subOption);

            switch (subOption) {
                case 1:
                    displayScoresForLevel(1); // Easy
                    break;
                case 2:
                    displayScoresForLevel(2); // Medium
                    break;
                case 3:
                    displayScoresForLevel(3); // Hard
                    break;
                default:
                    printf("Invalid sub-option. Please choose a valid level.\n");
                    break;
            }
            break;
        } else if (optionx >= 1 && optionx <= 3) {
            // Play Hangman with the chosen level
            playHangmanWithRandomWordAndLevel(racine, optionx);
            break;
        } else {
            printf("Invalid option. Please choose a valid option.\n");
        }
    }
    break;

        case 7:
            // Multiplayer Game in LAN

            int choice_multi;
             printf("              ------------------------------------------------------\n");
            printf("              |          Welcome to Hangman  Multiplayer Beta v1.0!  |\n");
            printf("              ------------------------------------------------------\n");
            printf("              |  Please choose an option:                            |\n");
            printf("              |  1  |    Run the server (Administrator mode)         |\n");
            printf("              |  2  |    Play the Hangman game (Player mode)         |\n");
            printf("              -------------------------------------------------------\n");
            printf("              Enter your choice: ");
            scanf("%d", &choice_multi);

            if (choice_multi == 1)
            {
                // launch server
                printf("\nLaunching the server...\n");
                run_server_handle_game_logic_and_clients(racine);
            }
            else if (choice_multi == 2)
            {
                // play client mode
                printf("\nConnecting and playing in client mode...\n");
                client_connect_and_play();
            }
            else
            {
                printf("\nInvalid choice. Please enter 1 or 2.\n");
            }
            break;

        case 8:
            // Quit
            printf("Thank you for playing! Goodbye.\n");
            exit(0);
            break;

        default:
            printf("Invalid choice. Please choose a valid option.\n");
            break;
        }

        // Prompt for user to continue

        printf("\nPress Enter to continue...");

        while (getchar() != '\n')
            ;      // Clear input buffer
        getchar(); // Wait for Enter key
                   // Play game
// Clear the terminal
#ifdef _WIN32
        system("cls");
#else
        system("clear");
#endif
    }

    return 0;
}
