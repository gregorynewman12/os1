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
    char *token;

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

int main()
{
    struct movie *list = processFile("csv3.csv");
    printf("File Processed successfully\n");

    return 0;
}