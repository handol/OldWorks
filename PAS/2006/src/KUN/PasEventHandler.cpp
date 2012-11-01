#include "Common.h"
#include "PasEventHandler.h"
#include "WorkInfo.h"
#include "PasLog.h"
#include "Mutex.h"
#include "SystemStatistic.h"
#include "MemoryPoolManager.h"
#include <ace/SOCK_Connector.h>

PasHandler::PasHandler(ACE_Reactor* pReactor, int handlertype)
: ACE_Event_Handler(pReactor)
{
	__activeObject = true;

	handlerType = handlertype;
	// @ 중요 4K == Memory pool 의 제일 작은 Block 크기와 동일하게.
	// Http Request/ Response 의 버퍼 크기와 동일하게.

	ACE_ASSERT(pReactor == reactor());

	myinfo[0] = 0;
	
	connectedFlag = false;
	
	recvBuffer.size(16 * 1024); 

	closeAfterSend = false;
	
	timeTickID = -1;
	totalReceiveSize = 0;

	_receiveTime.set(0, 0);
	_connectTime.set(0, 0);
	_sendTime.set(0, 0);
	
	tracelog = NULL;
	jobDone = false;
	
	pSysStat = SystemStatistic::instance();
}

PasHandler::~PasHandler(void)
{
	if(timeTickID >= 0)
		stopTimeTick();
		
	if(sendQueue.size() > 0)
		deleteSendQueue();

	ACE_Reactor_Mask mask = ACE_Event_Handler::ALL_EVENTS_MASK | ACE_Event_Handler::DONT_CALL;
	removeEvent(mask);

	__activeObject = false;
}

int PasHandler::deleteSendQueue()
{
	int qsize = sendQueue.size();
	if (qsize > 0)
	{
		PAS_INFO2("PasHandler:: sendQueue size=%d [%s]", qsize, getMyInfo());
		while(!sendQueue.empty())
		{
			ACE_Message_Block* pMB = sendQueue.front();
			MemoryPoolManager::instance()->free(pMB);
			sendQueue.erase(sendQueue.begin());
		}
	}
	return qsize;
}


int PasHandler::handle_input(ACE_HANDLE fd)
{
	ACE_ASSERT(valid());

	PAS_TRACE2("PasEventHandler(%d)::handle_input - fd[%d]", handlerType, fd);
	
	if (fd < 0)
	{
		PAS_NOTICE2("PasHandler::handle_input() called with FD=%d - %s", fd, getMyInfo());
		onCloseByPeer();
		return -1;
	}

	if (sock.get_handle() < 0)
	{
		PAS_NOTICE2("PasHandler::handle_input() called with SOCK=%d - %s", sock.get_handle() , getMyInfo());
		onRecvFail();
		return -1;
	}

	_onReceived();

	return 0; // hold read_mask
}


int PasHandler::_onReceived()
{
	// 데이터 수신
	int recvSize = sock.recv(recvBuffer.wr_ptr(), recvBuffer.space());

	// 데이터 수신
	if(recvSize > 0)
	{
		PAS_TRACE2("%d bytes received from peer. fd[%d]", recvSize, get_handle());

		totalReceiveSize += recvSize;
		ACE_ASSERT(recvSize <= recvBuffer.space());
		recvBuffer.wr_ptr(recvSize);
		onReceived();
	}
	
	// 접속 종료
	else if(recvSize == 0 || ACE_OS::last_error() == ECONNRESET)
	{
		PAS_TRACE("Connection closed by peer.");
		onCloseByPeer();
		return -1;
	}

	// 수신 실패
	else
	{
		PAS_NOTICE1("Receive fail. %s", ACE_OS::strerror(ACE_OS::last_error()));
		onRecvFail();
		return -1;
	}

	return 0;
}


int PasHandler::handle_output(ACE_HANDLE fd)
{
	PAS_TRACE1("PasHandler::handle_output >> fd[%d]", fd);

	if (fd < 0)
	{
		PAS_NOTICE2("PasHandler::handle_input() called with FD=%d - %s", fd, getMyInfo());
		onSendFail();
		return -1;
	}

	if (sock.get_handle() < 0)
	{
		PAS_NOTICE2("PasHandler::handle_input() called with SOCK=%d - %s", sock.get_handle() , getMyInfo());
		onSendFail();
		return -1;
	}
	
	ACE_ASSERT(get_handle() == fd);

	if(isConnectingNow())
		onConnect();

	if (onSendable() >= 0)
	{
		// 송신할 데이터가 없다면
		if(sendQueue.empty())
			return -1; // remove write_mask

		// 2006.09.29
		if (closeAfterSend)
			requestClose();
	}
	else
	{
		onSendFail();
		//return -1; // remove write_mask
	}

	return 0; // hold write_mask
}



