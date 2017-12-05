//      estiamte PI
extern int thread_count,n;
extern double sum;
extern pthread_mutex_t mutex;
void* locksum(void* rank)
{
    long my_rank=(long)rank;
    double factor;
    long long i;
    long long local_n=n / thread_count;
    long long local_first_i=local_n*my_rank;
    long long local_last_i=local_first_i+local_n;
    double local_sum=0.;
    if(local_first_i % 2 ==0)
    {
        factor=1.;
    }
    else factor=-1.;
    for(i=local_first_i;i<=local_last_i;++i,factor=-factor)
    {
        local_sum+=factor/(2*i+1);
    }
    pthread_mutex_lock(&mutex);
    sum += local_sum;
    pthread_mutex_unlock(&mutex);
    return NULL;
}