/**
@file AuthAgent.h

@brief PASGW와 AUTH 간의 통신 담당

Pasgw 와 Auth 의 통신을 담당한다.
결과 셋팅은 UserInfo 에 한다.
Auth와 접속이 끊어지면, 장애상태로 변경되며 이후의 모든 인증은 접속이 될 때 까지 성공 처리한다.
장애상태가 되려면, 오류카운트가 일정 수치가 되어야 하며 config 에 설정 할 수 있다.(ErrorCount)
Auth에 재접속 시도는 일정 시간이 지나면 시도한다. config 에 설정 할 수 있다.(RetryDelay)
최초 인증 후 재인증을 하는 시점은, userinfo 가 사라졌거나 config 의 Authentication 시간이 경과 하였을 경우 재인증을 거친다.


@author 현근창
@date 2006.10.30
*/
#ifndef __AUTH_AGENT_H__
#define __AUTH_AGENT_H__

#include "AuthInfo.h"
#include "MyLog.h"
#include "AuthTypes.h"

#include <ace/SOCK_Stream.h>

class AuthAgent : public ACE_Task<PAS_SYNCH>
{
	// Member Functions
public:
	static AuthAgent *instance(ACE_Thread_Manager* threadManager=0);
	void stop();
	int putWork(AUTH::RequestBody &reqBody, UserInfo *userinfo);
	virtual ~AuthAgent();
	virtual int svc();

protected:
	bool connecttoAuth(const char* ip, const int port);
	void closesocket();
	AuthInfoMessageBlock *getAuthInfo(ACE_Message_Block *mesg);

private:
	static AuthAgent *oneInstance;
	AuthAgent(ACE_Thread_Manager* threadManager=0);
	int sendtoAuth(AUTH::RequestBody *sendbuff, int size);
	int recvfromAuth(AUTH::ResponseBody *recvbuff, int size);
	AuthState getAuthState( AUTH::ResponseBody *respBody );
	bool connCheck();
	void incErrCount();
	void decErrCount();
	void responseLog(const AUTH::ResponseBody &responseBody, const char *pment=NULL);
	void requestLog(const AUTH::RequestBody &requestBody, const char *pment=NULL);

	// Member Variables
public:
protected:
private:
	int errCount;
	long errTime;
	bool run;
	bool isconnected;
	MyLog *authlog;
	ACE_SOCK_Stream sock;
};

#endif
