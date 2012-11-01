// -*- C++ -*-

/**
@file 

@author

@brief msg watch server 에 주기적으로 보고.

*/

#ifndef WATCH_REPORTER_H
#define	WATCH_REPORTER_H

#include "Common.h"


#define MAX_NAME_LEN            80

typedef struct _msgwatchmsg
{
	char   ServerID[MAX_NAME_LEN];
	char   ServiceID[MAX_NAME_LEN];
	int    MessageCount;
} WatchSvrMesgType;

class WatchReporter:
	public ACE_Task<PAS_SYNCH>
{
public:
	static WatchReporter *instance(ACE_Thread_Manager* threadManager=0);
	
	
	virtual ~WatchReporter(void);

	virtual int svc(void);

	
	void	stop()
	{
		runFlag = false;
	}

	static	void	IncreaseMsgWatchSvr(void);
	static	int	sendMsgWatchServer(void);

private:
	static WatchReporter *oneInstance;
	static	int	msgwatchsvrCount;
	static	int	myPort;
	static	char	myHost[256];
	
	WatchReporter(ACE_Thread_Manager* threadManager=0);
	
	bool runFlag;

};

#endif
