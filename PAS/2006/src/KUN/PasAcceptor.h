#ifndef __PAS_ACCEPT_EVENT_HANDLER_H__
#define __PAS_ACCEPT_EVENT_HANDLER_H__

/**
@brief 단말기 접속 처리

Client(단말기)의 접속을 받아서 ClientHandler 를 생성하고
ACE 이벤트에 등록하는 등 각종 초기화 작업을 한다.
즉, 접속후 처음 시작하는 클래스이다.
*/


#include <ace/Event_Handler.h>
#include <ace/SOCK_Acceptor.h> 
#include <ace/Pipe.h>

#include "Common.h"
#include "SystemStatistic.h"
#include "ClientHandler.h"

class PasAcceptor :
	public ACE_Event_Handler
{
// 멤버 함수
public:
	static unsigned	int	numAccepts;
	
	PasAcceptor(ACE_Reactor* pReactor, int _numofQ=1);
	virtual ~PasAcceptor(void);

	virtual int handle_input(ACE_HANDLE fd = ACE_INVALID_HANDLE);
	virtual int handle_timeout(const ACE_Time_Value &current_time, const void* /* act */);
	
	// 아래 두 함수는 ACE_Event_Handler을 상속받으면 꼭 재정의 해야 한다.
	virtual ACE_HANDLE get_handle(void) const;
	virtual void set_handle(ACE_HANDLE fd);

	int open(int listenPort);
	void stop();

private:
	ClientHandler* createHandler() const;
	
// 멤버 변수
private:
	ACE_SOCK_Acceptor acceptor;
	SystemStatistic* pSysStat;
	int	numOfQueues;
	int	timeTickID;
};

#endif
