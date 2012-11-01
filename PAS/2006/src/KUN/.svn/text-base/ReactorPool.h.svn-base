#ifndef REACTORPOOL_H__
#define REACTORPOOL_H__

/**
@brief ACE Reactor 관리 클래스

Client(단말기)가 접속하면 ClientHandler 를 생성하고
이후에 발생하는 모든 이벤트를 ACE Reactor 에 일임한다.
이 때, 어떤 Reactor 를 사용할 것인지 ReactorPool 에 물어본다.
ReactorPool 은 Reactor 들을 순서대로 알려준다.
*/


#include <ace/Reactor.h>

#include "ReactorInfo.h"
#include "MyLog.h"


class ReactorPool
{
// 멤버함수
public:
	
	static ReactorPool* instance();
	ACE_Reactor *createMaster();
	 ACE_Reactor *masterReactor();
	 ReactorInfo *workerReactor();
	 int createWorkers(int _numWorkers);
	 void stopWorkers();
	 int getNumWorkers()
	 {
	 	return numWorkers;
	 }

	 void	print(MyLog *log);


private:

	ReactorPool();
	~ReactorPool();
	void deleteAll();

// 멤버변수
public:

private:
	static ReactorPool *oneInstance;

	ACE_Reactor *master;
	ReactorInfo *workerList;
	int	numWorkers;
	int reactorIterator;

	ACE_Thread_Mutex	poolLock;
	
};

#endif
