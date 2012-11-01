#ifndef __SANTA_EVENT_HANDLER_H__
#define __SANTA_EVENT_HANDLER_H__

/**
@brief Santa 서버 인증 요청 수행

인증 결과(예:인증성공/실패)는 맴버변수 santaState 에 저장된다.
*/

#include "HttpTypes.h"
#include "HttpResponse.h"
#include "HttpRequest.h"
#include "HttpUrlParser.h"
#include "FastString.hpp"
#include "PasEventHandler.h"
#include "SantaTypes.h"
#include "SantaResponse.h"

#include "UserInfo.h"
#include "TransactionInfo.h"

class SantaHandler :
	public PasHandler
{
// 타입선언
public:
	

// 멤버함수 선언
public:
	SantaHandler(ACE_Reactor* pReactor, PasHandler* pRequester);
	virtual ~SantaHandler(void);

	int handle_timeout(const ACE_Time_Value &current_time, const void* act);

	virtual void onReceived();
	
	int request(const host_t& host, const int port, const SANTA::id_t& id, const SANTA::password_t& password, const SANTA::svcID_t& svcID, const SANTA::MDN_t& MDN);
	int request(const host_t& host, const int port, const SANTA::id_t& id, const SANTA::password_t& password, const SANTA::svcID_t& svcID, const SANTA::MSIN_t& MSIN);
	int request(const host_t& host, const int port, const SANTA::id_t& id, const SANTA::password_t& password, const SANTA::svcID_t& svcID, const SANTA::IMSI_t& IMSI);

	SANTA::MDN_t getMDN() { return santaResponse.getMDN(); }
	SANTA::IMSI_t getIMSI() { return santaResponse.getIMSI(); }

	bool isRemovable();
	void set(ACE_Reactor* pReactor, PasHandler* _pRequester);

	/// SANTA 인증이 성공하면 true, 그 외에는 false
	bool isPassed();
	void	setPassed();

	/// SANTA 의 인증 상태를 리턴
	SantaState getState();

	int start(Transaction *trInfo);

	virtual void close();

protected:
	virtual void onRecvFail();
	virtual void onSendFail();
	virtual void onConnect();
	
	void setState(SantaState _state);
	virtual char* setMyInfo();
	void requestLog(const host_t& host, const int port, const SANTA::svcID_t& svcID, const SANTA::MSIN_t& MSIN);
	void responseLog(const char *pment=NULL, SANTA::ResponseCode_t resp="", SANTA::IMSI_t imsi="", SANTA::MDN_t mdn="");
	int santaRequest(const char* phoneNum);
	
private:
	void onReceivedResponseHeader();
	void onReceivedResponseBody();
	int onCompletedReceiveResponse();

	int createRequestHeader(HTTP::header_t* pDestHeader, const url_t& url);
	bool isCompletedReceiveResponseBody();
	void init();
	int connect(const host_t& host, int port);

// 맴버변수 선언
public :
	static MyLog santalog;

private:
	HTTP::Response httpResponse;
	PasHandler* pRequester;
	SANTA::Response santaResponse;
	SantaRecvState state;					// 패킷 송수신 상태
	SantaState santaState;					// 인증 상태

	Transaction *requeterTr;

	host_t santahost;
	int santaport;
};

#endif
