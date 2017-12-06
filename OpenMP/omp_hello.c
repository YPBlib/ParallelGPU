//$ gcc -g -Wall -fopenmp -o omp_hello omp_hello.c
//$ ./omp_hello 4
#include<stdio.h>
#include<stdlib.h>
#include<omp.h>

void Hello(void)
{
    int local_rank=omp_get_thread_num();
    int thread_count = omp_get_num_threads();
    printf("Hello from thread %d of %d \n",local_rank,thread_count);
}

int main(void)
{   
    int thread_count = strtol(argv[1],NULL,10);
#pragma omp parallel num_threads(thread_count);
    Hello();

    return 0;
}












