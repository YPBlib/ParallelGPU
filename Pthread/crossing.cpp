#include <pthread.h>
#include <unistd.h>
#include <string>
#include <cstdio>
#include <vector>
#include <map>
#include <ctime>
#include <chrono>
#include <iostream>

using std::string;
using std::vector;
using int3 = int[3];

#define RIGHT(ch) ((directValue[tolower(ch)]+4-1)%4)
#define LEFT(ch) ((directValue[tolower(ch)]+1)%4)
#define WAITING 0
#define RUNNING 1
#define HANGING 2


#define CAMERA do{\
                    printf("\nt=%ld\n",time(nullptr));\
                    printf("\t%ld",camera[0].size());\
                    if(!camera[0].empty())printf("(%d)",camera[NORTH][0]);\
                    printf("\n");\
                    printf("%ld",camera[3].size());\
                    if(!camera[3].empty())printf("(%d)",camera[WEST][0]);\
                    printf("\t\t");\
                    printf("%ld",camera[1].size());\
                    if(!camera[1].empty())printf("(%d)",camera[EAST][0]);\
                    printf("\n");\
                    printf("\t%ld",camera[2].size());\
                    if(!camera[2].empty())printf("(%d)",camera[SOUTH][0]);\
                    printf("\n");\
                }while(0);

const int NORTH=0;
const int EAST=1;
const int SOUTH=2;
const int WEST=3;

std::map<char,int> directValue{{'n',NORTH},{'e',EAST},{'s',SOUTH},{'w',WEST}};
char dictionary[4]={'N','E','S','W'};

struct thread
{
    pthread_t* self=nullptr;
    
    thread()
    {
        self=new pthread_t;
    }

    int create(const pthread_attr_t* attr, void*(*start_routine)(void*),void* arg)
    {
        return pthread_create(self,attr,start_routine,arg);
    }
    void join(void** retval)
    {
        pthread_join(*self,retval);
    }

    ~thread()
    {
        if(self)delete self;
    }
};

pthread_mutex_t mux[4]{PTHREAD_MUTEX_INITIALIZER,PTHREAD_MUTEX_INITIALIZER,PTHREAD_MUTEX_INITIALIZER,PTHREAD_MUTEX_INITIALIZER};
pthread_cond_t cond[4]{PTHREAD_COND_INITIALIZER,PTHREAD_COND_INITIALIZER,PTHREAD_COND_INITIALIZER,PTHREAD_COND_INITIALIZER};
pthread_rwlock_t camera_rwlock[4]{PTHREAD_RWLOCK_INITIALIZER,PTHREAD_RWLOCK_INITIALIZER,PTHREAD_RWLOCK_INITIALIZER,PTHREAD_RWLOCK_INITIALIZER};
pthread_rwlock_t status_rwlock[4]{PTHREAD_RWLOCK_INITIALIZER,PTHREAD_RWLOCK_INITIALIZER,PTHREAD_RWLOCK_INITIALIZER,PTHREAD_RWLOCK_INITIALIZER};
int status[4]{WAITING,WAITING,WAITING,WAITING};
pthread_cond_t* conders;


int car_num;
// std::vector is not thread_safe, which might cause unknown bug.
vector<int> camera[4];

inline int left(const int& i)
{
    return (i+1)%4;
}

inline int right(const int& i)
{
    return (i+3)%4;
}