/**
return -1 : on error
return sendSize: when the data was sent completely
return sendSize: when the data was sent partly, or sending is pending
*/
int	PasHandler::sendData(const char* buf, size_t bufSize)
{
	if (!isConnected())
	{
		PAS_INFO1("Send fail. Because sock is not connected. %s", getMyInfo());
		PHTR_INFO1("PasHandler::sendData() called with closed sock. %s", getMyInfo());
		return -1;
	}

	// 송신한 시각을 기록.
	_sendTime = ACE_OS::gettimeofday();
	
	// 데이터 송신
	int sendSize = sock.send(buf, bufSize, &ACE_Time_Value::zero);
	
	// 송신 에러
	if(sendSize < 0)
	{
		if(errno == ETIME || errno == EWOULDBLOCK)
		{
			// 소켓 송신 버퍼가 가득 차서 송신 못 함
			PAS_TRACE1("Socket send buffer full. %s", getMyInfo());
			PHTR_DEBUG1("Socket send buffer full. %s", getMyInfo());
			return 0;
		}

		PAS_INFO3("Send error. FD[%d] %s %s", sock.get_handle(), ACE_OS::strerror(ACE_OS::last_error()), getMyInfo());
		PHTR_INFO3("Send error. FD[%d] %s %s", sock.get_handle(), ACE_OS::strerror(ACE_OS::last_error()), getMyInfo());
		return -1;
	}

	// 버퍼의 데이터를 모두 송신
	if(sendSize == static_cast<int>(bufSize))
	{
		PAS_TRACE1("Data fully sent. %s", getMyInfo());
		PHTR_DEBUG1("Data fully sent. %s", getMyInfo());
	}

	// 일부만 송신
	else
	{
		PAS_TRACE1("Data partly sent. %s", getMyInfo());
		PHTR_DEBUG1("Data partly sent. %s", getMyInfo());
	}
	
	return sendSize;
}


int PasHandler::onSendable()
{
	bool isError = false;

	// 데이터 송신
	while(!sendQueue.empty())
	{
		ACE_Message_Block* pSendData = sendQueue.front();

		if (pSendData == NULL)
		{
			PAS_NOTICE1("PasHandler::onSendable() - sendQueue.front() is NULL - %s",
				getMyInfo());
			isError = true;
			break;
		}
		
		// 데이터 송신
		int sendSize = sendData(pSendData->rd_ptr(), pSendData->length());

		if (sendSize == 0)
			break;
		
		if (sendSize < 0)
		{
			if (isConnected()) 
			{
				// connect 성공 후에 send fail 한 경우에만 실패 처리 하자.
				// connect 자체가 실패한 경우에도 handle_output() 에서  일단 onConnect() 를 호출하므로 connectedFlag 가 true 가 되었다가, 아래에서 false 로 변한다.
				PAS_INFO1("onSendable >> Send Fail. Calling onSendFail(). %s", getMyInfo());
				connectedFlag = false;
				isError = true;
			}		 
			else
			{
				PAS_INFO1("onSendable >> Send fail. Not connected. %s", getMyInfo());
			}

			break;
		}
		
		// 버퍼의 데이터를 모두 송신
		if(sendSize == static_cast<int>(pSendData->length()))
		{
			sendQueue.erase(sendQueue.begin());
			MemoryPoolManager::instance()->free(pSendData);

			if(sendQueue.empty())
				onSendQueueEmpty();
		}
		// 일부만 송신
		else
		{
			pSendData->rd_ptr(sendSize);
		}
	}
	
	if (isError)
		return -1;
	else
		return 0;
	
}


