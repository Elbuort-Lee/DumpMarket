#include "RedisApiImpl.h"

CRedisApiImpl* CRedisApiImpl::s_redisApiImpl = nullptr;
CRedisApiImpl::CRedisApiImpl()
{
    m_pRedisContext = nullptr;
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
        freeReplyObject(pRedisReply);
        return -1;
    }
    printf("subscribe %s success!\n",key);
    freeReplyObject(pRedisReply);
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
            if (strcmp(pRedisReply->element[1]->str, "QuoteE") == 0)
            {
                m_spi->OnRecvQuoteE(pRedisReply->element[2]->str, pRedisReply->element[2]->len);
            }
            else if (strcmp(pRedisReply->element[1]->str, "Trans") == 0)
            {
                m_spi->OnRecvTrans(pRedisReply->element[2]->str, pRedisReply->element[2]->len);
            }
            else if (strcmp(pRedisReply->element[1]->str, "KStreams") == 0)
            {
                m_spi->OnRecvKline(pRedisReply->element[2]->str, pRedisReply->element[2]->len, 1);
            }
            freeReplyObject(pRedisReply);
        }
        else
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        
    }
}
