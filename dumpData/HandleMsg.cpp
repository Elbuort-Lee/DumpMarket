#include "HandleMsg.h"
HandleMsg::HandleMsg()
{
    m_path = "E:/DCE/QuoteE";
    m_fileNameQuoteE = "QuoteE";
    m_fileNameTrans = "Trans";
    m_fileNameKline1 = "Kline1";
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
            printf("open file[%s] failed.\n", fileOutQuoteE);
        }
        memset(szMsg, 0, sizeof(szMsg));
        snprintf(szMsg, sizeof(szMsg), "localtime,symbol,"
            "bidpx0,bidvol0,bidpx1,bidvol1,bidpx2,bidvol2,bidpx3,bidvol3,bidpx4,bidvol4,"
            "bidpx5,bidvol5,bidpx6,bidvol6,bidpx7,bidvol7,bidpx8,bidvol8,bidpx9,bidvol9,"
            "bidpx10,bidvol10,bidpx11,bidvol11,bidpx12,bidvol12,bidpx13,bidvol13,bidpx14,bidvol14,"
            "bidpx15,bidvol15,bidpx16,bidvol16,bidpx17,bidvol17,bidpx18,bidvol18,bidpx19,bidvol19,"
            "askpx0,askvol0,askpx1,askvol1,askpx2,askvol2,askpx3,askvol3,askpx4,askvol4,"
            "askpx5,askvol5,askpx6,askvol6,askpx7,askvol7,askpx8,askvol8,askpx9,askvol9,"
            "askpx10,askvol10,askpx11,askvol11,askpx12,askvol12,askpx13,askvol13,askpx14,askvol14,"
            "askpx15,askvol15,askpx16,askvol16,askpx17,askvol17,askpx18,askvol18,askpx19,askvol19\n");
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
            printf("open file[%s] failed.\n", fileOutTrans);
        }
        memset(szMsg, 0, sizeof(szMsg));
        snprintf(szMsg, sizeof(szMsg), "eventtime,localtime,tradetime,symbol,filledpx,filledqty,make,tradeid,bidid,askid\n");
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
            printf("open file[%s] failed.\n", fileOutKline1);
        }
        memset(szMsg, 0, sizeof(szMsg));
        snprintf(szMsg, sizeof(szMsg), "eventtime,localtime,starttime,endtime,symbol,cumqty,cumamt,activeqty,activeamt,openpx,closepx,highpx,lowpx,isrunend,num,interval,firstid,lastid\n");
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
    ret = m_api->Connect("8.219.188.209", 6379, "080814080", 3);
    ret = m_api->RegisterSpi(this);
    ret = m_api->Psubscrib("QuoteE");
    ret = m_api->Psubscrib("Trans");
    ret = m_api->Psubscrib("KStreams");
}

void HandleMsg::StartThread()
{
    m_quoteE_th = std::thread(&HandleMsg::HandleQuoteE, this);
    m_trans_th = std::thread(&HandleMsg::HandleTrans, this);
    m_kline1_th = std::thread(&HandleMsg::HandleKline1, this);
}

void HandleMsg::OnRecvQuoteE(const char * msg, int len)
{
    //printf("msg=%s,len=%d\n", msg, len);
    if (len > 800)
    {
        PushQuoteE(msg);
    }
}

void HandleMsg::OnRecvTrans(const char * msg, int len)
{
    //printf("msg=%s,len=%d\n", msg, len);
    PushTrans(msg);
}

void HandleMsg::OnRecvKline(const char * msg, int len, int type)
{
    //printf("msg=%s,type=%d,len=%d\n", msg, type, len);
    if (type == 1)
    {
        PushKline1(msg);
    }
}

