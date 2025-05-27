/*
K线数据的计算和管理
*/
#ifndef BARMGR_H
#define BARMGR_H

#include <map>
#include <vector>
#include <string>
#include "CommonDef.h"
#include "DBApi.h"
#include "BarPub.h"

class BarMgr
{

protected:
	std::map<std::string,BARINF>	m_InstrumentBar;	//每个合约不同K线数据，key由合约代码+K线级别组成

	//每个产品不同K线数据，key由产品ID+K线级别组成，值是相同产品不同合约的K线组成的列表
	//例如铜的1分钟K线，key="cu_1min",值则是cu2301,2302,2303等不同合约的k线组成的list。即铜不同合约的1分钟K线存放在同一张表中
	std::map<std::string,std::vector<BARINF>>	m_ProductBarList;

	unsigned int		m_nCurrentMin;	//当前数据分钟数，数据跨分钟时会发动结算

private:
	DBApi	*m_pDBApi;
	BarPub		m_barpub;

public:
	BarMgr( void );

	virtual ~BarMgr( void );

	void SetDBApi(DBApi *pDBApid) { m_pDBApi = pDBApid; }

	void AddBar(BARINF &bar);

	//结算K线，在行情接收到的数据跳秒时调用此函数
	//datatime：行情数据time_t
	void CloseBar(__time64_t	datatime);

};


#endif /* BARMGR_H */

