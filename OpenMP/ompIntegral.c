#include<stdio.h>
#include<stdlib.h>
#include<omp.h>

void Trap(double a,bouble b,int n,double* ptrToGlobalResult);

int main(int argc, char* argv[])
{
    double GlobalResult=0.;
    double a,b;
    int n;
    int thread_count;
    thread_count=strol(argv[1],NILL,10);
    scanf("%lf%lf%d",&a,&b,&n);
#pragma omp parallel num_threads(thread_count)
    TRap(a,b,&GlobalResult);
    printf("With n = %d trapezoids, our estimate of the integral from %f to %f is %.14e\n",a,b,GlobalResult);
    return 0;
}

void Trap(double a,double b,int n, double* ptrToGlobalResult)
{
    double h,x,local_result;
    double local_a,local_b;
    int i,local_n;
    int local_rank=omp_get_num_threads();
    h=(b-a)/n;
    local_n=n/thread_count;
    local_a=a+local_rank*local_n*h;
    local_b=local_a+local_n*h;
    local_result=(f(local_a)+f(local_b))/2.;
    for(i=0;i<local_n-1;++i)
    {
        x=local_a+i*h;
        local_result+=f(x);        
    }
    local_result *= h;
#pragma omp critical
    *ptrToGlobalResult += local_result;
}













