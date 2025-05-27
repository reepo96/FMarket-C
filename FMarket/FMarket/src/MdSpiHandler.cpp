#include "MdSpiHandler.h"
#include "LogControl.h"
#include "Utility.h"
#include "MysqlDB.h"

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <winbase.h>

extern HANDLE xinhao;
extern LogControl	g_log;
#define CFG_INI_FILE "./config.ini"
#define	INSTRUMENT_FILE "./allinstrument.txt"
#define	PRODUCT_FILE "./allproduct.txt"

//K线间隔
const int g_barlevel[] = { 1 }; //{ 1,5,15,30,60,1440 };

CMdSpiHandler::CMdSpiHandler(CThostFtdcMdApi *pUserApi) : m_pUserMdApi(pUserApi)
{
	std::string sInstrument = "";//new char[1024*1024];
	std::string sProduct = "";
	int line_len = 1024 * 1024;
	char *szLine = new char[line_len];// [1024] = { 0 };
	memset(szLine, 0, line_len);
	char szTableName[30] = { 0 };
	//int iLineLen = 0;
	//int iTotLen = 0;
	Utility::CIniMgr ini;
	ini.InitFile(CFG_INI_FILE);

	memset(m_chBrokerID,0,sizeof(m_chBrokerID));
	memset(m_chUserID,0,sizeof(m_chUserID));
	memset(m_chPassword,0,sizeof(m_chPassword));
	memset(m_chExchangeID,0,sizeof(m_chExchangeID));

	ini.GetProfileStr("config","BrokerID","9999",m_chBrokerID,sizeof(m_chBrokerID),NULL);
	ini.GetProfileStr("config","UserID","00001",m_chUserID,sizeof(m_chUserID),NULL);
	ini.GetProfileStr("config","Password","",m_chPassword,sizeof(m_chPassword),NULL);
	ini.GetProfileStr("config","ExchangeID","SHFE",m_chExchangeID,sizeof(m_chExchangeID),NULL);

	//初始化要订阅的合约号
	FILE *fp = fopen(INSTRUMENT_FILE,"r");
	if( fp )
	{
		while( NULL != fgets(szLine, line_len,fp) )
		{
			/*iLineLen = strlen(szLine);
			iTotLen += iLineLen;
			if( iTotLen >= 1024*1024 )
				break;

			strcat(pInstrument,szLine);*/
			sInstrument += std::string(szLine);
		}

		md_InstrumentID = Utility::split(sInstrument,":");

		fclose(fp);
		fp = NULL;
	}

	//组合合约初始化
	std::vector<std::string>::iterator Instrumentit = md_InstrumentID.begin();
	std::map<std::string, WholeComInstrument *>::iterator ComInstIt;

	char szProduct[100] = { 0 };
	for(; Instrumentit != md_InstrumentID.end(); Instrumentit++)
	{
		strncpy(szProduct, (*Instrumentit).c_str(), sizeof(szProduct)-1);
		for (int i = 0; i < strlen(szProduct); i++)
		{
			if (szProduct[i] >= 30 && szProduct[i] <= 39)
			{
				szProduct[i] = 0;
				ComInstIt = m_ProductComInstrument.find(std::string(szProduct));
				if(ComInstIt == m_ProductComInstrument.end())
				{
					WholeComInstrument *pComInstrument = new WholeComInstrument(std::string(szProduct), "000");
					pComInstrument->AddInstrument((*Instrumentit));
					m_ProductComInstrument.insert(std::map<std::string, WholeComInstrument *>::value_type(std::string(szProduct), pComInstrument));
				}
				else
				{
					(*ComInstIt).second->AddInstrument((*Instrumentit));
				}
			}
		}
	}

	//初始化要订阅的产品
	fp = fopen(PRODUCT_FILE, "r");
	if (fp)
	{
		while (NULL != fgets(szLine, line_len, fp))
		{
			sProduct += std::string(szLine);
		}

		m_sProductID = Utility::split(sProduct, ":");

		fclose(fp);
		fp = NULL;
	}
	delete[]szLine;
	szLine = NULL;

	m_nCurrentMin = 0;
	m_pDBApi = new MysqlDB();
	m_barmgr.SetDBApi(m_pDBApi);

	//建表
	std::vector<std::string>::iterator it = m_sProductID.begin();
	for(;it!= m_sProductID.end();it++)
	{ 
		for (int i = 0; i < sizeof(g_barlevel) / sizeof(int); i++)
		{
			//按照产品名和K线间隔创建表，即同种产品同类型的K线不同合约数据存放同一张表
			sprintf(szTableName, "t_market_data_%s%dmin", (*it).c_str(), g_barlevel[i]);
			m_pDBApi->CreateTable(szTableName);

			sprintf(szTableName, "t_market_data_%s%dmin_mem", (*it).c_str(), g_barlevel[i]);
			m_pDBApi->CreateMemTable(szTableName);
		}
	}
}

