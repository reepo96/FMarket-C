#include "BarPub.h"

#include "zmq.h"

BarPub::BarPub()
{
	int rc=0;

	m_Context = NULL;
	m_publisher = NULL;
	
	m_Context = zmq_ctx_new ();
	
	printf("It's PUB mq!!\n");
	m_publisher = zmq_socket(m_Context, ZMQ_PUB);
	//rc = zmq_bind (m_publisher, "tcp://*:5555");
	zmq_bind(m_publisher, "ipc:///barmsg");
}

BarPub:: ~BarPub()
{
	if( m_publisher )
		zmq_close (m_publisher);
	if( m_Context )
 		zmq_ctx_destroy (m_Context);
 		
 	m_Context = NULL;
	m_publisher = NULL;
}


int BarPub::SendData(BARINF *pBar)
{
	if (m_publisher == NULL || pBar == NULL)
	{
		printf("m_publisher=0x%x,product=0x%x,pMarketData=0x%x fail\n", m_publisher, pBar);
		return false;
	}

	int iSendLen = 0;

	strcpy(m_MsgHead.topic, "onbar");
	strcpy(m_MsgHead.ProductID, pBar->ProductID);
	
	iSendLen = zmq_send(m_publisher, (void *)&m_MsgHead, sizeof(m_MsgHead), ZMQ_SNDMORE);
	if (iSendLen == -1)
	{
		printf("zmq_send fail1\n");
		return false;
	}

	iSendLen = zmq_send(m_publisher, pBar, sizeof(BARINF), 0);
	if (iSendLen == -1)
	{
		printf("zmq_send fail1\n");
		return false;
	}

	return true;
}


