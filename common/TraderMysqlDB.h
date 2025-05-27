#ifndef TRADER_MYSQLDB_H
#define TRADER_MYSQLDB_H

#include "TraderDBApi.h"
#include "mysql.h"

class TraderMysqlDB: public traderDBApi
{

protected:
	MYSQL	*m_con;

private:
	bool ConnectDB();
	void CloseDB();

public:
	TraderMysqlDB( void );

	virtual ~TraderMysqlDB( void );

	/*
	���ݲ���id���Ҳ���������Ϣ
	*/
	virtual bool GetStrategy(int strategyid, STRATEGY& strategy);

	//����ί�е�
	virtual bool InsertOrder(CThostFtdcInputOrderField &orderfield);

	//����ί�е�
	virtual bool UpdateOrder(CThostFtdcOrderField &orderfield);

	//���ݺ�Լ�Ų��Ҹú�Լ����δ�ɽ���ί�е�
	virtual bool GetUnclosedOrder(const char *instrumentid, std::map<int, std::string> &orderlist);

	//����ָ���ʻ��µ�ǰ��Ʒ�����һ��δ�ɽ�ί�е�����
	//instrumentid:��Լ��
	//BuyOrSell:1-��2-��
	//EntryOrExit���е��Ŀ�ƽ�����ͣ�0-���֣�1-ƽ�֣�3-ƽ���
	//����ί�е���
	virtual int GetLastUnclosedOrder(const char *instrumentid, int BuyOrSell,int EntryOrExit);

	//����ָ���ʻ��µ�ǰ��Ʒ�����һ������ί�е�����
	//instrumentid:��Լ��
	//BuyOrSell:1-��2-��
	//EntryOrExit���е��Ŀ�ƽ�����ͣ�0-���֣�1-ƽ�֣�3-ƽ���
	//����ί�е���
	virtual int GetLastTodayOrder(const char *instrumentid, int BuyOrSell, int EntryOrExit);

	//����ָ���ʻ��µ�ǰ��Ʒ��δ�ɽ�ί�е�����
	//instrumentid:��Լ��
	//����δ�ɽ�ί�е�����
	virtual int GetUnclosedOrderCount(const char *instrumentid, int accountIndex = 0);

	//���ص�ǰ��ʽӦ�õ��ʻ��µ�ǰ��Ʒ�ĵ���ί�е�����
	//instrumentid:��Լ��
	//����ί�е�����
	virtual int GetTodayOrderCount(const char *instrumentid, int accountIndex = 0);
};


#endif /* TRADER_MYSQLDB_H */