void CMdSpiHandler::deletemyself()
{
	delete this;
}

void CMdSpiHandler::OnFrontConnected()
{
	g_log.WriteLog2File(LogControl::LINFO,"Enter OnFrontConnected");
	SetEvent(xinhao);
	//ReqUserLogin();
}

void CMdSpiHandler::RegisterFensUserInfo()
{
	g_log.WriteLog2File(LogControl::LINFO,"Enter RegisterFensUserInfo,brokerid:%s,userid:%s",m_chBrokerID,m_chUserID);
	
	CThostFtdcFensUserInfoField pFens = { 0 };
	strcpy_s(pFens.BrokerID, m_chBrokerID);
	strcpy_s(pFens.UserID, m_chUserID);
	pFens.LoginMode = THOST_FTDC_LM_Trade;
	m_pUserMdApi->RegisterFensUserInfo(&pFens);
}

void CMdSpiHandler::ReqUserLogin()
{
	g_log.WriteLog2File(LogControl::LINFO,"Enter ReqUserLogin,brokerid:%s",m_chBrokerID);

	CThostFtdcReqUserLoginField reqUserLogin = { 0 };
	strcpy_s(reqUserLogin.BrokerID, m_chBrokerID);
	//strcpy_s(reqUserLogin.UserID, m_chUserID);
	//strcpy_s(reqUserLogin.UserID, m_chPassword);
	int num = m_pUserMdApi->ReqUserLogin(&reqUserLogin, 2);
	g_log.WriteLog2File(LogControl::LINFO,"login num = %d",num);
}

void CMdSpiHandler::ReqUserLogout()
{
	CThostFtdcUserLogoutField a = { 0 };
	strcpy_s(a.BrokerID, m_chBrokerID);
	strcpy_s(a.UserID, m_chUserID);
	int num = m_pUserMdApi->ReqUserLogout(&a, 1);
	g_log.WriteLog2File(LogControl::LINFO,
		"ReqUserLogout,brokerid:%s,UserID:%s,Logoutnum:%d",m_chBrokerID,m_chUserID,num);
}

void CMdSpiHandler::OnHeartBeatWarning(int nTimeLapse)
{
	g_log.WriteLog2File(LogControl::LWARNING,"Enter OnHeartBeatWarning,nTimeLapse:%d",nTimeLapse);
}

// 当客户端与交易托管系统通信连接断开时，该方法被调用
void CMdSpiHandler::OnFrontDisconnected(int nReason)
{
	// 当发生这个情况后，API会自动重新连接，客户端可不做处理
	g_log.WriteLog2File(LogControl::LWARNING,"Enter OnFrontDisconnected,nReason:%d",nReason);
}

