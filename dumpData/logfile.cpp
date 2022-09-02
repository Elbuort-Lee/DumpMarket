/**
 * @file      : logfile.cpp
 * @copyright : 上海卡方信息科技有限公司
 * @author    : jggu
 * @date      : 2021/06/10 22:17
 * @brief     : 简单的日志文件处理
 */

#include "logfile.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>

#include <time.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <sys/time.h>
#endif

#include <thread>

#ifndef __min
#define __min(a,b) (((a) < (b)) ? (a) : (b))
#endif // !__min

#ifdef _WIN32
#include <io.h>
#include <process.h>
#include <direct.h>
#include <Windows.h>
#define PATHMARK  '\\'
#define MAKEPATH(a) mkdir(a)

#define access _access
#define sh_fopen(a,b,c) _fsopen(a, b, c)
#else
#define SH_DENYNO 0x40
#define SH_DENYWR 0x20

#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#define PATHMARK  '/'
#define MAKEPATH(a) mkdir(a,0777)
#define sh_fopen(a, b, c) fopen(a, b)
#define sh_open(a, b, c) InternalOpen(a, b, S_IREAD | S_IWRITE)

#endif

#pragma warning(disable:4996)

#ifdef _WIN32
int gettimeofday(struct timeval *tp, void *tzp)
{
    time_t clock;
    struct tm tm;
    SYSTEMTIME wtm;
    GetLocalTime(&wtm);
    tm.tm_year = wtm.wYear - 1900;
    tm.tm_mon = wtm.wMonth - 1;
    tm.tm_mday = wtm.wDay;
    tm.tm_hour = wtm.wHour;
    tm.tm_min = wtm.wMinute;
    tm.tm_sec = wtm.wSecond;
    tm.tm_isdst = -1;
    clock = mktime(&tm);
    tp->tv_sec = clock;
    tp->tv_usec = wtm.wMilliseconds * 1000;
    return (0);
}

tm* gmtime_r(time_t const* const _Time, tm* tmloc)
{
	gmtime_s(tmloc, _Time);
	//tmloc = localtime(_Time);
	return tmloc;
}
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLogFile::CLogFile(const char *szLogPath, const char* szNamePrefix, int fdelay)
    : m_bStop(false)
{
    memset(m_logpath, 0, sizeof(m_logpath));
    memset(m_nameprefix, 0, sizeof(m_nameprefix));
    if (szLogPath != NULL && szLogPath[0] != '\0')
        strncpy(m_logpath, szLogPath, __min(strlen(szLogPath), sizeof(m_logpath)));
    else
        strcpy(m_logpath, ".");  // 缺省为当前目录下
    m_logpath[sizeof(m_logpath) - 1] = '\0';

    if (szNamePrefix != NULL && szNamePrefix[0] != '\0')
        strncpy(m_nameprefix, szNamePrefix, __min(strlen(szNamePrefix), sizeof(m_nameprefix)));
    else memset(m_nameprefix, 0, sizeof(m_nameprefix));
    m_logFile = NULL;

    InternalOpen();
}

CLogFile::~CLogFile()
{
    Close();
}

void CLogFile::Close()
{
    m_bStop = true;
    if (m_logThread.joinable())
        m_logThread.join();

    if (m_logFile != NULL)
    {
        fclose(m_logFile);
    }

    m_logFile = NULL;
}

bool CLogFile::Open(char *szLogPath)
{
    m_bStop = false;
    memset(m_logpath, 0, sizeof(m_logpath));
    std::lock_guard<std::mutex> lock(m_mutex);
    if (szLogPath != NULL && szLogPath[0] != '\0')
        strncpy(m_logpath, szLogPath, __min(strlen(szLogPath), sizeof(m_logpath)));
    m_logpath[sizeof(m_logpath) - 1] = '\0';
    return(InternalOpen());
}

void CLogFile::WriteLogEx(int level, const char *pFormat, ...)
{
    va_list pArg;
    char chMsg[10240] = { 0 };
    int i;
    struct tm tmloc;
    if (level < m_writeLevel) //日志级别小于当前日志，就不保存日志了
        return;

    struct timeval tv;
    gettimeofday(&tv, NULL);
    time_t time = tv.tv_sec;
    //memcpy(&tmloc, localtime((const time_t*)&time), sizeof(tmloc));
	//手工切换时区为东八
	time = time + 28800;
	gmtime_r(&time, &tmloc);

    i = snprintf(chMsg, sizeof(chMsg), "%04d%02d%02d %02d:%02d:%02d.%06ld ",
        tmloc.tm_year + 1900, tmloc.tm_mon + 1, tmloc.tm_mday,
        tmloc.tm_hour, tmloc.tm_min, tmloc.tm_sec, tv.tv_usec);

    va_start(pArg, pFormat);
    i += vsprintf(chMsg + i, pFormat, pArg);
    va_end(pArg);

    if (chMsg[i - 1] != '\n')
    {
        strcat(chMsg, "\n");
    }

    std::lock_guard<std::mutex> lock(m_mutex);
    m_logQueue.push(new std::string(chMsg));
    m_logCv.notify_one();
}

void CLogFile::ThreadLog()
{
    std::string* pMsg = nullptr;
    while (!m_bStop)
    {
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            bool bResult = m_logCv.wait_for(lock, std::chrono::milliseconds(20), [this] {return !m_logQueue.empty(); });
            if (bResult)
            {
                pMsg = this->m_logQueue.front();
                m_logQueue.pop();
            }
            else
            {
                continue;
            }
        }
        if (nullptr != pMsg && nullptr != m_logFile)
        {
            fprintf(m_logFile, "%s", pMsg->c_str());
            fflush(m_logFile);
            delete pMsg;
            pMsg = nullptr;
        }
    }
}

bool CLogFile::InternalOpen()
{
    int shflag;
    int len;
    struct tm tmloc;
    char fname[1024];
    Close();

    m_bStop = false;

    len = strlen(m_logpath);
    if (m_logpath[len - 1] == PATHMARK)
        m_logpath[len - 1] = '\0';

    time_t lastftime;
    time(&lastftime);
    memcpy(&tmloc, localtime(&lastftime), sizeof(tmloc));
    m_logdate = ((tmloc.tm_year + 1900) * 100 + tmloc.tm_mon + 1) * 100 + tmloc.tm_mday;

    if (access(m_logpath, 0) != 0)
    {
        MAKEPATH(m_logpath);
    }

    sprintf(fname, "%s%c%s_%d.log", m_logpath, PATHMARK, m_nameprefix, m_logdate);
    /**********************************************************/
    shflag = SH_DENYNO;
    m_logFile = sh_fopen(fname, "a+t", shflag);
    if (m_logFile == NULL)
    {
        m_logFile = sh_fopen(fname, "w+t", shflag);
    }

    m_logThread = std::move(std::thread(&CLogFile::ThreadLog, this));
    return(m_logFile != NULL);
}

void CLogFile::ToFlush()
{
    std::lock_guard<std::mutex> lock(m_mutex);

    if (m_logFile != NULL)
    {
        fflush(m_logFile);
    }
}

void CLogFile::SetWriteLevel(int level)
{
    m_writeLevel = level;
}

