
//$ gcc -g -Wall -fopenmp -o omp_hello omp_hello.c
//$ ./omp_hello 4
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/*

#include <omp.h>
void Hello(void)
{
    int local_rank=omp_get_thread_num();
    int thread_count = omp_get_num_threads();
    printf("Hello from thread %d of %d \n",local_rank,thread_count);
    double i=0.6,j=2,k=5;
    while(1)
    {
      i=sin(j)*200.4;
      j=cos(k)*1000/45;
      k=i*i*j;
    }
}

int main(int argc,char**argv)
{   
    int thread_count = strtol(argv[1],NULL,10);
#pragma omp parallel num_threads(thread_count)
    Hello();

    return 0;
}
*/




#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>

void* f(const void* l)
{
  double i=0.6,j=2,k=5;
  int x=*(int*)l;
  printf("%d\n",x);
  while(1)
  {
    i=j*200.4+j*j;
    j=k*1000/45;
    k=i*i*j;
  }
  
}



#define pnum 2000

int main()
{
  int pt[pnum]={0};
  pthread_t pd[pnum];
  void *r;
  for(int i=0;i<pnum;++i)
  {
    pt[i]=pthread_create(&pd[i],NULL,f,&i);
  }

  for(int i=0;i<pnum;++i)
  {
    pthread_join(pd[i],&r);
  }

  return 0;
}


