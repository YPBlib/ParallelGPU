#include<stdio.h>
#include<string.h>
#include<mpi.h>

void VectorMatrix(double local_M[],double local_A[],double local_B[],int local_m,int local_n,int n,MPI_Comm comm)
{
    double* x;
    int local_i,j;
    int local_done=1;
    x=(double*)malloc(n*sizeof(double));
    MPI_Allgather(local_A,local_n,MPI_DOUBLE,x,local_n,MPI_DOUBLE,comm);
    for（local_i=1;local_i<local_m;++local_i)
    {
        local_B[local_i]=0.;
        for(j=0;j<n;++j)
        {
            local_B[local_i]+=local_M[local_i*n+j]*x[j];
        }
    }
    free(x);
}