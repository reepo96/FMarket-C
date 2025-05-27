#include "WholeComInstrument.h"


WholeComInstrument::WholeComInstrument(std::string sProductID, const char *postfix):m_sProductID(sProductID)
{
	sprintf(m_sInstrumentID, "%s%s", sProductID.c_str(), postfix);
}

WholeComInstrument::~WholeComInstrument( void )
{
}

void WholeComInstrument::AddInstrument(std::string sInstrument)
{
	m_sInstrumentList.push_back(sInstrument);
}

bool WholeComInstrument::AddBar(BARINF &bar, BARINF *pNewBar)
{
	if (pNewBar == NULL)
		return false;

	double		TotOpenInterest = 0.0f;
	int	num = 0;

	m_barlist.push_back(bar);
	std::vector<BARINF>::iterator it = m_barlist.begin();
	if (m_barlist.size() == m_sInstrumentList.size())
	{
		//得到全部合约，开始计算本合约
		memcpy(pNewBar, &bar,sizeof(bar));
		strcpy(pNewBar->InstrumentID, m_sInstrumentID);

		num = m_barlist.size();
		for (int i = 0; i < num; i++)
		{
			TotOpenInterest += m_barlist[i].OpenInterest;
		}
		if (TotOpenInterest < 1.0f)
		{
			m_barlist.clear();
			return false;
		}

		//组合合约各值，根据各组成合约成分持仓量占比来计算权重
		pNewBar->HighestPrice = 0.0f;
		pNewBar->OpenPrice = 0.0f;
		pNewBar->LowestPrice = 0.0f;
		pNewBar->ClosePrice = 0.0f;
		pNewBar->PreTotVolume =0;
		pNewBar->Volume = 0;
		pNewBar->PreTotTurnover = 0.0f;
		pNewBar->Turnover = 0.0f;
		pNewBar->PreTotOpenInterest = 0.0f;
		pNewBar->OpenInterest = 0.0f;

		for (int i = 0; i < num; i++)
		{
			pNewBar->HighestPrice += (m_barlist[i].HighestPrice * m_barlist[i].OpenInterest) / TotOpenInterest;
			pNewBar->OpenPrice += (m_barlist[i].OpenPrice * m_barlist[i].OpenInterest) / TotOpenInterest;
			pNewBar->LowestPrice += (m_barlist[i].LowestPrice * m_barlist[i].OpenInterest) / TotOpenInterest;
			pNewBar->ClosePrice += (m_barlist[i].ClosePrice * m_barlist[i].OpenInterest) / TotOpenInterest;
			pNewBar->Volume += (m_barlist[i].Volume * m_barlist[i].OpenInterest) / TotOpenInterest;
			pNewBar->PreTotTurnover += (m_barlist[i].PreTotTurnover * m_barlist[i].OpenInterest) / TotOpenInterest;
			pNewBar->Turnover += (m_barlist[i].Turnover * m_barlist[i].OpenInterest) / TotOpenInterest;
			pNewBar->PreTotOpenInterest += (m_barlist[i].PreTotOpenInterest * m_barlist[i].OpenInterest) / TotOpenInterest;
			pNewBar->OpenInterest += (m_barlist[i].OpenInterest * m_barlist[i].OpenInterest) / TotOpenInterest;
		}

		m_barlist.clear();
		return true;
	}
	else
	{
		return false;
	}
}