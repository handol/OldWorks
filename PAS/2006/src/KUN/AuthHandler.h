#ifndef __AUTH_EVENT_HANDLER_H__
#define __AUTH_EVENT_HANDLER_H__

/**
@brief PasAuth 인증 처리

장애가 발생하면 장애카운트를 증가시키고
config 에 설정된 값에 도달하면 장애 상황으로 판단하고
이후의 모든 인증은 성공 처리한다.
장애상황 해지는, 장애카운트가 모두 감소하면 해지된다.
*/

#include "AuthTypes.h"
#include "PasEventHandler.h"
#include "UserInfo.h"


class AuthHandler :
	public PasHandler
{
// 멤버함수
public:
	AuthHandler(ACE_Reactor* pReactor);
	~AuthHandler(void);

	int handle_timeout(const ACE_Time_Value& current_time, const void* act);

	int start(const int seq, const char* pMin, const unsigned int ip, const int port, const int startConn, 
		const int newBrowser, int g3GCode, const char* pMdn = NULL, const char* pMsModel = NULL);

	AuthState getState();
	bool isPassed();
	bool isRemovable();
	void set(UserInfo *userInfo);
	AUTH::RequestBody *getRequestBody();
	UserInfo *getUserInfo();
	virtual void close();

protected:
	virtual void onConnect();
	virtual void onRecvFail();
	virtual void onSendFail();
	virtual char *setMyInfo();

private:
	int connect();
	int request();
	int onCompletedReceiveResponse();
	void init();
	virtual void onReceived();

// 멤버변수
public:

protected:

private:
	AUTH::RequestBody requestBody;

	AUTH::ResponseBody responseBody;

	AuthState authState;

	UserInfo *puserInfo;
};

#endif
