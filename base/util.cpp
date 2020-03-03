#include "util.h"
#include <unistd.h>
#include <time.h>
#include <sys/time.h>


std::string trim(const std::string& s){
	if (s.empty())
		return s;

	int b = 0, e = s.length()-1;
	//trim before
	while (s[b] == ' ' || s[b] == '\t' || s[b] == '\r' || s[b] == '\n')b++;
	while (s[e] == ' ' || s[e] == '\t' || s[e] == '\r' || s[e] == '\n')e--;
	
	return s.substr(b, e-b+1);
}
void crash(){
    int* p = nullptr;
    *p = 0;
}

bool daemon_run()
{
    return ::daemon(1, 0) == 0;
}

pid_t getProcessId(){
    return ::getpid();
}

pthread_t getThreadid(){
    return ::pthread_self();
}

void getLocalTime(struct tm * pTm, int* usec){
    if(!pTm)return; //如果tm为空，即使usec不为空，也不去获取时间
    time_t cur;
    if(usec){   //需要获得微妙
        struct timeval tv;
        ::gettimeofday(&tv, nullptr);
        cur = tv.tv_sec;
        *usec = tv.tv_usec;
    }else{  //不需要微妙
        ::time(&cur);
    }
    //返回值是cur的地址
    ::localtime_r(&cur, pTm);
    //PS: tm结构体中，year是1900到现在的年数，mon取值范围是[0,11]
//    fprintf(stderr, "%p %p\n", now, pTm);
}

