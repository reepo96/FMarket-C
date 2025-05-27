// FuturesMarket.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <winbase.h>

#include "MdSpiHandler.h"
#include "LogControl.h"
#include "Utility.h"

#define CFG_INI_FILE "./config.ini"

HANDLE xinhao = CreateEvent(NULL, false, false, NULL);
LogControl	g_log;
int			g_logLevel = 4;

int main(int argc, char* argv[])
{
	char szFrontMdAddr[100] = {0};
	Utility::CIniMgr ini;

	g_log.WriteLog2File(LogControl::LINFO,"��ʼ��������...");

	ini.InitFile(CFG_INI_FILE);
	ini.GetProfileStr("config","FrontMdAddr","tcp://127.0.0.1:41213",szFrontMdAddr,sizeof(szFrontMdAddr),NULL);
  	ini.CloseFile();

	g_log.WriteLog2File(LogControl::LINFO,"����FrontMdAddr:%s",szFrontMdAddr);

	CThostFtdcMdApi  *pUserMdApi =	CThostFtdcMdApi::CreateFtdcMdApi(".\\flow\\", false, false);
	CMdSpiHandler handler(pUserMdApi);
	pUserMdApi->RegisterSpi(&handler);
	pUserMdApi->RegisterFront(szFrontMdAddr);
	pUserMdApi->Init();

	WaitForSingleObject(xinhao, INFINITE);
	handler.ReqUserLogin();
	WaitForSingleObject(xinhao, INFINITE);

	//��������
	handler.SubscribeMarketData();

	while( 1 )
	{
		::Sleep(1000);
	}


	return 0;
}

