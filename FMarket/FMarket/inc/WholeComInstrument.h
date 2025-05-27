/*
������Ϻ�Լ����һ��Ʒ��ȫ����Լ����µĺ�Լ
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
	std::string		m_sProductID;		//����Ϻ�ԼƷ����
	TThostFtdcInstrumentIDType	m_sInstrumentID;	//����Ϻ�Լ����
	std::vector<BARINF>		m_barlist;	//��ɱ���Լ��������Լ��K������
	std::vector<std::string>	m_sInstrumentList;	//��ɱ���Լ�ĺ�Լ��

private:

public:
	WholeComInstrument(std::string sProductID,const char *postfix);

	virtual ~WholeComInstrument( void );

	void AddInstrument(std::string sInstrument);

	//�������true����pNewBar����������ϵ�bar
	bool AddBar(BARINF &bar, BARINF *pNewBar);

};


#endif /* WHOLECOMINSTRUMENT_H */

