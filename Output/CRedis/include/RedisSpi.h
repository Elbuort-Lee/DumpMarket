#pragma once

class CRedisSpi
{
public:
	virtual void OnFutureQuoteE(const char* msg, int len) {};//�̿�
    virtual void OnFutureTrans(const char* msg, int len) {};//��ʳɽ�
    virtual void OnFutureKline1(const char* msg, int len) {};//K��-1����
    virtual void OnFutureKline3(const char* msg, int len) {};//K��-3����
    virtual void OnFutureKline5(const char* msg, int len) {};//K��-5����
    virtual void OnFutureKline15(const char* msg, int len) {};//K��-15����
    virtual void OnFutureKline240(const char* msg, int len) {};//K��-4h

	virtual void OnSpotKline1m(const char* msg, int len) {};//�ֻ�K��
	virtual void OnSpotQuoteE(const char* msg, int len) {};//�ֻ��̿�
	virtual void OnSpotTrans(const char* msg, int len) {};//�ֻ���ʳɽ�
    virtual void OnRecvOtherMsg(const char* msg, int len) {};//����
	



};