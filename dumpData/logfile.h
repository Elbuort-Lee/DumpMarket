/**
 * @file      : logfile.h
 * @copyright : �Ϻ�������Ϣ�Ƽ����޹�˾
 * @author    : jggu
 * @date      : 2021/06/10 21:46
 * @brief     : �򵥵���־�ļ�
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

//��־�ȼ�
enum LEV_RANKS
{
    LEV_ALL = 1,//ALL Level����͵ȼ��ģ����ڴ�������־��¼��
    LEV_DEBUG,
    LEV_INFO,
    LEV_WARN,
    LEV_ERROR,
    LEV_FATAL,//ϵͳ�����󣬻��¼��SYSERR.LOG��
    LEV_OFF, //�ر���־
};

class CLogFile
{
public:
    virtual bool InternalOpen();

public:
    /**
     * @brief    :   ����д��־����
     * @function :   CLogFile::SetWriteLevel
     * @param    :   int level
     * @return   :   void
     * @retval   :   
     * @remark   :   
     * @date     :   2021/06/10
     */
    void SetWriteLevel(int level);
    /**
     * @brief    :   ǿ��ˢ���ļ����ݵ�����
     * @function :   CLogFile::ToFlush
     * @return   :   void
     * @retval   :   
     * @remark   :   
     * @date     :   2021/06/10
     */
    void ToFlush();
    /**
     * @brief    :   ����־�ļ�
     * @function :   CLogFile::Open
     * @param    :   char * szLogPath - Ŀ¼
     * @return   :   bool
     * @retval   :   
     * @remark   :   
     * @date     :   2021/06/10
     */
    bool Open(char *szLogPath = NULL);
    /**
     * @brief    :   д��־�ļ�
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
     * @brief    :   �ر��ļ�
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
    FILE *m_logFile;      // ��־�ļ����
    char m_logpath[256];   // ��־�ļ����·��
    char m_nameprefix[64]; // �ļ���ǰ׺
    int  m_logdate;        // ��ǰ��־�ļ�����

    int m_writeLevel;    //  for disable writing File�ļ���ʾ����־�ȼ�

    std::queue<std::string*> m_logQueue; // ��־����
    std::condition_variable m_logCv;
    std::mutex m_mutex;   // ������
    volatile bool m_bStop;

    void ThreadLog();
    std::thread m_logThread;
};
