#include "TraderMysqlDB.h"
#include "Utility.h"

#define CFG_INI_FILE "./config.ini"

TraderMysqlDB::TraderMysqlDB( void )
{
	m_con=NULL;

	ConnectDB();
}

TraderMysqlDB::~TraderMysqlDB( void )
{
}

bool TraderMysqlDB::ConnectDB()
{
	char dbip[30]={0};
	char dbuser[30]={0};
	char dbpasswd[30]={0};
	char dbname[50]={0};

	Utility::CIniMgr ini;
	ini.InitFile(CFG_INI_FILE);

	ini.GetProfileStr("mysql","dbip","localhost",dbip,sizeof(dbip),NULL);
	ini.GetProfileStr("mysql","usr","root",dbuser,sizeof(dbuser),NULL);
	ini.GetProfileStr("mysql","passwd","123456",dbpasswd,sizeof(dbpasswd),NULL);
	ini.GetProfileStr("mysql","traderdb","",dbname,sizeof(dbname),NULL);

	m_con = mysql_init((MYSQL*)0);
	if( m_con == NULL )
		return false;

	if(mysql_real_connect(m_con, dbip, dbuser, dbpasswd, dbname, 3306, NULL, 0) == NULL )
	{
		printf("connect mysql database fail:%s\n",mysql_error(m_con) );
		mysql_close(m_con);
		m_con = NULL;
		return false;
	}

	return true;
}

void TraderMysqlDB::CloseDB()
{
	if( m_con )
	{
		mysql_close(m_con);
		m_con = NULL;
	}
}

bool  TraderMysqlDB::GetStrategy(int strategyid, STRATEGY& strategy)
{
	char szSql[200] = { 0 };
	MYSQL_RES *res;     //行的一个查询结果集
	MYSQL_ROW row;   //数据行的列
	int cnt = 0;

	sprintf(szSql, "SELECT * FROM t_strategy,t_datasource WHERE t_strategy.strategyid = t_datasource.strategyid and t_strategy.strategyid=%d", strategyid);

	if (mysql_query(m_con, szSql))
	{
		printf("query Strategy fail:%s\n", mysql_error(m_con));
		CloseDB();
		return false;
	}

	res = mysql_store_result(m_con);
	if (!res) {
		printf("Couldn't get result %s\n", mysql_error(m_con));
		return false;
	}

	int row_num = mysql_num_rows(res);
	if (row_num == 0)
		return false;

	while (row = mysql_fetch_row(res))
	{
		strategy.strategyid = atoi(row[0]);
		strncpy(strategy.strategyname, row[1], sizeof(strategy.strategyname) - 1);

		STRATEGYDS ds = { 0 };
		
		ds.strategyid = atoi(row[3]);
		strncpy(ds.datasource, row[4],sizeof(ds.datasource)-1);
		ds.starttime = atof(row[5]);
		ds.endtime = atof(row[6]);
		strncpy(ds.freq, row[7], sizeof(ds.freq) - 1);
		ds.Position = atof(row[8]);
		ds.map = atoi(row[9]);
		ds.strategy = atoi(row[10]);

		strategy.ds.push_back(ds);
	}

	return true;
}


bool TraderMysqlDB::InsertOrder(CThostFtdcInputOrderField &orderfield)
{
	if (m_con == NULL && false == ConnectDB())
		return false;

	time_t ttNow = time(NULL);
	struct tm tmNow = *localtime(&ttNow);
	char szTime[30] = { 0 };
	char szSql[300] = { 0 };

	sprintf(szTime, "%d-%02d-%02d %02d:%02d:%02d", tmNow.tm_year + 190, tmNow.tm_mon + 1, tmNow.tm_mday, tmNow.tm_hour, tmNow.tm_min, tmNow.tm_sec);

	sprintf(szSql, "insert into t_order values (%d,'',%s,%s,%s,%s,%s,%c,%c,%c,%f,%d,0,%d,'c')",
		atoi(orderfield.OrderRef), szTime, orderfield.InstrumentID, orderfield.BrokerID, orderfield.InvestorID, orderfield.UserID, orderfield.OrderPriceType,
		orderfield.Direction, orderfield.CombHedgeFlag, orderfield.LimitPrice, orderfield.VolumeTotalOriginal, orderfield.VolumeTotalOriginal);

	//存数据
	if (mysql_query(m_con, szSql))
	{
		printf("insert data to t_order fail:%s\n", mysql_error(m_con));
		CloseDB();
		return false;
	}
	return true;
}

bool TraderMysqlDB::UpdateOrder(CThostFtdcOrderField &orderfield)
{
	if (m_con == NULL && false == ConnectDB())
		return false;

	char szSql[300] = { 0 };

	sprintf(szSql, "update t_order set OrderSysID='%s',VolumeTraded=%f,VolumeTotal=%f,OrderStatus='%c' where orderref=%d",
		orderfield.OrderSysID, orderfield.VolumeTraded, orderfield.VolumeTotal, orderfield.OrderStatus, atoi(orderfield.OrderRef));

	//存数据
	if (mysql_query(m_con, szSql))
	{
		printf("update t_order fail:%s\n", mysql_error(m_con));
		CloseDB();
		return false;
	}
	return true;
}

