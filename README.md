# DumpMarket
dump btc market to csv
version 1.0.1.0 加日志和配置文件
CRedis:
V1.0.2.0 需要把KStreams 换成Kline1m，但没动
V1.0.3.0 增加publish
V1.0.4.0 删除publish函数，因为发布订阅不能在同一个库中
V1.0.5.0 增加publish 发布订阅可以在一个库，之前智障了
wli 20220930 V2.0.0.0 更改为合约期货，之前的现货不维护了

dumpData:
v1.0.2.0 : 修复HandleTrans中的m_quoteE_cv为m_trans_cv，修复HandleKline1中的m_quoteE_cv为m_kline1_cv
wli 20220930 V2.0.0.0_dumpdata 落盘合约期货数据，之前现货数据丢弃
wli 20221003 V2.0.0.1_dumpdata 修改json.c大小写敏感
