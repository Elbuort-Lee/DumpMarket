/*
v1.0.2.0 : �޸�HandleTrans�е�m_quoteE_cvΪm_trans_cv���޸�HandleKline1�е�m_quoteE_cvΪm_kline1_cv
wli 20220930 V2.0.0.0_dumpdata ���̺�Լ�ڻ����ݣ�֮ǰ�ֻ����ݶ���
wli 20221003 V2.0.0.1_dumpdata �޸�json.c��Сд����
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
    virtual void OnRecvOtherMsg(const char* msg, int len);//����

    virtual void OnFutureQuoteE(const char* msg, int len);//�̿�
    virtual void OnFutureTrans(const char* msg, int len);//��ʳɽ�
    virtual void OnFutureKline1(const char* msg, int len);//K��-1����
    virtual void OnFutureKline3(const char* msg, int len);//K��-3����
    virtual void OnFutureKline5(const char* msg, int len);//K��-5����
    virtual void OnFutureKline15(const char* msg, int len);//K��-15����

protected:
    //ȷ����Ϣ��Ѹ�����̣�����ȷ����Ϣ��˳���ԣ���Ҫ�����̣߳���������������������Ϣ��
    //��촦���ַ����ķ����ǣ�ʹ��ָ��ȥȡ���������ݣ������н����ַ�������������Ҫ�ַ����ĸ�ʽ�̶������������㣬���Բ��á�
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