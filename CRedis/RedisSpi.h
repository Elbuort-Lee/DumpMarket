#pragma once

class CRedisSpi
{
public:
    virtual void OnRecvQuoteE(const char* msg, int len) = 0;//�̿�
    virtual void OnRecvTrans(const char* msg, int len) = 0;//��ʳɽ�
    //type=1-60,���Ӽ���Сʱ�����ռ����¼���
    virtual void OnRecvKline(const char* msg, int len, int type) = 0;//K��-����
    virtual void OnRecvOtherMsg(const char* msg, int len) = 0;//����

};