bool TraderMysqlDB::GetUnclosedOrder(const char *instrumentid, std::map<int, std::string> &orderlist)
{
	char szSql[200] = { 0 };
	MYSQL_RES *res;     //行的一个查询结果集
	MYSQL_ROW row;   //数据行的列
	int cnt = 0;

	sprintf(szSql, "select * from t_order where InstrumentID ='%s' and OrderStatus <> '0'", instrumentid);

	if (mysql_query(m_con, szSql))
	{
		printf("query data from t_order fail:%s\n", mysql_error(m_con));
		CloseDB();
		return false;
	}

	res = mysql_store_result(m_con);
	if (!res) {
		printf("Couldn't get result %s\n", mysql_error(m_con));
		return false;
	}

	int row_num = mysql_num_rows(res);

	while (row = mysql_fetch_row(res))
	{
		int orderid = atoi(row[0]);
		std::string ordersysid = std::string(row[1]);
		orderlist.insert(std::map<int, std::string>::value_type(orderid, ordersysid));
	}

	return true;
}

int TraderMysqlDB::GetLastUnclosedOrder(const char *instrumentid, int BuyOrSell, int EntryOrExit)
{
	char szSql[200] = { 0 };
	MYSQL_RES *res;     //行的一个查询结果集
	MYSQL_ROW row;   //数据行的列
	int cnt = 0;

	char chDirection = (0x30 + BuyOrSell - 1);
	char chCombOffsetFlag = (0x30 + EntryOrExit);

	sprintf(szSql, "select * from t_order where InstrumentID ='%s' and OrderStatus <> '0' and Direction = '%c' and CombOffsetFlag = '%c' ORDER BY orderref DESC",
		instrumentid, chDirection, chCombOffsetFlag);

	if (mysql_query(m_con, szSql))
	{
		printf("query data from t_order fail:%s\n", mysql_error(m_con));
		CloseDB();
		return -1;
	}

	res = mysql_store_result(m_con);
	if (!res) {
		printf("Couldn't get result %s\n", mysql_error(m_con));
		return -1;
	}

	int row_num = mysql_num_rows(res);

	if (row = mysql_fetch_row(res))
	{
		int orderid = atoi(row[0]);
		return orderid;
	}

	return -1;
}

int TraderMysqlDB::GetLastTodayOrder(const char *instrumentid, int BuyOrSell, int EntryOrExit)
{
	char szSql[200] = { 0 };
	MYSQL_RES *res;     //行的一个查询结果集
	MYSQL_ROW row;   //数据行的列
	int cnt = 0;

	char chDirection = (0x30 + BuyOrSell - 1);
	char chCombOffsetFlag = (0x30 + EntryOrExit);
	char szToday[30] = { 0 };
	time_t ttNow = time(NULL);
	struct tm tmNow = *localtime(&ttNow);

	sprintf(szToday, "%d-%02d-%02d%", tmNow.tm_year, tmNow.tm_mon, tmNow.tm_mday);

	sprintf(szSql, "select * from t_order where InstrumentID ='%s' and Direction = '%c' and CombOffsetFlag = '%c' and ordertime like '%s' ORDER BY orderref DESC",
		instrumentid, chDirection, chCombOffsetFlag, szToday);

	if (mysql_query(m_con, szSql))
	{
		printf("query data from t_order fail:%s\n", mysql_error(m_con));
		CloseDB();
		return -1;
	}

	res = mysql_store_result(m_con);
	if (!res) {
		printf("Couldn't get result %s\n", mysql_error(m_con));
		return -1;
	}

	int row_num = mysql_num_rows(res);

	if (row = mysql_fetch_row(res))
	{
		int orderid = atoi(row[0]);
		return orderid;
	}

	return -1;
}

int TraderMysqlDB::GetUnclosedOrderCount(const char *instrumentid, int accountIndex)
{
	char szSql[200] = { 0 };
	MYSQL_RES *res;     //行的一个查询结果集
	MYSQL_ROW row;   //数据行的列
	int cnt = 0;

	sprintf(szSql, "select count(*) from t_order where InstrumentID ='%s' and OrderStatus <> '0'",instrumentid);

	if (mysql_query(m_con, szSql))
	{
		printf("query data from t_order fail:%s\n", mysql_error(m_con));
		CloseDB();
		return 0;
	}

	res = mysql_store_result(m_con);
	if (!res) {
		printf("Couldn't get result %s\n", mysql_error(m_con));
		return 0;
	}

	int row_num = mysql_num_rows(res);

	if (row = mysql_fetch_row(res))
	{
		cnt = atoi(row[0]);
		return cnt;
	}

	return 0;
}

int TraderMysqlDB::GetTodayOrderCount(const char *instrumentid, int accountIndex)
{
	char szSql[200] = { 0 };
	MYSQL_RES *res;     //行的一个查询结果集
	MYSQL_ROW row;   //数据行的列
	int cnt = 0;

	char szToday[30] = { 0 };
	time_t ttNow = time(NULL);
	struct tm tmNow = *localtime(&ttNow);

	sprintf(szToday, "%d-%02d-%02d%", tmNow.tm_year, tmNow.tm_mon, tmNow.tm_mday);

	sprintf(szSql, "select count(*) from t_order where InstrumentID ='%s'and ordertime like '%s'",instrumentid, szToday);

	if (mysql_query(m_con, szSql))
	{
		printf("query data from t_order fail:%s\n", mysql_error(m_con));
		CloseDB();
		return 0;
	}

	res = mysql_store_result(m_con);
	if (!res) {
		printf("Couldn't get result %s\n", mysql_error(m_con));
		return 0;
	}

	int row_num = mysql_num_rows(res);

	if (row = mysql_fetch_row(res))
	{
		cnt = atoi(row[0]);
		return cnt;
	}

	return 0;
}



