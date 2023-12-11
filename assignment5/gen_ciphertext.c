#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

int main()
{
    FILE *cipher, *message, *key;
    char m, k, c, n = '\n';
    char validChars[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', ' '};

    cipher = fopen("cipher.txt", "w");
    message = fopen("plaintext1", "r");
    key = fopen("key.txt", "r");

    m = getc(message);
    k = getc(key);
    while (m != '\n')
    {
        c = validChars[(m + k) % 27];
        printf("Writing encrypted character: %c\n", c);
        fwrite(&c, sizeof(char), 1, cipher);
        m = getc(message);
        k = getc(key);
    }
    fwrite(&n, sizeof(char), 1, cipher);

    fclose(cipher);
    fclose(message);
    fclose(key);
}