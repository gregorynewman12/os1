#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

// Function to write to stderr and exit
void writeError(char *message)
{
    fprintf(stderr, "%s", message);
    exit(1);
}

// Decryption function
char *decrypt(char *ciphertext, char *key)
{
    char c, p, k;
    char validChars[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', ' '};
    char *plaintext = malloc(strlen(ciphertext));
    memset(plaintext, '\0', strlen(ciphertext));
    int i, j;

    // Iterates through ciphertext char by char
    for (i = 0; i < strlen(ciphertext); i++)
    {
        c = ciphertext[i];
        k = key[i];
        if (c != '\n')
        {
            // Matches ciphertext char to char in validChars
            for (j = 0; j < 27; j++)
            {
                if (c == validChars[j])
                {
                    c = j;
                    break;
                }
            }
            // Matches key char to char in validChars
            for (j = 0; j < 27; j++)
            {
                if (k == validChars[j])
                {
                    k = j;
                    break;
                }
            }
            p = (c - k) % 27; // Decryption algorithm
            while (p < 0)     // Adjusts negative numbers
                p += 27;
            p = validChars[p];
            plaintext[i] = p; // Builds plaintext char by char
        }
    }
    return plaintext;
}

int main(int argc, char *argv[])
{
    int listenSocketFD, establishedConnectionFD, portNumber, charsRead;
    socklen_t sizeOfClientInfo;
    char buffer[256];
    struct sockaddr_in serverAddress, clientAddress;
    if (argc < 2) // Catches improper usage
    {
        fprintf(stderr, "USAGE: %s port\n", argv[0]);
        exit(1);
    } // Check usage & args
    // Set up the address struct for this process (the server)
    memset((char *)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
    portNumber = atoi(argv[1]);                                  // Get the port number, convert to an integer from a string
    serverAddress.sin_family = AF_INET;                          // Create a network-capable socket
    serverAddress.sin_port = htons(portNumber);                  // Store the port number
    serverAddress.sin_addr.s_addr = INADDR_ANY;                  // Any address is allowed for connection to this process
    // Set up the socket
    listenSocketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
    if (listenSocketFD < 0)
        writeError("ERROR opening socket");
    // Enable the socket to begin listening
    if (bind(listenSocketFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to port
        writeError("ERROR on binding");
    listen(listenSocketFD, 5); // Flip the socket on - it can now receive up to 5 connections
    while (1)
    {
        FILE *plain; // File to briefly store plaintext after decryption

        // Accept a connection, blocking if one is not available until one connects
        sizeOfClientInfo = sizeof(clientAddress);                                                               // Get the size of the address for the client that will connect
        establishedConnectionFD = accept(listenSocketFD, (struct sockaddr *)&clientAddress, &sizeOfClientInfo); // Accept
        if (establishedConnectionFD < 0)
            writeError("ERROR on accept");
        // printf("SERVER: Connected Client at port %d\n", ntohs(clientAddress.sin_port));

        // Gets greeting from client
        char *expectedGreeting = "I am dec_client";
        memset(buffer, '\0', sizeof(buffer));                                     // Clear out the buffer again for reuse
        charsRead = recv(establishedConnectionFD, buffer, sizeof(buffer) - 1, 0); // Read data from the socket, leaving \0 at end

        // Continues processing if correct message is received.
        if (charsRead == strlen(expectedGreeting) && strcmp(buffer, expectedGreeting) == 0)
        {
            // Sends confirmation that communication can continue
            char *confirmation = "OK to continue";
            charsRead = send(establishedConnectionFD, confirmation, strlen(confirmation), 0); // Send success back
            if (charsRead != strlen(confirmation))
                writeError("ERROR writing to socket");

            // Opens file to store received message
            char receivedMessage[70000];
            memset(receivedMessage, '\0', 70000);

            // Section to get the complete message
            while (1)
            {
                memset(buffer, '\0', 256);
                charsRead = recv(establishedConnectionFD, buffer, 255, 0); // Read the client's message from the socket
                if (charsRead < 0)
                    writeError("ERROR reading message from socket");
                buffer[255] = '\0';
                while (strstr(buffer, "@") == NULL) // Keep reading if terminator not received
                {
                    // Write what you got
                    strcat(receivedMessage, buffer);
                    // Read more chars
                    charsRead = recv(establishedConnectionFD, buffer, 255, 0); // Read the client's message from the socket
                    if (charsRead < 0)
                        writeError("ERROR reading more message from socket");
                    buffer[255] = '\0';
                }
                int terminalLocation = strstr(buffer, "@") - buffer; // Where is the terminal
                buffer[terminalLocation] = '\n';
                buffer[terminalLocation + 1] = '\0';
                strcat(receivedMessage, buffer);
                // printf(receivedMessage);
                // FILE *messagefile = fopen("messageReceived", "w");
                // fprintf(messagefile, receivedMessage);
                // fclose(messagefile);
                break;
            }

            // Sends message that server received message
            char *confirmMessageReceived = "message received";
            charsRead = send(establishedConnectionFD, confirmMessageReceived, strlen(confirmMessageReceived), 0); // Send success back
            if (charsRead != strlen(confirmMessageReceived))
                writeError("ERROR writing to socket");
            // printf("Message received.\n");

            // Opens file to store received message
            char receivedKey[70000];
            memset(receivedKey, '\0', 70000);

            // Section to get the complete key
            while (1)
            {
                memset(buffer, '\0', 256);
                charsRead = recv(establishedConnectionFD, buffer, 255, 0); // Read the client's message from the socket
                if (charsRead < 0)
                    writeError("ERROR reading message from socket");
                // printf("Server received: [%s]\n", buffer);
                buffer[255] = '\0';
                while (strstr(buffer, "@") == NULL) // Keep reading if terminator not received
                {
                    // Write what you got
                    strcat(receivedKey, buffer);
                    // Read more chars
                    charsRead = recv(establishedConnectionFD, buffer, 255, 0); // Read the client's message from the socket
                    if (charsRead < 0)
                        writeError("ERROR reading more message from socket");
                    buffer[255] = '\0';
                }
                int terminalLocation = strstr(buffer, "@") - buffer; // Where is the terminal
                buffer[terminalLocation] = '\n';
                buffer[terminalLocation + 1] = '\0';
                strcat(receivedKey, buffer);
                // printf(receivedKey);

                // FILE *keyfile = fopen("keyReceived", "w");
                // fprintf(keyfile, receivedKey);
                // fclose(keyfile);
                break;
            }

            // Sends message that server received key
            // printf("SERVER: Moving to send key.\n");
            char *confirmKeyReceived = "key received";
            charsRead = send(establishedConnectionFD, confirmKeyReceived, strlen(confirmKeyReceived), 0); // Send success back
            if (charsRead != strlen(confirmKeyReceived))
                writeError("ERROR writing to socket");
            // printf("SERVER: Sent confirmation message.\n");

            // PERFORMS THE DECRYPTION
            char *ptext = decrypt(receivedMessage, receivedKey);
            FILE *plainfile = fopen("plaintext", "w");
            fprintf(plainfile, "%s\n", ptext);
            fclose(plainfile);

            // This section sends the plaintext back to the server.
            int charsWritten, exitIfTrue = 0;
            plainfile = fopen("plaintext", "r");
            while (1)
            {
                fread(buffer, 1, 255, plainfile);
                if (strchr(buffer, '\n') != NULL) // If there is a newline found in the buffer, strip it and replace with null term
                {
                    int newlineLocation = strchr(buffer, '\n') - buffer;
                    buffer[newlineLocation] = '@';
                    exitIfTrue = 1; // Found newline and will exit after sending this chunk
                }
                charsWritten = send(establishedConnectionFD, buffer, 255, 0); // Write to the server
                if (charsWritten < 0)
                    writeError("CLIENT: ERROR writing plaintext to buffer.\n");
                // printf("Wrote: [%s]\n", buffer);
                while (charsWritten < strlen(buffer))
                {
                    char *resumeSendPoint = &buffer[charsWritten];
                    int additionalWritten = send(establishedConnectionFD, resumeSendPoint, 255 - charsWritten, 0); // Write more chars to the server
                    charsWritten += additionalWritten;
                }
                // Send terminating indicator and exit
                if (exitIfTrue)
                    break;
            }
            fclose(plainfile);
            remove("plaintext");
            // remove("messageReceived");
            // remove("keyReceived");
            free(ptext);
            // printf("Done sending key.\n");
        }
        close(establishedConnectionFD); // Close the existing socket which is connected to the client
    }
    close(listenSocketFD); // Close the listening socket
    return 0;
}