// 当客户端发出登录请求之后，该方法会被调用，通知客户端登录是否成功
void CMdSpiHandler::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
										   CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	g_log.WriteLog2File(LogControl::LINFO,"Enter OnRspUserLogin");
	if (pRspUserLogin)
	{
		g_log.WriteLog2File(LogControl::LINFO,"\tTradingDay [%s]\n\tLoginTime [%s]\n\tBrokerID [%s]\n\
			\tUserID [%s]\n\tSystemName [%s]\n\tMaxOrderRef [%s]\n\tSHFETime [%s]\n\tDCETime [%s]\n\
			\tCZCETime [%s]\n\tFFEXTime [%s]\n\tINETime [%s]\n\tFrontID [%d]\n\tSessionID [%d]", 
			pRspUserLogin->TradingDay, pRspUserLogin->LoginTime, pRspUserLogin->BrokerID, pRspUserLogin->UserID,
			pRspUserLogin->SystemName, pRspUserLogin->MaxOrderRef, pRspUserLogin->SHFETime, pRspUserLogin->DCETime,
			pRspUserLogin->CZCETime,pRspUserLogin->FFEXTime,pRspUserLogin->INETime,pRspUserLogin->FrontID,pRspUserLogin->SessionID);
	}
	if (pRspInfo)
	{
		g_log.WriteLog2File(LogControl::LINFO,"\tErrorMsg [%s]\n\tErrorID [%d]", 
			pRspInfo->ErrorMsg,pRspInfo->ErrorID);
	}
	g_log.WriteLog2File(LogControl::LINFO,"\tnRequestID [%d]\n\tbIsLast [%d]", nRequestID,bIsLast);

	if (pRspInfo->ErrorID != 0) {
		// 端登失败，客户端需进行错误处理
		g_log.WriteLog2File(LogControl::LERROR,"Failed to login, errorcode=%d errormsg=%s requestid=%d chain = %d",
			pRspInfo->ErrorID, pRspInfo->ErrorMsg, nRequestID, bIsLast);

		exit(-1);
	}
	SetEvent(xinhao);
}

//登出请求响应
void CMdSpiHandler::OnRspUserLogout(CThostFtdcUserLogoutField* pUserLogout, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast)
{
	char szInf[1024] = {0};
	sprintf(szInf,"Enter OnRspUserLogout:RequestID [%d],IsLast [%d],",nRequestID,bIsLast);
	if (pUserLogout)
	{
		sprintf(szInf,"%s,BrokerID [%s],UserID [%s]",szInf,pUserLogout->BrokerID,pUserLogout->UserID);
	}
	if (pRspInfo)
	{
		sprintf(szInf,"%s,ErrorMsg [%s],ErrorID [%s]",szInf,pRspInfo->ErrorMsg,pRspInfo->ErrorMsg);
	}

	g_log.WriteLog2File(LogControl::LINFO,szInf);
	
	SetEvent(xinhao);
	//pUserApi->Release();
}

void CMdSpiHandler::UnSubscribeMarketData()
{
	int md_num = 0;
	char **ppInstrumentID = new char*[5000];
	for (int count1 = 0; count1 <= md_InstrumentID.size() / 500; count1++)
	{
		if (count1 < md_InstrumentID.size() / 500)
		{
			int a = 0;
			for (a; a < 500; a++)
			{
				ppInstrumentID[a] = const_cast<char *>(md_InstrumentID.at(md_num).c_str());
				md_num++;
			}
			int result = m_pUserMdApi->UnSubscribeMarketData(ppInstrumentID, a);
			if(result == 0)
			{
				g_log.WriteLog2File(LogControl::LDEBUG,"退订行情请求1......发送成功");
			}
			else
			{
				g_log.WriteLog2File(LogControl::LWARNING,"退订行情请求1......发送失败，错误序号=[%d]", result);
			}
		}
		else if (count1 == md_InstrumentID.size() / 500)
		{
			int count2 = 0;
			for (count2; count2 < md_InstrumentID.size() % 500; count2++)
			{
				ppInstrumentID[count2] = const_cast<char *>(md_InstrumentID.at(md_num).c_str());
				md_num++;
			}
			int result = m_pUserMdApi->UnSubscribeMarketData(ppInstrumentID, count2);
			if(result == 0)
			{
				g_log.WriteLog2File(LogControl::LDEBUG,"退订行情请求2......发送成功");
			}
			else
			{
				g_log.WriteLog2File(LogControl::LWARNING,"退订行情请求2......发送失败，错误序号=[%d]", result);
			}
		}
	}
}

void CMdSpiHandler::OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	char szInf[1024] = {0};
	sprintf(szInf,"Enter OnRspUnSubMarketData:RequestID [%d],IsLast [%d],",nRequestID,bIsLast);

	if (pSpecificInstrument)
	{
		sprintf(szInf,"%s InstrumentID [%s]", szInf,pSpecificInstrument->InstrumentID);
	}
	if (pRspInfo)
	{
		sprintf(szInf,"%s ErrorMsg [%s],ErrorID [%d]", szInf,pRspInfo->ErrorMsg,pRspInfo->ErrorID);
	}

	g_log.WriteLog2File(LogControl::LINFO,szInf);

	if (bIsLast)
	{
		SetEvent(xinhao);
	}
};

