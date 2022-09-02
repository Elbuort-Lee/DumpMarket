#pragma once
#include "marketstruct.h"
#include "RedisApi.h"
#include "RedisSpi.h"

#include<stdio.h>
#include<queue>
#include<thread>
#include<mutex>
#include<condition_variable>
#include <sys/timeb.h>
#include <string.h>

class HandleMsg : public CRedisSpi
{
public:
    HandleMsg();
    ~HandleMsg();
    void Init(int date);
    void CloseFile();
    void Start();
    void StartThread();
public:
    virtual void OnRecvQuoteE(const char* msg, int len);//�̿�
    virtual void OnRecvTrans(const char* msg, int len);//��ʳɽ�
    //type=1-60,���Ӽ���Сʱ�����ռ����¼���
    virtual void OnRecvKline(const char* msg, int len, int type);//K��-����
    virtual void OnRecvOtherMsg(const char* msg, int len);//����

protected:
    //ȷ����Ϣ��Ѹ�����̣�����ȷ����Ϣ��˳���ԣ���Ҫ�����̣߳���������������������Ϣ��
    //��촦���ַ����ķ����ǣ�ʹ��ָ��ȥȡ���������ݣ������н����ַ�������������Ҫ�ַ����ĸ�ʽ�̶������������㣬���Բ��á�
    void PushQuoteE(const char* msg);
    void PushTrans(const char* msg);
    void PushKline1(const char* msg);
    
    void HandleQuoteE();
    void HandleTrans();
    void HandleKline1();

private:
    std::string m_path;
    std::string m_fileNameQuoteE;
    std::string m_fileNameTrans;
    std::string m_fileNameKline1;
    int m_date;


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



};