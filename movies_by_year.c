#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

// Function to find the name of the largest .csv file in the directory
char *findLargestCSV()
{
    DIR *directory;
    struct dirent *entry;

    directory = opendir(".");
    long maxFileSize = -5;
    struct stat *stats = malloc(sizeof(struct stat));

    while ((entry = readdir(directory)) != NULL)
    {
        char *fname = entry->d_name;
        char *type = ".csv";

        // Validation to make sure only .csv files are read
        if (strcmp(type, &fname[strlen(fname) - 4]) == 0)
        {
            stat(entry->d_name, stats);

            if (stats->st_size > maxFileSize)
            {
                maxFileSize = stats->st_size;
            }
        }
    }

    closedir(directory);
    char *largestFileName;

    directory = opendir(".");
    while ((entry = readdir(directory)) != NULL)
    {
        char *fname = entry->d_name;
        char *type = ".csv";

        // Validation to make sure only .csv files are read
        if (strcmp(type, &fname[strlen(fname) - 4]) == 0)
        {
            stat(entry->d_name, stats);

            if (stats->st_size == maxFileSize)
            {
                largestFileName = fname;
            }
        }
    }

    closedir(directory);
    printf("Largest file is %s at %ld bytes\n", largestFileName, maxFileSize);
    return largestFileName;
}

// Function to find the name of the smallest .csv file in the directory
char *findSmallestCSV()
{
    DIR *directory;
    struct dirent *entry;

    directory = opendir(".");
    long minFileSize = -5;
    struct stat *stats = malloc(sizeof(struct stat));

    while ((entry = readdir(directory)) != NULL)
    {
        char *fname = entry->d_name;
        char *type = ".csv";

        // Validation to make sure only .csv files are read
        if (strcmp(type, &fname[strlen(fname) - 4]) == 0)
        {
            stat(entry->d_name, stats);

            if (stats->st_size < minFileSize || minFileSize == -5)
            {
                minFileSize = stats->st_size;
            }
        }
    }

    closedir(directory);
    char *smallestFileName;

    directory = opendir(".");
    while ((entry = readdir(directory)) != NULL)
    {
        char *fname = entry->d_name;
        char *type = ".csv";

        // Validation to make sure only .csv files are read
        if (strcmp(type, &fname[strlen(fname) - 4]) == 0)
        {
            stat(entry->d_name, stats);

            if (stats->st_size == minFileSize)
            {
                smallestFileName = fname;
            }
        }
    }

    closedir(directory);
    printf("Largest file is %s at %ld bytes\n", smallestFileName, minFileSize);
    return smallestFileName;
}

int main()
{
    /*
    Prompt user to select 1 or exit program
    */
    int mainMenu = 0;
    // Loops the main program indefinitely
    while (!mainMenu)
    {
        char *prompt = "\n1. Select file to process\n2. Exit the program\n\nEnter your selection: ";
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
                printf("%s", "\nInvalid input.\nPlease enter a number from 1 to 2: ");
            }
        }
        char selection = lineEntered[0];

        // User selected 1 at the main menu
        if (selection == 49)
        {
            printf("Which file would you like to process?\n");
            printf("Enter 1 to pick the largest file.\n");
            printf("Enter 2 to pick the smallest file.\n");
            printf("Enter 3 to specify the name of a file.\n\n");
            printf("Enter a selection from 1 to 3: ");

            // Input validation
            ok = 0;
            while (!ok)
            {
                numCharsEntered = getline(&lineEntered, &bufferSize, stdin);

                if (lineEntered[0] >= 49 && lineEntered[0] <= 51 && strlen(lineEntered) == 2)
                {
                    printf("\n");
                    ok = 1;
                }

                else
                {
                    printf("%s", "\nInvalid input.\nPlease enter a number from 1 to 3: ");
                }
            }
            selection = lineEntered[0];

            // The file name to process
            char *fileNameToProcess;

            // User wishes to process largest file
            if (*lineEntered == 49)
            {
                fileNameToProcess = findLargestCSV();
                printf("User entered 1. Exiting.\n");
            }

            // User wishes to process smallest file
            else if (*lineEntered == 50)
            {
                fileNameToProcess = findSmallestCSV();
                printf("User entered 2. Exiting.\n");
            }

            // User wishes to enter file name
            else if (*lineEntered == 51)
            {
                printf("User entered 3. Exiting.\n");
            }
            /* File Processing */
        }

        // User selected 2 at the main menu
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