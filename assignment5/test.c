#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

int main()
{
    FILE *plaintext = fopen("plaintext1", "r");
    char text[20];
    memset(text, '\0', 1);
    fread(text, 1, 13, plaintext);
    printf("Text: %s\n", text);
    printf("Length expect 13: %d\n", strlen(text));
    int i;
    for (i = 0; i < 18; i++)
    {
        printf("Character %d: %d\n", i, text[i]);
    }
}