// car_id, direction
void* routine(void* arg)
{
    auto ptr=static_cast<int*>(arg);
    int carid=ptr[0];
    int src=-1;
    char flag;
    bool deadlock=true;

    if(carid!=-1)
    {
        src=ptr[1];
        flag= dictionary[src];
    }

    if(carid==-1)
    {
        for(int i=0;i<4;++i) pthread_rwlock_rdlock(&camera_rwlock[i]);
        for(int i=0;i<4;++i)
        {
            if(camera[i].empty())
            {
                deadlock=false;
                break;
            }
        }
        for(int i=0;i<4;++i) pthread_rwlock_unlock(&camera_rwlock[i]);
    }

    if(carid!=-1)
    {
        int head=-1;
        // waiting queue
        while(head!=carid)
        {
            pthread_rwlock_rdlock(&camera_rwlock[src]);

            if(!camera[src].empty())
            {
                head=camera[src][0];
            }
            pthread_rwlock_unlock(&camera_rwlock[src]);
            
        }

        status[src]=RUNNING;
        printf("Car No.%d from %c arrives at crossing head=%d\n",carid,flag,head);
         
        if(head!=-1)
        {
            // arriving phase
            printf("thread %d ,dead while cond_waiting[%d][%c]\n",carid,carid,dictionary[src]);
            pthread_cond_wait(&conders[carid],&mux[src]);
            printf("thread %d at line %d ,recv cond_wait[%c]\n",carid,__LINE__,dictionary[src]);
        } 
    }

    if(carid==-1)
    {
        sleep(1);
        if(deadlock)
        {
            int min=car_num+1,min_i;
            for(int i=0;i<4;++i)
            {
                if(camera[i].size()<min)
                {
                    min=camera[i].size();
                    min_i=i;
                }
            }
            char ch = dictionary[min_i];
            printf("DEADLOCK detected, direction %c has the fewest car(s), let it go\n",ch);
            int f=camera[min_i][0];
            pthread_cond_signal(&conders[f]);
            printf("signal deadlock done\n");
        }
        else
        {
            for(int i=0;i<4;++i)
            {
                if((!camera[i].empty()) && camera[right(i)].empty())
                {
                    printf("thread %d at line %d , before cond_signal\n",carid,__LINE__);
                    int f1=camera[i][0];
                    pthread_cond_signal(&conders[f1]);
                    printf("thread %d signal_conders[%d][%c] done\n",carid,f1,dictionary[i]);
                    if(camera[left(i)].empty() &&(!camera[left(left(i))].empty()))
                    {
                        int f2=left(left(i));
                        f2=camera[f2][0];
                        pthread_cond_signal(&conders[f2]);
                        printf("thread -1 also  conders[%d][%c] done\n",f2,dictionary[left(left(i))]  );
                    }
                    break;
                }
            }
        }
    }

    if(carid!=-1)
    {
        // crossing phase
        printf("car No.%d from %c is crossing\n",carid,flag);
        
        // leaving phase
        printf("car No.%d from %c finished leaves\n",carid,flag);

        // the concurrency part
        int l=camera[left(src)].empty()?0:1;
        int r=camera[right(src)].empty()?0:1;
        int thou=camera[left(left(src))].empty()?0:1;
        int me=camera[src].empty()?0:1;

        int sum=l+r+thou+me;
        printf("thread %d[%c] is at concurrent part,sum=%d\n",carid,dictionary[src],sum);

        int un_result = pthread_mutex_unlock(&mux[src]);
        int lock_result = pthread_mutex_lock(&mux[src]);
        
        for(int i=0;i<4;++i)pthread_rwlock_wrlock(&camera_rwlock[i]);
        camera[src].erase(camera[src].begin(),camera[src].begin()+1);
        CAMERA;
        for(int i=0;i<4;++i)pthread_rwlock_unlock(&camera_rwlock[i]);
        sleep(1);
       

        if(sum==1)
        {
            if(me==1)
            {
                int f = camera[src][0];
                
                pthread_cond_signal(&conders[f]);
                printf("thread %d[%c]  signal_conders[%d] done\n",carid,dictionary[src],f);
                status[src]=WAITING;
            }
            else if(l==1)
            {
                int f=camera[left(src)][0];
                pthread_cond_signal(&conders[f]);
                status[src]=WAITING;
            }  
            else if(r==1)
            {
                int f=camera[right(src)][0];
                pthread_cond_signal(&conders[f]);
                status[src]=WAITING;
            }
            else if(thou==1 && status[left(left(src))]==WAITING)
            {
                int f=camera[left(left(src))][0];
                pthread_cond_signal(&conders[f]);
                status[src]=WAITING;
            }
        }
        else if(sum==2)
        {
            if(thou==0&&r==0)
            {
                int f=camera[left(src)][0];
                pthread_cond_signal(&conders[f]);
                status[src]=WAITING;
            }
            else if(thou==0&&l==0)
            {
                int f=camera[right(src)][0];
                pthread_cond_signal(&conders[f]);
                status[src]=WAITING;
            }
            else if(l==0&&r==0)
            {
                int f=camera[src][0];
                pthread_cond_signal(&conders[f]);
                status[src]=WAITING;
            }
            else if(thou==0&&me==0)
            {
                int f1=camera[left(src)][0];
                int f2=camera[right(src)][0];
                pthread_cond_signal(&conders[f1]);
                pthread_cond_signal(&conders[f2]);
                status[src]=WAITING;
            }
            else if(me==0&&r==0)
            {
                while(status[left(left(src))!=WAITING]);
                int f=camera[left(src)][0];
                pthread_cond_signal(&conders[f]);
                status[src]=HANGING;
            }
            else if(me==0&&r==0)
            {
                if(status[left(left(src))==WAITING])
                {
                    int f=camera[left(left(src))][0];
                    pthread_cond_signal(&conders[f]);
                }
                status[src]=WAITING;
            }
        }
        else if(sum==3)
        {
            if(me==0)
            {
                int f=camera[left(src)][0];
                pthread_cond_signal(&conders[f]);
                status[src]=WAITING;
            }
            else if(l==0)
            {
                int f=camera[left(left(src))][0];
                pthread_cond_signal(&conders[f]);
                status[src]=WAITING;
            }
            else if(thou==0)
            {
                int f1=camera[left(src)][0];
                int f2=camera[right(src)][0];
                pthread_cond_signal(&conders[f1]);
                pthread_cond_signal(&conders[f2]);
                status[src]=WAITING;
            }
            else // if(r==0)
            {
                if(status[left(left(src))]==WAITING)
                {
                    int f=camera[left(src)][0];
                    pthread_cond_signal(&conders[f]);
                    status[src]=WAITING;
                }
                else if(status[left(left(src))]==RUNNING)
                {
                    status[src]=HANGING;
                    while(status[left(left(src))==RUNNING]);
                    int f=camera[left(src)][0];
                    pthread_cond_signal(&conders[f]);
                    status[src]=WAITING;
                }
                else if(status[left(left(src))]==HANGING)
                {
                    status[src]=WAITING;
                }
            }
        }

        else // if(sum==4)
        {
            int f=camera[left(src)][0];
            pthread_cond_signal(&conders[f]);
            status[src]=WAITING;
        }
    }

    printf("thread %d exit\n",carid);
    return nullptr;
}