/**
handle_close()는 socket close 와 관련이 없다.
event 가 remove 될 때 호출된다.
*/
int PasHandler::handle_close(ACE_HANDLE fd, ACE_Reactor_Mask close_mask)
{
	if(close_mask == READ_MASK)
	{
		PAS_TRACE1("PasHandler::handle_close >> READ_MASK, fd[%d]", fd);
	}

	if(close_mask == WRITE_MASK)
	{
		PAS_TRACE1("PasHandler::handle_close >> WRITE_MASK, fd[%d]", fd);
	}

	if(close_mask == ACCEPT_MASK)
	{
		PAS_TRACE1("PasHandler::handle_close >> ACCEPT_MASK, fd[%d]", fd);
	}

	if(close_mask == CONNECT_MASK)
	{
		PAS_TRACE1("PasHandler::handle_close >> CONNECT_MASK, fd[%d]", fd);
	}

	if(close_mask == TIMER_MASK)
	{
		PAS_TRACE1("PasHandler::handle_close >> TIMER_MASK, fd[%d]", fd);
	}

	return 0;
}

int PasHandler::handle_exception(ACE_HANDLE /* fd */)
{
	PAS_INFO1("%s handle_exception() MASK %X", getMyInfo());
	PHTR_INFO1("%s handle_exception() MASK %X", getMyInfo());

	return 0;
}

int PasHandler::handle_exit(ACE_Process *)
{
	PAS_NOTICE1("%s handle_exit()", getMyInfo());
	PHTR_NOTICE1("%s handle_exit()", getMyInfo());

	return 0;
}

int PasHandler::handle_signal(int signum, siginfo_t* /* pSigInfo */, ucontext_t* /* pUContext */)
{
	PAS_NOTICE2("%s handle_signal()", getMyInfo(), signum);
	PHTR_NOTICE2("%s handle_signal()", getMyInfo(), signum);
	return 0;
}

void PasHandler::close()
{
	connectedFlag = false;
	
	ACE_Reactor_Mask mask = ACE_Event_Handler::ALL_EVENTS_MASK | ACE_Event_Handler::DONT_CALL;
	removeEvent(mask);
	stopTimeTick();

	sock.close();

	if (! sendQueue.empty())
	{
		PAS_INFO1("close() SEND Q NOT EMPTY [%s]", getMyInfo());
	}
}

void PasHandler::onReceived()
{
	PAS_TRACE1("PasHandler::onReceived >> fd[%d]", get_handle());
}

void PasHandler::onCommand(CommandID /* cid */, PasHandler* /* pEH */, void* /* arg1 */, void* /* arg2 */)
{
	PAS_TRACE1("PasHandler::onReceived >> fd[%d]", get_handle());
}

ACE_HANDLE PasHandler::get_handle(void) const
{
	return sock.get_handle();
}

void PasHandler::set_handle(ACE_HANDLE fd)
{
	sock.set_handle(fd);
}

void PasHandler::addEvent(const int eventMask)
{
	reactor()->register_handler(this, eventMask);
	
	if(eventMask == READ_MASK)
	{
		PAS_TRACE1("PasHandler::addEvent >> READ_MASK, fd[%d]", get_handle());
	}

	if(eventMask == WRITE_MASK)
	{
		PAS_TRACE1("PasHandler::addEvent >> WRITE_MASK, fd[%d]", get_handle());
	}

	if(eventMask == ACCEPT_MASK)
	{
		PAS_TRACE1("PasHandler::addEvent >> ACCEPT_MASK, fd[%d]", get_handle());
	}

	if(eventMask == CONNECT_MASK)
	{
		PAS_TRACE1("PasHandler::addEvent >> CONNECT_MASK, fd[%d]", get_handle());
	}

	if(eventMask == TIMER_MASK)
	{
		PAS_TRACE1("PasHandler::addEvent >> TIMER_MASK, fd[%d]", get_handle());
	}
}

void PasHandler::removeEvent(const int eventMask)
{
	reactor()->remove_handler(this, eventMask);
	
	if(eventMask == READ_MASK)
	{
		PAS_TRACE1("PasHandler::removeEvent >> READ_MASK, fd[%d]", get_handle());
	}

	if(eventMask == WRITE_MASK)
	{
		PAS_TRACE1("PasHandler::removeEvent >> WRITE_MASK, fd[%d]", get_handle());
	}

	if(eventMask == ACCEPT_MASK)
	{
		PAS_TRACE1("PasHandler::removeEvent >> ACCEPT_MASK, fd[%d]", get_handle());
	}

	if(eventMask == CONNECT_MASK)
	{
		PAS_TRACE1("PasHandler::removeEvent >> CONNECT_MASK, fd[%d]", get_handle());
	}

	if(eventMask == TIMER_MASK)
	{
		PAS_TRACE1("PasHandler::removeEvent >> TIMER_MASK, fd[%d]", get_handle());
	}
}

