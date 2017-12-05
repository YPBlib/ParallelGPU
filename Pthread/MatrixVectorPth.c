#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
// assuming matrix M[m][n] and vector V[n] are all global
void* MatrixVectorPth(void* rank)
{
    long my_rank=(long) rank;
    int i,j;
    int local_m = m/thread_count;
    int first_row = my_rank*local_m;
    int last_row = (my_rank+1)*local_m -1;
    for(i=first_row;i<=last_row;++I)
    {
        y[i]=0.;
        for(j=0;j<n;++j)
        {
            y[i]+=M[i][j]*V[j]
        }
    }
    return NULL;
}
