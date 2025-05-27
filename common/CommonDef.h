/************************************************************************
*
���ܣ��Զ�������ģ����ؽṹ
���ߣ�
**************************************************************************/ 
#ifndef FUTURESMARKETDEF_H
#define FUTURESMARKETDEF_H

#include <time.h>
#include <vector>
#include "ThostFtdcUserApiDataType.h"

#pragma pack(1)

//K����Ϣ
typedef struct BarInf
{
	TThostFtdcInstrumentIDType	ProductID;		//��Ʒ����
	TThostFtdcInstrumentIDType	InstrumentID;	//��Լ����
    char	State;					//״̬��0����һ��K�ߣ�1���м�K�ߣ�2�����һ��K��
	char	BarType;				//K�����ͣ�1-���ӣ�2-Сʱ��3-�գ�4-�ܣ�5-��
	int		BarLeve;				//K�߼���(��������K�߲�ͬ����ͳһת�ɷ�������,���磺1���ӡ�5���ӡ�15���ӡ�60����(��ӦʱK��)��1440���ӣ���Ӧ��K��)
    __time64_t	time;				//K��ʱ��
	unsigned int	min;			//K�߷��Ӽ����tick,time/60
    double		HighestPrice;		//��߼�
	double		OpenPrice;			//���̼�
    double	    LowestPrice;		//��ͼ�
    double		ClosePrice;			//���̼�
	int			PreTotVolume;		//��K��֮ǰ�ܳɽ���
    int			Volume;				//�ɽ���
	double		PreTotTurnover;		//��K��֮ǰ�ܳɽ����
    double		Turnover;			//�ɽ����
	double		PreTotOpenInterest;	//��K��֮ǰ�ֲܳ���
	double		OpenInterest;		//�ֲ���
}BARINF;

//zeroMq��Ϣͷ
typedef struct tagZMQMsgHead
{
	char topic[20];	//��Ϣtopic,"marktick","onbar"��
	TThostFtdcInstrumentIDType	ProductID;		//��Ʒ����
}ZMQMSGHEAD;

//��������Դ
typedef struct strategy_ds
{
	int		strategyid;		//��Ӧ�Ĳ���id
	char	datasource[30];	//����Դ������Ӧ��ԼID�����磺cu2305,cu2306,zn2305
	double	starttime;		//K�߿�ʼʱ�䣬��ʽ�μ�TB�����磺20230323.093000�����Ϊ0��ʾ�ӵ�ǰʱ�俪ʼ
	double	endtime;		//K�߽���ʱ�䣬��ʽ�μ�TB�����磺20230323.123020�����Ϊ0��ʾ����ǰʱ��
	char	freq[10];		//K�����ڣ���ʽ�μ�TB�����磺1mon,1d,1m,5m
	double	Position;		//��λ
	int		map;			//ӳ��,0-�Զ���1-�ֶ�
	int		strategy;		//���ж��壬����1-ί��ƫ�ƣ�2-....
}STRATEGYDS;

//����
typedef struct strategy
{
	int		strategyid;			//����id,ÿ��������Ψһ��id
	char	strategyname[100];	//������,ÿ��������Ψһ�Ĳ����������ݲ���id���߲�����ȥ���ö�Ӧ�Ĳ��Դ���
	std::vector< STRATEGYDS>	ds;	//���Ե�����Դ
}STRATEGY;

#pragma pack()

//-----end-----
#endif


