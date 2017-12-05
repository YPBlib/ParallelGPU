// $ gcc -g -Wall -o pthHello PthreadsHello.c -lpthread
// $ ./pthHello <number of threaads>
#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>

int thread_count; 

void* Hello(void* rank);
int main(int argc,char* argv[])
{
    long thread;
    pthread_t* thread_handles;
    thread_count=strtol(argv[1],NULL,10);

}













void* Hello(void* rank)
{
    long my_rank=(long)rank;
    printf("Hello from thread %d of %d \n",my_rank.thread_count);
    return NULL;
}
