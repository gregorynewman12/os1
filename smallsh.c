/*
smallsh.c
Author: Gregory Newman
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

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

// This function performs variable expansion on $$
char *checkForVarExpansion(char *tokenVar, char *expVar)
{
    int i;
    for (i = 0; i < strlen(tokenVar) - 1; i++)
    {
        if (tokenVar[i] == '$' && tokenVar[i + 1] == '$')
        {
            char newToken[2048];
            char newTokenTail[2048];
            memset(newToken, '\0', 2048);
            memset(newTokenTail, '\0', 2048);
            int j;
            int k = 0; // Variable to follow the insert location in newTokenTail
            // Puts the part of the token AFTER "$$" into newTokenTail
            for (j = i + 2; j <= strlen(tokenVar); j++)
            {
                newTokenTail[k] = tokenVar[j];
                k++;
            }
            // Puts the part of the token BEFORE "$$" into newToken
            for (j = 0; j < i; j++)
            {
                newToken[j] = tokenVar[j];
            }
            tokenVar[i] == '\0';
            strcat(newToken, expVar);
            strcat(newToken, newTokenTail);
            tokenVar = newToken;
            // Runs recursively to check the new token for additional "$$" substrings
            tokenVar = checkForVarExpansion(tokenVar, expVar);
        }
    }
    return tokenVar;
}

// Variable and handler to control SIGTSTP signals
int foregroundOnly = 0;
void stopHandler(int signo)
{
    char *enable = "Entering foreground only mode\n";
    char *disable = "Exiting foreground only mode\n";
    if (foregroundOnly == 0)
    {
        foregroundOnly = 1;
        write(STDOUT_FILENO, enable, 30);
    }
    else if (foregroundOnly == 1)
    {
        foregroundOnly = 0;
        write(STDOUT_FILENO, disable, 30);
    }
}

int main()
{
    char *lineEntered = NULL;
    int numCharsEntered = -5;
    int currChar = -5;
    size_t bufferSize = 0;
    int running = 1;
    int exitStatusCode = -5; // Will store exit status code of last foreground process
    int waitStatus;
    __pid_t *runningPIDs[64];
    int i;
    for (i = 0; i < (sizeof(runningPIDs) / sizeof(runningPIDs[0])); i++)
    {
        runningPIDs[i] = NULL;
    }

    // Initialize signal handling
    signal(SIGINT, SIG_IGN);
    struct sigaction stop_action = {0};
    stop_action.sa_handler = stopHandler;
    stop_action.sa_flags = 0;
    sigaction(SIGTSTP, &stop_action, NULL);

    printf("Welcome to Gregory's shell!\n\n");
    while (running)
    {
        fflush(stdout);
        // Checks if any background processes have terminated
        int i;
        for (i = 0; i < (sizeof(runningPIDs) / sizeof(runningPIDs[0])); i++)
        {
            int pStatus;
            if (runningPIDs[i] != NULL)
            {
                int res = waitpid(-1, &pStatus, WNOHANG);
                if (res > 0)
                {
                    int eStatusCode = WEXITSTATUS(pStatus);
                    printf("Background process %d is done. Exit status code %d\n", *runningPIDs[i], eStatusCode);
                    fflush(stdout);
                    runningPIDs[i] = NULL;
                }
            }
        }
        printf(": ");
        fflush(stdout);
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
        if (lineEntered[0] == ' ' || lineEntered[0] == '#' || lineEntered[0] == '\0')
        {
            isCommentOrEmpty = 1;
        }
        char *token = strtok(lineEntered, " ");
        while (token != NULL)
        {
            char pid[50];
            int pidInt = getpid();
            sprintf(pid, "%d", pidInt);
            token = checkForVarExpansion(token, pid);
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

            // If token is an ampersand and foreground only is disabled AND there are no additional tokens
            else if (token[0] == '&' && foregroundOnly == 0 && (token = strtok(NULL, " ")) == NULL)
            {
                runInBackground = 1;
                noMoreArguments = 1;
            }

            token = strtok(NULL, " "); // Get the next token
        }

        // Creates array of arguments entered
        struct arg *argsList = head;

        free(lineEntered);
        lineEntered = NULL;

        /*
        Running Commands section
        */

        // Checks if it was a comment or empty (do nothing)
        if (isCommentOrEmpty)
        {
        }

        // Checks if command was valid
        else if (!validCommand)
        {
            printf("Invalid command.\n");
        }
        // Valid command. Will process
        else
        {
            char *cd = "cd";
            char *exit = "exit";
            char *status = "status";

            // Command is cd
            if (strcmp(command, cd) == 0)
            {
                char cwd[256];
                // First makes sure it has no input or output file
                if (hasInputFile || hasOutputFile)
                {
                    printf("Error. Redirection unsupported.\n");
                }

                // 0 arguments (will change directory to $HOME)
                else if (numOfArguments == 0)
                {
                    int err = chdir(getenv("HOME"));
                    if (err == -1)
                    {
                        printf("Error: Invalid path\n");
                    }
                    printf("Current directory: %s\n", getcwd(cwd, sizeof(cwd)));
                }
                // 1 argument (will attempt to change directory to specified path)
                else if (numOfArguments == 1)
                {
                    int err = chdir(argsList->argument);
                    if (err == -1)
                    {
                        printf("Error: Invalid path\n");
                    }
                    printf("Current directory: %s\n", getcwd(cwd, sizeof(cwd)));
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
                // If the last command terminated by signal
                else
                {
                    if (WTERMSIG(waitStatus))
                    {
                        printf("terminated by signal 2\n");
                        fflush(stdout);
                    }
                    else if (exitStatusCode == -5)
                    {
                        printf("exit value 0\n");
                        fflush(stdout);
                    }
                    else
                    {
                        printf("exit value %d\n", exitStatusCode);
                        fflush(stdout);
                    }
                }
            }

            // Command is exit
            else if (strcmp(command, exit) == 0)
            {
                if (hasInputFile || hasOutputFile)
                {
                    printf("Error. Redirection unsupported.\n");
                    fflush(stdout);
                }

                else if (numOfArguments != 0)
                {
                    printf("Error. Exit command takes no arguments.\n");
                    fflush(stdout);
                }
                else
                {
                    running = 0;
                }
            }

            // Command is some other command
            else
            {
                int id = fork();

                // Checks to make sure fork succeeded
                if (id == -1)
                {
                    printf("Fork failed.\n");
                    fflush(stdout);
                }

                // Child process
                else if (id == 0)
                {
                    // All child processes must ignore SIGSTOP
                    signal(SIGSTOP, SIG_IGN);
                    if (!runInBackground)
                    {
                        signal(SIGINT, SIG_DFL);
                    }
                    // Creates array to hold arguments which will be passed into execvp
                    // Size of array is enough to hold arguments AND command, and NULL pointer
                    char *argsArray[numOfArguments + 2];
                    int i;
                    for (i = 0; i < numOfArguments + 2; i++)
                    {
                        // First value must be the command
                        if (i == 0)
                        {
                            argsArray[i] = command;
                        }
                        // last argument needs to be NULL
                        else if (i == numOfArguments + 1)
                        {
                            argsArray[i] = NULL;
                        }
                        //
                        else
                        {
                            // No arguments were entered or there are no more arguments
                            if (argsList != NULL)
                            {
                                argsArray[i] = argsList->argument;
                                if (argsList->next != NULL)
                                {
                                    argsList = argsList->next;
                                }
                            }
                        }
                    }

                    // Resets argsList back to the head of the linked list
                    argsList = head;

                    // If there is an input file
                    if (hasInputFile)
                    {
                        int iFile = open(inputFile, O_RDONLY);
                        if (iFile == -1)
                        {
                            printf("Failed to open/create the file.\n");
                            fflush(stdout);
                            return 1;
                        }
                        dup2(iFile, STDIN_FILENO);
                        close(iFile);
                    }
                    // If there is an output file
                    if (hasOutputFile)
                    {
                        int oFile = open(outputFile, O_WRONLY | O_TRUNC | O_CREAT, 0777);
                        if (oFile == -1)
                        {
                            printf("Failed to open/create the file.\n");
                            fflush(stdout);
                            return 1;
                        }
                        dup2(oFile, STDOUT_FILENO);
                        close(oFile);
                    }

                    // If there is no input file and it is to run in background,
                    // redirects stdin to null
                    if (!hasInputFile && runInBackground)
                    {
                        int iFile = open("/dev/null", O_RDONLY);
                        if (iFile == -1)
                        {
                            printf("Failed to open/create the file.\n");
                            fflush(stdout);
                            return 1;
                        }
                        dup2(iFile, STDIN_FILENO);
                        close(iFile);
                    }

                    // If there is no output file and it is to run in background,
                    // redirects stdout to null
                    if (!hasOutputFile && runInBackground)
                    {
                        int oFile = open("/dev/null", O_WRONLY | O_TRUNC);
                        if (oFile == -1)
                        {
                            printf("Failed to open/create the file.\n");
                            fflush(stdout);
                            return 1;
                        }
                        dup2(oFile, STDOUT_FILENO);
                        close(oFile);
                    }

                    // Tries to execute the command
                    int err;
                    err = execvp(command, argsArray);
                    if (err == -1)
                    {
                        return 1;
                    }
                }
                // Parent process
                else
                {
                    // Foreground command
                    if (!runInBackground)
                    {
                        waitStatus;
                        waitpid(id, &waitStatus, 0);
                        if (WTERMSIG(waitStatus))
                        {
                            printf("Terminated by signal 2\n");
                            fflush(stdout);
                        }
                        else if (WIFEXITED(waitStatus))
                        {
                            exitStatusCode = WEXITSTATUS(waitStatus);
                            if (exitStatusCode == 1)
                            {
                                printf("Failed: No such command or incorrect syntax.\n");
                                fflush(stdout);
                            }
                        }
                    }

                    // Puts the process in the background
                    else if (runInBackground)
                    {
                        int pidCounter = 0;
                        int pidWritten = 0;
                        while (!pidWritten)
                        {
                            if (runningPIDs[pidCounter] == NULL)
                            {
                                runningPIDs[pidCounter] = &id;
                                pidWritten = 1;
                                printf("Background pid: %d\n", id);
                                fflush(stdout);
                            }
                            pidCounter++;
                        }
                    }
                }
            }
        }
    }
    return 0;
}