void CMdSpiHandler::SubscribeMarketData()//ê?DD?é
{
	int md_num = 0;
	char **ppInstrumentID = new char*[5000];
	for (int count1 = 0; count1 <= md_InstrumentID.size() / 500; count1++)
	{
		if (count1 < md_InstrumentID.size() / 500)
		{
			int a = 0;
			for (a; a < 500; a++)
			{
				ppInstrumentID[a] = const_cast<char *>(md_InstrumentID.at(md_num).c_str());
				md_num++;
			}
			int result = m_pUserMdApi->SubscribeMarketData(ppInstrumentID, a);
			if(result == 0)
			{
				g_log.WriteLog2File(LogControl::LDEBUG,"订阅行情请求1......发送成功");
			}
			else
			{
				g_log.WriteLog2File(LogControl::LWARNING,"订阅行情请求1......发送失败，错误序号=[%d]", result);
			}
		}
		else if (count1 == md_InstrumentID.size() / 500)
		{
			int count2 = 0;
			for (count2; count2 < md_InstrumentID.size() % 500; count2++)
			{
				ppInstrumentID[count2] = const_cast<char *>(md_InstrumentID.at(md_num).c_str());
				md_num++;
			}
			int result = m_pUserMdApi->SubscribeMarketData(ppInstrumentID, count2);
			if(result == 0)
			{
				g_log.WriteLog2File(LogControl::LDEBUG,"订阅行情请求2......发送成功");
			}
			else
			{
				g_log.WriteLog2File(LogControl::LWARNING,"订阅行情请求2......发送失败，错误序号=[%d]", result);
			}
		}
	}
}

void CMdSpiHandler::OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	char szInf[1024] = {0};
	sprintf(szInf,"Enter OnRspSubMarketData:RequestID [%d],IsLast [%d],",nRequestID,bIsLast);

	if (pSpecificInstrument)
	{
		sprintf(szInf,"%s InstrumentID [%s]", szInf,pSpecificInstrument->InstrumentID);
	}
	if (pRspInfo)
	{
		sprintf(szInf,"%s ErrorMsg [%s],ErrorID [%d]", szInf,pRspInfo->ErrorMsg,pRspInfo->ErrorID);
	}
	g_log.WriteLog2File(LogControl::LINFO,szInf);
	
	if (bIsLast)
	{
		SetEvent(xinhao);
	}
};

//深度行情通知
void CMdSpiHandler::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData)
{
	if( pDepthMarketData == NULL )
		return;

#if CAL_DEBUG ==1
	m_pDBApi->InsertTmpData(pDepthMarketData);
#endif

	BARINF barinf = { 0 };
	BARINF combar = { 0 };//组合合约
	bool	bCreateComBar = false;	//是否创建了组合合约bar
	char szDataTime[40] = { 0 };
	struct tm tmDataTime = { 0 };
	sprintf(szDataTime, "%s %s", pDepthMarketData->TradingDay, pDepthMarketData->UpdateTime);
	sscanf(szDataTime, "%04d%02d%02d %d:%d:%d", &tmDataTime.tm_year, &tmDataTime.tm_mon, &tmDataTime.tm_mday,
		&tmDataTime.tm_hour, &tmDataTime.tm_min, &tmDataTime.tm_sec);
	tmDataTime.tm_year -= 1900;
	tmDataTime.tm_mon--;

	//barinf.time = _mktime64(&tmDataTime);

	m_iMarketDataCount++;
	
	//if( m_iMarketDataCount%4 == 0 )
	//	printf("OnRtnDepthMarketData...[%d]\n",m_iMarketDataCount);
	if( strcmp(pDepthMarketData->InstrumentID,"cu2408") == 0)
		printf("InstrumentID=%s,UpdateTime=%s\n", pDepthMarketData->InstrumentID, pDepthMarketData->UpdateTime);

	return;
	
	barinf.HighestPrice = pDepthMarketData->HighestPrice;
	barinf.OpenPrice = pDepthMarketData->OpenPrice;
	barinf.LowestPrice = pDepthMarketData->LowestPrice;
	barinf.ClosePrice = pDepthMarketData->LastPrice;
	barinf.Volume = pDepthMarketData->Volume;
	barinf.Turnover = pDepthMarketData->Turnover;
	barinf.OpenInterest = pDepthMarketData->OpenInterest;

	strcpy(barinf.InstrumentID, pDepthMarketData->InstrumentID);
	strcpy(barinf.ProductID, pDepthMarketData->InstrumentID);

	//去除合约里面的数字，即为产品名
	for (int i = 0; i < strlen(barinf.ProductID);i++)
	{
		if (barinf.ProductID[i] >= 0x30 && barinf.ProductID[i] <= 0x39)
		{
			barinf.ProductID[i] = 0;
			break;
		}
	}

	//m_marketpub.SendData(barinf.ProductID, pDepthMarketData);

	//组合合约combar
	std::map<std::string, WholeComInstrument *>::iterator comit = m_ProductComInstrument.find(std::string(barinf.ProductID));
	if(comit != m_ProductComInstrument.end())
	{ 
		//bCreateComBar = (*comit).second->AddBar(barinf, &combar);
	}

	if( m_nCurrentMin>0 &&  barinf.time/60 > m_nCurrentMin )
	{
		//m_barmgr.CloseBar( barinf.time );
	}

	m_nCurrentMin = barinf.time/60;
	barinf.min = m_nCurrentMin;

	for (int i = 0; i < sizeof(g_barlevel) / sizeof(int); i++)
	{
		barinf.BarLeve = g_barlevel[i];
		if (barinf.BarLeve == 60)
		{
			barinf.BarType = 2;
		}
		else if (barinf.BarLeve == 1440)
		{
			barinf.BarType = 3;
		}
		else if (barinf.BarLeve == 10080)
		{
			barinf.BarType = 4;
		}
		else
		{
			barinf.BarType = 1;
		}
		//m_barmgr.AddBar(barinf);//加入不同K线

		if (bCreateComBar)
		{
			combar.BarType = barinf.BarType;
			//m_barmgr.AddBar(combar);//加入组合合约不同K线
		}
	}	
};

