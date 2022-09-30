#pragma once

enum class SecurityType
{
    invaild = 0,
    spot =1, //�ֻ�
    future = 2,//�ڻ�
};

enum class KlineType
{
    invaild = 0,
    minute1 = 1,
    minute3 = 3,
    minute5 = 5,
    minute15 = 15,
    minute30 = 30,
    hour1 = 60,
    hour2 = 120,
    hour4 = 240,
};

struct QuoteE
{
    long long servertime;//�ɽ�ʱ��
    long long localtime;//����ʱ��
    SecurityType securityType;//��������
    char symbol[40];
    double bidpx[10];
    double bidvol[10];
    double askpx[10];
    double askvol[10];
};


struct Trans
{
    long long servertime;//�ɽ�ʱ��
    long long localtime;//����ʱ��
    SecurityType securityType;//��������
    char symbol[40];//���׶�
    double filledpx;//�ɽ��۸�
    double filledqty;//�ɽ�����
    bool make;//���Ƿ������з�����1-true,0-false,��˴γɽ���һ��������������������һ���������뵥��
};

struct Kline
{
    long long servertime;//������ʱ��
    long long localtime;//����ʱ��
    SecurityType securityType;//�����г�����
    KlineType klineType;//K�߼��
    long long starttime;//���K�ߵ���ʼʱ��
    long long endtime;//���K�ߵĽ���ʱ��
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
    bool isrunend;//���K���Ƿ����(�Ƿ��Ѿ���ʼ��һ��K��)0-false,1-true
};
