#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

char *decrypt(char *ciphertext, char *key)
{
    char c, p, k;
    char validChars[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', ' '};
    char *plaintext = malloc(strlen(ciphertext));
    int i, j;

    for (i = 0; i < strlen(ciphertext); i++)
    {
        c = ciphertext[i];
        k = key[i];
        if (c != '\n')
        {
            for (j = 0; j < 27; j++)
            {
                if (c == validChars[j])
                {
                    c = j;
                    break;
                }
            }
            for (j = 0; j < 27; j++)
            {
                if (k == validChars[j])
                {
                    k = j;
                    break;
                }
            }
            p = (c - k) % 27;
            while (p < 0)
                p += 27;
            p = validChars[p];
            plaintext[i] = p;
        }
    }
    return plaintext;
}

int main()
{
    char *orig = "TSPOJZEP IKPX JWPOWBIGPX INICHQLCROS";
    char *keyy = "ALLPTVBQUVXYTAELHKECIOQLSFAAXAYMLZAD";
    printf("Original: %s %d\n", orig, strlen(orig));
    char *ptext = decrypt(orig, keyy);
    printf("Plaintex: %s %d\n", ptext, strlen(ptext));
}