#include "TraderHandler.h"
#include "LogControl.h"
#include "Utility.h"

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <winbase.h>

extern	HANDLE g_hTraderEvent;
extern LogControl	g_log;
#define CFG_INI_FILE "./config.ini"
#define	PRODUCT_FILE "./product.ini"

CTraderHandler::CTraderHandler(CThostFtdcTraderApi *pUserApi) : m_pUserTraderApi(pUserApi)
{
	std::string sProduct = "";
	char szLine[1024] = {0};
	Utility::CIniMgr ini;
	ini.InitFile(CFG_INI_FILE);

	memset(m_chBrokerID,0,sizeof(m_chBrokerID));
	memset(m_chUserID,0,sizeof(m_chUserID));
	memset(m_chPassword,0,sizeof(m_chPassword));
	memset(m_chExchangeID,0,sizeof(m_chExchangeID));
	memset(m_chAuthCode, 0, sizeof(m_chAuthCode));
	memset(m_chAppID, 0, sizeof(m_chAppID));

	ini.GetProfileStr("config","BrokerID","9999",m_chBrokerID,sizeof(m_chBrokerID),NULL);
	ini.GetProfileStr("config","UserID","00001",m_chUserID,sizeof(m_chUserID),NULL);
	ini.GetProfileStr("config","Password","",m_chPassword,sizeof(m_chPassword),NULL);
	ini.GetProfileStr("config","ExchangeID","SHFE",m_chExchangeID,sizeof(m_chExchangeID),NULL);
	ini.GetProfileStr("config", "AuthCode", "0000000000000000", m_chAuthCode, sizeof(m_chAuthCode), NULL);
	ini.GetProfileStr("config", "AppID", "simnow_client_test", m_chAppID, sizeof(m_chAppID), NULL);

	//初始化要订阅的产品
	FILE *fp = fopen(PRODUCT_FILE,"r");
	if( fp )
	{
		while( NULL != fgets(szLine,1023,fp) )
		{
			sProduct += std::string(szLine);
		}

		m_sProductID = Utility::split(sProduct,":");

		fclose(fp);
		fp = NULL;
	}

	m_nRequestID = 0;
	m_product_Instrument_list = NULL;
}

void CTraderHandler::OnFrontConnected()
{
	SetEvent(g_hTraderEvent);
}

//客户端认证
void CTraderHandler::ReqAuthenticate()
{
	CThostFtdcReqAuthenticateField a = { 0 };
	strcpy_s(a.BrokerID, m_chBrokerID);
	strcpy_s(a.UserID, m_chUserID);
	strcpy_s(a.AuthCode, m_chAuthCode);
	strcpy_s(a.AppID, m_chAppID);

	int res = m_pUserTraderApi->ReqAuthenticate(&a, m_nRequestID++);
}

void CTraderHandler::OnRspAuthenticate(CThostFtdcRspAuthenticateField *pRspAuthenticateField, CThostFtdcRspInfoField *pRspInfo,
	int nRequestID, bool bIsLast)
{
	SetEvent(g_hTraderEvent);
}

void CTraderHandler::ReqUserLogin()
{
	CThostFtdcReqUserLoginField reqUserLogin = { 0 };
	strcpy_s(reqUserLogin.BrokerID, m_chBrokerID);
	strcpy_s(reqUserLogin.UserID, m_chUserID);
	strcpy_s(reqUserLogin.Password, m_chPassword);
	strcpy_s(reqUserLogin.LoginRemark, "test");
	// 发出登陆请求
	m_pUserTraderApi->ReqUserLogin(&reqUserLogin, m_nRequestID++);
}

void CTraderHandler::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	SetEvent(g_hTraderEvent);
}

void CTraderHandler::ReqUserLogout()
{
	CThostFtdcUserLogoutField req = { 0 };
	strcpy_s(req.BrokerID, m_chBrokerID);
	strcpy_s(req.UserID, m_chUserID);
	m_pUserTraderApi->ReqUserLogout(&req, m_nRequestID++);
}

void CTraderHandler::OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	SetEvent(g_hTraderEvent);
}

//请求查询合约
void CTraderHandler::ReqQryInstrument(std::map<std::string, std::vector<std::string>> *product_instrument_list)
{
	CThostFtdcQryInstrumentField req = { 0 };
	m_pUserTraderApi->ReqQryInstrument(&req, m_nRequestID++);
	m_product_Instrument_list = product_instrument_list;
}

void CTraderHandler::OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (pInstrument == NULL || (pRspInfo && pRspInfo->ErrorID != 0))
	{
		if (bIsLast)
			SetEvent(g_hTraderEvent);
		return;
	}
	/*printf("InstrumentName:%s,InstrumentID:%s,ProductClass:%d,PositionType:%d,ProductID:%s,UnderlyingInstrID:%s\n",
		pInstrument->InstrumentName, pInstrument->InstrumentID, pInstrument->ProductClass, pInstrument->PositionType, pInstrument->ProductID, pInstrument->UnderlyingInstrID);*/

	//只保留需要订阅的产品
	std::string	sProduct = std::string(pInstrument->ProductID);
	std::vector<std::string>::iterator product_it = m_sProductID.begin();
	std::map<std::string, std::vector<std::string>>::iterator map_it;

	for (; product_it != m_sProductID.end(); product_it)
	{
		if ((*product_it) == sProduct && m_product_Instrument_list)
		{
			map_it = m_product_Instrument_list->find(sProduct);
			if (map_it == m_product_Instrument_list->end())
			{
				std::vector<std::string> InstrumentList;
				InstrumentList.push_back(std::string(pInstrument->InstrumentID));
				m_product_Instrument_list->insert(std::map<std::string, std::vector<std::string>>::value_type(sProduct, InstrumentList));
			}
			else
			{
				(*map_it).second.push_back(std::string(pInstrument->InstrumentID));
			}
		}
	}
	

	if (bIsLast)
	{
		SetEvent(g_hTraderEvent);
	}
}