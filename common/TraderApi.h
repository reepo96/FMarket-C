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

	//���ӽ��׽ӿ�
	virtual bool connect() = 0;

	//��ѯƷ�ֵĺ�Լ�б�
	//product_instrument_list:���صĺ�Լ�б�����map��key��Ʒ��ID��map��ֵ�Ǹ�Ʒ�����н����еĺ�ԼID�б�
	virtual bool QryProductInstrument(std::map<std::string, std::vector<std::string>> *product_instrument_list) = 0;
	
	virtual bool SavBar(const char * productid, int barlevel, std::vector<BARINF> barlist) = 0;

	virtual bool InsertBarToMemTab(BARINF &bar) = 0;

	virtual bool UpdateBarToMemTab(BARINF &bar) = 0;
};


#endif /* TRADERAPI_H */

