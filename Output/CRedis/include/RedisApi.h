#pragma once



#include "RedisSpi.h"

class CRedisApi
{
public:
    static CRedisApi* Create();
    virtual ~CRedisApi();

    virtual int Connect(const char* ip, int port, const char* passwd, int db) = 0;
    virtual int RegisterSpi(CRedisSpi* spi) = 0;

    virtual void* GetValue(const char* key) = 0;
    virtual int SetValue(const char* key, void* value) = 0;

    virtual int Psubscrib(const char* key) = 0;
    virtual int Publish(const char* key, const char* msg, int msgLen) = 0;
};

