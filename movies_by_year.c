#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

int main()
{
    /*
    Prompt user to select 1 or exit program
    */
    int mainMenu = 0;
    // Loops the main program indefinitely
    while (!mainMenu)
    {
        char *prompt = "\n1. Select file to process\n2. Exit the program";
        printf("%s", prompt);
        char *lineEntered = NULL;
        int numCharsEntered = -5;
        int currChar = -5;
        size_t bufferSize = 0;
        int ok = 0;

        // Loops until the correct input is given
        while (!ok)
        {
            numCharsEntered = getline(&lineEntered, &bufferSize, stdin);

            if (lineEntered[0] >= 49 && lineEntered[0] <= 50 && strlen(lineEntered) == 2)
            {
                printf("%s", "\n");
                ok = 1;
            }

            else
            {
                printf("%s", "Invalid input.\nPlease enter a number from 1 to 2: ");
            }
        }
        char selection = lineEntered[0];

        if (selection == 1)
        {
            printf("Selected 1. Moving on to data processing.\n\n\n");
        }

        else
        {
            printf("Exiting program.\n\n\n");
            mainMenu = 1;
        }
    }

    /*

    */

    return 0;
}