#include "HandleMsg.h"
HandleMsg::HandleMsg(const char* configName)
{
    m_clog = new CLogFile(".", "dumpdata");
    m_clog->WriteLogEx(LEV_INFO, "%s:%d dump version %s", __FILE__, __LINE__, VERSION);
    bool ret = ReadConfig(configName);
    if (ret)
    {
        m_clog->WriteLogEx(LEV_INFO, "%s:%d read config is succeed.(filepath=%s)(ip=%s)(port=%d)(passwd=%s)(dbid=%d)",
            __FILE__, __LINE__, m_path.c_str(), m_ip.c_str(), m_port, m_passwd.c_str(),m_dbId);
    }
    else
    {
        m_clog->WriteLogEx(LEV_ERROR, "%s:%d read config is failed.(filepath=%s)(ip=%s)(port=%d)(passwd=%s)(dbid=%d)", 
            __FILE__, __LINE__, m_path.c_str(), m_ip.c_str(), m_port, m_passwd.c_str(), m_dbId);
    }
    m_fileNameQuoteE = "FutureQuoteE";
    m_fileNameTrans = "FutureTrans";
    m_fileNameKline1 = "FutureKline";
    m_api = CRedisApi::Create();
}

HandleMsg::~HandleMsg()
{
    CloseFile();
    if (m_quoteE_th.joinable())
    {
        m_quoteE_th.join();
    }
    if (m_trans_th.joinable())
    {
        m_trans_th.join();
    }
    if (m_kline1_th.joinable())
    {
        m_kline1_th.join();
    }
}

void HandleMsg::Init(int date)
{
    m_date = date;
    m_clog->WriteLogEx(LEV_INFO, "%s:%d [Init](date=%d)", __FILE__, __LINE__, m_date);
    //quoteE
    char szMsg[2048];
    char fileOutQuoteE[256] = { 0 };
    snprintf(fileOutQuoteE, sizeof(fileOutQuoteE), "%s/%s.%d.csv", m_path.c_str(), m_fileNameQuoteE.c_str(), m_date);
    //加锁
    {
        std::lock_guard<std::mutex> lck(m_close_quoteE_lock);
        m_fpQuoteE = fopen(fileOutQuoteE, "ab+");
        if (m_fpQuoteE == NULL)
        {
            m_clog->WriteLogEx(LEV_ERROR, "%s:%d open file[%s] failed.", __FILE__, __LINE__, fileOutQuoteE);
        }
        memset(szMsg, 0, sizeof(szMsg));
        snprintf(szMsg, sizeof(szMsg), "servertime,localtime,securityType,symbol,"
            "bidpx0,bidvol0,bidpx1,bidvol1,bidpx2,bidvol2,bidpx3,bidvol3,bidpx4,bidvol4,"
            "bidpx5,bidvol5,bidpx6,bidvol6,bidpx7,bidvol7,bidpx8,bidvol8,bidpx9,bidvol9,"
            "askpx0,askvol0,askpx1,askvol1,askpx2,askvol2,askpx3,askvol3,askpx4,askvol4,"
            "askpx5,askvol5,askpx6,askvol6,askpx7,askvol7,askpx8,askvol8,askpx9,askvol9\n");
        fseek(m_fpQuoteE, 0, SEEK_END);
        fwrite(szMsg, 1, strlen(szMsg), m_fpQuoteE);
        fflush(m_fpQuoteE);
    }
    
    //Trans
    char fileOutTrans[256] = { 0 };
    snprintf(fileOutTrans, sizeof(fileOutTrans), "%s/%s.%d.csv", m_path.c_str(), m_fileNameTrans.c_str(), m_date);
    //加锁
    {
        std::lock_guard<std::mutex> lck(m_close_trans_lock);
        m_fpTrans = fopen(fileOutTrans, "ab+");
        if (m_fpTrans == NULL)
        {
            m_clog->WriteLogEx(LEV_ERROR, "%s:%d open file[%s] failed.", __FILE__, __LINE__, fileOutTrans);
        }
        memset(szMsg, 0, sizeof(szMsg));
        snprintf(szMsg, sizeof(szMsg), "servertime,localtime,securityType,symbol,filledpx,filledqty,make\n");
        fseek(m_fpTrans, 0, SEEK_END);
        fwrite(szMsg, 1, strlen(szMsg), m_fpTrans);
        fflush(m_fpTrans);
    }
    
    //Kline
    char fileOutKline1[256] = { 0 };
    snprintf(fileOutKline1, sizeof(fileOutKline1), "%s/%s.%d.csv", m_path.c_str(), m_fileNameKline1.c_str(), m_date);
    //加锁
    {
        std::lock_guard<std::mutex> lck(m_close_kline1_lock);
        m_fpKline1 = fopen(fileOutKline1, "ab+");
        if (m_fpKline1 == NULL)
        {
            m_clog->WriteLogEx(LEV_ERROR, "%s:%d open file[%s] failed.", __FILE__, __LINE__, fileOutKline1);
        }
        memset(szMsg, 0, sizeof(szMsg));
        snprintf(szMsg, sizeof(szMsg), "servertime,localtime,securityType,klineType,symbol,starttime,endtime,num,openpx,closepx,highpx,lowpx,cumqty,cumamt,activeqty,activeamt,isrunend\n");
        fseek(m_fpKline1, 0, SEEK_END);
        fwrite(szMsg, 1, strlen(szMsg), m_fpKline1);
        fflush(m_fpKline1);
    }
}

