/*
wli 20220911 V1.0.2.0 ��Ҫ��KStreams ����Kline1m����û��
wli 20220916 V1.0.3.0 ����publish 
wli 20220926 V1.0.4.0 ɾ��publish��������Ϊ�������Ĳ�����ͬһ������
wli 20220928 V1.0.5.0 ����publish ��������������tcp,�÷���������ͬһ������
wli 20220930 V2.0.0.0 ����Ϊ��Լ�ڻ���֮ǰ���ֻ���ά����
wli 20221008 V2.0.0.1_credis ����4h�Ķ���
*/

#pragma once
#include "RedisApi.h"

#ifdef _WIN32
#include <Windows.h>
#include <hiredis.h>
#else
#include <hiredis/hiredis.h>
#endif

#include <string.h>
#include <thread>

#define VERSION "V2.0.0.1_credis"



class CRedisApiImpl : public CRedisApi
{
public:
    CRedisApiImpl();
    ~CRedisApiImpl();
    static CRedisApiImpl* GetInstance();
    bool IsCreated();

    virtual int Connect(const char* ip, int port, const char* passwd, int db);
    virtual int RegisterSpi(CRedisSpi* spi);

    virtual void* GetValue(const char* key);
    virtual int SetValue(const char* key, void* value);

    virtual int Psubscrib(const char* key);

    virtual int Publish(const char* key, const char* msg, int msgLen);

protected:
    void RecvMsg();



private:
    bool m_isCreated = false;
    static CRedisApiImpl* s_redisApiImpl;
    CRedisSpi* m_spi;


    redisContext* m_pRedisContext;
    redisContext* m_pRedisPublish;
    std::thread m_recvmsg_th;

    


};