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
	根据策略id查找策略配置信息
	*/
	virtual bool GetStrategy(int strategyid, STRATEGY& strategy);

	//插入委托单
	virtual bool InsertOrder(CThostFtdcInputOrderField &orderfield);

	//更新委托单
	virtual bool UpdateOrder(CThostFtdcOrderField &orderfield);

	//根据合约号查找该合约所有未成交的委托单
	virtual bool GetUnclosedOrder(const char *instrumentid, std::map<int, std::string> &orderlist);

	//返回指定帐户下当前商品的最后一个未成交委托单索引
	//instrumentid:合约号
	//BuyOrSell:1-买，2-卖
	//EntryOrExit：托单的开平仓类型，0-开仓，1-平仓，3-平今仓
	//返回委托单号
	virtual int GetLastUnclosedOrder(const char *instrumentid, int BuyOrSell,int EntryOrExit);

	//返回指定帐户下当前商品的最后一个当日委托单索引
	//instrumentid:合约号
	//BuyOrSell:1-买，2-卖
	//EntryOrExit：托单的开平仓类型，0-开仓，1-平仓，3-平今仓
	//返回委托单号
	virtual int GetLastTodayOrder(const char *instrumentid, int BuyOrSell, int EntryOrExit);

	//返回指定帐户下当前商品的未成交委托单数量
	//instrumentid:合约号
	//返回未成交委托单数量
	virtual int GetUnclosedOrderCount(const char *instrumentid, int accountIndex = 0);

	//返回当前公式应用的帐户下当前商品的当日委托单数量
	//instrumentid:合约号
	//返回委托单数量
	virtual int GetTodayOrderCount(const char *instrumentid, int accountIndex = 0);
};


#endif /* TRADER_MYSQLDB_H */

