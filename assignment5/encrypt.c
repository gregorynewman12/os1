#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

char *encrypt(char *plaintext, char *key)
{
    char c, p, k;
    char validChars[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', ' '};
    char *ciphertext = malloc(strlen(plaintext));
    int i, j;

    for (i = 0; i < strlen(plaintext); i++)
    {
        p = plaintext[i];
        k = key[i];
        if (p != '\n')
        {
            for (j = 0; j < 27; j++)
            {
                if (p == validChars[j])
                {
                    p = j;
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
            c = (p + k) % 27;
            c = validChars[c];
            ciphertext[i] = c;
        }
    }
    return ciphertext;
}

int main()
{
    char *orig = "THE RED GOOSE FLIES AT MIDNIGHT STOP";
    char *keyy = "WAYPZAFZZDVNZGGVWG KVIAZPBDINSBYS NFPSTCKNPZIKIDFHCPQDCUUVCWMTUTSWYGCQINCMALLIDFPGUUZXOHRFDTYMLQHJLZZJLRKABWIUQN KGYWUFBOYUMJVBFDBDR EHKEYVCSKPGJKEUUZWHWETEZJZRLRIKKEJPCDGJDKPMUIWNGRKRKCKZBIPMPMLZRKNICIGFSKHBTCOOINFHQPFRYKSMLSKBBNZEKUZCEFDXHG EUEMZUGEGQLHBSGSUIGNT AKDGNPCIPXRTIEBOIHUTDVLKBUSXW XWJPRLUJUIF QNEHQMDJV";
    printf("Original: %s %d\n", orig, strlen(orig));
    char *ctext = encrypt(orig, keyy);
    printf("Cipherte: %s %d\n", ctext, strlen(ctext));
}