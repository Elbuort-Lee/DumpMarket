#pragma once

class CRedisSpi
{
public:
	virtual void OnFutureQuoteE(const char* msg, int len) {};//盘口
    virtual void OnFutureTrans(const char* msg, int len) {};//逐笔成交
    virtual void OnFutureKline1(const char* msg, int len) {};//K线-1分钟
    virtual void OnFutureKline3(const char* msg, int len) {};//K线-3分钟
    virtual void OnFutureKline5(const char* msg, int len) {};//K线-5分钟
    virtual void OnFutureKline15(const char* msg, int len) {};//K线-15分钟
    virtual void OnFutureKline240(const char* msg, int len) {};//K线-4h

	virtual void OnSpotKline1m(const char* msg, int len) {};//现货K线
	virtual void OnSpotQuoteE(const char* msg, int len) {};//现货盘口
	virtual void OnSpotTrans(const char* msg, int len) {};//现货逐笔成交
    virtual void OnRecvOtherMsg(const char* msg, int len) {};//其他
	



};