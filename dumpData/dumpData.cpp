// dumpData.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include "HandleMsg.h"
#include "toolGeneral.hpp"

#include <iostream>
#include <thread>
int main()
{
    int date = ToolGen::currentDate();
    int time = 0;
    HandleMsg msg("dumpdata.ini");
    msg.StartThread();
    msg.Init(date);
    msg.Start();
    //判断时间，当发现跨天，需要换文件存储，实现24小时自动化切换。
    while (true)
    {
        time = ToolGen::currentTime();
        if (time > 235940000 )
        {
            std::this_thread::sleep_for(std::chrono::seconds(20));
            date = ToolGen::currentDate();
            msg.CloseFile();
            msg.Init(date);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    return 0;
}

