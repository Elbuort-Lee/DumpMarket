#pragma once


struct QuoteE
{
    long long localtime;//本地时间
    char symbol[40];
    double bidpx[20];
    double bidvol[20];
    double askpx[20];
    double askvol[20];
};


struct Trans
{
    long long eventtime;//事件时间
    long long localtime;//本地时间
    long long tradetime;//成交时间
    long long tradeid;//交易ID
    long long bidid;//买方的订单ID
    long long askid;//卖方的订单ID
    char symbol[40];//交易对
    double filledpx;//成交价格
    double filledqty;//成交数量
    bool make;//买方是否是做市方。如1-true,0-false,则此次成交是一个主动卖出单，否则是一个主动买入单。
};

struct Kline1
{
    long long eventtime;//事件时间
    long long localtime;//本地时间
    long long starttime;//这根K线的起始时间
    long long endtime;//这根K线的结束时间
    long long firstid;//这根K线期间第一笔成交ID
    long long lastid;//这根K线期间末一笔成交ID
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
    char interval[4];//K线间隔
    bool isrunend;//这根K线是否完结(是否已经开始下一根K线)0-false,1-true
};