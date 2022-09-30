#pragma once

class CRedisSpi
{
public:
    virtual void OnFutureQuoteE(const char* msg, int len) = 0;//盘口
    virtual void OnFutureTrans(const char* msg, int len) = 0;//逐笔成交
    virtual void OnFutureKline1(const char* msg, int len) = 0;//K线-1分钟
    virtual void OnFutureKline3(const char* msg, int len) = 0;//K线-3分钟
    virtual void OnFutureKline5(const char* msg, int len) = 0;//K线-5分钟
    virtual void OnFutureKline15(const char* msg, int len) = 0;//K线-15分钟
    virtual void OnRecvOtherMsg(const char* msg, int len) = 0;//其他

};