// $ mpicc -g -Wall -o helloMPI helloMPI.c
// $ mpiexec -n <number of process> ./helloMPI
#include<stdio.h>
#include<string.h>
#include<mpi.h>
const int MAX_STRING = 100;
int main(void)
{
    char greeting[MAX_STRING];
    int comm_sz;
    int my_rank;

    MPI_Init(NULL,NULL);    //int MPI_Init(int* argc_p,char*** argv_p);
    MPI_Comm_size(MPI_COMM_WORLD,&comm_sz);     //int MPI_Comm_size(MPI_Comm comm,int* comm_sz_p);
    MPI_Comm_rank(MPI_COMM_WORLD,&my_rank);     //int MPI_Comm_rank(MPI_Comm comm,int* my_rank_p);
    if(my_rank!=0)
    {
        sprintf(greeting,"Greetings from process %d of %d!",my_rank,comm_sz);
        MPI_Send(greeting,strlen(greeting)+1,MPI_CHAR,0,0,MPI_COMM_WORLD);
//  int MPI_Send(void* msg_buf_p,int msg_size,MPI_Datatype msg_type,int dest,int tag,MPI_Comm communicator);
//  tag = 0 for printing, tag =1 for computing
    }
    else
    {
        printf("Greetings from process %d of %d! \n",my_rank,comm_sz);
        for(int q=1;q<comm_sz;++q)
        {
            MPI_Recv(greeting,MAX_STRING,MPI_CHAR,q,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
//  int MPI_Recv(void* msg_buf_p,int buf_size,MPI_Datatype buf_type,int source,int tag,MPI_Comm communicator,MPI_Status* status_p);
            printf("%s\n",greeting);
        }
    }
    MPI_Finalize();     //int MPI_Finalize(void);
    return 0;
}
/*
*        send & recv:
*        recv_comm==send_comm;recv_tag==send_tag;dest==r;source==q;
*        recv_type == send_type; recv_buf_sz>=send_buf_sz;
*/
//     SPMD single program, multiple data

