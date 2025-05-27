/*
K�����ݵļ���͹���
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
	std::map<std::string,BARINF>	m_InstrumentBar;	//ÿ����Լ��ͬK�����ݣ�key�ɺ�Լ����+K�߼������

	//ÿ����Ʒ��ͬK�����ݣ�key�ɲ�ƷID+K�߼�����ɣ�ֵ����ͬ��Ʒ��ͬ��Լ��K����ɵ��б�
	//����ͭ��1����K�ߣ�key="cu_1min",ֵ����cu2301,2302,2303�Ȳ�ͬ��Լ��k����ɵ�list����ͭ��ͬ��Լ��1����K�ߴ����ͬһ�ű���
	std::map<std::string,std::vector<BARINF>>	m_ProductBarList;

	unsigned int		m_nCurrentMin;	//��ǰ���ݷ����������ݿ����ʱ�ᷢ������

private:
	DBApi	*m_pDBApi;
	BarPub		m_barpub;

public:
	BarMgr( void );

	virtual ~BarMgr( void );

	void SetDBApi(DBApi *pDBApid) { m_pDBApi = pDBApid; }

	void AddBar(BARINF &bar);

	//����K�ߣ���������յ�����������ʱ���ô˺���
	//datatime����������time_t
	void CloseBar(__time64_t	datatime);

};


#endif /* BARMGR_H */