void HandleMsg::CloseFile()
{ 
    if (m_fpQuoteE != NULL)
    {
        std::lock_guard<std::mutex> lck(m_close_quoteE_lock);
        fclose(m_fpQuoteE);
        m_fpQuoteE = NULL;
    }
    if (m_fpTrans != NULL)
    {
        std::lock_guard<std::mutex> lck(m_close_trans_lock);
        fclose(m_fpTrans);
        m_fpTrans = NULL;
    }
    if (m_fpKline1 != NULL)
    {
        std::lock_guard<std::mutex> lck(m_close_kline1_lock);
        fclose(m_fpKline1);
        m_fpKline1 = NULL;
    }
}

void HandleMsg::Start()
{
    int ret = -1;
    ret = m_api->Connect(m_ip.c_str(), m_port, m_passwd.c_str(), m_dbId);
    m_clog->WriteLogEx(LEV_INFO, "%s:%d [Start](Connect=%d)", __FILE__, __LINE__, ret);
    ret = m_api->RegisterSpi(this);
    m_clog->WriteLogEx(LEV_INFO, "%s:%d [Start](RegisterSpi=%d)", __FILE__, __LINE__, ret);
    ret = m_api->Psubscrib("FutureQuoteE");
    m_clog->WriteLogEx(LEV_INFO, "%s:%d [Start](FutureQuoteE Psubscrib=%d)", __FILE__, __LINE__, ret);
    ret = m_api->Psubscrib("FutureTrans");
    m_clog->WriteLogEx(LEV_INFO, "%s:%d [Start](FutureTrans Psubscrib=%d)", __FILE__, __LINE__, ret);
    ret = m_api->Psubscrib("FutureKline1");
    m_clog->WriteLogEx(LEV_INFO, "%s:%d [Start](FutureKline1 Psubscrib=%d)", __FILE__, __LINE__, ret);
    ret = m_api->Psubscrib("FutureKline3");
    m_clog->WriteLogEx(LEV_INFO, "%s:%d [Start](FutureKline3 Psubscrib=%d)", __FILE__, __LINE__, ret);
    ret = m_api->Psubscrib("FutureKline5");
    m_clog->WriteLogEx(LEV_INFO, "%s:%d [Start](FutureKline5 Psubscrib=%d)", __FILE__, __LINE__, ret);
    ret = m_api->Psubscrib("FutureKline15");
    m_clog->WriteLogEx(LEV_INFO, "%s:%d [Start](FutureKline15 Psubscrib=%d)", __FILE__, __LINE__, ret);
}

void HandleMsg::StartThread()
{
    m_quoteE_th = std::thread(&HandleMsg::HandleQuoteE, this);
    m_trans_th = std::thread(&HandleMsg::HandleTrans, this);
    m_kline1_th = std::thread(&HandleMsg::HandleKline1, this);
}


