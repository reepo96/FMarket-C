#ifndef MYSQLDB_H
#define MYSQLDB_H

#define CAL_DEBUG 1

#include "DBApi.h"
#include "mysql.h"

class MysqlDB: public DBApi
{

protected:
	MYSQL	*m_con;
	int		m_sqlbuf_len;
	char	*m_sqlbuf;

private:
	bool ConnectDB();
	void CloseDB();

public:
	MysqlDB( void );

	virtual ~MysqlDB( void );

	//创建普通表
	virtual bool CreateTable(const char *tablename);

	//创建内存表
	virtual bool CreateMemTable(const char *tablename);
	
	virtual bool SavBar(const char * productid,int barlevel,std::vector<BARINF> barlist);

	virtual bool InsertBarToMemTab(BARINF &bar);

	virtual bool UpdateBarToMemTab(BARINF &bar);

	virtual bool InsertTmpData(CThostFtdcDepthMarketDataField *pDepthMarketData);

	/*
	barlist:返回的K线列表
	productid：品种ID
	instrumentid：合约ID
	freq：K线频率，统一转成分钟，比如日K线，freq=24*60=1440
	beginMin：查询的开始分钟tick
	endMin：查询的结束分钟tick，默认值0表示查到最后
	*/
	virtual bool GetBar(std::vector<BARINF>& barlist,const char * productid, const char * instrumentid, int freq, int beginMin, int endMin = 0);
};


#endif /* MYSQLDB_H */

