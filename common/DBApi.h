#ifndef DBAPI_H
#define DBAPI_H

#include <stdio.h>
#include <vector>
#include <map>
#include <string>
#include "CommonDef.h"
#include "ThostFtdcUserApiStruct.h"

class DBApi
{

protected:

private:

public:
	DBApi( void ){};

	virtual ~DBApi( void ){};

	//������ͨ��
	virtual bool CreateTable(const char *tablename) = 0;

	//�����ڴ��
	virtual bool CreateMemTable(const char *tablename) = 0;
	
	virtual bool SavBar(const char * productid, int barlevel, std::vector<BARINF> barlist) = 0;

	virtual bool InsertBarToMemTab(BARINF &bar) = 0;

	virtual bool UpdateBarToMemTab(BARINF &bar) = 0;

	virtual bool InsertTmpData(CThostFtdcDepthMarketDataField *pDepthMarketData) = 0;

	/*
	barlist:���ص�K���б�
	productid��Ʒ��ID
	instrumentid����ԼID
	freq��K��Ƶ�ʣ�ͳһת�ɷ��ӣ�������K�ߣ�freq=24*60=1440
	beginMin����ѯ�Ŀ�ʼ����tick
	endMin����ѯ�Ľ�������tick��Ĭ��ֵ0��ʾ�鵽���
	*/
	virtual bool GetBar(std::vector<BARINF>& barlist, const char * productid, const char * instrumentid, int freq, int beginMin, int endMin = 0)=0;
};


#endif /* DBAPI_H */

