#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* struct for movie information */
struct movie
{
    char *title;
    int year;
    char *languages[5];
    float rating;
    struct student *next;
};

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

    // This section iterates through the languages contained in brackets
    char *saveptr2;
    char *langArray[5];
    // Int to use in while loop
    int i = 0;
    char *langTok = strtok_r(languages, ";", &saveptr2);
    while (langTok != NULL)
    {
        langArray[i] = langTok;
        i++;
        langTok = strtok_r(NULL, ";", &saveptr2);
    }
    int j;
    for (j = 0; j < i; j++)
    {
        currMovie->languages[j] = langArray[j];
    }

    // Tokenizes and saves rating
    token = strtok_r(NULL, ",", &saveptr);
    currMovie->rating = atof(token);

    // Set next node to NULL
    currMovie->next == NULL;

    return currMovie;
}

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

    // Read the file line by line
    while ((nread = getline(&currLine, &len, movieFile)) != -1)
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
    free(currLine);
    fclose(movieFile);
    return head;
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("You must provide the name of the file to process\n");
        printf("Example usage: ./movies movies_sample_1.csv\n");
        return EXIT_FAILURE;
    }
    struct movie *list = processFile(argv[1]);
    // Permanent pointer to the head of the linked list
    const struct movie *listHead = list;

    /*
    User Interaction Section
    */
    char *prompt = "\n1. Show movies released in the specified year\n2. Show highest rated movie for each year\n3. Show the title and year of release of all movies in a specific language\n4. Exit from the program\n\nEnter a selection from 1 to 4: ";
    printf("%s", prompt);
    char *lineEntered = NULL;
    int numCharsEntered = -5;
    int currChar = -5;
    size_t bufferSize = 0;
    int ok = 0;

    while (!ok)
    {
        numCharsEntered = getline(&lineEntered, &bufferSize, stdin);

        if (lineEntered[0] >= 49 && lineEntered[0] <= 52 && strlen(lineEntered) == 2)
        {
            printf("%s", "\n");
            ok = 1;
        }

        else
        {
            printf("%s", "Invalid input.\nPlease enter a number from 1 to 4: ");
        }
    }
    char selection = lineEntered[0];

    // See all movies released for a given year
    if (selection = 49)
    {
        printf("%s", "Enter a year for which you'd like to see movies: ");
        lineEntered = NULL;
        numCharsEntered = -5;
        currChar = -5;
        bufferSize = 0;
        int ok = 0;
        while (!ok)
        {
            numCharsEntered = getline(&lineEntered, &bufferSize, stdin);

            if (atoi(lineEntered))
            {
                ok = 1;
            }

            else
            {
                printf("%s", "Invalid input.\nPlease enter a year to see movies from: ");
            }
        }
        int yearToShow = atoi(lineEntered);
        // Variable to track how many movies found for a given year
        int numMoviesOfYear = 0;
        printf("%s", "\n");
        while (list != NULL)
        {
            if (list->year == yearToShow)
            {
                printf("%s\n", list->title);
                numMoviesOfYear++;
            }
            list = list->next;
        }
        if (numMoviesOfYear == 0)
        {
            printf("%s\n", "There were no movies for the entered year.");
        }
        list = listHead;
        printf("%s", "\n");
    }

    else if (selection = 50)
    {
        /*
        Show highest rated movies each year
        */
        printf("%s", "Highest rated movie each year:\n");
        int currYear;
        for (currYear = 1900; currYear <= 2025; currYear++)
        {
            int maxRating = -5;
            while (list != NULL)
            {
                if (list->year == currYear)
                {
                    if (list->rating > maxRating)
                    {
                        maxRating = list->rating;
                    }
                }
                list = list->next;
            }
            list = listHead;
            while (list != NULL)
            {
                if (list->rating == maxRating)
                {
                    printf("%d %.1f %s", list->year, list->rating, list->title);
                    break;
                }
                list = list->next;
            }
            list = listHead;
        }
        printf("%s", "\n\nMade it out safe...\n");
    }

    else if (selection = 51)
    {
        /*
        Show highest rated movies each year
        */
    }

    else if (selection = 52)
    {
        /*
        Exit from the program
        */
        printf("%s", "Exiting the program.\n");
    }

    printf("%s\n", "End of program.");

    return 0;
}