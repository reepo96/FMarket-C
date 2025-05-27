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

	//��Լ����+K�߼�����ɵ�key
	sprintf(szKey,"%s_%dmin",bar.InstrumentID,bar.BarLeve);
	std::string sInstrumentKey = std::string(szKey);

	//��ƷID+K�߼�����ɵ�key
	sprintf(szKey,"%s_%dmin",bar.ProductID,bar.BarLeve);
	std::string sProductKey = std::string(szKey);

	std::map<std::string,std::vector<BARINF>>::iterator ProductIt;

	//�鿴��Լ��bar�Ƿ����
	std::map<std::string,BARINF>::iterator InstrumentBarIt = m_InstrumentBar.find(sInstrumentKey);
	if( InstrumentBarIt == m_InstrumentBar.end() )//������
	{
		m_InstrumentBar.insert(std::map<std::string,BARINF>::value_type(sInstrumentKey,bar));

		m_barpub.SendData(&bar);

		m_pDBApi->InsertBarToMemTab(bar);
	}
	else //����
	{
		//�ж��¾�bar�Ƿ���ͬһ��ʱ��
		if( ( (*InstrumentBarIt).second.min/bar.BarLeve ) == (bar.min/bar.BarLeve ) ) //ͬʱ��
		{
			if( (*InstrumentBarIt).second.HighestPrice < bar.ClosePrice)
				(*InstrumentBarIt).second.HighestPrice = bar.ClosePrice;

			if( (*InstrumentBarIt).second.LowestPrice > bar.ClosePrice)
				(*InstrumentBarIt).second.LowestPrice = bar.ClosePrice;

			(*InstrumentBarIt).second.ClosePrice = bar.ClosePrice;

			//bar�ĳɽ�����ǵ�ǰ�ɽ����Ŀ��գ�Ҫ��ȥ֮ǰ�ĳɽ������ǵ�ǰK�ߵĳɽ����
			(*InstrumentBarIt).second.Turnover = bar.Turnover - (*InstrumentBarIt).second.PreTotTurnover;

			//bar�ĳɽ����ǵ�ǰ�ɽ����Ŀ��գ�Ҫ��ȥ֮ǰ�ĳɽ������ǵ�ǰK�ߵĳɽ���
			(*InstrumentBarIt).second.Volume = bar.Volume - (*InstrumentBarIt).second.PreTotVolume;

			//bar�ĳɽ����ǵ�ǰ�ֲ����Ŀ��գ�Ҫ��ȥ֮ǰ�ĳֲ������ǵ�ǰK�ߵĳɽ���
			(*InstrumentBarIt).second.OpenInterest = bar.OpenInterest - (*InstrumentBarIt).second.PreTotOpenInterest;

			m_barpub.SendData(&((*InstrumentBarIt).second));
			m_pDBApi->UpdateBarToMemTab((*InstrumentBarIt).second);
		}
		else //��ͬʱ��
		{
			//ʱ�α仯������㵱ǰbar

			bar.PreTotTurnover = (*InstrumentBarIt).second.PreTotTurnover + (*InstrumentBarIt).second.Turnover;
			bar.PreTotVolume = (*InstrumentBarIt).second.PreTotVolume + (*InstrumentBarIt).second.Volume;
			bar.OpenPrice = bar.HighestPrice = bar.LowestPrice = bar.ClosePrice;

			//�鿴��Ʒ�б����Ƿ����bar
			ProductIt = m_ProductBarList.find(sProductKey);
			if( ProductIt == m_ProductBarList.end() ) //������
			{
				std::vector<BARINF>	barlist;
				barlist.push_back( (*InstrumentBarIt).second );
				m_ProductBarList.insert( std::map<std::string,std::vector<BARINF>>::value_type(sProductKey,barlist));
			}
			else //����
			{
				(*ProductIt).second.push_back( (*InstrumentBarIt).second );
			}

			m_barpub.SendData(&bar);
			m_pDBApi->UpdateBarToMemTab(bar);

			m_InstrumentBar[sInstrumentKey] = bar;	//����ԭֵ
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

	//����m_ProductBarList
	std::map<std::string,std::vector<BARINF>>::iterator ProductIt = m_ProductBarList.begin();
	for(;ProductIt != m_ProductBarList.end();ProductIt++)
	{
		if (m_pDBApi && (*ProductIt).second.size() > 0)
		{
			m_pDBApi->SavBar((*ProductIt).second[0].ProductID, (*ProductIt).second[0].BarLeve, (*ProductIt).second);
		}
		(*ProductIt).second.clear();//�������
	}
}