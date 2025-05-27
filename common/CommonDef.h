/************************************************************************
*
功能：自定义行情模块相关结构
作者：
**************************************************************************/ 
#ifndef FUTURESMARKETDEF_H
#define FUTURESMARKETDEF_H

#include <time.h>
#include <vector>
#include "ThostFtdcUserApiDataType.h"

#pragma pack(1)

//K线信息
typedef struct BarInf
{
	TThostFtdcInstrumentIDType	ProductID;		//产品代码
	TThostFtdcInstrumentIDType	InstrumentID;	//合约代码
    char	State;					//状态，0：第一个K线；1：中间K线；2：最后一根K线
	char	BarType;				//K线类型：1-分钟，2-小时，3-日，4-周，5-月
	int		BarLeve;				//K线级别(分钟数，K线不同类型统一转成分钟数）,比如：1分钟、5分钟、15分钟、60分钟(对应时K线)，1440分钟（对应日K线)
    __time64_t	time;				//K线时间
	unsigned int	min;			//K线分钟级别的tick,time/60
    double		HighestPrice;		//最高价
	double		OpenPrice;			//开盘价
    double	    LowestPrice;		//最低价
    double		ClosePrice;			//收盘价
	int			PreTotVolume;		//此K线之前总成交量
    int			Volume;				//成交量
	double		PreTotTurnover;		//此K线之前总成交金额
    double		Turnover;			//成交金额
	double		PreTotOpenInterest;	//此K线之前总持仓量
	double		OpenInterest;		//持仓量
}BARINF;

//zeroMq消息头
typedef struct tagZMQMsgHead
{
	char topic[20];	//消息topic,"marktick","onbar"等
	TThostFtdcInstrumentIDType	ProductID;		//产品代码
}ZMQMSGHEAD;

//策略数据源
typedef struct strategy_ds
{
	int		strategyid;		//对应的策略id
	char	datasource[30];	//数据源名，对应合约ID，比如：cu2305,cu2306,zn2305
	double	starttime;		//K线开始时间，格式参见TB，比如：20230323.093000，如果为0表示从当前时间开始
	double	endtime;		//K线结束时间，格式参见TB，比如：20230323.123020，如果为0表示到当前时间
	char	freq[10];		//K线周期，格式参见TB，比如：1mon,1d,1m,5m
	double	Position;		//仓位
	int		map;			//映射,0-自动，1-手动
	int		strategy;		//自行定义，比如1-委托偏移，2-....
}STRATEGYDS;

//策略
typedef struct strategy
{
	int		strategyid;			//策略id,每个策略有唯一的id
	char	strategyname[100];	//策略名,每个策略有唯一的策略名，根据策略id或者策略名去调用对应的策略代码
	std::vector< STRATEGYDS>	ds;	//策略的数据源
}STRATEGY;

#pragma pack()

//-----end-----
#endif