void HandleMsg::OnRecvOtherMsg(const char * msg, int len)
{
    printf("msg=%s,len=%d\n", msg, len);
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
            printf("m_fpQuoteE open file failed.\n");
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
            //处理字符串，落盘
            //b'{"lastUpdateId": 22902310367, "bids": [["21435.80000000", "0.16895000"], ["21434.97000000", "0.56736000"], ["21434.96000000", "0.25006000"], ["21434.95000000", "0.62500000"], ["21434.93000000", "0.07000000"]], 
            //"asks": [["21437.50000000", "0.02211000"], ["21437.60000000", "0.00844000"], ["21437.90000000", "0.00688000"], ["21437.91000000", "0.02000000"], ["21438.07000000", "0.01999000"]]}'
            timeb t;
            ftime(&t);
            QuoteE quote;
            memset(&quote, 0, sizeof(QuoteE));
            quote.localtime = t.time * 1000 + t.millitm;
            strncpy(quote.symbol, "btcusdt",sizeof(quote.symbol));
            const char* pbids = "bids";
            auto pindex = strstr(msg.c_str(), pbids);
            if (pindex != nullptr)
            {
                pindex += 10;
                quote.bidpx[0] = atof(pindex);
                pindex = strchr(pindex, ',');
                pindex += 3;
                quote.bidvol[0] = atof(pindex);
                pindex = strchr(pindex, ',');
                pindex += 4;
                quote.bidpx[1] = atof(pindex);
                pindex = strchr(pindex, ',');
                pindex += 3;
                quote.bidvol[1] = atof(pindex);
                pindex = strchr(pindex, ',');
                pindex += 4;
                quote.bidpx[2] = atof(pindex);
                pindex = strchr(pindex, ',');
                pindex += 3;
                quote.bidvol[2] = atof(pindex);
                pindex = strchr(pindex, ',');
                pindex += 4;
                quote.bidpx[3] = atof(pindex);
                pindex = strchr(pindex, ',');
                pindex += 3;
                quote.bidvol[3] = atof(pindex);
                pindex = strchr(pindex, ',');
                pindex += 4;
                quote.bidpx[4] = atof(pindex);
                pindex = strchr(pindex, ',');
                pindex += 3;
                quote.bidvol[4] = atof(pindex);
                pindex = strchr(pindex, ',');
                pindex += 4;
                quote.bidpx[5] = atof(pindex);
                pindex = strchr(pindex, ',');
                pindex += 3;
                quote.bidvol[5] = atof(pindex);
                pindex = strchr(pindex, ',');
                pindex += 4;
                quote.bidpx[6] = atof(pindex);
                pindex = strchr(pindex, ',');
                pindex += 3;
                quote.bidvol[6] = atof(pindex);
                pindex = strchr(pindex, ',');
                pindex += 4;
                quote.bidpx[7] = atof(pindex);
                pindex = strchr(pindex, ',');
                pindex += 3;
                quote.bidvol[7] = atof(pindex);
                pindex = strchr(pindex, ',');
                pindex += 4;
                quote.bidpx[8] = atof(pindex);
                pindex = strchr(pindex, ',');
                pindex += 3;
                quote.bidvol[8] = atof(pindex);
                pindex = strchr(pindex, ',');
                pindex += 4;
                quote.bidpx[9] = atof(pindex);
                pindex = strchr(pindex, ',');
                pindex += 3;
                quote.bidvol[9] = atof(pindex);
                pindex = strchr(pindex, ',');
                pindex += 4;
                quote.bidpx[10] = atof(pindex);
                pindex = strchr(pindex, ',');
                pindex += 3;
                quote.bidvol[10] = atof(pindex);
                pindex = strchr(pindex, ',');
                pindex += 4;
                quote.bidpx[11] = atof(pindex);
                pindex = strchr(pindex, ',');
                pindex += 3;
                quote.bidvol[11] = atof(pindex);
                pindex = strchr(pindex, ',');
                pindex += 4;
                quote.bidpx[12] = atof(pindex);
                pindex = strchr(pindex, ',');
                pindex += 3;
                quote.bidvol[12] = atof(pindex);
                pindex = strchr(pindex, ',');
                pindex += 4;
                quote.bidpx[13] = atof(pindex);
                pindex = strchr(pindex, ',');
                pindex += 3;
                quote.bidvol[13] = atof(pindex);
                pindex = strchr(pindex, ',');
                pindex += 4;
                quote.bidpx[14] = atof(pindex);
                pindex = strchr(pindex, ',');
                pindex += 3;
                quote.bidvol[14] = atof(pindex);
                pindex = strchr(pindex, ',');
                pindex += 4;
                quote.bidpx[15] = atof(pindex);
                pindex = strchr(pindex, ',');
                pindex += 3;
                quote.bidvol[15] = atof(pindex);
                pindex = strchr(pindex, ',');
                pindex += 4;
                quote.bidpx[16] = atof(pindex);
                pindex = strchr(pindex, ',');
                pindex += 3;
                quote.bidvol[16] = atof(pindex);
                pindex = strchr(pindex, ',');
                pindex += 4;
                quote.bidpx[17] = atof(pindex);
                pindex = strchr(pindex, ',');
                pindex += 3;
                quote.bidvol[17] = atof(pindex);
                pindex = strchr(pindex, ',');
                pindex += 4;
                quote.bidpx[18] = atof(pindex);
                pindex = strchr(pindex, ',');
                pindex += 3;
                quote.bidvol[18] = atof(pindex);
                pindex = strchr(pindex, ',');
                pindex += 4;
                quote.bidpx[19] = atof(pindex);
                pindex = strchr(pindex, ',');
                pindex += 3;
                quote.bidvol[19] = atof(pindex);
            }
            else
            {
                //print
                continue;
            }
            const char* pasks = "asks";
            auto pindex2 = strstr(msg.c_str(), pasks);
            if (pindex2 != nullptr)
            {
                pindex2 += 10;
                quote.askpx[0] = atof(pindex2);
                pindex2 = strchr(pindex2, ',');
                pindex2 += 3;
                quote.askvol[0] = atof(pindex2);
                pindex2 = strchr(pindex2, ',');
                pindex2 += 4;
                quote.askpx[1] = atof(pindex2);
                pindex2 = strchr(pindex2, ',');
                pindex2 += 3;
                quote.askvol[1] = atof(pindex2);
                pindex2 = strchr(pindex2, ',');
                pindex2 += 4;
                quote.askpx[2] = atof(pindex2);
                pindex2 = strchr(pindex2, ',');
                pindex2 += 3;
                quote.askvol[2] = atof(pindex2);
                pindex2 = strchr(pindex2, ',');
                pindex2 += 4;
                quote.askpx[3] = atof(pindex2);
                pindex2 = strchr(pindex2, ',');
                pindex2 += 3;
                quote.askvol[3] = atof(pindex2);
                pindex2 = strchr(pindex2, ',');
                pindex2 += 4;
                quote.askpx[4] = atof(pindex2);
                pindex2 = strchr(pindex2, ',');
                pindex2 += 3;
                quote.askvol[4] = atof(pindex2);
                pindex2 = strchr(pindex2, ',');
                pindex2 += 4;
                quote.askpx[5] = atof(pindex2);
                pindex2 = strchr(pindex2, ',');
                pindex2 += 3;
                quote.askvol[5] = atof(pindex2);
                pindex2 = strchr(pindex2, ',');
                pindex2 += 4;
                quote.askpx[6] = atof(pindex2);
                pindex2 = strchr(pindex2, ',');
                pindex2 += 3;
                quote.askvol[6] = atof(pindex2);
                pindex2 = strchr(pindex2, ',');
                pindex2 += 4;
                quote.askpx[7] = atof(pindex2);
                pindex2 = strchr(pindex2, ',');
                pindex2 += 3;
                quote.askvol[7] = atof(pindex2);
                pindex2 = strchr(pindex2, ',');
                pindex2 += 4;
                quote.askpx[8] = atof(pindex2);
                pindex2 = strchr(pindex2, ',');
                pindex2 += 3;
                quote.askvol[8] = atof(pindex2);
                pindex2 = strchr(pindex2, ',');
                pindex2 += 4;
                quote.askpx[9] = atof(pindex2);
                pindex2 = strchr(pindex2, ',');
                pindex2 += 3;
                quote.askvol[9] = atof(pindex2);
                pindex2 = strchr(pindex2, ',');
                pindex2 += 4;
                quote.askpx[10] = atof(pindex2);
                pindex2 = strchr(pindex2, ',');
                pindex2 += 3;
                quote.askvol[10] = atof(pindex2);
                pindex2 = strchr(pindex2, ',');
                pindex2 += 4;
                quote.askpx[11] = atof(pindex2);
                pindex2 = strchr(pindex2, ',');
                pindex2 += 3;
                quote.askvol[11] = atof(pindex2);
                pindex2 = strchr(pindex2, ',');
                pindex2 += 4;
                quote.askpx[12] = atof(pindex2);
                pindex2 = strchr(pindex2, ',');
                pindex2 += 3;
                quote.askvol[12] = atof(pindex2);
                pindex2 = strchr(pindex2, ',');
                pindex2 += 4;
                quote.askpx[13] = atof(pindex2);
                pindex2 = strchr(pindex2, ',');
                pindex2 += 3;
                quote.askvol[13] = atof(pindex2);
                pindex2 = strchr(pindex2, ',');
                pindex2 += 4;
                quote.askpx[14] = atof(pindex2);
                pindex2 = strchr(pindex2, ',');
                pindex2 += 3;
                quote.askvol[14] = atof(pindex2);
                pindex2 = strchr(pindex2, ',');
                pindex2 += 4;
                quote.askpx[15] = atof(pindex2);
                pindex2 = strchr(pindex2, ',');
                pindex2 += 3;
                quote.askvol[15] = atof(pindex2);
                pindex2 = strchr(pindex2, ',');
                pindex2 += 4;
                quote.askpx[16] = atof(pindex2);
                pindex2 = strchr(pindex2, ',');
                pindex2 += 3;
                quote.askvol[16] = atof(pindex2);
                pindex2 = strchr(pindex2, ',');
                pindex2 += 4;
                quote.askpx[17] = atof(pindex2);
                pindex2 = strchr(pindex2, ',');
                pindex2 += 3;
                quote.askvol[17] = atof(pindex2);
                pindex2 = strchr(pindex2, ',');
                pindex2 += 4;
                quote.askpx[18] = atof(pindex2);
                pindex2 = strchr(pindex2, ',');
                pindex2 += 3;
                quote.askvol[18] = atof(pindex2);
                pindex2 = strchr(pindex2, ',');
                pindex2 += 4;
                quote.askpx[19] = atof(pindex2);
                pindex2 = strchr(pindex2, ',');
                pindex2 += 3;
                quote.askvol[19] = atof(pindex2);
            }
            else
            {
                //print
                continue;
            }
            char szMsg[2048];
            snprintf(szMsg,sizeof(szMsg),"%lld,%s,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,"
                "%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,"
                "%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,"
                "%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f\n",
                quote.localtime,quote.symbol,
                quote.bidpx[0],quote.bidvol[0], quote.bidpx[1], quote.bidvol[1], quote.bidpx[2], quote.bidvol[2], quote.bidpx[3], quote.bidvol[3], quote.bidpx[4], quote.bidvol[4],
                quote.bidpx[5], quote.bidvol[5], quote.bidpx[6], quote.bidvol[6], quote.bidpx[7], quote.bidvol[7], quote.bidpx[8], quote.bidvol[8], quote.bidpx[9], quote.bidvol[9], 
                quote.bidpx[10], quote.bidvol[10], quote.bidpx[11], quote.bidvol[11], quote.bidpx[12], quote.bidvol[12], quote.bidpx[13], quote.bidvol[13], quote.bidpx[14], quote.bidvol[14], 
                quote.bidpx[15], quote.bidvol[15], quote.bidpx[16], quote.bidvol[16], quote.bidpx[17], quote.bidvol[17], quote.bidpx[18], quote.bidvol[18], quote.bidpx[19], quote.bidvol[19], 
                quote.askpx[0], quote.askvol[0], quote.askpx[1], quote.askvol[1], quote.askpx[2], quote.askvol[2], quote.askpx[3], quote.askvol[3], quote.askpx[4], quote.askvol[4],
                quote.askpx[5], quote.askvol[5], quote.askpx[6], quote.askvol[6], quote.askpx[7], quote.askvol[7], quote.askpx[8], quote.askvol[8], quote.askpx[9], quote.askvol[9],
                quote.askpx[10], quote.askvol[10], quote.askpx[11], quote.askvol[11], quote.askpx[12], quote.askvol[12], quote.askpx[13], quote.askvol[13], quote.askpx[14], quote.askvol[14],
                quote.askpx[15], quote.askvol[15], quote.askpx[16], quote.askvol[16], quote.askpx[17], quote.askvol[17], quote.askpx[18], quote.askvol[18], quote.askpx[19], quote.askvol[19]);
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

void HandleMsg::HandleTrans()
{
    while (true)
    {
        if (m_fpTrans == NULL)
        {
            printf("m_fpTrans open file failed.\n");
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            continue;
        }
        std::string msg;
        msg.clear();
        {
            std::unique_lock<std::mutex> lck(m_trans_lock);
            if (m_quoteE_cv.wait_for(lck, std::chrono::milliseconds(10), [this] {return !this->m_trans_que.empty(); }))
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
            //处理字符串，落盘
            //b'{"e": "trade", "E": 1661095114826, "s": "BTCUSDT", "t": 1673117582, "p": "21420.48000000", 
            //"q": "0.11250000", "b": 12688310805, "a": 12688310802, "T": 1661095114826, "m": false, "M": true}'
            timeb t;
            ftime(&t);
            Trans trans;
            memset(&trans, 0, sizeof(Trans));
            trans.localtime = t.time * 1000 + t.millitm;
            strncpy(trans.symbol, "btcusdt", sizeof(trans.symbol));
            const char* ptrade = "trade";
            auto pindex = strstr(msg.c_str(), ptrade);
            if (pindex != nullptr)
            {
                pindex = strchr(pindex, 'E');
                trans.eventtime = atoll(pindex + 4);
                pindex = strchr(pindex, 't');
                trans.tradeid = atoll(pindex + 4);
                pindex = strchr(pindex, 'p');
                trans.filledpx = atof(pindex + 5);
                pindex = strchr(pindex, 'q');
                trans.filledqty = atof(pindex + 5);
                pindex = strchr(pindex, 'b');
                trans.bidid = atoll(pindex + 4);
                pindex = strchr(pindex, 'a');
                trans.askid = atoll(pindex + 4);
                pindex = strchr(pindex, 'T');
                trans.tradetime = atoll(pindex + 4);
                pindex = strchr(pindex, 'm');
                pindex += 4;
                if (pindex[0] == 'f')
                {
                    trans.make = false;
                }
                else
                {
                    trans.make = true;
                }
            }
            else
            {
                //print
                continue;
            }
            char szMsg[512];
            snprintf(szMsg, sizeof(szMsg), "%lld,%lld,%lld,%s,%f,%f,%d,%lld,%lld,%lld\n",
                trans.eventtime, trans.localtime, trans.tradetime, trans.symbol, trans.filledpx, trans.filledqty,
                trans.make, trans.tradeid, trans.bidid, trans.askid);
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

void HandleMsg::HandleKline1()
{
    while (true)
    {
        if (m_fpKline1 == NULL)
        {
            printf("m_fpKline1 open file failed.\n");
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            continue;
        }
        std::string msg;
        msg.clear();
        {
            std::unique_lock<std::mutex> lck(m_kline1_lock);
            if (m_quoteE_cv.wait_for(lck, std::chrono::milliseconds(10), [this] {return !this->m_kline1_que.empty(); }))
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
            //处理字符串，落盘
            //b'{"e": "kline", "E": 1661098020003, "s": "BTCUSDT", "k": {"t": 1661097960000, "T": 1661098019999, "s": "BTCUSDT", "i": "1m",
            //"f": 1673236693, "L": 1673238653, "o": "21427.44000000", "c": "21437.71000000", "h": "21440.65000000", "l": "21427.39000000",
            //"v": "45.51908000", "n": 1961, "x": true, "q": "975616.85525710", "V": "27.68864000", "Q": "593474.74260060", "B": "0"}}'
            timeb t;
            ftime(&t);
            Kline1 line;
            memset(&line, 0, sizeof(Kline1));
            line.localtime = t.time * 1000 + t.millitm;
            strncpy(line.symbol, "btcusdt", sizeof(line.symbol));
            const char* pline = "kline";
            auto pindex = strstr(msg.c_str(), pline);
            if (pindex != nullptr)
            {
                pindex = strchr(pindex, 'E');
                line.eventtime = atoll(pindex + 4);
                pindex = strchr(pindex, 't');
                line.starttime = atoll(pindex + 4);
                pindex = strchr(pindex, 'T');//检查这个数据是否正确
                line.endtime = atoll(pindex + 4);
                pindex = strchr(pindex, 'i');
                strncpy(line.interval, pindex + 5, 2);
                pindex = strchr(pindex, 'f');
                line.firstid = atoll(pindex + 4);
                pindex = strchr(pindex, 'L');
                line.lastid = atoll(pindex + 4);
                pindex = strchr(pindex, 'o');
                line.openpx = atof(pindex + 5);
                pindex = strchr(pindex, 'c');
                line.closepx = atof(pindex + 5);
                pindex = strchr(pindex, 'h');
                line.highpx = atof(pindex + 5);
                pindex = strchr(pindex, 'l');
                line.lowpx = atof(pindex + 5);
                pindex = strchr(pindex, 'v');
                line.cumqty = atof(pindex + 5);
                pindex = strchr(pindex, 'n');
                line.num = atoll(pindex + 4);
                pindex = strchr(pindex, 'x');
                pindex += 4;
                if (pindex[0] == 'f')
                {
                    line.isrunend = false;
                }
                else
                {
                    line.isrunend = true;
                }
                pindex = strchr(pindex, 'q');
                line.cumamt = atof(pindex + 5);
                pindex = strchr(pindex, 'V');
                line.activeqty = atof(pindex + 5);
                pindex = strchr(pindex, 'Q');
                line.activeamt = atof(pindex + 5);
            }
            else
            {
                //print
                continue;
            }
            char szMsg[512];
            snprintf(szMsg, sizeof(szMsg), "%lld,%lld,%lld,%lld,%s,%f,%f,%f,%f,%f,%f,%f,%f,%d,%lld,%s,%lld,%lld\n",
                line.eventtime, line.localtime, line.starttime, line.endtime, line.symbol, line.cumqty, line.cumamt, line.activeqty, line.activeamt,
                line.openpx, line.closepx, line.highpx, line.lowpx, line.isrunend, line.num, line.interval, line.firstid, line.lastid);
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

