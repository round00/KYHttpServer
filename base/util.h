
#ifndef KYIMSERVER_UTIL_H
#define KYIMSERVER_UTIL_H
#include <string>
#include <vector>

std::string trim(const std::string& s);
//分割字符串
std::vector<std::string> split(const std::string& s, char sep=' ');
//比较字符串，无视大小写
bool equalNocase(const std::string& s1, const std::string& s2);

void crash();


bool daemon_run();

pid_t getProcessId();

pthread_t getThreadid();

//可重入，返回当前时间的 tm 结构体和微妙
void getLocalTime(struct tm * pTm, int* usec);
#endif //KYIMSERVER_UTIL_H