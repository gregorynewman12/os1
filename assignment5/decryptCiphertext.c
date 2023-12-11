#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main()
{
    FILE *plain, *cipher, *key;
    char p, k, c, n = '\n';
    char validChars[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', ' '};
    int i;

    plain = fopen("plain.txt", "w");
    cipher = fopen("cipher.txt", "r");
    key = fopen("key.txt", "r");

    c = getc(cipher);
    k = getc(key);
    while (c != '\n')
    {
        for (i = 0; i < 27; i++)
        {
            if (c == validChars[i])
                c = i;
        }
        for (i = 0; i < 27; i++)
        {
            if (k == validChars[i])
                k = i;
        }
        p = c - k;
        while (p < 0)
            p += 27;
        p = p % 27;
        p = validChars[p];
        fwrite(&p, sizeof(char), 1, plain);
        c = getc(cipher);
        k = getc(key);
    }
    fwrite(&n, sizeof(char), 1, plain);

    fclose(plain);
    fclose(cipher);
    fclose(key);
}