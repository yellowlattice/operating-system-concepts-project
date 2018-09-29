#include "pthread.h"
#include "stdio.h"
#include <stdlib.h>
#include <stdint.h>

#define M 3
#define K 2
#define N 3

int A[M][K] = {{1,4},{2,5},{3,6}};
int B[K][N] = {{8,7,6},{5,4,3}};
int C[M][N];

//fork():copy pthread:share

/* structure for passing data to threads */
struct v
{
   int i; /*row*/
   int j; /*column*/
};

void* thread_multi(void *arg)
{
    struct v *p = (void *) arg;
    int row = p->i;
    int col = p->j;

    C[row][col] = 0;

    for(int k = 0; k < K; k++)
        C[row][col] += A[row][k]*B[k][col];

    pthread_exit((void *)(intptr_t)C[row][col]);
}

int main()
{
    pthread_t multi[M][N]; //thread id
    void* result[M][N];
    /*We have to create M * N worker threads*/
    for(int i = 0; i < M; i++)
        for(int j = 0; j < N; j++)
        {
            struct v *data=(struct v*)malloc(sizeof(struct v));
            data->i = i;
            data->j = j;
            /*Now create the thread passing it data as a parameter*/
            pthread_create(&multi[i][j],NULL,thread_multi,(void *)data);
        }

    for(int i = 0; i < M; i++)
    {
        for(int j = 0; j < N; j++)
        {
            pthread_join(multi[i][j], (void *) &result[i][j]);
	    //C[i][j] = (intptr_t)product[i][j];
            printf(" pid:%lu %ld \n", multi[i][j], (intptr_t)result[i][j]);
        }
        printf("\n");
    }
    //for(int i = 0; i < M; i++)
    //{
        //for(int j = 0; j < N; j++)
        //{
	    //printf(" %ld ", (intptr_t)C[i][j]);
        //}
    //printf("\n");
    //}
    
    exit(0);
}
