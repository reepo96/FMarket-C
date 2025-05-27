#ifndef LogControl_H
#define LogControl_H

#include <stdio.h>

class LogControl
{
public:
		typedef enum LogLevel{LDEBUG,LINFO,LWARNING,LERROR} LOGLEVEL;

protected:
		FILE * m_pFile;
		unsigned int m_hEvent;
		char			m_szLogInfo[1024];

private:

public:
	LogControl( void );

	virtual ~LogControl( void );
	
	void WriteLog2File( LOGLEVEL logLevel, const char * msg... );
};


#endif /* LogControl_H */

