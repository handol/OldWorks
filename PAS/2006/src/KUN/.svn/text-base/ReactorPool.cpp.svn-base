#include "Common.h"
#include "ReactorPool.h"
#include "PasLog.h"


#include <ace/Singleton.h>
#include <ace/Select_Reactor.h>
#include <ace/Dev_Poll_Reactor.h>
#include <ace/TP_Reactor.h>

/*
ACE_Reactor 의 기본은 ACE_Select_Reactor 이다.
ACE_Dev_Poll_Reactor 는 pipe 에 대해서는 동작하지 않으나 socket 처리에는 문제 없다.

ACE_Dev_Poll_Reactor can generally handle a much larger number of file descriptors than select()-based reactors. 
Furthermore, since `/dev/poll' and `/dev/epoll' both return a set of file descriptors that are active, there is no need
to "walk" the set of file descriptors to determine which ones are active, such as what is done with the select() and poll() system calls.
All returned file descriptors are active. This makes event dispatching very efficient.
*/


/*
reactor 를 여러개 사용하는 방안.

http://www.mamiyami.com/document/cpp_network_prog_volume2/   에서 section 4.3 참조
ACE Programmer's Guide 에서 section 16.4.1 장 ACE_TP_Reactor 참조
*/

#ifdef PAS_HAS_DEV_POLL
typedef	ACE_Dev_Poll_Reactor My_ACE_Reactor;
#else
// Do NOT use TP Reactor because rise exception segment fault
//typedef	ACE_TP_Reactor My_ACE_Reactor;
typedef	ACE_Select_Reactor My_ACE_Reactor;
#endif


static ACE_THR_FUNC_RETURN run_reactor(void *arg)
{
	ReactorInfo *rInfo = static_cast<ReactorInfo *> (arg);
	ACE_Reactor *reactor = rInfo->pReactor;

	rInfo->thrID = ACE_OS::thr_self ();
	reactor->owner (ACE_OS::thr_self ());
	PAS_NOTICE1("Reactor Start [Thr=%d]", rInfo->thrID);
	reactor->run_reactor_event_loop ();
	PAS_NOTICE1("Reactor Stop [Thr=%d]", rInfo->thrID);
	return 0;
}

ReactorPool::ReactorPool()
{
	master = NULL;
	workerList = NULL;
	numWorkers = 0;
	reactorIterator = 0;
}

ReactorPool::~ReactorPool()
{
	deleteAll();
}

ReactorPool* ReactorPool::oneInstance = NULL;
ReactorPool* ReactorPool::instance()
{
	if (oneInstance==NULL)
		oneInstance = new ReactorPool();
	return oneInstance;
	//return ACE_Singleton<ReactorPool, PAS_SYNCH_MUTEX>::instance();
}

 ACE_Reactor* ReactorPool::masterReactor()
 {
	return master;
 }

 
ReactorInfo* ReactorPool::workerReactor()
{
	int numOfScan = 0;
	bool fullScan = (numOfScan == numWorkers);

	// idle 한 reactor 을 찾아 리턴한다
	while(fullScan == false)
	{
		reactorIterator = (reactorIterator+1) % numWorkers;
		if(workerList[reactorIterator].busy == false)
			return &workerList[reactorIterator];
		
		numOfScan++;
		fullScan = (numOfScan == numWorkers);
	}


	// fullscan 을 했는데도 idle 한것이 없다면 그냥 아무거나 하나 리턴
	reactorIterator = (reactorIterator+1) % numWorkers;

	PAS_NOTICE1("All Worker Busy. Return Reactor [%d]", reactorIterator);

	return &workerList[reactorIterator];
}


ACE_Reactor* ReactorPool::createMaster()
{
	master = new ACE_Reactor(new My_ACE_Reactor());

	// 새 master reactor를   global reactor 로 설정하고,
	// 원본 reactor를 return 받는다.
	ACE_Reactor *orginal_reactor = ACE_Reactor::instance (master); 

	// 원본 reactor를 제거한다.
	std::auto_ptr<ACE_Reactor> delete_instance (orginal_reactor);

	PAS_NOTICE1("Original Reactor [%X]", orginal_reactor);
	PAS_NOTICE1("Master Reactor [%X]", master);
	return master;
}

int	ReactorPool::createWorkers(int _numWorkers)
{
	numWorkers = _numWorkers;
	//workerList = new ACE_Reactor*[numWorkers];
	workerList = new ReactorInfo [numWorkers];

	PAS_NOTICE1("ReactorPool::createWorkers() [%d] workers", numWorkers);
	for(int i=0; i<numWorkers; i++)
	{
		workerList[i].pReactor = new ACE_Reactor(new My_ACE_Reactor());
		ACE_Thread_Manager::instance()->spawn(run_reactor, &(workerList[i]) ); 	
	}
	return numWorkers;
}

void	ReactorPool::stopWorkers()
{
	
	for(int i=0; i<numWorkers; i++)
	{
		// end_event_loop() 로는 stop 이 안된다.
		workerList[i].pReactor->end_reactor_event_loop(); 
	}
}

void	ReactorPool::deleteAll()
{
	for(int i=0; i<numWorkers; i++)
	{
		delete workerList[i].pReactor;
	}
	delete workerList;
	delete master;
}

// worker 별  client 수 출력 
void	ReactorPool::print(MyLog *log)
{
	if (log==NULL) return;
	int sum = 0;
	for(int i=0; i<numWorkers; i++)
	{
		//log->logprint(LVL_INFO, "Worker [%d]: Thread=%d, Reactor=%X, Clients=%d\n",
		sum += workerList[i].numClients;
		log->logprint(LVL_INFO, "Worker [%d]: Thread=%d, Reactor=%X, Clients=%d\n",
			i+1, workerList[i].thrID, workerList[i].pReactor, workerList[i].numClients);
	}
	log->logprint(LVL_INFO, "Worker [SUM]: Clients=%d\n", sum);
}

