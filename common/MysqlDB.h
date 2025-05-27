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

	//������ͨ��
	virtual bool CreateTable(const char *tablename);

	//�����ڴ��
	virtual bool CreateMemTable(const char *tablename);
	
	virtual bool SavBar(const char * productid,int barlevel,std::vector<BARINF> barlist);

	virtual bool InsertBarToMemTab(BARINF &bar);

	virtual bool UpdateBarToMemTab(BARINF &bar);

	virtual bool InsertTmpData(CThostFtdcDepthMarketDataField *pDepthMarketData);

	/*
	barlist:���ص�K���б�
	productid��Ʒ��ID
	instrumentid����ԼID
	freq��K��Ƶ�ʣ�ͳһת�ɷ��ӣ�������K�ߣ�freq=24*60=1440
	beginMin����ѯ�Ŀ�ʼ����tick
	endMin����ѯ�Ľ�������tick��Ĭ��ֵ0��ʾ�鵽���
	*/
	virtual bool GetBar(std::vector<BARINF>& barlist,const char * productid, const char * instrumentid, int freq, int beginMin, int endMin = 0);
};


#endif /* MYSQLDB_H */

