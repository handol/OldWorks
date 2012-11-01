#ifndef __PAS_EVENT_HANDLER_H__
#define __PAS_EVENT_HANDLER_H__

/**
@brief 모든 핸들러의 부모 클래스

모든 핸들러의 부모 클래스이며 독립적으로 생성될 수 없다.(상속전용)
ACE_Event_Handler 를 상속 하고 있으므로 ACE Reactor 에 등록하여
이벤트를 받을 수 있다.
ClientHandler, SantaHandler, AuthHandler, CPHandler 등이 이 클래스를 상속받는다.

상대로 부터(단말, CP) 데이터를 받으면 Reactor 가 PasHandler 의 handle_input 이 호출된다.
*/


#include "Common.h"
#include <ace/Event_Handler.h>
#include <ace/Reactor.h>
#include <ace/SOCK_Stream.h>
#include <ace/Message_Block.h>
#include <ace/Mutex.h>
#include <ace/Pipe.h>
#include "WorkInfo.h"
#include "SystemStatistic.h"
#include "ActiveObjectChecker.h"

#include "MyLog.h"

#define	MYID_LEN	(255)
class PasHandler :
	public ACE_Event_Handler, public ActiveObjectChecker
{
// 멤버 타입
public:
	enum CommandID
	{
		CID_CP_Connected,
		CID_CP_Completed,
		CID_CP_Closed,
		CID_CP_SSLData,
		CID_CP_Received_Header,
		CID_CP_Received_PartOfBody,
		CID_Santa_Completed,
		CID_Santa_Closed,
		CID_Santa_TimeOut
	};

	enum HandlerTypeValue
	{
		HT_ClientHandler = 1,
		HT_CpHandler,
		HT_AuthHandler,
		HT_SantaHandler
	};

// 멤버 함수
public:
	PasHandler(ACE_Reactor* pReactor, int handlertype=0);
	virtual ~PasHandler(void);

	virtual int handle_input(ACE_HANDLE fd=ACE_INVALID_HANDLE);
	virtual int handle_output(ACE_HANDLE fd=ACE_INVALID_HANDLE);
	virtual int handle_close(ACE_HANDLE fd, ACE_Reactor_Mask close_mask);
	virtual int handle_exception(ACE_HANDLE fd=ACE_INVALID_HANDLE);
	virtual int handle_exit(ACE_Process *);
	virtual int handle_signal(int signum, siginfo_t* =0, ucontext_t* =0);

	virtual ACE_HANDLE get_handle(void) const;
	virtual void set_handle(ACE_HANDLE fd);

	int _onReceived();
	virtual void onReceived();
	
	virtual int onSendable();
	virtual void onCloseByPeer();

	virtual void onCommand(CommandID cid, PasHandler* pEH, void* arg1 = NULL, void* arg2 = NULL);
	
	void requestClose();

	/// 종료
	/**
	 * 연결된 소켓을 종료하고, 오브젝트를 메모리에서 제거한다.
	 **/
	virtual void close();

	void addEvent(const int eventMask);
	void removeEvent(const int eventMask);

	/// timer 이벤트 등록
	/**
	 * 시간이 만료되면 handle_timeout()이 호출된다.
	 * return 받은 timer id는 removeTimerEvent() 에서 사용한다.
	 *
	 * @see removeTimerEvent()
	 *
	 * @return 성공일경우 timer id, 실패일 경우 -1
	 **/
	long addTimerEvent(const void* arg, const ACE_Time_Value& delay, const ACE_Time_Value& interval = ACE_Time_Value::zero);
	long addTimerEvent(PasHandler* pEH, const void* arg, const ACE_Time_Value& delay, const ACE_Time_Value& interval = ACE_Time_Value::zero);
	void removeTimerEvent(const long timerID);

	void addNotifyEvent();
	void addNotifyEvent(ACE_Event_Handler* pEH);

	const ACE_Time_Value& getReceiveTime() const;
	const ACE_Time_Value& getSendTime() const;
	const ACE_Time_Value& getConnectTime() const;
	const ACE_Time_Value& getConnectionRequestTime() const;
	const ACE_Time_Value& getCreateTime() const;
	const ACE_Time_Value& getCloseTime() const;


	void startTimeTick(const int intervalSec, const int intervalUSec = 0);
	void stopTimeTick();

	void setTraceLog(MyLog *log);

	const char* getMyInfo() const;

	bool isConnected();

	int connect(const host_t &host, int port);

protected:
	/// recv 중에 connection close 를 감지한 경우 호출됨
	virtual void onRecvFail();
	
	/// send 중에 connection close 를 감지한 경우 호출됨
	virtual void onSendFail();

	int deleteSendQueue();
	int enSendQueue(const char* buf, size_t bufSize);

	/// 송신 데이터가 모두 송신되었을 경우
	virtual void onSendQueueEmpty();


	/// 소켓 종료
	void sockClose();

	virtual void onConnect();
	void setJobDone();

	virtual	char* setMyInfo();

	void setCloseAfterSend();
	
	void startReceiveTimer();
	void stopReceiveTimer();
	bool isIdle(const ACE_Time_Value& currentTime, ACE_Time_Value &maxIdleTime);
	
	void startConnectTimer();
	void stopConnectTimer();
	bool isConnectingNow();
	bool isConnectTimeOut(const ACE_Time_Value& current_time, ACE_Time_Value &maxtime);


private:
	int	sendData(const char* buf, size_t bufSize);
	

// 멤버 변수
protected:
	ACE_SOCK_Stream sock;
	ACE_Message_Block recvBuffer;
	SystemStatistic* pSysStat;
	size_t totalReceiveSize;
	int	handlerType;

	MyLog *tracelog; /// phone trace log
	bool jobDone;
	PasMessageQueue sendQueue;

	bool connectedFlag;
	ACE_Time_Value _receiveTime; // 수신 대기 할 때 현재 시간값으로 세팅.
	ACE_Time_Value _sendTime; // 최근 송신 시각
	ACE_Time_Value _connectTime; // 접속 시도 시작할 때 현재 시간값으로 세팅. 접속되면 0 으로 세팅.

	char myinfo[MYID_LEN+1];
	
private:

	long timeTickID;

	bool closeAfterSend; // 단말 요청 헤더에 Connection: close 가 포함된 경우.
	bool __activeObject; // 현재 object가 정상적으로 메모리에 할당된 object 인지를 기록하는 flag

};

#endif
