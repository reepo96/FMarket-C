#if !defined(MDSPI_HANDLER_H)
#define MDSPI_HANDLER_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include <vector>
#include <map>
#include "ThostFtdcMdApi.h"
#include "DataCollect.h"
#include "BarMgr.h"
#include "DBApi.h"
#include "WholeComInstrument.h"
#include "MarketPub.h"

//行情类
class CMdSpiHandler : public CThostFtdcMdSpi
{
public:
	CMdSpiHandler(CThostFtdcMdApi *pUserApi);
	~CMdSpiHandler() {}
	void deletemyself();

	virtual void OnFrontConnected();

	void RegisterFensUserInfo();

	void ReqUserLogin();

	void ReqUserLogout();

	virtual void OnHeartBeatWarning(int nTimeLapse);

	// 当客户端与交易托管系统通信连接断开时，该方法被调用
	virtual void OnFrontDisconnected(int nReason);

	// 当客户端发出登录请求之后，该方法会被调用，通知客户端登录是否成功
	virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	//登出请求响应
	virtual void OnRspUserLogout(CThostFtdcUserLogoutField* pUserLogout, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast);

	void UnSubscribeMarketData();

	virtual void OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	void SubscribeMarketData();

	virtual void OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	//深度行情通知
	virtual void OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData);

	//订阅询价请求
	void SubscribeForQuoteRsp();

	virtual void OnRspSubForQuoteRsp(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	void UnSubscribeForQuoteRsp();

	virtual void OnRspUnSubForQuoteRsp(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	//询价通知
	virtual void OnRtnForQuoteRsp(CThostFtdcForQuoteRspField *pForQuoteRsp);

	void ReqQryMulticastInstrument(int TopicID,int nRequestID);

	virtual void OnRspQryMulticastInstrument(CThostFtdcMulticastInstrumentField *pMulticastInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

private:
	// 指向CThostFtdcMduserApi实例的指针
	CThostFtdcMdApi *m_pUserMdApi;

	/// 会员代码
	TThostFtdcBrokerIDType m_chBrokerID;
	/// 交易用户代码
	TThostFtdcUserIDType m_chUserID;
	/// 交易用户密码
	TThostFtdcPasswordType m_chPassword;
	/// 交易所代码
	TThostFtdcExchangeIDType m_chExchangeID;

	std::vector<std::string> md_InstrumentID;
	unsigned int m_iMarketDataCount;

	std::vector<std::string> m_sProductID;	//要订阅的产品id

	std::map<std::string, WholeComInstrument *>	m_ProductComInstrument;//品种全组合合约

private:
	BarMgr	m_barmgr;
	unsigned int m_nCurrentMin;//当前数据分钟数，数据跨分钟时会发动结算

	DBApi	*m_pDBApi;
	MarketPub	m_marketpub;
};

#endif