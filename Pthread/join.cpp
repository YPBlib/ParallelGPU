#include <pthread.h>
#include <unistd.h>
#include <string>
#include <cstdio>
#include <iostream>
#include <vector>
#include <map>

using std::string;
using std::cout;
using std::cin;
using std::endl;
using std::vector;




const int NORTH=0;
const int EAST=1;
const int SOUTH=2;
const int WEST=3;

#define RIGHT(ch) (directions[ch]+4-1)%4

volatile int ask=0;
vector<vector<int>> camera;

std::map<char,int> directions{{'n',NORTH},{'e',EAST},{'s',SOUTH},{'w',WEST}};

struct thread
{
    pthread_t* self=nullptr;
    int thread_id;
    
    thread()
    {
        self=new pthread_t;
    }

    ~thread()
    {
        if(self)delete self;
    }
    int create(const pthread_attr_t* attr, void*(*start_routine)(void*),void* arg,int src )
    {
	return pthread_create(self,attr,start_routine,arg);
    }
    void join(void** retval)
    {
	pthread_join(*self,retval);
    }
};

// car id, direction, car_mod(positive or negative)
void* routine(void* arg)
{
    auto ptr=static_cast<int*>(arg);
    int carid=ptr[0];
    int src=ptr[1];
    int positive=ptr[2];
    for(int m=0;m<10;++m)
    {
        sleep(1);
        printf("car No. %d from _ is crossng\n",carid);
    }
    return nullptr;
}


int main(int argc, char* argv[])
{
    string str(argv[1]);
    
    pthread_mutex_t mutex_a,mutex_b,mutex_c,mutex_d;
    auto handlers=new thread[str.size()];
    
    

    using int3=int[3];
    auto args = new int3[str.size()];
    for(auto i=0;i<str.size();++i)
    {
        args[i][0]=i;   // car id
        args[i][1]=i%4; // direction
        args[i][2]=0;   // nothing
        handlers[i].create(nullptr,routine,args[i],args[i][1]);
    }
    for(int m=0;m<10;++m)
    {
        sleep(1);
        printf("join done!\n");
    }
	
	// 不同核数机器会有不同实现 不能保证主线程与create出来的线程中 各自printf的执行顺序
	// 貌似，有可能是create后立即执行，也可能是遇到join语句才执行
    
    for(auto i=0;i<str.size();++i)
    {
        handlers[i].join(nullptr);
    }
   
    delete[] handlers;
    return 0;
}
