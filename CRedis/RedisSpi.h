#pragma once

class CRedisSpi
{
public:
    virtual void OnRecvQuoteE(const char* msg, int len) = 0;//盘口
    virtual void OnRecvTrans(const char* msg, int len) = 0;//逐笔成交
    //type=1-60,分钟级，小时级，日级，月级等
    virtual void OnRecvKline(const char* msg, int len, int type) = 0;//K线-级别
    virtual void OnRecvOtherMsg(const char* msg, int len) = 0;//其他

};