//订阅询价请求
void CMdSpiHandler::SubscribeForQuoteRsp()
{
	int md_num = 0;
	char **ppInstrumentID = new char*[5000];
	for (int count1 = 0; count1 <= md_InstrumentID.size() / 500; count1++)
	{
		if (count1 < md_InstrumentID.size() / 500)
		{
			int a = 0;
			for (a; a < 500; a++)
			{
				ppInstrumentID[a] = const_cast<char *>(md_InstrumentID.at(md_num).c_str());
				md_num++;
			}
			int result = m_pUserMdApi->SubscribeForQuoteRsp(ppInstrumentID, a);
			if(result == 0)
			{
				g_log.WriteLog2File(LogControl::LDEBUG,"订阅询价请求1......发送成功");
			}
			else
			{
				g_log.WriteLog2File(LogControl::LWARNING,"订阅询价请求1......发送失败，错误序号=[%d]", result);
			}
		}
		else if (count1 == md_InstrumentID.size() / 500)
		{
			int count2 = 0;
			for (count2; count2 < md_InstrumentID.size() % 500; count2++)
			{
				ppInstrumentID[count2] = const_cast<char *>(md_InstrumentID.at(md_num).c_str());
				md_num++;
			}
			int result = m_pUserMdApi->SubscribeForQuoteRsp(ppInstrumentID, count2);
			if(result == 0)
			{
				g_log.WriteLog2File(LogControl::LDEBUG,"订阅询价请求2......发送成功");
			}
			else
			{
				g_log.WriteLog2File(LogControl::LWARNING,"订阅询价请求2......发送失败，错误序号=[%d]", result);
			}
		}
	}
}

void CMdSpiHandler::OnRspSubForQuoteRsp(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	char szInf[1024] = {0};
	sprintf(szInf,"Enter OnRspSubForQuoteRsp:RequestID [%d],IsLast [%d],",nRequestID,bIsLast);

	if (pSpecificInstrument)
	{
		sprintf(szInf,"%s InstrumentID [%s]", szInf,pSpecificInstrument->InstrumentID);
	}
	if (pRspInfo)
	{
		sprintf(szInf,"%s ErrorMsg [%s],ErrorID [%d]", szInf,pRspInfo->ErrorMsg,pRspInfo->ErrorID);
	}
	g_log.WriteLog2File(LogControl::LINFO,szInf);

	if (bIsLast)
	{
		SetEvent(xinhao);
	}
};

