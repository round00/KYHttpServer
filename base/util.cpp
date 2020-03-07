#include "util.h"
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>


std::string trim(const std::string& s){
	if (s.empty())
		return s;

	int b = 0, e = s.length()-1;
	//trim before
	while (s[b] == ' ' || s[b] == '\t' || s[b] == '\r' || s[b] == '\n')b++;
	while (s[e] == ' ' || s[e] == '\t' || s[e] == '\r' || s[e] == '\n')e--;
	
	return s.substr(b, e-b+1);
}

std::vector<std::string> spilt(const std::string& s, char sep){
    std::vector<std::string> strs;
    int last = 0, i = 0, len = s.length();
    while(i<len){
        if(s[i]==sep){
            if(last < i)
                strs.push_back(s.substr(last, i-last));
            last = i+1;
        }
        ++i;
    }
    if(last<i){
        strs.push_back(s.substr(last, i-last));
    }
    return strs;
}

bool equalNocase(const std::string& s1, const std::string& s2){
    if(s1.length()!=s2.length())return false;
    int len = s1.length();
    for(int i = 0; i<len; ++i){
        char c1 = s1[i], c2 = s2[i];
        if(c1>='a' && c1<='z')c1 = c1-'a'+'A';
        if(c2>='a' && c2<='z')c2 = c2-'a'+'A';
        if(c1!=c2)return false;
    }
    return true;
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

