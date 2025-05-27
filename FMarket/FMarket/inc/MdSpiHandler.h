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

//������
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

	// ���ͻ����뽻���й�ϵͳͨ�����ӶϿ�ʱ���÷���������
	virtual void OnFrontDisconnected(int nReason);

	// ���ͻ��˷�����¼����֮�󣬸÷����ᱻ���ã�֪ͨ�ͻ��˵�¼�Ƿ�ɹ�
	virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	//�ǳ�������Ӧ
	virtual void OnRspUserLogout(CThostFtdcUserLogoutField* pUserLogout, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast);

	void UnSubscribeMarketData();

	virtual void OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	void SubscribeMarketData();

	virtual void OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	//�������֪ͨ
	virtual void OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData);

	//����ѯ������
	void SubscribeForQuoteRsp();

	virtual void OnRspSubForQuoteRsp(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	void UnSubscribeForQuoteRsp();

	virtual void OnRspUnSubForQuoteRsp(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	//ѯ��֪ͨ
	virtual void OnRtnForQuoteRsp(CThostFtdcForQuoteRspField *pForQuoteRsp);

	void ReqQryMulticastInstrument(int TopicID,int nRequestID);

	virtual void OnRspQryMulticastInstrument(CThostFtdcMulticastInstrumentField *pMulticastInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

private:
	// ָ��CThostFtdcMduserApiʵ����ָ��
	CThostFtdcMdApi *m_pUserMdApi;

	/// ��Ա����
	TThostFtdcBrokerIDType m_chBrokerID;
	/// �����û�����
	TThostFtdcUserIDType m_chUserID;
	/// �����û�����
	TThostFtdcPasswordType m_chPassword;
	/// ����������
	TThostFtdcExchangeIDType m_chExchangeID;

	std::vector<std::string> md_InstrumentID;
	unsigned int m_iMarketDataCount;

	std::vector<std::string> m_sProductID;	//Ҫ���ĵĲ�Ʒid

	std::map<std::string, WholeComInstrument *>	m_ProductComInstrument;//Ʒ��ȫ��Ϻ�Լ

private:
	BarMgr	m_barmgr;
	unsigned int m_nCurrentMin;//��ǰ���ݷ����������ݿ����ʱ�ᷢ������

	DBApi	*m_pDBApi;
	MarketPub	m_marketpub;
};

#endif