
#ifndef ZEROMQ_MARKETPUB_H_HEADER_INCLUDED_
#define ZEROMQ_MARKETPUB_H_HEADER_INCLUDED_

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>

#include "CommonDef.h"
#include "ThostFtdcUserApiStruct.h"

#pragma pack(1)


#pragma pack()

class MarketPub
{

public:

	MarketPub();

	virtual ~MarketPub();
	
	int SendData(const char *product, CThostFtdcDepthMarketDataField *pMarketData);
	
protected:
	void *m_Context;
	void *m_publisher;
	
	ZMQMSGHEAD m_MsgHead;

};



#endif /* ZEROMQ_MARKETPUB_H_HEADER_INCLUDED_ */
