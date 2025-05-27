#ifndef TRADERAPI_H
#define TRADERAPI_H

#include <stdio.h>
#include <vector>
#include <map>
#include <string>


class TraderApi
{

protected:

private:

public:
	TraderApi( void ){};

	virtual ~TraderApi( void ){};

	//连接交易接口
	virtual bool connect() = 0;

	//查询品种的合约列表
	//product_instrument_list:返回的合约列表，其中map的key是品种ID，map的值是该品种所有交易中的合约ID列表
	virtual bool QryProductInstrument(std::map<std::string, std::vector<std::string>> *product_instrument_list) = 0;
	
	virtual bool SavBar(const char * productid, int barlevel, std::vector<BARINF> barlist) = 0;

	virtual bool InsertBarToMemTab(BARINF &bar) = 0;

	virtual bool UpdateBarToMemTab(BARINF &bar) = 0;
};


#endif /* TRADERAPI_H */

