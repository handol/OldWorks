// -*- C++ -*-

/**
@file 

@author DMS

@brief pas monitor, msg watch server 에 주기적으로 보고.

*/

#ifndef MONITOR_REPORTER_H
#define	MONITOR_REPORTER_H

#include "Common.h"

typedef struct pasmonmsg
{
	char host[40];
	int port;
	int count;
} PasMonMesgType;

class MonitorReporter:
	public ACE_Task<PAS_SYNCH>
{
public:
	static MonitorReporter *instance(ACE_Thread_Manager* threadManager=0);
	
	
	virtual ~MonitorReporter(void);

	virtual int svc(void);

	
	void	stop()
	{
		runFlag = false;
	}

	static	void	increasePasMon(void);
	static	int	sendPasMon(void);	

private:
	static MonitorReporter *oneInstance;
	static	int	pasmonCount;
	static	int	myPort;
	static	char	myHost[256];
	MonitorReporter(ACE_Thread_Manager* threadManager=0);
	
	bool runFlag;

};

#endif
