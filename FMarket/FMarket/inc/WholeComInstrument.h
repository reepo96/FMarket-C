/*
整体组合合约，由一个品种全部合约组成新的合约
*/
#ifndef WHOLECOMINSTRUMENT_H
#define WHOLECOMINSTRUMENT_H

#include <map>
#include <vector>
#include <string>
#include "CommonDef.h"

class WholeComInstrument
{

protected:
	std::string		m_sProductID;		//本组合合约品种名
	TThostFtdcInstrumentIDType	m_sInstrumentID;	//本组合合约代码
	std::vector<BARINF>		m_barlist;	//组成本合约的其它合约的K线数据
	std::vector<std::string>	m_sInstrumentList;	//组成本合约的合约名

private:

public:
	WholeComInstrument(std::string sProductID,const char *postfix);

	virtual ~WholeComInstrument( void );

	void AddInstrument(std::string sInstrument);

	//如果返回true，则pNewBar被赋予新组合的bar
	bool AddBar(BARINF &bar, BARINF *pNewBar);

};


#endif /* WHOLECOMINSTRUMENT_H */

