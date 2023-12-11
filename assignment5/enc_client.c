#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

void writeError(char *message)
{
    fprintf(stderr, "%s", message);
    exit(1);
}

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
        writeError("ERROR: CLIENT: Error opening plaintext file.\n");
    key = fopen(key_file_name, "r");
    if (key == NULL)
        writeError("ERROR: CLIENT: Error opening key file.\n");

    int i;
    // Reads number of chars in key file
    for (i = getc(key); i != EOF; i = getc(key))
    {
        keyLength += 1;
        if (!(i >= 65 && i <= 90) && i != 32 && i != 10)
            writeError("ERROR: CLIENT: Invalid characters in key.\n");
    }
    rewind(key);
    // Reads number of chars in plaintext file
    for (i = getc(plaintext); i != EOF; i = getc(plaintext))
    {
        plaintextLength += 1;
        if (!(i >= 65 && i <= 90) && i != 32 && i != 10)
            writeError("ERROR: CLIENT: Invalid characters in plaintext.\n");
    }
    rewind(plaintext);

    if (keyLength < plaintextLength)
        writeError("ERROR: Key too short.\n");

    // Set up the server address struct
    memset((char *)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
    serverAddress.sin_family = AF_INET;                          // Create a network-capable socket
    serverAddress.sin_port = htons(portNumber);                  // Store the port number
    serverHostInfo = gethostbyname("localhost");                 // Convert the machine name into a special form of address
    if (serverHostInfo == NULL)
        writeError("CLIENT: ERROR, no such host\n");
    memcpy((char *)&serverAddress.sin_addr.s_addr, (char *)serverHostInfo->h_addr, serverHostInfo->h_length);
    // Copy in the address

    // Set up the socket
    socketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
    if (socketFD < 0)
        writeError("CLIENT: ERROR opening socket.\n");

    // Connect to server
    if (connect(socketFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to addy
        writeError("CLIENT: ERROR connecting to socket.\n");

    // This section sends the plaintext to the server.
    int exitIfTrue = 0;
    while (1)
    {
        fgets(buffer, 254, plaintext);
        if (strchr(buffer, '\n') != NULL) // If there is a newline found in the buffer, strip it and replace with null term
        {
            int newlineLocation = strchr(buffer, '\n') - buffer;
            buffer[newlineLocation] = '@';
            buffer[newlineLocation + 1] = '@';
            buffer[newlineLocation + 2] = '\0';
            exitIfTrue = 1; // Found newline and will exit after sending this chunk
        }
        charsWritten = send(socketFD, buffer, strlen(buffer), 0); // Write to the server
        if (charsWritten < 0)
            writeError("CLIENT: ERROR writing plaintext to buffer.\n");
        while (charsWritten < strlen(buffer))
        {
            char *resumeSendPoint = &buffer[charsWritten];
            int additionalWritten = send(socketFD, resumeSendPoint, strlen(buffer), 0); // Write more chars to the server
            charsWritten += additionalWritten;
        }
        // Send terminating indicator and exit
        if (exitIfTrue)
            break;
    }

    fclose(plaintext);

    // Waits for confirmation that server received the plaintext
    memset(buffer, '\0', sizeof(buffer));                      // Clear out the buffer again for reuse
    charsRead = recv(socketFD, buffer, sizeof(buffer) - 1, 0); // Read data from the socket, leaving \0 at end
    if (charsRead != 16)                                       // Expects message "message received"
        writeError("CLIENT: ERROR reading confirmation from socket.\n");
    // printf("CLIENT: Server received plaintext\n");

    // This section sends the key to the server.
    exitIfTrue = 0;
    while (1)
    {
        fgets(buffer, 254, key);
        if (strchr(buffer, '\n') != NULL) // If there is a newline found in the buffer, strip it and replace with null term
        {
            int newlineLocation = strchr(buffer, '\n') - buffer;
            buffer[newlineLocation] = '@';
            buffer[newlineLocation + 1] = '@';
            buffer[newlineLocation + 2] = '\0';
            exitIfTrue = 1; // Found newline and will exit after sending this chunk
        }
        charsWritten = send(socketFD, buffer, strlen(buffer), 0); // Write to the server
        // printf("Sent %d characters\n", charsWritten);
        // printf("Data sent: %s\n", buffer);
        if (charsWritten < 0)
            writeError("CLIENT: ERROR writing key to buffer.");
        while (charsWritten < strlen(buffer))
        {
            char *resumeSendPoint = &buffer[charsWritten];
            int additionalWritten = send(socketFD, resumeSendPoint, strlen(buffer), 0); // Write more chars to the server
            charsWritten += additionalWritten;
        }
        if (exitIfTrue)
            break;
    }
    // printf("Done sending key.\n");

    // Waits for confirmation that server received the key
    memset(buffer, '\0', sizeof(buffer));                      // Clear out the buffer again for reuse
    charsRead = recv(socketFD, buffer, sizeof(buffer) - 1, 0); // Read data from the socket, leaving \0 at end
    if (charsRead != 12)                                       // Expects message "key received"
        writeError("CLIENT: ERROR reading confirmation message from socket.\n");
    // printf("CLIENT: Server received key\n");

    fclose(key);

    // Section to get ciphertext back from server
    while (1)
    {
        memset(buffer, '\0', 256);
        charsRead = recv(socketFD, buffer, 255, 0); // Read the client's message from the socket
        if (charsRead < 0)
            writeError("ERROR reading intial ciphertext from socket\n");
        while (strstr(buffer, "@@") == NULL) // Keep reading if terminator not received
        {
            // Print what you got
            printf("%s", buffer);
            // Read more chars
            charsRead = recv(socketFD, buffer, 255, 0); // Read the client's message from the socket
            if (charsRead < 0)
                writeError("ERROR reading more ciphertext from socket\n");
        }
        int terminalLocation = strstr(buffer, "@@") - buffer; // Where is the terminal
        buffer[terminalLocation] = '\n';
        buffer[terminalLocation + 1] = '\0';
        printf("%s", buffer);
        break;
    }

    close(socketFD); // Close the socket

    return 0;
}