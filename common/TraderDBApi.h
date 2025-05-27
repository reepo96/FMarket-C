#ifndef TRADER_DBAPI_H
#define TRADER_DBAPI_H

#include <stdio.h>
#include <vector>
#include <map>
#include <string>
#include "CommonDef.h"
#include "ThostFtdcUserApiStruct.h"

class traderDBApi
{

protected:

private:

public:
	traderDBApi( void ){};

	virtual ~traderDBApi( void ){};

	/*
	���ݲ���id���Ҳ���������Ϣ
	*/
	virtual bool GetStrategy(int strategyid, STRATEGY& strategy) = 0;

	//����ί�е�
	virtual bool InsertOrder(CThostFtdcInputOrderField &orderfield) = 0;

	//����ί�е�
	virtual bool UpdateOrder(CThostFtdcOrderField &orderfield) = 0;

	//���ݺ�Լ�Ų��Ҹú�Լ����δ�ɽ���ί�е�
	virtual bool GetUnclosedOrder(const char *instrumentid, std::map<int, std::string> &orderlist) = 0;

	//����ָ���ʻ��µ�ǰ��Ʒ�����һ��δ�ɽ�ί�е�����
	//instrumentid:��Լ��
	//BuyOrSell:1-��2-��
	//EntryOrExit���е��Ŀ�ƽ�����ͣ�0-���֣�1-ƽ�֣�3-ƽ���
	//����ί�е���
	virtual int GetLastUnclosedOrder(const char *instrumentid, int BuyOrSell, int EntryOrExit) =0;

	//����ָ���ʻ��µ�ǰ��Ʒ�����һ������ί�е�����
	//instrumentid:��Լ��
	//BuyOrSell:1-��2-��
	//EntryOrExit���е��Ŀ�ƽ�����ͣ�0-���֣�1-ƽ�֣�3-ƽ���
	//����ί�е���
	virtual int GetLastTodayOrder(const char *instrumentid, int BuyOrSell, int EntryOrExit) = 0;

	//����ָ���ʻ��µ�ǰ��Ʒ��δ�ɽ�ί�е�����
	//instrumentid:��Լ��
	//����ί�е�����
	virtual int GetUnclosedOrderCount(const char *instrumentid,int accountIndex=0) = 0;

	//���ص�ǰ��ʽӦ�õ��ʻ��µ�ǰ��Ʒ�ĵ���ί�е�����
	//instrumentid:��Լ��
	//����ί�е�����
	virtual int GetTodayOrderCount(const char *instrumentid, int accountIndex = 0) = 0;
};


#endif /* DBAPI_H */

