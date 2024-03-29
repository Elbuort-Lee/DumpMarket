/*
v1.0.2.0 : 修复HandleTrans中的m_quoteE_cv为m_trans_cv，修复HandleKline1中的m_quoteE_cv为m_kline1_cv
wli 20220930 V2.0.0.0_dumpdata 落盘合约期货数据，之前现货数据丢弃
wli 20221003 V2.0.0.1_dumpdata 修改json.c大小写敏感
*/
#pragma once
#include "marketstruct.h"
#include "RedisApi.h"
#include "RedisSpi.h"
#include "profile.h"
#include "logfile.h"
#include "CJsonObject.hpp"

#include<stdio.h>
#include<queue>
#include<thread>
#include<mutex>
#include<condition_variable>
#include <sys/timeb.h>
#include <string.h>

#define VERSION "V2.0.0.1_dumpdata"

class HandleMsg : public CRedisSpi
{
public:
    HandleMsg(const char* configName);
    ~HandleMsg();
    void Init(int date);
    void CloseFile();
    void Start();
    void StartThread();
public:
    virtual void OnRecvOtherMsg(const char* msg, int len);//其他

    virtual void OnFutureQuoteE(const char* msg, int len);//盘口
    virtual void OnFutureTrans(const char* msg, int len);//逐笔成交
    virtual void OnFutureKline1(const char* msg, int len);//K线-1分钟
    virtual void OnFutureKline3(const char* msg, int len);//K线-3分钟
    virtual void OnFutureKline5(const char* msg, int len);//K线-5分钟
    virtual void OnFutureKline15(const char* msg, int len);//K线-15分钟
	virtual void OnSpotKline1m(const char* msg, int len) {};//现货K线
	virtual void OnSpotQuoteE(const char* msg, int len) {};//现货盘口
	virtual void OnSpotTrans(const char* msg, int len) {};//现货逐笔成交
protected:
    //确保消息能迅速落盘，并且确保消息的顺序性，需要三个线程，三个队列来处理三种消息。
    //最快处理字符串的方法是，使用指针去取定长的数据，来进行解析字符串，这样的需要字符串的格式固定，本场景满足，所以采用。
    void PushQuoteE(const char* msg);
    void PushTrans(const char* msg);
    void PushKline1(const char* msg);
    
    void HandleQuoteE();
    void HandleTrans();
    void HandleKline1();

    bool ReadConfig(const char* fileName);
    KlineType KlineTypeChange(const char* str);
    
private:
    std::string m_path;
    std::string m_fileNameQuoteE;
    std::string m_fileNameTrans;
    std::string m_fileNameKline1;
    int m_date;
    std::string m_ip;
    int m_port;
    std::string m_passwd;
    int m_dbId;


    CRedisApi* m_api = nullptr;
    std::queue<std::string> m_quoteE_que;
    std::queue<std::string> m_trans_que;
    std::queue<std::string> m_kline1_que;

    std::condition_variable m_quoteE_cv;
    std::condition_variable m_trans_cv;
    std::condition_variable m_kline1_cv;

    std::mutex m_quoteE_lock;
    std::mutex m_trans_lock;
    std::mutex m_kline1_lock;

    FILE* m_fpQuoteE = nullptr;
    FILE* m_fpTrans = nullptr;
    FILE* m_fpKline1 = nullptr;

    std::thread m_quoteE_th;
    std::thread m_trans_th;
    std::thread m_kline1_th;

    std::mutex m_close_quoteE_lock;
    std::mutex m_close_trans_lock;
    std::mutex m_close_kline1_lock;

    CLogFile * m_clog = nullptr;

};