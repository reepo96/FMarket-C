// FMarket.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <winbase.h>

#include "MdSpiHandler.h"
#include "TraderHandler.h"
#include "LogControl.h"
#include "Utility.h"

#define CFG_INI_FILE "./config.ini"

HANDLE xinhao = CreateEvent(NULL, false, false, NULL);
HANDLE g_hTraderEvent = CreateEvent(NULL, false, false, NULL);
LogControl	g_log;
int			g_logLevel = 4;

int main()
{
	char szFrontMdAddr[100] = { 0 };
	char szFrontAddr[100] = { 0 };
	Utility::CIniMgr ini;

	std::map<std::string, std::vector<std::string>>	product_Instrument_list;//产品对应的合约列表

  	g_log.WriteLog2File(LogControl::LINFO, "开始连接行情...");

	ini.InitFile(CFG_INI_FILE);
	ini.GetProfileStr("config", "FrontMdAddr", "tcp://127.0.0.1:41213", szFrontMdAddr, sizeof(szFrontMdAddr), NULL);
	ini.GetProfileStr("config", "FrontAddr", "tcp://127.0.0.1:41213", szFrontAddr, sizeof(szFrontAddr), NULL);
	ini.CloseFile();

	//暂时不用查询合约信息
	//登录交易接口
	/*CThostFtdcTraderApi *pUTraderApi = CThostFtdcTraderApi::CreateFtdcTraderApi(".\\flow\\");
	CTraderHandler trader(pUTraderApi);
	pUTraderApi->RegisterSpi(&trader);
	pUTraderApi->SubscribePrivateTopic(THOST_TERT_QUICK);
	pUTraderApi->SubscribePublicTopic(THOST_TERT_QUICK);
	pUTraderApi->RegisterFront(szFrontAddr);
	pUTraderApi->Init();
	WaitForSingleObject(g_hTraderEvent, INFINITE);

	//直连模式
	trader.ReqAuthenticate();
	WaitForSingleObject(g_hTraderEvent, INFINITE);
	trader.ReqUserLogin();
	WaitForSingleObject(g_hTraderEvent, INFINITE);

	//查询合约信息
	trader.ReqQryInstrument(&product_Instrument_list);

	//查询完合约信息后退出登录
	WaitForSingleObject(g_hTraderEvent, INFINITE);
	trader.ReqUserLogout();
	WaitForSingleObject(g_hTraderEvent, INFINITE);

	g_log.WriteLog2File(LogControl::LINFO, "连接FrontMdAddr:%s", szFrontMdAddr);*/

	//登录行情接口
	CThostFtdcMdApi  *pUserMdApi = CThostFtdcMdApi::CreateFtdcMdApi(".\\flow\\", false, false);
	CMdSpiHandler handler(pUserMdApi);
	pUserMdApi->RegisterSpi(&handler);
	pUserMdApi->RegisterFront(szFrontMdAddr);
	pUserMdApi->Init();

	WaitForSingleObject(xinhao, INFINITE);
	handler.ReqUserLogin();
	WaitForSingleObject(xinhao, INFINITE);

	//订阅行情
	handler.SubscribeMarketData();

	while (1)
	{
		::Sleep(1000);
	}


	return 0;
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门提示: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
