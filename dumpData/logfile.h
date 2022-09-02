/**
 * @file      : logfile.h
 * @copyright : 上海卡方信息科技有限公司
 * @author    : jggu
 * @date      : 2021/06/10 21:46
 * @brief     : 简单的日志文件
 */

#pragma once

#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <queue>
#include <condition_variable>

#include "stdarg.h"


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//日志等级
enum LEV_RANKS
{
    LEV_ALL = 1,//ALL Level是最低等级的，用于打开所有日志记录。
    LEV_DEBUG,
    LEV_INFO,
    LEV_WARN,
    LEV_ERROR,
    LEV_FATAL,//系统级错误，会记录到SYSERR.LOG中
    LEV_OFF, //关闭日志
};

class CLogFile
{
public:
    virtual bool InternalOpen();

public:
    /**
     * @brief    :   设置写日志级别
     * @function :   CLogFile::SetWriteLevel
     * @param    :   int level
     * @return   :   void
     * @retval   :   
     * @remark   :   
     * @date     :   2021/06/10
     */
    void SetWriteLevel(int level);
    /**
     * @brief    :   强制刷新文件内容到磁盘
     * @function :   CLogFile::ToFlush
     * @return   :   void
     * @retval   :   
     * @remark   :   
     * @date     :   2021/06/10
     */
    void ToFlush();
    /**
     * @brief    :   打开日志文件
     * @function :   CLogFile::Open
     * @param    :   char * szLogPath - 目录
     * @return   :   bool
     * @retval   :   
     * @remark   :   
     * @date     :   2021/06/10
     */
    bool Open(char *szLogPath = NULL);
    /**
     * @brief    :   写日志文件
     * @function :   CLogFile::WriteLogEx
     * @param    :   int level
     * @param    :   const char * pFormat
     * @param    :   ...
     * @return   :   void
     * @retval   :   
     * @remark   :   
     * @date     :   2021/06/10
     */
#ifndef _WIN32
    void WriteLogEx(int level, const char *pFormat, ...) __attribute__((format(printf, 3, 4)));
#else
    void WriteLogEx(int level, const char *pFormat, ...);
#endif
    
    /**
     * @brief    :   关闭文件
     * @function :   CLogFile::Close
     * @return   :   void
     * @retval   :   
     * @remark   :   
     * @date     :   2021/06/10
     */
    void Close();
    CLogFile(const char *szLogPath = NULL, const char* pNamePrefix = NULL, int fdelay = 60);
    virtual ~CLogFile();

protected:
    FILE *m_logFile;      // 日志文件句柄
    char m_logpath[256];   // 日志文件存放路径
    char m_nameprefix[64]; // 文件名前缀
    int  m_logdate;        // 当前日志文件日期

    int m_writeLevel;    //  for disable writing File文件显示的日志等级

    std::queue<std::string*> m_logQueue; // 日志缓冲
    std::condition_variable m_logCv;
    std::mutex m_mutex;   // 互斥量
    volatile bool m_bStop;

    void ThreadLog();
    std::thread m_logThread;
};
