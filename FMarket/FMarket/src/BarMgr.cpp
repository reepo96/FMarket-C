#include "BarMgr.h"


BarMgr::BarMgr( void )
{
	m_nCurrentMin = 0;
	m_pDBApi = NULL;
}

BarMgr::~BarMgr( void )
{
}

void BarMgr::AddBar(BARINF &bar)
{
	if (m_pDBApi == NULL)
		return;

	char szKey[100] = {0};

	//合约代码+K线级别组成的key
	sprintf(szKey,"%s_%dmin",bar.InstrumentID,bar.BarLeve);
	std::string sInstrumentKey = std::string(szKey);

	//产品ID+K线级别组成的key
	sprintf(szKey,"%s_%dmin",bar.ProductID,bar.BarLeve);
	std::string sProductKey = std::string(szKey);

	std::map<std::string,std::vector<BARINF>>::iterator ProductIt;

	//查看合约的bar是否存在
	std::map<std::string,BARINF>::iterator InstrumentBarIt = m_InstrumentBar.find(sInstrumentKey);
	if( InstrumentBarIt == m_InstrumentBar.end() )//不存在
	{
		m_InstrumentBar.insert(std::map<std::string,BARINF>::value_type(sInstrumentKey,bar));

		m_barpub.SendData(&bar);

		m_pDBApi->InsertBarToMemTab(bar);
	}
	else //存在
	{
		//判断新旧bar是否在同一个时段
		if( ( (*InstrumentBarIt).second.min/bar.BarLeve ) == (bar.min/bar.BarLeve ) ) //同时段
		{
			if( (*InstrumentBarIt).second.HighestPrice < bar.ClosePrice)
				(*InstrumentBarIt).second.HighestPrice = bar.ClosePrice;

			if( (*InstrumentBarIt).second.LowestPrice > bar.ClosePrice)
				(*InstrumentBarIt).second.LowestPrice = bar.ClosePrice;

			(*InstrumentBarIt).second.ClosePrice = bar.ClosePrice;

			//bar的成交金额是当前成交金额的快照，要减去之前的成交金额才是当前K线的成交金额
			(*InstrumentBarIt).second.Turnover = bar.Turnover - (*InstrumentBarIt).second.PreTotTurnover;

			//bar的成交量是当前成交量的快照，要减去之前的成交量才是当前K线的成交量
			(*InstrumentBarIt).second.Volume = bar.Volume - (*InstrumentBarIt).second.PreTotVolume;

			//bar的成交量是当前持仓量的快照，要减去之前的持仓量才是当前K线的成交量
			(*InstrumentBarIt).second.OpenInterest = bar.OpenInterest - (*InstrumentBarIt).second.PreTotOpenInterest;

			m_barpub.SendData(&((*InstrumentBarIt).second));
			m_pDBApi->UpdateBarToMemTab((*InstrumentBarIt).second);
		}
		else //不同时段
		{
			//时段变化，则结算当前bar

			bar.PreTotTurnover = (*InstrumentBarIt).second.PreTotTurnover + (*InstrumentBarIt).second.Turnover;
			bar.PreTotVolume = (*InstrumentBarIt).second.PreTotVolume + (*InstrumentBarIt).second.Volume;
			bar.OpenPrice = bar.HighestPrice = bar.LowestPrice = bar.ClosePrice;

			//查看产品列表中是否存在bar
			ProductIt = m_ProductBarList.find(sProductKey);
			if( ProductIt == m_ProductBarList.end() ) //不存在
			{
				std::vector<BARINF>	barlist;
				barlist.push_back( (*InstrumentBarIt).second );
				m_ProductBarList.insert( std::map<std::string,std::vector<BARINF>>::value_type(sProductKey,barlist));
			}
			else //存在
			{
				(*ProductIt).second.push_back( (*InstrumentBarIt).second );
			}

			m_barpub.SendData(&bar);
			m_pDBApi->UpdateBarToMemTab(bar);

			m_InstrumentBar[sInstrumentKey] = bar;	//覆盖原值
		}
	}
}

void BarMgr::CloseBar(__time64_t	datatime)
{
	if(m_nCurrentMin == 0 )
	{
		m_nCurrentMin = datatime/60;
		return ;
	}
	else if( m_nCurrentMin == (datatime/60) )
	{
		return;
	}

	m_nCurrentMin = datatime/60;

	//遍历m_ProductBarList
	std::map<std::string,std::vector<BARINF>>::iterator ProductIt = m_ProductBarList.begin();
	for(;ProductIt != m_ProductBarList.end();ProductIt++)
	{
		if (m_pDBApi && (*ProductIt).second.size() > 0)
		{
			m_pDBApi->SavBar((*ProductIt).second[0].ProductID, (*ProductIt).second[0].BarLeve, (*ProductIt).second);
		}
		(*ProductIt).second.clear();//清除数据
	}
}