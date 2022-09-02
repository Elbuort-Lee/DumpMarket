#pragma once


struct QuoteE
{
    long long localtime;//����ʱ��
    char symbol[40];
    double bidpx[20];
    double bidvol[20];
    double askpx[20];
    double askvol[20];
};


struct Trans
{
    long long eventtime;//�¼�ʱ��
    long long localtime;//����ʱ��
    long long tradetime;//�ɽ�ʱ��
    long long tradeid;//����ID
    long long bidid;//�򷽵Ķ���ID
    long long askid;//�����Ķ���ID
    char symbol[40];//���׶�
    double filledpx;//�ɽ��۸�
    double filledqty;//�ɽ�����
    bool make;//���Ƿ������з�����1-true,0-false,��˴γɽ���һ��������������������һ���������뵥��
};

struct Kline1
{
    long long eventtime;//�¼�ʱ��
    long long localtime;//����ʱ��
    long long starttime;//���K�ߵ���ʼʱ��
    long long endtime;//���K�ߵĽ���ʱ��
    long long firstid;//���K���ڼ��һ�ʳɽ�ID
    long long lastid;//���K���ڼ�ĩһ�ʳɽ�ID
    long long num;//���K���ڼ�ɽ�����
    double openpx;//���K���ڼ��һ�ʳɽ���
    double closepx;//���K���ڼ�ĩһ�ʳɽ���
    double highpx;//���K���ڼ���߳ɽ���
    double lowpx;//���K���ڼ���ͳɽ���
    double cumqty;//���K���ڼ�ɽ���
    double cumamt;//���K���ڼ�ɽ���
    double activeqty;//��������ĳɽ���
    double activeamt;//��������ĳɽ���
    char symbol[40];//���׶�
    char interval[4];//K�߼��
    bool isrunend;//���K���Ƿ����(�Ƿ��Ѿ���ʼ��һ��K��)0-false,1-true
};