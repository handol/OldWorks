// -*- C++ -*-

/**
@file 

@author

@brief msg watch server 에 주기적으로 보고.

*/

#ifndef MY_ACCEPTOR_H
#define MY_ACCEPTOR_H

#include "Common.h"

class MyAcceptor:
	public ACE_Task<PAS_SYNCH>
{
public:
	static MyAcceptor *instance(ACE_Thread_Manager* threadManager=0);
	
	
	virtual ~MyAcceptor(void);

	virtual int svc(void);

	int startListen(int listenPort);

	int waitConnection(int usec);
	int acceptConnection();
	int handleConnection();
	
	void	stop();

	static	void	IncreaseMsgWatchSvr(void);
	static	int	sendMsgWatchServer(void);

private:
	static MyAcceptor *oneInstance;

	int	mySockfd;
	int	myPort;
	char	myHost[256];

	int numAccepts;
	int numL4Checks;
	MyAcceptor(ACE_Thread_Manager* threadManager=0);
	
	bool runFlag;

};

#endif
