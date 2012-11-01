#ifndef		__PASD_SIGNAL_HANDLER_H_
#define		__PASD_SIGNAL_HANDLER_H_

/**
@brief 시스템 시그널 관리 클래스

기본적으로 몇가지 시그널(예:SIGPIPE) 등을 핸들링 하지만
하는 일은 없다.
*/


#include <ace/Event_Handler.h>

class PasSignalHandler : public	ACE_Event_Handler
{
// 멤버함수
public:
	PasSignalHandler();
	virtual ~PasSignalHandler();

	virtual int	handle_signal(int signum, siginfo_t * = 0, ucontext_t * = 0);

private:
	int changeLogLevel();
	int printSystemStatistic();

// 멤버변수
private:
};

#endif // __PASD_SIGNAL_HANDLER_H_
