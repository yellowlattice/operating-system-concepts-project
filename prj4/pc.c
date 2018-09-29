#include <stdlib.h>
#include "stdio.h"
#include "pthread.h"
#include <semaphore.h>
#include <unistd.h>
#define BUFFER_SIZE 5


typedef int buffer_item;

buffer_item buffer[BUFFER_SIZE];
pthread_mutex_t mutex;
pthread_t tid;       
sem_t full, empty;


int p = -1;


int insert_item(buffer_item item)
{
   /* insert item into buffer */
    if(p < BUFFER_SIZE - 1)
    {
        p++;
        buffer[p] = item;
        return 0;
    }
    else
        return -1;
}


int remove_item(buffer_item *item)
{
   /* insert item into buffer */
    if(p >= 0)
    {
        *item = buffer[p];
        p--;
        return 0;
    }
    else
        return -1;
}


void *producer(void *var)
{
    buffer_item item;

    while(1)
    {
        /* Sleep for a random period of time */
        sleep(rand() % 10);
        /* generate a random number */
        item = rand() % 100;

        /* wait empty and acquire the mutex lock */
        sem_wait(&empty);
        pthread_mutex_lock(&mutex);

        /* critical section */
        if(insert_item(item))
            fprintf(stderr, "report error condition\n");
        else
            printf("producer produced %d\n", item);

        /* release the mutex lock and signal full */
        pthread_mutex_unlock(&mutex);
        sem_post(&full);
    }
}


void *consumer(void *var)
{
    buffer_item item;

    while(1)
    {
        /* sleep for a random period of time */
        sleep(rand() % 10);

        /* wait full and acquire the mutex lock */
        sem_wait(&full);
        pthread_mutex_lock(&mutex);

        /* critical section */
        if(remove_item(&item))
            fprintf(stderr, "report error condition\n");
        else
            printf("consumer consumed %d\n", item);

        /* release the mutex lock and signal empty */
        pthread_mutex_unlock(&mutex);
        sem_post(&empty);
    }
}


int main(int argc, char *argv[])
{

    /* 1. Get command line arguments argv[1], argv[2], argv[3] */
    int sleeptime = atoi(argv[1]);
    int producernum = atoi(argv[2]);
    int consumernum = atoi(argv[3]);

    /* 2. Initialize buffer */
    pthread_mutex_init(&mutex, NULL);
    sem_init(&full, 0, 0);
    sem_init(&empty, 0, BUFFER_SIZE);


    /* 3. Create the producer threads */
    for(int i = 0; i < producernum; i++)
        pthread_create(&tid,NULL,producer,NULL);

    /* 4. Create the consumer threads */
    for(int i = 0; i < consumernum; i++)
        pthread_create(&tid,NULL,consumer,NULL);


    /* 5. Sleep */
    sleep(sleeptime);

    /* 6. Exit */
    printf("Exiting\n");

    exit(0);
}