void CMdSpiHandler::UnSubscribeForQuoteRsp()
{
	int md_num = 0;
	char **ppInstrumentID = new char*[5000];
	for (int count1 = 0; count1 <= md_InstrumentID.size() / 500; count1++)
	{
		if (count1 < md_InstrumentID.size() / 500)
		{
			int a = 0;
			for (a; a < 500; a++)
			{
				ppInstrumentID[a] = const_cast<char *>(md_InstrumentID.at(md_num).c_str());
				md_num++;
			}
			int result = m_pUserMdApi->UnSubscribeForQuoteRsp(ppInstrumentID, a);
			if(result == 0)
			{
				g_log.WriteLog2File(LogControl::LDEBUG,"退订询价请求1......发送成功");
			}
			else
			{
				g_log.WriteLog2File(LogControl::LWARNING,"退订询价请求1......发送失败，错误序号=[%d]", result);
			}
		}
		else if (count1 == md_InstrumentID.size() / 500)
		{
			int count2 = 0;
			for (count2; count2 < md_InstrumentID.size() % 500; count2++)
			{
				ppInstrumentID[count2] = const_cast<char *>(md_InstrumentID.at(md_num).c_str());
				md_num++;
			}
			int result = m_pUserMdApi->UnSubscribeForQuoteRsp(ppInstrumentID, count2);
			if(result == 0)
			{
				g_log.WriteLog2File(LogControl::LDEBUG,"退订询价请求2......发送成功");
			}
			else
			{
				g_log.WriteLog2File(LogControl::LWARNING,"退订询价请求2......发送失败，错误序号=[%d]", result);
			}
		}
	}
}

void CMdSpiHandler::OnRspUnSubForQuoteRsp(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	char szInf[1024] = {0};
	sprintf(szInf,"Enter OnRspUnSubForQuoteRsp:RequestID [%d],IsLast [%d],",nRequestID,bIsLast);

	if (pSpecificInstrument)
	{
		sprintf(szInf,"%s InstrumentID [%s]", szInf,pSpecificInstrument->InstrumentID);
	}
	if (pRspInfo)
	{
		sprintf(szInf,"%s ErrorMsg [%s],ErrorID [%d]", szInf,pRspInfo->ErrorMsg,pRspInfo->ErrorID);
	}
	g_log.WriteLog2File(LogControl::LINFO,szInf);

	if (bIsLast)
	{
		SetEvent(xinhao);
	}
};

//询价通知
void CMdSpiHandler::OnRtnForQuoteRsp(CThostFtdcForQuoteRspField *pForQuoteRsp)
{
	if (pForQuoteRsp)
	{
		g_log.WriteLog2File(LogControl::LINFO,
			"OnRtnForQuoteRsp:TradingDay [%s],InstrumentID [%s],ForQuoteSysID [%s],ForQuoteTime [%s],ActionDay [%s],ExchangeID [%s]",
			pForQuoteRsp->TradingDay,pForQuoteRsp->InstrumentID,pForQuoteRsp->ForQuoteSysID,pForQuoteRsp->ForQuoteTime,pForQuoteRsp->ActionDay,pForQuoteRsp->ExchangeID);
	}
	else
	{
		g_log.WriteLog2File(LogControl::LINFO,"OnRtnForQuoteRsp");
	}
}

void CMdSpiHandler::ReqQryMulticastInstrument(int TopicID,int nRequestID)
{
	CThostFtdcQryMulticastInstrumentField a = { 0 };
	a.TopicID = TopicID;
	int result = m_pUserMdApi->ReqQryMulticastInstrument(&a, nRequestID);
	if(result == 0)
	{
		g_log.WriteLog2File(LogControl::LDEBUG,"请求查询组播合约......发送成功");
	}
	else
	{
		g_log.WriteLog2File(LogControl::LWARNING,"请求查询组播合约......发送失败，错误序号=[%d]", result);
	}
}

void CMdSpiHandler::OnRspQryMulticastInstrument(CThostFtdcMulticastInstrumentField *pMulticastInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	char szInf[1024] = {0};
	sprintf(szInf,"Enter OnRspQryMulticastInstrument:RequestID [%d],IsLast [%d],",nRequestID,bIsLast);
	g_log.WriteLog2File(LogControl::LINFO,szInf);
};