void HandleMsg::OnRecvOtherMsg(const char * msg, int len)
{
    m_clog->WriteLogEx(LEV_INFO, "%s:%d [OnRecvOtherMsg](msg=%s)(len=%d)", __FILE__, __LINE__, msg, len);
}

void HandleMsg::OnFutureQuoteE(const char * msg, int len)
{
    PushQuoteE(msg);
}

void HandleMsg::OnFutureTrans(const char * msg, int len)
{
    PushTrans(msg);
}

void HandleMsg::OnFutureKline1(const char * msg, int len)
{
    PushKline1(msg);
}

void HandleMsg::OnFutureKline3(const char * msg, int len)
{
    PushKline1(msg);
}

void HandleMsg::OnFutureKline5(const char * msg, int len)
{
    PushKline1(msg);
}

void HandleMsg::OnFutureKline15(const char * msg, int len)
{
    PushKline1(msg);
}

void HandleMsg::PushQuoteE(const char * msg)
{
    std::lock_guard<std::mutex> lck(m_quoteE_lock);
    m_quoteE_que.push(msg);
    m_quoteE_cv.notify_one();
}

void HandleMsg::PushTrans(const char * msg)
{
    std::lock_guard<std::mutex> lck(m_trans_lock);
    m_trans_que.push(msg);
    m_trans_cv.notify_one();
}

void HandleMsg::PushKline1(const char * msg)
{
    std::lock_guard<std::mutex> lck(m_kline1_lock);
    m_kline1_que.push(msg);
    m_kline1_cv.notify_one();
}

void HandleMsg::HandleQuoteE()
{
    while (true)
    {
        //24h运行必备
        if (m_fpQuoteE == NULL)
        {
            m_clog->WriteLogEx(LEV_ERROR, "%s:%d m_fpQuoteE open file failed.", __FILE__, __LINE__);
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            continue;
        }
        std::string msg;
        msg.clear();
        {
            std::unique_lock<std::mutex> lck(m_quoteE_lock);
            if (m_quoteE_cv.wait_for(lck, std::chrono::milliseconds(10), [this] {return !this->m_quoteE_que.empty(); }))
            {
                msg = m_quoteE_que.front();
                m_quoteE_que.pop();
            }
            else
            {
                continue;
            }
        }
        if (!msg.empty())
        {
            neb::CJsonObject sz_json;
            sz_json.Clear();
            sz_json.Parse(msg);
            std::string sz_value;
            sz_value.clear();
            bool isok = sz_json.Get("e", sz_value);
            if (isok && strcmp(sz_value.c_str(), "depthUpdate") == 0)
            {
                timeb t;
                ftime(&t);
                QuoteE quote;
                memset(&quote, 0, sizeof(QuoteE));
                quote.localtime = t.time * 1000 + t.millitm;
                sz_json.Get("T", quote.servertime);
                sz_value.clear();
                sz_json.Get("s", sz_value);
                strcpy(quote.symbol, sz_value.c_str());
                quote.securityType = SecurityType::future;
                for (int i = 0; i < sz_json["b"].GetArraySize(); i++)
                {
                    quote.bidpx[i] = std::atof(sz_json["b"][i](0).c_str());
                    quote.bidvol[i] = std::atof(sz_json["b"][i](1).c_str());
                }
                for (int i = 0; i < sz_json["a"].GetArraySize(); i++)
                {
                    quote.askpx[i] = std::atof(sz_json["a"][i](0).c_str());
                    quote.askvol[i] = std::atof(sz_json["a"][i](1).c_str());
                }
                char szMsg[2048];
                snprintf(szMsg, sizeof(szMsg), "%lld,%lld,%d,%s,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,"
                    "%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f\n",
                    quote.servertime, quote.localtime, quote.securityType, quote.symbol,
                    quote.bidpx[0], quote.bidvol[0], quote.bidpx[1], quote.bidvol[1], quote.bidpx[2], quote.bidvol[2], quote.bidpx[3], quote.bidvol[3], quote.bidpx[4], quote.bidvol[4],
                    quote.bidpx[5], quote.bidvol[5], quote.bidpx[6], quote.bidvol[6], quote.bidpx[7], quote.bidvol[7], quote.bidpx[8], quote.bidvol[8], quote.bidpx[9], quote.bidvol[9],
                    quote.askpx[0], quote.askvol[0], quote.askpx[1], quote.askvol[1], quote.askpx[2], quote.askvol[2], quote.askpx[3], quote.askvol[3], quote.askpx[4], quote.askvol[4],
                    quote.askpx[5], quote.askvol[5], quote.askpx[6], quote.askvol[6], quote.askpx[7], quote.askvol[7], quote.askpx[8], quote.askvol[8], quote.askpx[9], quote.askvol[9]);
                std::lock_guard<std::mutex> lck(m_close_quoteE_lock);
                if (m_fpQuoteE)
                {
                    fseek(m_fpQuoteE, 0, SEEK_END);
                    fwrite(szMsg, 1, strlen(szMsg), m_fpQuoteE);
                    fflush(m_fpQuoteE);
                }
            }
        }
    }
}

