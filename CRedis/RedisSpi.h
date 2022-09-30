#pragma once

class CRedisSpi
{
public:
    virtual void OnFutureQuoteE(const char* msg, int len) = 0;//�̿�
    virtual void OnFutureTrans(const char* msg, int len) = 0;//��ʳɽ�
    virtual void OnFutureKline1(const char* msg, int len) = 0;//K��-1����
    virtual void OnFutureKline3(const char* msg, int len) = 0;//K��-3����
    virtual void OnFutureKline5(const char* msg, int len) = 0;//K��-5����
    virtual void OnFutureKline15(const char* msg, int len) = 0;//K��-15����
    virtual void OnRecvOtherMsg(const char* msg, int len) = 0;//����

};