long PasHandler::addTimerEvent(PasHandler* pEH, const void* arg, const ACE_Time_Value& delay, const ACE_Time_Value& interval)
{
	PAS_TRACE2("ddTimerEvent() %s Handler[%X]", getMyInfo(),  pEH);

	return reactor()->schedule_timer(pEH, arg, delay, interval);
}

long PasHandler::addTimerEvent(const void* arg, const ACE_Time_Value& delay, const ACE_Time_Value& interval)
{
	PAS_TRACE1("addTimerEvent() %s", getMyInfo());

	return reactor()->schedule_timer(this, arg, delay, interval);
}

void PasHandler::removeTimerEvent(const long timerID)
{
	PAS_TRACE2("removeTimerEvent() %s time[%d]", getMyInfo(),  timerID);

	reactor()->cancel_timer(timerID);
}

void PasHandler::addNotifyEvent(ACE_Event_Handler* pEH)
{
	PAS_TRACE2("addNotifyEvent() %s Handler[%X]", getMyInfo(),  pEH);

	reactor()->notify(pEH);
}

void PasHandler::addNotifyEvent()
{
	PAS_TRACE1("addNotifyEvent() %s", getMyInfo());

	reactor()->notify(this);
}

/**
return -1: error
return 0: when sent directly
return 1: when data was put in Q
*/
int PasHandler::enSendQueue(const char* buf, size_t bufSize)
{
	PAS_TRACE1("PasHandler::enSendQueue >> fd[%d]", get_handle());
	PHTR_DEBUG1("PasHandler::enSendQueue >> fd[%d]", get_handle());

	int	sendSize = 0;
	if(isConnected() && sendQueue.empty())
	{
		sendSize = sendData(buf, bufSize);
			
		if (sendSize > 0)
		{
			PAS_TRACE3("Send directly %d/%d bytes. %s", 
				sendSize, bufSize, getMyInfo());

			if(sendSize == static_cast<int>(bufSize))
				return 0; // completely sent
		}		

		if (sendSize < 0)
		{
			PAS_INFO1("Send fail. %s", getMyInfo());
			onSendFail();
			return -1;
		}
	}
	
	// 다 못 보낸 경우 Q 를 이용해서 보낸다.
	if (sendQueue.empty())
		addEvent(WRITE_MASK);
		
	int remainDataSize = bufSize - sendSize;
	ACE_Message_Block* pMB = MemoryPoolManager::instance()->alloc(remainDataSize);
	int copyResult = pMB->copy(buf + sendSize, remainDataSize);
	if(copyResult < 0)
	{
		PAS_ERROR1("MessageBlock 복사 실패, fd[%d]", get_handle());
		return -1;
	}

	sendQueue.push_back(pMB);

	PAS_TRACE1("Put %d bytes to send queue.", pMB->length());
	return 1;
}



void PasHandler::startTimeTick(const int intervalSec, const int intervalUSec)
{
	PAS_TRACE1("PasHandler::startTimeTick [%X]", this);

	if(timeTickID < 0)
	{
		ACE_Time_Value interval(intervalSec, intervalUSec);
		timeTickID = addTimerEvent(NULL, interval, interval);
	}
}

void PasHandler::stopTimeTick()
{
	PAS_TRACE2("PasHandler(%d)::stopTimeTick [%X]", handlerType, this);

	if(timeTickID >= 0)
	{
		removeTimerEvent(timeTickID);
		timeTickID = -1;
	}
}

void PasHandler::startReceiveTimer()
{
	_receiveTime = ACE_OS::gettimeofday();
}

void PasHandler::stopReceiveTimer()
{
	_receiveTime.set(0, 0);
}

bool PasHandler::isIdle( const ACE_Time_Value& currentTime, ACE_Time_Value &maxIdleTime )
{
	if(_receiveTime == ACE_Time_Value::zero && _sendTime == ACE_Time_Value::zero)
	{
		PAS_TRACE("Result of idle check is busy.");
		return false;
	}

	if( (currentTime > _receiveTime + maxIdleTime) && (currentTime > _sendTime + maxIdleTime) )
	{
		PAS_TRACE("Result of idle check is idle.");
		return true;
	}

	PAS_TRACE("Result of idle check is busy.");

	return false;
}

