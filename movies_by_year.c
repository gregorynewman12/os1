#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <unistd.h>

// Movie struct, same as assignment #1
struct movie
{
    char *title;
    int year;
    char *languages;
    float rating;
    struct movie *next;
};

// Creates a struct for an individual movie
struct movie *createMovie(char *currLine)
{
    // Pointer to hold current movie being created
    struct movie *currMovie = malloc(sizeof(struct movie));

    // Keeps track of where in the string the "cursor" is
    char *saveptr;

    // Tokenizes and saves title
    char *token = strtok_r(currLine, ",", &saveptr);
    // calloc takes two arguments: the number of blocks to allocate
    // and the size of each block. Here it allocates enough char-sized
    // blocks to hold all the chars and the null terminator.
    currMovie->title = calloc(strlen(token) + 1, sizeof(char));
    strcpy(currMovie->title, token);

    // Tokenizes and saves year
    token = strtok_r(NULL, ",", &saveptr);
    currMovie->year = atoi(token);

    // Tokenizes and saves languages
    token = strtok_r(NULL, ",", &saveptr);
    char *languages = token;
    // Trims the leading "["
    languages = &languages[1];
    // Trims the tailing "]"
    languages[strlen(languages) - 1] = '\0';
    currMovie->languages = calloc(strlen(token) + 1, sizeof(char));
    strcpy(currMovie->languages, languages);

    // Tokenizes and saves rating
    token = strtok_r(NULL, ",", &saveptr);
    currMovie->rating = atof(token);

    // Set next node to NULL
    currMovie->next == NULL;

    return currMovie;
}

// Creates a linked list of movie structs for each movie in the file
struct movie *processFile(char *filePath)
{
    // Open the specified file for reading only
    FILE *movieFile = fopen(filePath, "r");

    char *currLine = NULL;
    size_t len = 0;
    size_t nread;

    // The head of the linked list
    struct movie *head = NULL;
    // The tail of the linked list
    struct movie *tail = NULL;
    int linesProcessed = 0;

    // Read the file line by line
    while ((nread = getline(&currLine, &len, movieFile)) != -1)
    {
        if (linesProcessed != 0)
        {
            // Get a new movie node corresponding to the current line
            struct movie *newNode = createMovie(currLine);
            // Is this the first node in the linked list?
            if (head == NULL)
            {
                // This is the first node in the linked list
                // Set the head and the tail to this node
                head = newNode;
                tail = newNode;
            }
            else
            {
                // This is not the first node.
                // Add this node to the list and advance the tail
                tail->next = newNode;
                tail = newNode;
            }
        }
        linesProcessed++;
    }
    free(currLine);
    currLine = NULL;
    fclose(movieFile);
    return head;
}

// Function to find the name of the largest .csv file in the directory
char *getLargestCSV()
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
    printf("Now processing the largest file [%s] at %ld bytes\n", largestFileName, maxFileSize);
    return largestFileName;
}

// Function to find the name of the smallest .csv file in the directory
char *getSmallestCSV()
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
    printf("Now processing the smallest file [%s] at %ld bytes\n", smallestFileName, minFileSize);
    return smallestFileName;
}

// Function for allowing user to enter a file name
char *getFileNameToRead()
{
    char *nameToRead = malloc(sizeof(char) * 400);
    char lineEntered[300];
    char *prompt = "Enter the complete file name: ";
    printf("%s", prompt);
    // Input Validation
    int ok = 0;
    // Loops until user enters a string that matches a file in the directory.
    while (!ok)
    {
        // Gets user input
        scanf("%299[^\n]", lineEntered);
        FILE *file;
        // File exists
        if (file = fopen(lineEntered, "r"))
        {
            fclose(file);
            ok = 1;
            nameToRead = lineEntered;
        }

        // File doesn't exist
        else
        {
            printf("File not found. Enter the name of the file: ");
        }
        int c;
        while ((c = getchar()) != '\n' && c != EOF)
        {
        }
    }
    printf("Name to read is: [%s] with length %ld\n", nameToRead, strlen(nameToRead));

    return nameToRead;
}

