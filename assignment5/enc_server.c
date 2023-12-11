#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

void writeError(char *message)
{
    fprintf(stderr, "%s", message);
    exit(1);
}

void encrypt(char *messageFile, char *keyFile, char *cipherFile)
{
    FILE *cipher, *message, *key;
    char m, k, c, n = '\n';
    char validChars[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', ' '};
    int i;

    cipher = fopen(cipherFile, "w");
    message = fopen(messageFile, "r");
    key = fopen(keyFile, "r");

    m = getc(message);
    k = getc(key);
    while (m != '\n')
    {
        for (i = 0; i < 27; i++)
        {
            if (m == validChars[i])
                m = i;
        }
        for (i = 0; i < 27; i++)
        {
            if (k == validChars[i])
                k = i;
        }
        c = (m + k) % 27;
        c = validChars[c];
        fwrite(&c, sizeof(char), 1, cipher);
        m = getc(message);
        k = getc(key);
    }
    fwrite(&n, sizeof(char), 1, cipher);

    fclose(cipher);
    fclose(message);
    fclose(key);
}

int main(int argc, char *argv[])
{
    int listenSocketFD, establishedConnectionFD, portNumber, charsRead;
    socklen_t sizeOfClientInfo;
    char buffer[256];
    char *messageReceivedFile = "receivedMessage";
    char *keyReceivedFile = "receivedKey";
    char *ciphertextFile = "cipher.txt";
    struct sockaddr_in serverAddress, clientAddress;
    if (argc < 2)
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
        FILE *receivedMessage, *receivedKey, *cipher;

        // Accept a connection, blocking if one is not available until one connects
        sizeOfClientInfo = sizeof(clientAddress);                                                               // Get the size of the address for the client that will connect
        establishedConnectionFD = accept(listenSocketFD, (struct sockaddr *)&clientAddress, &sizeOfClientInfo); // Accept
        if (establishedConnectionFD < 0)
            writeError("ERROR on accept");
        // printf("SERVER: Connected Client at port %d\n", ntohs(clientAddress.sin_port));

        // Opens file to store received message
        receivedMessage = fopen(messageReceivedFile, "w");

        // Section to get the complete message
        while (1)
        {
            memset(buffer, '\0', 256);
            charsRead = recv(establishedConnectionFD, buffer, 255, 0); // Read the client's message from the socket
            if (charsRead < 0)
                writeError("ERROR reading message from socket");
            while (strstr(buffer, "@@") == NULL) // Keep reading if terminator not received
            {
                // Write what you got
                fwrite(buffer, sizeof(char), strlen(buffer), receivedMessage);
                // Read more chars
                charsRead = recv(establishedConnectionFD, buffer, 255, 0); // Read the client's message from the socket
                if (charsRead < 0)
                    writeError("ERROR reading more message from socket");
            }
            int terminalLocation = strstr(buffer, "@@") - buffer; // Where is the terminal
            buffer[terminalLocation] = '\n';
            buffer[terminalLocation + 1] = '\0';
            fwrite(buffer, sizeof(char), strlen(buffer), receivedMessage);
            break;
        }
        fclose(receivedMessage);

        // Sends message that server received message
        char *confirmMessageReceived = "message received";
        charsRead = send(establishedConnectionFD, confirmMessageReceived, strlen(confirmMessageReceived), 0); // Send success back
        if (charsRead != strlen(confirmMessageReceived))
            writeError("ERROR writing to socket");

        // Opens file to store received key
        receivedKey = fopen(keyReceivedFile, "w");

        // Section to get complete key
        while (1)
        {
            memset(buffer, '\0', 256);
            charsRead = recv(establishedConnectionFD, buffer, 255, 0); // Read the client's message from the socket
            if (charsRead < 0)
                writeError("ERROR reading key from socket");
            while (strstr(buffer, "@@") == NULL) // Keep reading if terminator not received
            {
                // Write what you got
                fwrite(buffer, sizeof(char), strlen(buffer), receivedKey);
                // Read more chars
                charsRead = recv(establishedConnectionFD, buffer, 255, 0); // Read the client's message from the socket
                if (charsRead < 0)
                    writeError("ERROR reading more key from socket");
            }
            int terminalLocation = strstr(buffer, "@@") - buffer; // Where is the terminal
            buffer[terminalLocation] = '\n';
            buffer[terminalLocation + 1] = '\0';
            fwrite(buffer, sizeof(char), strlen(buffer), receivedKey);
            break;
        }
        fclose(receivedKey);

        // Sends message that server received key
        char *confirmKeyReceived = "key received";
        charsRead = send(establishedConnectionFD, confirmKeyReceived, strlen(confirmKeyReceived), 0); // Send success back
        if (charsRead != strlen(confirmKeyReceived))
            writeError("ERROR writing to socket");

        // PERFORMS THE ENCRYPTION
        encrypt(messageReceivedFile, keyReceivedFile, ciphertextFile);

        // SENDS CIPHERTEXT
        cipher = fopen(ciphertextFile, "r");
        int charsWritten, exitIfTrue = 0;
        while (1)
        {
            fgets(buffer, 254, cipher);
            if (strchr(buffer, '\n') != NULL) // If there is a newline found in the buffer, strip it and replace with null term
            {
                int newlineLocation = strchr(buffer, '\n') - buffer;
                buffer[newlineLocation] = '@';
                buffer[newlineLocation + 1] = '@';
                buffer[newlineLocation + 2] = '\0';
                exitIfTrue = 1; // Found newline and will exit after sending this chunk
            }
            charsWritten = send(establishedConnectionFD, buffer, strlen(buffer), 0); // Write to the server
            if (charsWritten < 0)
                writeError("CLIENT: ERROR writing to buffer.");
            while (charsWritten < strlen(buffer))
            {
                char *resumeSendPoint = &buffer[charsWritten];
                int additionalWritten = send(establishedConnectionFD, resumeSendPoint, strlen(buffer), 0); // Write more chars to the server
                charsWritten += additionalWritten;
            }
            // Send terminating indicator and exit
            if (exitIfTrue)
                break;
        }

        close(establishedConnectionFD); // Close the existing socket which is connected to the client
    }
    close(listenSocketFD); // Close the listening socket
    return 0;
}