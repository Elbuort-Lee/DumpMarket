/*
wli 20220911 V1.0.2.0 需要把KStreams 换成Kline1m，但没动
wli 20220916 V1.0.3.0 增加publish 
wli 20220926 V1.0.4.0 删除publish函数，因为发布订阅不能在同一个库中
wli 20220928 V1.0.5.0 增加publish ，管理两个或多个tcp,让发布订阅在同一个库中
wli 20220930 V2.0.0.0 更改为合约期货，之前的现货不维护了
wli 20221008 V2.0.0.1_credis 增加4h的订阅
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