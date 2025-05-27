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

	//创建普通表
	virtual bool CreateTable(const char *tablename) = 0;

	//创建内存表
	virtual bool CreateMemTable(const char *tablename) = 0;
	
	virtual bool SavBar(const char * productid, int barlevel, std::vector<BARINF> barlist) = 0;

	virtual bool InsertBarToMemTab(BARINF &bar) = 0;

	virtual bool UpdateBarToMemTab(BARINF &bar) = 0;

	virtual bool InsertTmpData(CThostFtdcDepthMarketDataField *pDepthMarketData) = 0;

	/*
	barlist:返回的K线列表
	productid：品种ID
	instrumentid：合约ID
	freq：K线频率，统一转成分钟，比如日K线，freq=24*60=1440
	beginMin：查询的开始分钟tick
	endMin：查询的结束分钟tick，默认值0表示查到最后
	*/
	virtual bool GetBar(std::vector<BARINF>& barlist, const char * productid, const char * instrumentid, int freq, int beginMin, int endMin = 0)=0;
};


#endif /* DBAPI_H */

