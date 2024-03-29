#include "RedisApiImpl.h"

CRedisApiImpl* CRedisApiImpl::s_redisApiImpl = nullptr;
CRedisApiImpl::CRedisApiImpl()
{
    printf("version : %s\n",VERSION);
    m_pRedisContext = nullptr;
    m_pRedisPublish = nullptr;
    m_spi = nullptr;
    m_recvmsg_th = std::thread(&CRedisApiImpl::RecvMsg, this);

    m_isCreated = true;
}

CRedisApiImpl::~CRedisApiImpl()
{
    if (m_pRedisContext)
    {
        redisFree(m_pRedisContext);
        m_pRedisContext = nullptr;
    }
    if (m_pRedisPublish)
    {
        redisFree(m_pRedisPublish);
        m_pRedisPublish = nullptr;
    }

}

CRedisApiImpl * CRedisApiImpl::GetInstance()
{
    if (s_redisApiImpl != nullptr)
    {
        return s_redisApiImpl;
    }
    else
    {
        s_redisApiImpl = new CRedisApiImpl();
        return s_redisApiImpl;
    }
}

bool CRedisApiImpl::IsCreated()
{
    return m_isCreated;
}

int CRedisApiImpl::Connect(const char* ip, int port, const char* passwd, int db)
{
    redisContext* pRedisContext = redisConnect(ip, port);
    redisReply *pRedisReply = (redisReply*)redisCommand(pRedisContext, "AUTH %s", passwd);
    if (NULL != pRedisReply)
    {
        freeReplyObject(pRedisReply);
    }
    pRedisReply = (redisReply*)redisCommand(pRedisContext, "SELECT %d",db);
    if (NULL != pRedisReply)
    {
        freeReplyObject(pRedisReply);
    }
    m_pRedisContext = pRedisContext;

    redisContext* pRedisContext1 = redisConnect(ip, port);
    redisReply *pRedisReply1 = (redisReply*)redisCommand(pRedisContext1, "AUTH %s", passwd);
    if (NULL != pRedisReply1)
    {
        freeReplyObject(pRedisReply1);
    }
    pRedisReply1 = (redisReply*)redisCommand(pRedisContext1, "SELECT %d", db);
    if (NULL != pRedisReply1)
    {
        freeReplyObject(pRedisReply1);
    }
    m_pRedisPublish = pRedisContext1;
    return 0;
}


int CRedisApiImpl::RegisterSpi(CRedisSpi * spi)
{
    m_spi = spi;
    return 0;
}

void * CRedisApiImpl::GetValue(const char * key)
{
    /*
    //用get命令获取数据
    char szRedisBuff[256] = { 0 };
    sprintf_s(szRedisBuff, "GET %s", "name");
    pRedisReply = (redisReply*)redisCommand(pRedisContext, szRedisBuff);
    if (NULL == pRedisReply)
    {
        printf("Get data Error!");
        return -1;
    }

    if (NULL == pRedisReply->str)
    {
        freeReplyObject(pRedisReply);
        return -1;
    }

    std::string strRes(pRedisReply->str);
    freeReplyObject(pRedisReply);
    std::cout << "value:"<<strRes << std::endl;
    */
    return nullptr;
}

int CRedisApiImpl::SetValue(const char * key, void * value)
{
    /*
    //向Redis写入数据
    pRedisReply = (redisReply *)redisCommand(pRedisContext, "SET keyName huantest");
    if (NULL != pRedisReply)
    {
        freeReplyObject(pRedisReply);
    }

    */
    return 0;
}

int CRedisApiImpl::Psubscrib(const char * key)
{
    redisReply *pRedisReply = (redisReply *)redisCommand(m_pRedisContext, "SUBSCRIBE %s", key);
    if (NULL == pRedisReply || pRedisReply->type != REDIS_REPLY_ARRAY)//订阅成功返回一个数组标识
    {
        printf("subscribe failed!\n");
        return -1;
    }
    printf("subscribe %s success!\n",key);
    if (pRedisReply != NULL)
    {
        freeReplyObject(pRedisReply);
    }
    return 0;
}

int CRedisApiImpl::Publish(const char * key, const char * msg, int msgLen)
{
    redisReply *pRedisReply = (redisReply *)redisCommand(m_pRedisPublish, "publish %s %s", key, msg);
    if (NULL == pRedisReply || pRedisReply->type != REDIS_REPLY_INTEGER)//成功推送几个就会有几个
    {
        printf("publish failed!\n");
        return -1;
    }
    if (pRedisReply != NULL)
    {
        freeReplyObject(pRedisReply);
    }
    return 0;
}




void CRedisApiImpl::RecvMsg()
{
    while (true)
    {
        if (m_pRedisContext)
        {
            void *_reply = nullptr;
            if (redisGetReply(m_pRedisContext, &_reply) != REDIS_OK)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                continue;
            }
            redisReply *pRedisReply = (redisReply*)_reply;
            //str可能为NULL，需要加保护
            if (strcmp(pRedisReply->element[0]->str, "message") != 0)
            {
                continue;
            }
            if (strcmp(pRedisReply->element[1]->str, "FutureQuoteE") == 0)
            {
                m_spi->OnFutureQuoteE(pRedisReply->element[2]->str, pRedisReply->element[2]->len);
            }
            else if (strcmp(pRedisReply->element[1]->str, "FutureTrans") == 0)
            {
                m_spi->OnFutureTrans(pRedisReply->element[2]->str, pRedisReply->element[2]->len);
            }
            else if (strcmp(pRedisReply->element[1]->str, "FutureKline1") == 0)
            {
                m_spi->OnFutureKline1(pRedisReply->element[2]->str, pRedisReply->element[2]->len);
            }
            else if (strcmp(pRedisReply->element[1]->str, "FutureKline3") == 0)
            {
                m_spi->OnFutureKline3(pRedisReply->element[2]->str, pRedisReply->element[2]->len);
            }
            else if (strcmp(pRedisReply->element[1]->str, "FutureKline5") == 0)
            {
                m_spi->OnFutureKline5(pRedisReply->element[2]->str, pRedisReply->element[2]->len);
            }
            else if (strcmp(pRedisReply->element[1]->str, "FutureKline15") == 0)
            {
                m_spi->OnFutureKline15(pRedisReply->element[2]->str, pRedisReply->element[2]->len);
            }
            else if (strcmp(pRedisReply->element[1]->str, "FutureKline240") == 0)
            {
                m_spi->OnFutureKline240(pRedisReply->element[2]->str, pRedisReply->element[2]->len);
            }
			else if (strcmp(pRedisReply->element[1]->str, "SpotKline1m") == 0)
			{
				m_spi->OnSpotKline1m(pRedisReply->element[2]->str, pRedisReply->element[2]->len);
			}
			else if (strcmp(pRedisReply->element[1]->str, "SpotQuoteE") == 0)
			{
				m_spi->OnSpotQuoteE(pRedisReply->element[2]->str, pRedisReply->element[2]->len);
			}
			else if (strcmp(pRedisReply->element[1]->str, "SpotTrans") == 0)
			{
				m_spi->OnSpotTrans(pRedisReply->element[2]->str, pRedisReply->element[2]->len);
			}
            freeReplyObject(pRedisReply);
        }
        else
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        
    }
}