void HandleMsg::HandleTrans()
{
    while (true)
    {
        if (m_fpTrans == NULL)
        {
            m_clog->WriteLogEx(LEV_ERROR, "%s:%d m_fpTrans open file failed.", __FILE__, __LINE__);
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            continue;
        }
        std::string msg;
        msg.clear();
        {
            std::unique_lock<std::mutex> lck(m_trans_lock);
            if (m_trans_cv.wait_for(lck, std::chrono::milliseconds(10), [this] {return !this->m_trans_que.empty(); }))
            {
                msg = m_trans_que.front();
                m_trans_que.pop();
            }
            else
            {
                continue;
            }
        }
        if (!msg.empty())
        {
            neb::CJsonObject sz_json;
            sz_json.Clear();
            sz_json.Parse(msg);
            std::string sz_value;
            sz_value.clear();
            bool isok = sz_json.Get("e", sz_value);
            if (isok && strcmp(sz_value.c_str(), "aggTrade") == 0)
            {
                timeb t;
                ftime(&t);
                Trans trans;
                memset(&trans, 0, sizeof(Trans));
                trans.localtime = t.time * 1000 + t.millitm;
                trans.servertime = atoll(sz_json("T").c_str());
                strcpy(trans.symbol, sz_json("s").c_str());
                trans.securityType = SecurityType::future;
                trans.filledpx = atof(sz_json("p").c_str());
                trans.filledqty = atof(sz_json("q").c_str());
                trans.make = (sz_json("m")[0] == 't') ? true : false;
                char szMsg[512];
                snprintf(szMsg, sizeof(szMsg), "%lld,%lld,%d,%s,%f,%f,%d\n",
                    trans.servertime, trans.localtime, trans.securityType, trans.symbol, trans.filledpx, trans.filledqty, trans.make);
                std::lock_guard<std::mutex> lck(m_close_trans_lock);
                if (m_fpTrans)
                {
                    fseek(m_fpTrans, 0, SEEK_END);
                    fwrite(szMsg, 1, strlen(szMsg), m_fpTrans);
                    fflush(m_fpTrans);
                }
            }
        }
    }
}

