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

// Creates linked list of arguments
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
    int running = 1;
    while (running)
    {
        printf(": ");
        numCharsEntered = getline(&lineEntered, &bufferSize, stdin);
        lineEntered[numCharsEntered - 1] = '\0';

        int counter = 0;    // Counter variable to count tokens entered
        char command[2048]; // Holds command name
        memset(command, '\0', 2048);
        struct arg *head = NULL; // Head of the linked list to hold arguments
        struct arg *tail = NULL; // Tail of the linked list to hold arguments
        int hasInputFile = 0;    // Tracks whether an input file was provided
        char inputFile[2048];    // Name of the input file, if any
        memset(inputFile, '\0', 2048);
        int hasOutputFile = 0; // Tracks whether an output file was provided
        char outputFile[2048]; // Name of the output file, if any
        memset(outputFile, '\0', 2048);
        int numOfArguments = 0;  // Tracks number of arguments entered
        int runInBackground = 0; // Tracks if '&' was entered
        int noMoreArguments = 0; // Ensures arguments can't be entered after input or
                                 // output file is specified so command is in the right order
        int validCommand = 1;    // Assumes a command is valid. Will be set to false if certain
                                 // conditions are not met.
        int isCommentOrEmpty = 0;

        /*
        This section separates the input into the needed information (command, args, etc)
        */
        if (lineEntered[0] == ' ' || lineEntered[0] == '#')
        {
            isCommentOrEmpty = 1;
        }
        char *token = strtok(lineEntered, " ");
        while (token != NULL)
        {
            // If this is the first token, it is the command. Also makes sure an input, output, or '&'
            // can't be entered before a command is specified.
            if (counter == 0 && token[0] != '<' && token[0] != '>' && token[0] != '&')
            {
                strcpy(command, token);
                counter++;
            }

            else if (counter == 0 && (token[0] == '<' || token[0] == '>' || (token[0] == '&' && strlen(token) == 1)))
            {
                validCommand = 0;
                counter++;
            }

            // If criteria here is met, the token is an argument
            // Counter can't be 0, first char isn't < or >, first char can be & only if the string length is more than 1,
            // and the program isn't done taking arguments
            else if (counter != 0 && token[0] != '<' && token[0] != '>' && !(token[0] == '&' && strlen(token) == 1) && noMoreArguments == 0 && runInBackground == 0)
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
                numOfArguments++;
            }

            // Prevents any more arguments once an input, output, or "&" has been specified.
            else if (counter != 0 && token[0] != '<' && token[0] != '>' && (noMoreArguments == 1 || runInBackground == 1))
            {
                validCommand = 0;
            }

            // Input file will be provided, if one has not already been specified
            else if (token[0] == '<' && hasInputFile == 0 && runInBackground == 0)
            {
                // Gets next token, which will be the input file name
                token = strtok(NULL, " ");

                // Now will make sure an input file is properly specified
                if (token == NULL || (token[0] == '&' && strlen(token) == 1) || (token[0] == '<' && strlen(token) == 1) || (token[0] == '>' && strlen(token) == 1))
                {
                    validCommand = 0;
                }
                else
                {
                    strcpy(inputFile, token);
                    hasInputFile = 1;
                    noMoreArguments = 1; // Ensures no more arguments can be entered now
                }
            }

            // If an input file has already been provided and user tries to provide
            // a second, sets validCommand to 0.
            else if (token[0] == '<' && hasInputFile == 1)
            {
                validCommand = 0;
            }

            // Output redirection specified, and does not already have an output file, and user hasn't already entered '&'
            else if (token[0] == '>' && hasOutputFile == 0 && runInBackground == 0)
            {
                // Gets next token, which will be the output file name
                token = strtok(NULL, " ");

                // Now will make sure an output file is properly specified
                if (token == NULL || (token[0] == '&' && strlen(token) == 1) || (token[0] == '<' && strlen(token) == 1) || (token[0] == '>' && strlen(token) == 1))
                {
                    validCommand = 0;
                }
                else
                {
                    strcpy(outputFile, token);
                    hasOutputFile = 1;
                    noMoreArguments = 1; // Ensures no more arguments can be entered now
                }
            }

            // Output file already provided, command will be invalidated
            else if (token[0] == '>' && hasOutputFile == 1)
            {
                validCommand = 0;
            }

            else if (token[0] == '&')
            {
                runInBackground = 1;
                noMoreArguments = 1;
            }

            token = strtok(NULL, " "); // Get the next token
        }

        struct arg *argsList = head;

        /*
        This section reports on the user's input

        if (isCommentOrEmpty)
        {
            printf("No command was entered (comment or empty)\n", command[0]);
        }
        else if (!validCommand)
        {
            printf("Invalid command.\n");
        }
        else
        {
            printf("Command name: %s\n", command);
            // Prints arguments, if any
            if (numOfArguments == 0)
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

            if (runInBackground)
            {
                printf("Run in background: True\n");
            }
            else
            {
                printf("Run in background: False\n");
            }

        }
        */
        free(lineEntered);
        lineEntered = NULL;

        /*
        Running Commands section
        */
        // Only moves on if the command was valid

        if (!validCommand)
        {
            printf("Invalid command.\n");
        }
        else
        {
            char *cd = "cd";
            char *exit = "exit";
            char *status = "status";

            // Command is cd
            if (strcmp(command, cd) == 0)
            {
                // First makes sure it has no input or output file
                if (hasInputFile || hasOutputFile)
                {
                    printf("Error. Redirection unsupported.\n");
                }

                // 0 arguments (will change directory to $HOME)
                else if (numOfArguments == 0)
                {
                    printf("Changing directory to $HOME.\n");
                }
                // 1 argument (will attempt to change directory to specified path)
                else if (numOfArguments == 1)
                {
                    printf("Changing directory to specified path.\n");
                }
                // Invalid number of arguments
                else
                {
                    printf("Incorrect arguments entered.\n");
                }
            }

            // Command is status
            else if (strcmp(command, status) == 0)
            {
                if (hasInputFile || hasOutputFile)
                {
                    printf("Error. Redirection unsupported.\n");
                }

                else if (numOfArguments != 0)
                {
                    printf("Error. Status command takes no arguments.\n");
                }
                else
                {
                    printf("\nExecuting the status command.\n\n");
                }
            }

            // Command is exit
            else if (strcmp(command, exit) == 0)
            {
                if (hasInputFile || hasOutputFile)
                {
                    printf("Error. Redirection unsupported.\n");
                }

                else if (numOfArguments != 0)
                {
                    printf("Error. Exit command takes no arguments.\n");
                }
                else
                {
                    printf("\nExecuting the exit command.\n\n");
                    running = 0;
                }
            }

            // Command is some other command
            else
            {
                printf("External command. Attempting to run.\n");
            }
        }
    }
    return 0;
}