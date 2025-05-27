
#ifndef ZEROMQ_BARPUB_H_HEADER_INCLUDED_
#define ZEROMQ_BARPUB_H_HEADER_INCLUDED_

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "CommonDef.h"
#include "ThostFtdcUserApiStruct.h"

#pragma pack(1)


#pragma pack()

class BarPub
{

public:

	BarPub();

	virtual ~BarPub();
	
	int SendData(BARINF *pBar);
	
protected:
	void *m_Context;
	void *m_publisher;
	
	ZMQMSGHEAD m_MsgHead;

};



#endif /* ZEROMQ_BARPUB_H_HEADER_INCLUDED_ */
