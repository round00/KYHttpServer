//
// Created by gjk on 2019/12/31.
//

#include <iostream>
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>
#include <stdarg.h>

#include "Logger.h"
#include "util.h"
#include "Mutex.h"
#include "Thread.h"
#include "Condition.h"

bool CLogger::m_bInit = false;
std::string CLogger::m_strLogDir;
FILE* CLogger::m_fLogFile = nullptr;
LOG_LEVEL CLogger::m_nLogLevel;
int CLogger::m_nLastCheckDay = 0;


bool CLogger::m_bRunning = false;
CThread CLogger::m_logThread(logThreadFunc);
std::queue<std::string>  CLogger::m_logQueue;
CMutex CLogger::m_mutexLogQueue;
CCondition CLogger::m_condLogQueue;

const int MAX_LOG_LENGTH = 1024;

bool CLogger::init(const char *logDir, LOG_LEVEL level) {
    if(m_bInit)return true;
    if(!logDir || !logDir[0]){
        m_strLogDir = "";
    }
    else{
        m_strLogDir = logDir;
    }

    m_nLogLevel = level;

    m_bRunning = true;
    if(!m_logThread.start()){
        return false;
    }
    m_bInit = true;
    return true;
}

void CLogger::unint() {
    m_bInit = false;
    if(m_logThread.isRunning()){
        m_bRunning = false;
        m_logThread.stop();
    }

    if(m_fLogFile){
        ::fclose(m_fLogFile);
        m_fLogFile = nullptr;
    }
    //清空日志输出队列
    while(!m_logQueue.empty())m_logQueue.pop();
    m_strLogDir.clear();
}

void CLogger::log(LOG_LEVEL level, const char *fileName, int lineNo, const char *format, ...){
    //还没有初始化好，不能写入日志
    if(!m_bInit)return;
    //低于设定日志等级时，不写入
    if(level < m_nLogLevel){
        return;
    }

    //=======================构造一行日志=======================
    // 日志输出格式：[LEVEL][16:27:30.339][pid][tid][filename:lineno] logcontent
    std::string strlogLine;
    //level
    strlogLine += "[" + getLogLevelStr(level) + "]";

    {//time
        struct tm now;
        int msec;
        getLocalTime(&now, &msec);
        msec /= 1000;
        char buf[64] = {0};
        snprintf(buf, 64, "[%02d:%02d:%02d.%03d]",
                now.tm_hour, now.tm_min, now.tm_sec, msec);

        strlogLine += buf;
    }
    //process id and thread id
    strlogLine += "[" + std::to_string((long)getProcessId()) + "]";
    strlogLine += "[" + std::to_string((long)getThreadid()) + "]";

    {//filename and line number
        char buf[256] = {0};
        snprintf(buf,256, "[%s:%d]", fileName, lineNo);
        strlogLine += buf;
    }
    {
        char buf[MAX_LOG_LENGTH];
        va_list vl;
        va_start(vl, format);
        vsnprintf(buf, MAX_LOG_LENGTH, format, vl);
        va_end(vl);
        strlogLine += " ";
        strlogLine += buf;
    }
    strlogLine += "\n";
    //=======================构造一行日志=======================

    //FATAL日志，写完就要挂掉，所以直接写，不用日志线程写
    if(level == LOG_LEVEL_FATAL){
        writeToFile(strlogLine);
        crash();
    }else{
        //将待写的日志加入日志队列即可，由日志写入线程处理
        CMutexLock lock(m_mutexLogQueue);
        m_logQueue.push(strlogLine);
        m_condLogQueue.notify();
    }
}

std::string CLogger::getLogFileName(){
    //设置的日志目录为空
    if(m_strLogDir.empty()){
        return "";
    }
    //确定目录是否存在，不存在则创建
    if(::access(m_strLogDir.c_str(), F_OK) != 0){
        if(::mkdir(m_strLogDir.c_str(), 0777)!=0){
            crash();
        }
    }
    //生成日志文件名称
    struct tm now;
    getLocalTime(&now, nullptr);
    char logFileName[64];
    snprintf(logFileName, 64,"%04d%02d%02d_%02d.log", now.tm_year+1900, now.tm_mon+1, now.tm_mday, now.tm_hour);
    std::string fullName = m_strLogDir + "/" + logFileName;

    return fullName;
}

std::string CLogger::getLogLevelStr(LOG_LEVEL level) {
    switch(level){
        case LOG_LEVEL_DEBUG:
            return "DEBUG";
        case LOG_LEVEL_INFO:
            return "INFO";
        case LOG_LEVEL_WARNING:
            return "WARNING";
        case LOG_LEVEL_ERROR:
            return "ERROR";
        case LOG_LEVEL_FATAL:
            return "FATAL";
        default:
            return "UNKNOWN";
    }
}

FILE* CLogger::getLogFile(){
    FILE * f = nullptr;
    std::string fileName = getLogFileName();
    if(fileName.empty()){
        f = stdout;
    }
    else{
        //文件不存在会创建
        f = ::fopen(fileName.c_str(), "a+");
        if(!f){
            crash();
        }
    }
    return f;
}

void CLogger::writeToFile(const std::string &s){
    int n = s.length(), hasWritten = 0;
    while(hasWritten<n){
        int nWrite = ::fwrite(s.c_str() + hasWritten, 1, n-hasWritten, m_fLogFile);
        hasWritten += nWrite;
    }
}

bool CLogger::checkGapADay(){
    struct tm now;
    getLocalTime(&now, nullptr);

    int curDay = now.tm_mday;
    bool bDiffDay = curDay != m_nLastCheckDay;
    m_nLastCheckDay = curDay;
    return bDiffDay;
}

void* CLogger::logThreadFunc(void *){
    while(m_bRunning){
        //获取到日志队列的访问权限后，先把日志队列的内容放到一个临时队列里，让队列锁抓紧释放
        std::queue<std::string> logQueue;
        {//等待读取日志待写队列
            CMutexLock lock(m_mutexLogQueue);
            while(m_logQueue.empty()){
                m_condLogQueue.wait(m_mutexLogQueue);
            }
            //先用临时变量把待打印日志取出来，以免占用锁导致其他写成不能写入
            logQueue.swap(m_logQueue);
        }
        //如果还没有打开日志文件，或者间隔了一天，就重新或者日志文件
        if(!m_fLogFile || checkGapADay()){
            m_fLogFile = getLogFile();
        }
        //打印日志
        while(!logQueue.empty()){
            std::string strLog = logQueue.front();
            logQueue.pop();
            writeToFile(strLog);
        }
    }
    return (void*)1;
}