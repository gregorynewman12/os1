/*
smallsh.c
Author: Gregory Newman
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

// A linked list to contain command arguments
struct arg
{
    char *argument;
    struct arg *next;
};

struct arg *createArg(char *argu)
{
    // Pointer to hold current movie being created
    struct arg *currArg = malloc(sizeof(struct arg));

    // calloc takes two arguments: the number of blocks to allocate
    // and the size of each block. Here it allocates enough char-sized
    // blocks to hold all the chars and the null terminator.
    currArg->argument = calloc(strlen(argu) + 1, sizeof(char));
    strcpy(currArg->argument, argu);

    // Set next node to NULL
    currArg->next == NULL;

    return currArg;
}

int main()
{
    printf("Welcome to Gregory's shell!\n\n");
    char *lineEntered = NULL;
    int numCharsEntered = -5;
    int currChar = -5;
    size_t bufferSize = 0;
    int exit = 0;
    while (!exit)
    {
        printf(": ");
        numCharsEntered = getline(&lineEntered, &bufferSize, stdin);
        lineEntered[numCharsEntered - 1] = '\0';

        int counter = 0;         // Counter variable to count tokens
        char command[100];       // Holds command name
        struct arg *head = NULL; // Head of the linked list to hold arguments
        struct arg *tail = NULL; // Tail of the linked list to hold arguments
        int hasInputFile = 0;    // Tracks whether an input file was provided
        char inputFile[100];     // Name of the input file, if any
        int hasOutputFile = 0;   // Tracks whether an output file was provided
        char outputFile[100];    // Name of the output file, if any
        int runInBackground = 0; // Tracks if '&' was entered

        // This section separates the input into the needed information (command, args, etc)
        char *token = strtok(lineEntered, " ");
        while (token != NULL)
        {
            // If the counter is 0 and isn't a newline or comment, the token is the command
            if (counter == 0)
            {
                strcpy(command, token);
                counter++;
            }

            else if (counter != 0 && token[0] != '<' && token[0] != '>')
            {
                struct arg *newNode = createArg(token);
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

            // Input file will be provided
            else if (token[0] == '<')
            {
                // Gets next token, which will be the input file name
                token = strtok(NULL, " ");
                strcpy(inputFile, token);
                hasInputFile = 1;
            }

            else if (token[0] == '>')
            {
                // Gets next token, which will be the output file name
                token = strtok(NULL, " ");
                strcpy(outputFile, token);
                hasOutputFile = 1;
            }

            else if (token[0] == '&')
            {
                runInBackground = 1;
            }

            token = strtok(NULL, " "); // Get the next token
        }

        struct arg *argsList = head;
        if (command == NULL)
        {
            printf("No command was entered (comment or empty)\n");
        }
        else
        {
            printf("Command name: %s\n", command);
            printf("Arguments:\n");
            // Prints arguments, if any
            if (argsList == NULL)
            {
                printf("There were no arguments entered.\n");
            }
            else
            {
                printf("Argument: %s\n", argsList->argument);
                argsList = argsList->next;
                while (argsList != NULL)
                {
                    printf("Argument: %s\n", argsList->argument);
                    argsList = argsList->next;
                }
            }

            // Reports whether an input file was provided
            if (hasInputFile)
            {
                printf("Name of input file: %s\n", inputFile);
            }
            else
            {
                printf("No input file was provided.\n");
            }

            // Reports whether an output file was provided
            if (hasOutputFile)
            {
                printf("Name of output file: %s\n", outputFile);
            }
            else
            {
                printf("No output file was provided.\n");
            }

            free(lineEntered);
            lineEntered = NULL;
        }
        return 0;
    }
}