void HandleMsg::HandleKline1()
{
    while (true)
    {
        if (m_fpKline1 == NULL)
        {
            m_clog->WriteLogEx(LEV_ERROR, "%s:%d m_fpKline1 open file failed.", __FILE__, __LINE__);
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            continue;
        }
        std::string msg;
        msg.clear();
        {
            std::unique_lock<std::mutex> lck(m_kline1_lock);
            if (m_kline1_cv.wait_for(lck, std::chrono::milliseconds(10), [this] {return !this->m_kline1_que.empty(); }))
            {
                msg = m_kline1_que.front();
                m_kline1_que.pop();
            }
            else
            {
                continue;
            }
        }
        if (!msg.empty())
        {
            auto ptr = strstr(msg.c_str(), "true");
            if (ptr == nullptr)
            {
                continue;
            }
            neb::CJsonObject sz_json;
            sz_json.Clear();
            sz_json.Parse(msg);
            std::string sz_value;
            sz_value.clear();
            bool isok = sz_json.Get("e", sz_value);
            if (isok && strcmp(sz_value.c_str(), "continuous_kline") == 0)
            {
                timeb t;
                ftime(&t);
                Kline line;
                memset(&line, 0, sizeof(Kline));
                line.localtime = t.time * 1000 + t.millitm;
                sz_json.Get("E", line.servertime);
                strcpy(line.symbol, sz_json("ps").c_str());
                line.securityType = SecurityType::future;
                line.starttime = atoll(sz_json["k"]("t").c_str());
                line.endtime = atoll(sz_json["k"]("T").c_str());
                line.num = atoll(sz_json["k"]("n").c_str());
                line.openpx = atof(sz_json["k"]("o").c_str());
                line.closepx = atof(sz_json["k"]("c").c_str());
                line.highpx = atof(sz_json["k"]("h").c_str());
                line.lowpx = atof(sz_json["k"]("l").c_str());
                line.cumqty = atof(sz_json["k"]("v").c_str());
                line.cumamt = atof(sz_json["k"]("q").c_str());
                line.activeqty = atof(sz_json["k"]("V").c_str());
                line.activeamt = atof(sz_json["k"]("Q").c_str());
                line.isrunend = (sz_json["k"]("x")[0] == 't') ? true : false;
                line.klineType = KlineTypeChange(sz_json["k"]("i").c_str());
                char szMsg[512];
                snprintf(szMsg, sizeof(szMsg), "%lld,%lld,%d,%d,%s,%lld,%lld,%lld,%f,%f,%f,%f,%f,%f,%f,%f,%d\n",
                    line.servertime, line.localtime, line.securityType, line.klineType, line.symbol,line.starttime, line.endtime, line.num,
                    line.openpx, line.closepx, line.highpx, line.lowpx, line.cumqty, line.cumamt, line.activeqty, line.activeamt,line.isrunend);
                std::lock_guard<std::mutex> lck(m_close_kline1_lock);
                if (m_fpKline1)
                {
                    fseek(m_fpKline1, 0, SEEK_END);
                    fwrite(szMsg, 1, strlen(szMsg), m_fpKline1);
                    fflush(m_fpKline1);
                }
            }
        }
    }
}

bool HandleMsg::ReadConfig(const char * fileName)
{

    TIniFile* p = new TIniFile();
    if (!p->Open(fileName))
    {
        delete p;
        p = nullptr;
        return false;
    }
    char szvalue[512] = { 0 };
    p->ReadString("dump", "outfilepath", "./", szvalue,sizeof(szvalue));
    m_path = szvalue;
    memset(szvalue, 0, sizeof(szvalue));
    p->ReadString("redis", "ip", "127.0.0.1", szvalue, sizeof(szvalue));
    m_ip = szvalue;
    m_port = p->ReadInt("redis", "port", 6379);
    memset(szvalue, 0, sizeof(szvalue));
    p->ReadString("redis", "passwd", "123456", szvalue, sizeof(szvalue));
    m_passwd = szvalue;
    m_dbId = p->ReadInt("redis", "dbid", 3);
    return true;
}

KlineType HandleMsg::KlineTypeChange(const char * str)
{
    if (strcmp(str, "1m") == 0)
    {
        return KlineType::minute1;
    }
    else if (strcmp(str, "3m") == 0)
    {
        return KlineType::minute3;
    }
    else if (strcmp(str, "5m") == 0)
    {
        return KlineType::minute5;
    }
    else if (strcmp(str, "15m") == 0)
    {
        return KlineType::minute15;
    }
    else if (strcmp(str, "30m") == 0)
    {
        return KlineType::minute30;
    }
    else if (strcmp(str, "1h") == 0)
    {
        return KlineType::hour1;
    }
    else if (strcmp(str, "2h") == 0)
    {
        return KlineType::hour2;
    }
    else if (strcmp(str, "4h") == 0)
    {
        return KlineType::hour4;
    }
    return KlineType::invaild;
}