void PasHandler::startConnectTimer()
{
	_connectTime = ACE_OS::gettimeofday();
}

void PasHandler::stopConnectTimer()
{
	_connectTime.set(0, 0);
}

bool PasHandler::isConnectingNow()
{
	return (_connectTime != ACE_Time_Value::zero);
}

bool PasHandler::isConnectTimeOut(const ACE_Time_Value& current_time, ACE_Time_Value &maxtime)
{
	if (_connectTime != ACE_Time_Value::zero && current_time - _connectTime >= maxtime)
		return true;
	else
		return false;
}

void PasHandler::requestClose()
{
	if (sendQueue.empty())
		sock.close_writer();
	else
		setCloseAfterSend();
}

void PasHandler::sockClose()
{
	sock.close();
}

bool PasHandler::isConnected()
{
	// 2007.1.3  -- 검사 강화.
	return (connectedFlag && get_handle() > 0);
	//return (get_handle() > 0);
}
	

void PasHandler::onConnect()
{
	connectedFlag = true;
	stopConnectTimer();
	PAS_TRACE("PasHandler::onConnect");
	addEvent(READ_MASK); // @dahee 20006.10.02
}

int PasHandler::connect(const host_t &host, int port)
{
	// 이미 접속되어 있음
	if(isConnected())
	{
		PAS_DEBUG("Already connected.");
		return -1;
	}

	// CP에 접속 요청
	PAS_TRACE2("Try connect to CP[%s:%d]", host.toStr(), port);
	PHTR_DEBUG2("PasHandler::connect >> Try connect to CP[%s:%d]", host.toStr(), port);

	ACE_INET_Addr addr(port, host);
	ACE_SOCK_Connector connector;
	if(connector.connect(sock, addr, &ACE_Time_Value::zero) < 0)
	{
		// EWOULDBLOCK 은 정상적인 경우이다.
		if(errno != EWOULDBLOCK)
		{
			PAS_INFO2("Connection fail. %s %s", ACE_OS::strerror(ACE_OS::last_error()), getMyInfo());
			PHTR_INFO2("PasHandler::connect FAIL %s %s", ACE_OS::strerror(ACE_OS::last_error()), getMyInfo());

			return -1;
		}
	}

	// connected 여부 확인을 보다 정확하게 하기 위한 안전장치
	if (sock.get_handle() <= 0)
	{
		PAS_NOTICE2("Connection fail. %s %s", ACE_OS::strerror(ACE_OS::last_error()), getMyInfo());
		PHTR_NOTICE2("PasHandler::socket creation FAIL %s %s", ACE_OS::strerror(ACE_OS::last_error()), getMyInfo());
		return -1;
	}

	// set SO_LINGER
/*	linger lingtime;
	lingtime.l_onoff = 1;
	lingtime.l_linger = 5;
	sock.set_option( SOL_SOCKET, SO_LINGER, (void*)&lingtime, sizeof(linger) );
*/

	startConnectTimer();
	addEvent(CONNECT_MASK);

	return 0;
}

/// recv 중에 connection close 를 감지한 경우 호출됨
void PasHandler::onRecvFail()
{
	PAS_INFO("onRecvFail");
	close(); 
}

/// send 중에 connection close 를 감지한 경우 호출됨
void PasHandler::onSendFail()
{
	PAS_INFO("onSendFail");
	close();
}

void PasHandler::setJobDone()
{
	PAS_TRACE("setJobDone");
	PHTR_DEBUG("setJobDone");

	jobDone = true;
}

void PasHandler::setTraceLog(MyLog *log)
{
	tracelog = log;
}

char* PasHandler::setMyInfo()
{
	snprintf(myinfo, MYID_LEN, "Type[%d] Sock[%d]",
		handlerType, get_handle());
	return myinfo;
	
	snprintf(myinfo, MYID_LEN, "Obj[%p] Type[%d] Sock[%d] totalRecv[%d] jobDone[%d]",
		this, handlerType, get_handle(), totalReceiveSize, jobDone);
	return myinfo;
}

const char* PasHandler::getMyInfo() const
{
	return myinfo;
}

void PasHandler::setCloseAfterSend()
{
	closeAfterSend = true;
}

void PasHandler::onSendQueueEmpty()
{
	PAS_TRACE("Send completed.");
}

void PasHandler::onCloseByPeer()
{
	PAS_TRACE("Close by peer.");
	close();
}