// Main loop of the program
void mainLoop()
{
    printf("Which file would you like to process?\n");
    printf("Enter 1 to pick the largest file.\n");
    printf("Enter 2 to pick the smallest file.\n");
    printf("Enter 3 to specify the name of a file.\n\n");
    printf("Enter a selection from 1 to 3: ");

    // Reassigns lineEntered
    char lineEntered[300];
    // Input validation
    int ok = 0;
    while (!ok)
    {
        scanf("%299[^\n]", lineEntered);

        if (lineEntered[0] >= 49 && lineEntered[0] <= 51 && strlen(lineEntered) == 1)
        {
            printf("\n");
            ok = 1;
        }

        else
        {
            printf("%s", "\nInvalid input.\nPlease enter a number from 1 to 3: ");
        }
        int c;
        while ((c = getchar()) != '\n' && c != EOF)
        {
        }
    }
    int selection = lineEntered[0];

    // The file name to process
    char *fileNameToProcess = malloc(sizeof(char) * 400);

    // User wishes to process largest file
    if (selection == 49)
    {
        fileNameToProcess = getLargestCSV();
    }

    // User wishes to process smallest file
    else if (selection == 50)
    {
        fileNameToProcess = getSmallestCSV();
    }

    // User wishes to enter file name
    else if (selection == 51)
    {
        fileNameToProcess = getFileNameToRead();
    }

    /*
    File Processing
    */
    struct movie *list = processFile(fileNameToProcess);
    // Permanent pointer to the head of the linked list
    struct movie *listHead = list;
    list = listHead;

    srand(time(NULL));
    int r;
    r = rand();
    if (r > 100000)
    {
        r = r % 100000;
    }
    else if (r == 100000)
    {
        r = r - 1;
    }
    char dirname[100];
    sprintf(dirname, "newmangr.movies.%d", r);
    char dirpath[200] = "/nfs/stak/users/newmangr/os1/newmangr.movies.";
    sprintf(dirpath, "%s%d", dirpath, r);
    mkdir(dirpath, S_IRWXU | S_IRGRP | S_IXGRP);
    printf("Created directory with name %s\n", dirname);

    /* Creating and writing files for each year */

    int currYear;
    int moviesCounter = 0;
    for (currYear = 1900; currYear <= 2023; currYear++)
    {
        int yearFile;
        while (list != NULL)
        {
            if (list->year == currYear)
            {
                if (moviesCounter == 0)
                {
                    char newFilePath[200];
                    sprintf(newFilePath, "%s/%d.txt", dirpath, currYear);
                    yearFile = open(newFilePath, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP);
                    if (yearFile < 0)
                    {
                        fprintf(stderr, "Could not open %s\n", newFilePath);
                        exit(1);
                    }
                }
                char titleToWrite[100];
                strcpy(titleToWrite, list->title);
                strcat(titleToWrite, "\n");
                write(yearFile, titleToWrite, strlen(titleToWrite) * sizeof(char));
                moviesCounter++;
            }
            list = list->next;
        }
        list = listHead;
        moviesCounter = 0;
        close(yearFile);
    }
}

// Main function
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
        char lineEntered[300];
        int ok = 0;

        // Loops until the correct input is given
        while (!ok)
        {
            scanf("%299[^\n]", lineEntered);
            if (lineEntered[0] >= 49 && lineEntered[0] <= 50 && strlen(lineEntered) == 1)
            {
                printf("%s", "\n");
                ok = 1;
            }

            else
            {
                printf("%s", "\nInvalid input.\nPlease enter a number from 1 to 2: ");
            }
            int c;
            while ((c = getchar()) != '\n' && c != EOF)
            {
            }
        }
        char selection = lineEntered[0];

        // User selected 1 at the main menu
        if (selection == 49)
        {
            mainLoop();
        }

        // User selected 2 at the main menu
        else
        {
            printf("Exiting program.\n\n");
            mainMenu = 1;
        }
    }

    /*

    */

    return 0;
}