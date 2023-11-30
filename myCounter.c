#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <pthread.h>
#include <unistd.h>

pthread_mutex_t myMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t myCond1 = PTHREAD_COND_INITIALIZER;
pthread_cond_t myCond2 = PTHREAD_COND_INITIALIZER;
pthread_t producer;
pthread_t consumer;
int myCount = 0;
int lastCount = -5;
int prodCons = 0; // Toggles between 0 and 1
                  // When producer is done, sets to 1
                  // When consumer is done, sets to 0

void *consumerFunction(void *arg)
{
    consumer = pthread_self(); // Declares thread
    printf("CONSUMER THREAD CREATED\n");
    fflush(stdout);
    while (1)
    {
        pthread_mutex_lock(&myMutex);
        printf("CONSUMER: myMutex locked\n");
        fflush(stdout);
        while (prodCons == 0) // Waits until myCount has been incremented
        {
            printf("CONSUMER: waiting on myCond1\n");
            fflush(stdout);
            pthread_cond_wait(&myCond1, &myMutex);
        }
        if (myCount < 10)
        {
            lastCount = myCount;
            myCount++;
            prodCons = 0;
            printf("myCount: %d -> %d\n", lastCount, myCount);
            fflush(stdout);
        }
        if (myCount >= 10)
        {
            printf("PROGRAM END\n");
            fflush(stdout);
            exit(0);
        }
        printf("CONSUMER: myMutex unlocked\n");
        fflush(stdout);
        pthread_mutex_unlock(&myMutex);
        pthread_cond_signal(&myCond2); // Signals consumer thread
        printf("CONSUMER: signaling myCond2\n\n");
        fflush(stdout);
    }
    return NULL;
}

int main()
{
    printf("PROGRAM START\n");
    fflush(stdout);
    if (pthread_create(&consumer, NULL, consumerFunction, NULL) != 0)
    {
        printf("Error: Consumer thread not created.\n");
    }

    while (1)
    {
        pthread_mutex_lock(&myMutex);
        printf("PRODUCER: myMutex locked\n");
        fflush(stdout);
        while (prodCons == 1) // Increments myCount if less than 10
        {
            printf("PRODUCER: waiting on myCond2\n");
            fflush(stdout);
            pthread_cond_wait(&myCond2, &myMutex);
        }
        if (myCount < 10)
        {
            lastCount = myCount;
            myCount++;
            prodCons = 1;
            printf("myCount: %d -> %d\n", lastCount, myCount);
            fflush(stdout);
        }
        if (myCount >= 10)
        {
            printf("PROGRAM END\n");
            fflush(stdout);
            exit(0);
        }
        // Unlocks mutex
        printf("PRODUCER: myMutex unlocked\n");
        fflush(stdout);
        pthread_mutex_unlock(&myMutex);
        printf("PRODUCER: signaling myCond1\n\n");
        fflush(stdout);
        pthread_cond_signal(&myCond1); // Signals consumer thread
    }
}