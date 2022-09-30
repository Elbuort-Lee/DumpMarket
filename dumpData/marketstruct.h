#pragma once

enum class SecurityType
{
    invaild = 0,
    spot =1, //现货
    future = 2,//期货
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
    long long servertime;//成交时间
    long long localtime;//本地时间
    SecurityType securityType;//币种类型
    char symbol[40];
    double bidpx[10];
    double bidvol[10];
    double askpx[10];
    double askvol[10];
};


struct Trans
{
    long long servertime;//成交时间
    long long localtime;//本地时间
    SecurityType securityType;//币种类型
    char symbol[40];//交易对
    double filledpx;//成交价格
    double filledqty;//成交数量
    bool make;//买方是否是做市方。如1-true,0-false,则此次成交是一个主动卖出单，否则是一个主动买入单。
};

struct Kline
{
    long long servertime;//服务器时间
    long long localtime;//本地时间
    SecurityType securityType;//币种市场类型
    KlineType klineType;//K线间隔
    long long starttime;//这根K线的起始时间
    long long endtime;//这根K线的结束时间
    long long num;//这根K线期间成交笔数
    double openpx;//这根K线期间第一笔成交价
    double closepx;//这根K线期间末一笔成交价
    double highpx;//这根K线期间最高成交价
    double lowpx;//这根K线期间最低成交价
    double cumqty;//这根K线期间成交量
    double cumamt;//这根K线期间成交额
    double activeqty;//主动买入的成交量
    double activeamt;//主动买入的成交额
    char symbol[40];//交易对
    bool isrunend;//这根K线是否完结(是否已经开始下一根K线)0-false,1-true
};
