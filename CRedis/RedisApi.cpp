#include "RedisApi.h"
#include "RedisApiImpl.h"

CRedisApi * CRedisApi::Create()
{
    CRedisApiImpl* p = CRedisApiImpl::GetInstance();
    if (p)
    {
        if (p->IsCreated())
        {
            return p;
        }
        else
        {
            delete p;
        }
    }
    return nullptr;
}

CRedisApi::~CRedisApi()
{
}
