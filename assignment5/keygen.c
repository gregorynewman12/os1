#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    char validChars[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', ' '};
    srand(time(NULL));

    if (argc != 2 || !(atoi(argv[1])))
    {
        fprintf(stderr, "ERROR: Incorrect format.\nPlease provide the length of the key in addition to the command.\n");
    }
    else
    {
        int keyLength = atoi(argv[1]);
        if (keyLength < 500)
        {
            char key[keyLength + 1];
            int i;
            for (i = 0; i < keyLength; i++)
            {
                int x = rand() % 27;
                key[i] = validChars[x];
            }
            key[keyLength] = '\0';

            printf("%s\n", key);
        }

        // Breaks the key up into smaller chunks to prevent garbage data.
        else
        {
            int currentLength = keyLength;
            while (currentLength > 0)
            {
                int chunkSize;
                if (currentLength >= 100)
                {
                    chunkSize = 100;
                }
                else
                {
                    chunkSize = currentLength;
                }
                char key[chunkSize + 1];
                currentLength = currentLength - chunkSize;
                int i;
                for (i = 0; i < chunkSize; i++)
                {
                    int x = rand() % 27;
                    key[i] = validChars[x];
                }
                key[chunkSize] = '\0';
                printf("%s", key);
            }
            printf("\n");
        }
    }
}