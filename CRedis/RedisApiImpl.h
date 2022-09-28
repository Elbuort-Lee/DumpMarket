/*
V1.0.2.0 ��Ҫ��KStreams ����Kline1m����û��
V1.0.3.0 ����publish 
V1.0.4.0 ɾ��publish��������Ϊ�������Ĳ�����ͬһ������

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

#define VERSION "V1.0.5.0"



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

    std::thread m_recvmsg_th;




};