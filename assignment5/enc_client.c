#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

int main(int argc, char *argv[])
{
    int socketFD, portNumber, charsWritten, totalCharsWritten, charsRead, plaintextLength = 0, keyLength = 0;
    FILE *plaintext, *key;
    struct sockaddr_in serverAddress;
    struct hostent *serverHostInfo;
    char buffer[256], plaintext_file_name[256], key_file_name[256];
    if (argc < 4)
    {
        fprintf(stderr, "USAGE: %s plaintext_file_name key_file_name port_number\n", argv[0]);
        exit(0);
    } // Check usage & args

    strlcpy(plaintext_file_name, argv[1], 255); // Copy plaintext file name
    strlcpy(key_file_name, argv[2], 255);       // Copy key file name
    portNumber = atoi(argv[3]);                 // Get the port number, convert to an integer from a string

    plaintext = fopen(plaintext_file_name, "r");
    if (plaintext == NULL)
    {
        fprintf(stderr, "ERROR: CLIENT: Error opening plaintext file.\n");
        exit(1);
    }
    key = fopen(key_file_name, "r");
    if (key == NULL)
    {
        fprintf(stderr, "ERROR: CLIENT: Error opening key file.\n");
        exit(1);
    }

    int i;
    // Reads number of chars in key file
    for (i = getc(key); i != EOF; i = getc(key))
    {
        keyLength += 1;
        if (!(i >= 65 && i <= 90) && i != 32 && i != 10)
        {
            fprintf(stderr, "ERROR: CLIENT: Invalid characters in key.\n");
            exit(1);
        }
    }
    fclose(key);
    // Reads number of chars in plaintext file
    for (i = getc(plaintext); i != EOF; i = getc(plaintext))
    {
        plaintextLength += 1;
        if (!(i >= 65 && i <= 90) && i != 32 && i != 10)
        {
            fprintf(stderr, "ERROR: CLIENT: Invalid characters in plaintext.\n");
            exit(1);
        }
    }
    fclose(plaintext);

    if (keyLength < plaintextLength)
    {
        fprintf(stderr, "ERROR: Key too short.\n");
        exit(1);
    }
    plaintext = fopen(plaintext_file_name, "r"); // Reopens plaintext
    key = fopen(key_file_name, "r");             // Reopens key

    // Set up the server address struct
    memset((char *)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
    serverAddress.sin_family = AF_INET;                          // Create a network-capable socket
    serverAddress.sin_port = htons(portNumber);                  // Store the port number
    serverHostInfo = gethostbyname("localhost");                 // Convert the machine name into a special form of address
    if (serverHostInfo == NULL)
    {
        fprintf(stderr, "CLIENT: ERROR, no such host\n");
        exit(0);
    }
    memcpy((char *)&serverAddress.sin_addr.s_addr, (char *)serverHostInfo->h_addr, serverHostInfo->h_length);
    // Copy in the address

    // Set up the socket
    socketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
    if (socketFD < 0)
    {
        fprintf(stderr, "CLIENT: ERROR opening socket.\n");
        exit(1);
    }

    // Connect to server
    if (connect(socketFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to addy
    {
        fprintf(stderr, "CLIENT: ERROR connecting to socket.\n");
        exit(1);
    }

    // This section sends the plaintext to the server.
    while (1)
    {
        fgets(buffer, 255, plaintext);
        if (strchr(buffer, '\n') != NULL) // If there is a newline found in the buffer, strip it and replace with null term
        {
            int newlineLocation = strchr(buffer, '\n') - buffer;
            buffer[newlineLocation] = '\0';
        }
        charsWritten = send(socketFD, buffer, strlen(buffer), 0); // Write to the server
        if (charsWritten < 0)
        {
            fprintf(stderr, "CLIENT: ERROR writing to socket.\n");
            exit(1);
        }
        while (charsWritten < strlen(buffer) - 1)
        {
            char *resumeSendPoint = &buffer[charsWritten];
            int additionalWritten = send(socketFD, resumeSendPoint, strlen(buffer), 0); // Write more chars to the server
            charsWritten += additionalWritten;
        }
        close(socketFD); // Close the socket
        break;
    }
    printf("CLIENT: Exited while loop correctly.\n");

    /*
    charsWritten = send(socketFD, buffer, strlen(buffer), 0); // Write to the server
    if (charsWritten < 0)
    {
        fprintf(stderr, "CLIENT: ERROR writing to socket.\n");
        exit(1);
    }
    if (charsWritten < strlen(buffer))
        printf("CLIENT: WARNING: Not all data written to socket!\n");

    // Get return message from server
    memset(buffer, '\0', sizeof(buffer));                      // Clear out the buffer again for reuse
    charsRead = recv(socketFD, buffer, sizeof(buffer) - 1, 0); // Read data from the socket, leaving \0 at end
    if (charsRead < 0)
    {
        fprintf(stderr, "CLIENT: ERROR reading from socket.\n");
        exit(1);
    }
    printf("CLIENT: I received this from the server: \"%s\"\n", buffer);
    close(socketFD); // Close the socket

    */
    return 0;
}