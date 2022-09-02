// DumpMarketFromRedis.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//


#include <Windows.h>
#include <hiredis.h>
#include <string>
#include <iostream>


int main()
{

    //redis默认监听端口为6387 可以再配置文件中修改 
    redisContext* pRedisContext = redisConnect("8.219.188.209", 6379);
    if (NULL == pRedisContext || pRedisContext->err)
    {
        printf("%s \r\n", pRedisContext->errstr);
        printf("Connect to redis server failed \n");
        return -1;
    }

    //输入Redis密码
    const char *pszRedisPwd = "080814080";
    redisReply *pRedisReply = (redisReply*)redisCommand(pRedisContext, "AUTH %s", pszRedisPwd);
    if (NULL != pRedisReply)
    {
        freeReplyObject(pRedisReply);
    }
    pRedisReply = (redisReply*)redisCommand(pRedisContext, "SELECT 3");
    if (NULL != pRedisReply)
    {
        freeReplyObject(pRedisReply);
    }
    //用get命令获取数据
    //char szRedisBuff[256] = { 0 };
    //sprintf_s(szRedisBuff, "GET %s", "name");
    //pRedisReply = (redisReply*)redisCommand(pRedisContext, szRedisBuff);
    //if (NULL == pRedisReply)
    //{
    //    printf("Get data Error!");
    //    return -1;
    //}

    //if (NULL == pRedisReply->str)
    //{
    //    freeReplyObject(pRedisReply);
    //    return -1;
    //}

    //std::string strRes(pRedisReply->str);
    //freeReplyObject(pRedisReply);
    //std::cout << "value:"<<strRes << std::endl;
    ////向Redis写入数据
    //pRedisReply = (redisReply *)redisCommand(pRedisContext, "SET keyName huantest");
    //if (NULL != pRedisReply)
    //{
    //    freeReplyObject(pRedisReply);
    //}



    //4、订阅消息  可以订阅多个消息 PSUBSCRIBE pattern 
    pRedisReply = (redisReply *)redisCommand(pRedisContext, "SUBSCRIBE %s", "KStreams");
    if (NULL == pRedisReply || pRedisReply->type != REDIS_REPLY_ARRAY)//订阅成功返回一个数组标识
    {
        std::cout << "subscribe failed!" << std::endl;
        freeReplyObject(pRedisReply);
        redisFree(pRedisContext);
        pRedisContext = NULL;
        return 0;
    }
    freeReplyObject(pRedisReply);

    //5、阻塞等待订阅消息
    while (true)
    {
        void *_reply = nullptr;
        if (redisGetReply(pRedisContext, &_reply) != REDIS_OK)
        {
            continue;
        }
        pRedisReply = (redisReply*)_reply;
        for (int nIndex = 0; nIndex < pRedisReply->elements; nIndex++)
        {
            std::cout << nIndex + 1 << ")";
            std::cout << pRedisReply->element[nIndex]->str << std::endl;
        }
        freeReplyObject(pRedisReply);
        std::cout << "***************" << std::endl;
    }
    redisFree(pRedisContext);
    pRedisContext = NULL;


    return 0;
}

