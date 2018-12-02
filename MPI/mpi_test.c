#include <mpi.h>
#include <stdio.h>
#include <string.h>


int main()
{
 char msg[4096]={0};
 MPI_Init(NULL,NULL);
 int all_proc;
 int my_rank;
 

 MPI_Comm_size(MPI_COMM_WORLD,&all_proc);
 MPI_Comm_rank(MPI_COMM_WORLD,&my_rank);

 if(my_rank!=0)
 {
   sprintf(msg,"process %d from %d",my_rank,all_proc);
   MPI_Send(msg,strlen(msg)+1,MPI_CHAR,0,0,MPI_COMM_WORLD);
 }
 else
 {
   printf("I am process 0\n");
   for(int i=0;i<all_proc;++i)
   {
     MPI_Recv(msg,4096,MPI_CHAR,i,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
     printf("%s\n",msg);
   }
 }
 MPI_Finalize();
 return 0;
}