int main(int argc, char* argv[])
{
    auto core_num=sysconf(_SC_NPROCESSORS_ONLN);
    printf("%ld core(s) available\n",core_num);
    
    string str(argv[1]);
    car_num=str.size();
    auto handlers=new thread[car_num+1];
    using int3=int[3];
    auto args = new int3[car_num+1];
    conders=new pthread_cond_t[car_num];
    
    for(int i=0;i<car_num;++i)
    {
        char c=tolower(str[i]);
        if(c=='n'||c=='e'||c=='s'||c=='w')
        {
            camera[directValue[c]].push_back(i);
        }
        else throw "invalid input string";
        pthread_cond_init(&conders[i],nullptr);
    }

    // main thread lock the 4 mux
    for(int i=0;i<4;++i)
    {
        pthread_mutex_lock(&mux[i]);
    }

    for(int i=0;i<car_num;++i)
    {
        args[i][0]=i;
        args[i][1]= directValue[str[i]];
        args[i][2]=2;
    }

    args[car_num][0]=-1;
    args[car_num][1]=-1;
    args[car_num][2]=-1;

    CAMERA
    
    for(int i=0;i<car_num+1;++i)
    {
        handlers[i].create(nullptr,routine,args[i]);
    }

    for(int i=0;i<car_num+1;++i)
    {
        handlers[i].join(nullptr);
    }

    delete [] handlers;
    delete [] args;
    delete []conders;
    printf("finished\n");

    // should call pthread::destroy to GC      
    return 0;
}

