#include <algorithm>
#include <strings.h>
#include <ctype.h>
#include <errno.h>

#include "ClientHandler.h"
#include "AuthHandler.h"
#include "SantaEventHandler.h"
#include "Common.h"
#include "HttpRequestHeader.h"
#include "HttpTypes.h"
#include "CpEventHandler.h"
#include "PasLog.h"
#include "Util.h"
#include "HashKey.h"
#include "Mutex.h"
#include "AclRouter.h"
#include "HotNumber.h"
#include "SantaTypes.h"
#include "Config.h"
#include "Util2.h"
#include "PhoneTraceMng.h"
#include "ClientRemover.h"
#include "StatFilterMng.h"
#include "HttpKTFSSLResult.h"
#include "KtfInfo.h"
#include "AuthAgent.h"
#include "ReactorInfo.h"
#include "SisorProxy.h"
#include "SisorQueue.h"
#include "Exception.h"


#include "MonitorReporter.h"
#include "WatchReporter.h"

#define DEFAULT_MULTI_PROXY "KUN00"

extern int errno;

using namespace std;

//const int MaxHashKeySize = 20;
//const int MaxBillInfoKeySize = 12;

unsigned int ClientHandler::numTransactionAlloc = 0;
unsigned int ClientHandler::numClientHandlers = 0;
unsigned int ClientHandler::numRequests = 0;
unsigned int ClientHandler::numResponses = 0;

unsigned int ClientHandler::sentDirectCnt = 0;
unsigned int ClientHandler::sentByQueCnt = 0;
	
ClientHandler::ClientHandler(ReactorInfo* rInfo)
: PasHandler(rInfo->pReactor, HT_ClientHandler), 
	santaHandler(rInfo->pReactor, this), 
	authHandler(rInfo->pReactor)
{
	myinfo[0] = 0;

	closeFuncExecuted = false;
	everRecvPacket = false;
	
	myReactor = rInfo;

	_pCP = NULL;
}

// construct 이후에 실행하기 위해.
void ClientHandler::init(int sockfd)
{
	myReactor->numClients++;
	numClientHandlers++;
	
	sock.set_handle(sockfd);

	// set SO_LINGER
/*	linger lingtime;
	lingtime.l_onoff = 1;
	lingtime.l_linger = 5;
	sock.set_option( SOL_SOCKET, SO_LINGER, (void*)&lingtime, sizeof(linger) );
*/
	startSession();
	
	changeState(CS_WAIT_REQUEST_HEADER);

	userInfo = NULL;
	currTransaction =  NULL;
	currHttpRequest = NULL;

	connectedFlag = true;
	
	startTimeTick(Config::instance()->client.timeoutCheckInterval);

	// 2006.12.19 연결하고 바로 끊는 놈들.
	startReceiveTimer(); // 시간값을 시작하는 것.
	
	paslog =  PasDataLog::instance();
	// 2006.12.19 화일 오픈이 너무 많은게 아닌지 의심. reopen 하지 말자.
	//paslog->openFiles(true);  // 혹시 로그 파일이 삭제되거나  한 경우 강제로 reopen (flag== true)

	setMyInfo();
	
	#ifdef HTTP_LOG
	httplog = new MyLog();
	httplog->open((char*)"./", (char*)"httpsize");
	#endif

	#ifdef HTTP_DUMP
	filedump = new FileDump();
	#endif

	ACE_ASSERT(myReactor->pReactor == reactor());

}

ClientHandler::~ClientHandler(void)
{
// !!! do NOTHING -- because this objected is deleted in itself.
}

/**
단말의 주소, 관련 socket 값을 sesssion 정보에 저장.
*/
void ClientHandler::startSession()
{
	
	ACE_INET_Addr peer;
	sock.get_remote_addr( peer );

	session.start(peer.get_host_addr(), peer.get_port_number(), get_handle());
	session.setLastTransactionTime();  // IDR 로그를 위한 것  (원시 과금 로그 )
	PHTR_DEBUG3("Session - Client from %s:%d", peer.get_host_addr(), peer.get_port_number(), get_handle());
}

void ClientHandler::finishSession()
{
	session.finish();	
}

void ClientHandler::changeState(ClientState _state)
{
	PAS_TRACE3("ChangeState from %d to %d ; %s", state, _state, session.getNumber());
	PHTR_DEBUG3("ChangeState from %d to %d ; %s", state, _state, session.getNumber());
	state = _state;
}

void ClientHandler::close()
{
	PasHandler::close();

	if(Config::instance()->sessionInfoUpdate.enable)
		updateSessionInfo();

	//_cp.close();
	if(_pCP != NULL)
	{
		delete _pCP;
		_pCP = NULL;
	}

	writeAbnormalCloseLog();

	deleteSendQueue();
	
	if (closeFuncExecuted)
	{
		PAS_DEBUG1("Close function called more then once. %s", getMyInfo());	
		return;
	}

	closeFuncExecuted = true;
	
	PAS_DEBUG1("Close session. %s", getMyInfo());
	PHTR_DEBUG("ClientHandler::close >> Close session.");

	finishSession();
	
	if (userInfo)
		userInfo->onClientClose();

	if(!transactionQueue.empty())
	{
		PAS_DEBUG2("Still %d transaction left in queue. %s", transactionQueue.size(), getMyInfo());
		PHTR_DEBUG2("Still %d transaction left in queue. %s", transactionQueue.size(), getMyInfo());
	}
	
	while(!transactionQueue.empty())
	{
		freeTransaction(FREE_FRONT, NULL);
	}

	if (myReactor->numClients > 0)
		myReactor->numClients--;

	if (numClientHandlers > 0)
		numClientHandlers--;

	#ifdef HTTP_LOG
	if (httplog)
		delete httplog;
	#endif 
	
	#ifdef HTTP_DUMP
	if( filedump )
		delete filedump;
	#endif

	PAS_INFO1("Phone Closed. %s", getMyInfo());
	
	if (this->tracelog != NULL)
	{
		delete this->tracelog;
		this->tracelog = NULL;
	}
	delete this;

	return;
}

void ClientHandler::onCloseByPeer()
{
	pSysStat->clientCloseByPeer(1);
	close();
}

void ClientHandler::onRecvFail()
{
	if(isConnected())
		pSysStat->clientCloseByHost(1);
	close();
}

/// send 중에 connection close 를 감지한 경우 호출됨
void ClientHandler::onSendFail()
{
	PAS_INFO1("Can't send to Phone. %s", getMyInfo());
	// socket close 상태 (fd == -1) 로 설정.
	// send fail 처리하지 말자..  TB solaris 8 처럼 느린 시스템에서 문제 발생 . 2006.11.01  -- handol
	//sock.close();
}

void ClientHandler::onReceived()
{
	PAS_TRACE2("ClientHandler::onReceived >> Received %d bytes, fd[%d]", recvBuffer.length(), get_handle());
	PHTR_DEBUG1("ClientHandler::onReceived >> Received %d bytes", recvBuffer.length());

	ReactorBusyGuard reactorBusyGuard(myReactor);

	everRecvPacket = true;
	
	startReceiveTimer(); // 시간값을 시작하는 것.
	
	if (get_handle() < 0)
	{
		PAS_NOTICE1("ClientHandler::onReceived - SOCK closed - %s", getMyInfo());
		if(isConnected())
			pSysStat->clientCloseByHost(1);
		close();
		return;
	}
	
	ACE_ASSERT(recvBuffer.length() > 0);
	

	int	result = 0;
	while(true)
	{
		// 작업 전 버퍼 사이즈
		const size_t oldRecvLength = recvBuffer.length();

		switch(state)
		{
		case CS_WAIT_REQUEST_HEADER:
			#ifdef HTTP_DUMP
			filedump->init("PHONE-REQU-HEAD", 1);
			filedump->before(recvBuffer.rd_ptr(), recvBuffer.length());
			#endif
			
			result = onReceivedRequestHeader();

			#ifdef HTTP_DUMP
			filedump->after(recvBuffer.length());
			#endif
			break;

		case CS_WAIT_REQUEST_BODY:
			#ifdef HTTP_DUMP
			filedump->init("PHONE-REQU-BODY", 1);
			filedump->before(recvBuffer.rd_ptr(), recvBuffer.length());
			#endif
			
			result = onReceivedRequestBody();

			#ifdef HTTP_DUMP
			filedump->after(recvBuffer.length());
			#endif
			break;

		case CS_SSL_CONNECTING:
			 /// SSL 서버에 연결 중이면 수신 대기 
			PAS_INFO1("ClientHandler::onReceived() >> new packet while SSL connecting. Number[%s]", session.getNumber());
			return;
		case CS_SANTA_WAITING:
			PAS_INFO1("ClientHandler::onReceived() >> new packet while waiting SANTA result. Number[%s]", session.getNumber());
			return;	

		default:
			// @todo 수정 필요.  이런 경우 발생하면 안된다.
			PAS_ERROR2("ClientHandler::onReceived() >> strange state=[%d] Number[%s] ", state, session.getNumber());
			recvBuffer.reset();
			changeState (CS_WAIT_REQUEST_HEADER );
			return;
		}

		if (result < 0)
		{
			// 2006.12.16
			PAS_NOTICE1("ClientHandler::onReceived >> CLOSE PHONE because of some error - %s", getMyInfo());
			if(isConnected())
				pSysStat->clientCloseByHost(1);
			close();
			return;
		}
		
		/// 요청 Herder/Body가 수신 완료
		if(state == CS_RECEIVED_REQUEST)
		{
			result = onCompletedReceiveRequest();
		}

		if (result < 0)
		{
			// 2006.12.16
			PAS_INFO1("ClientHandler:: CLOSE PHONE because of some error - ", getMyInfo());
			if(isConnected())
				pSysStat->clientCloseByHost(1);
			close();
			return;
		}
		
		// 작업 후 버퍼 사이즈
		const size_t newRecvLength = recvBuffer.length();

		// 처리한 데이터가 없다면
		bool noDataProcessed = (oldRecvLength == newRecvLength);
		if( noDataProcessed || recvBuffer.length() == 0 )
			break;
	}

	// 수신 버퍼 정리
	// recvBuffer.reset() 하지 않는가? --> 하면 안돤다. 다 처린된 recvBuffer를 crunch()하면 reset()과 동일 효과. 2006.09.22
	recvBuffer.crunch();

	// 장애 발생 -- 계속 수신되는 recvBuff 를 consume(read) 하지 않아서 버퍼가 full 난 경우. 
	if(recvBuffer.space() == 0)
	{
		//ACE_ASSERT(false);
		PAS_NOTICE1("recvBuff full. %s", getMyInfo());
		// 수신 상태 초기화
		changeState (CS_WAIT_REQUEST_HEADER);
		recvBuffer.reset();
	}
}

/**
혹시 단말에서 뉴라인을 잘못 보내는 경우에,
패킷을 먹어 없애기 위해.
*/
int	ClientHandler::consumeRecvBuffer()
{
	char *ptr = recvBuffer.rd_ptr();
	int	i=0;
	int	end = MIN(4, recvBuffer.length() );
	while(i < end)
	{
		if (*ptr != '\r' && *ptr != '\n')
			break;
		ptr++;
		i++;
	}

	if (i>0) 
	{
		recvBuffer.rd_ptr(i);
		
		PAS_NOTICE2("WRONG HTTP MESG: %s, cosumed %d bytes of wrong http", 
			session.getNumber(), i);
		PHTR_NOTICE2("WRONG HTTP MESG: %s, cosumed %d bytes of wrong http", 
			session.getNumber(), i);
		
	}
	return i;
}

/**
HTTP mesg 인지 검사.
*/
bool ClientHandler::isHttpHeaderCandidate(char *buff, int size)
{
	if (size < 3)
		return false;

	if ((buff[0] & 0x80) == 0 && isalpha(buff[0])	
		&& (buff[1] & 0x80) == 0 && isalpha(buff[1])	
		&& (buff[2] & 0x80) == 0 && isalpha(buff[2])	
	)
		return true;
	else
		return false;
}

int ClientHandler::onReceivedRequestHeader()
{
	bool trAllocated = false;
	
	// 수신된 데이터가 없다면 skip
	if(recvBuffer.length() == 0)
		return 0;

	int	orgRecvLeng = recvBuffer.length();

	HTTP::RequestHeader httpRequestHeader;

	HTTP::header_t header;
	int findHeader = 0;

	if (isHttpHeaderCandidate(recvBuffer.rd_ptr(), recvBuffer.length()))
	{
		findHeader = httpRequestHeader.getHeader(&header, recvBuffer.rd_ptr(), recvBuffer.length());
		PAS_TRACE1("Request header from phone.\n%s", header.c_str());
	}
	else
	{
		findHeader = -1;

		if (session.isSSL() == false)
		{
			// 이상한 패킷이 단말에서 올라온 경우이다.
			
			PAS_NOTICE3("ClientHandler::onReceivedRequestHeader >> Strange Header from PHONE: Non Alphabet - %s Buffer=%d Qsize=%d", 
					getMyInfo(), recvBuffer.length(), transactionQueue.size());
			PHTR_NOTICE3("ClientHandler::onReceivedRequestHeader >> Strange Header from PHONE: Non Alphabet - %s Buffer=%d Qsize=%d",
					getMyInfo(), recvBuffer.length(), transactionQueue.size());
			PAS_NOTICE_DUMP("Strange Header from PHONE: Non Alphabet", recvBuffer.rd_ptr(), recvBuffer.length());

			// 버퍼에 수신된 모든 내용을 무시하자. 2006.12.16
			recvBuffer.rd_ptr( recvBuffer.length() );
			
			if (transactionQueue.size() == 0)
			{
				// 이상하게 동작한 단말은 잘라 버리자. 단 현재 처리 중인 transaction 이 없는 경우에만. 2006.12.16
				PAS_INFO1("CLOSE PHONE. Because of strange header. %s", getMyInfo());
				return -1;
			}
			else
			{
				printRecentTransaction();
				return 0;
			}
		}
	}

	PAS_TRACE3("Request received from client. MesgLen[%d] FindHeader[%d] isSSL[%d]", 
		recvBuffer.length(), findHeader, session.isSSL());
	PHTR_DEBUG3("Request received from client. MesgLen[%d] FindHeader[%d] isSSL[%d]", 
		recvBuffer.length(), findHeader, session.isSSL());

	int setHeader = 0;
	if(session.isSSL())
	{
		if (findHeader < 0)
		{
			onReceivedSSLData(currTransaction);
			return 0;
		}	

		// advance read pointer
		if (static_cast<int>(header.size()) <= orgRecvLeng)
		{
			recvBuffer.rd_ptr(header.size());
		}
		else
		{
			recvBuffer.rd_ptr(orgRecvLeng);
			PAS_INFO4("ClientHandler:: Odd Length Buffer=%d header=%d Qsize=%d %s", 
				orgRecvLeng, header.size(), transactionQueue.size(), getMyInfo());
			PHTR_INFO4("ClientHandler:: Odd Length Buffer=%d header=%d Qsize=%d %s", 
				orgRecvLeng, header.size(), transactionQueue.size(), getMyInfo());
		}
		

		PAS_DEBUG4("ClientHandler::after header parsing: Buffer=%d header=%d Qsize=%d %s", 
				recvBuffer.length(), header.size(), transactionQueue.size(), getMyInfo());
		PHTR_DEBUG4("ClientHandler::after header parsing: Buffer=%d header=%d Qsize=%d %s", 
				recvBuffer.length(), header.size(), transactionQueue.size(), getMyInfo());
		
		//새 Transaction 이 개시될려고 한다.
		// 새로  Transaction 을 할당받아  currTransaction,  currHttpRequest 을 설정한다.
		if (currTransaction==NULL)
		{
			trAllocated = true;
			allocTransaction();
			if (currTransaction==NULL)
			{
				PAS_ERROR1("ClientHandler::onReceivedRequestHeader >> Memory alloc failed: Transaction - %s", setMyInfo());
				return -1;
			}
		}
		
		setHeader = currHttpRequest->setHeader(header);
		if(setHeader < 0)
		{
			PAS_INFO4("ClientHandler::onReceivedRequestHeader SSL >> Parsing Fail. %s Buffer=%d header=%d Qsize=%d", 
					getMyInfo(), recvBuffer.length(), header.size(), transactionQueue.size());
			PHTR_INFO4("ClientHandler::onReceivedRequestHeader SSL >> Parsing Fail. %s Buffer=%d header=%d Qsize=%d",
					getMyInfo(), recvBuffer.length(), header.size(), transactionQueue.size());
			PAS_INFO_DUMP("Strange Header from PHONE: SSL", header.toStr(), header.size());

			if (trAllocated)
				freeTransaction(FREE_BACK, currTransaction);

			if (transactionQueue.size() == 0)
			{
				// 이상하게 동작한 단말은 잘라 버리자. 단 현재 처리 중인 transaction 이 없는 경우에만. 2006.12.16
				PAS_INFO1("ClientHandler:: CLOSE PHONE because of Strange Header", getMyInfo());
				return -1;
			}
			else
			{
				printRecentTransaction();
				return 0;
			}
		}
		else
		{
			HTTP::RequestMethod reqMethod = currHttpRequest->getHeader()->getMethod();

			// SSL 접속 요청
			if(reqMethod == HTTP::RM_CONNECT)
			{
				//PAS_DEBUG_DUMP("SSL CONNECT", header.toStr(), header.size());
				startSSL(currTransaction);

				// 아마도 free 하는게 낫다. 그러나 테스트 필요. 여기서 free 하지 않아도  close() 에서 일괄 처리. 2006.12.15
				// freeTransaction(FREE_BACK, currTransaction);
				return 0;
			}
			else if(reqMethod == HTTP::RM_RESULT)
			{
				//PAS_DEBUG_DUMP("SSL RESULT", header.toStr(), header.size());
				finishSSL(currTransaction, true);
				
				if (trAllocated)
					freeTransaction(FREE_BACK, currTransaction);
				return 0;
			}
			else		
			{
				// RESULT 없이 일반적이 request 가 들어온 경우 이다.
				finishSSL(currTransaction);
				session.endSSL(); // recursive 를 피하기 위해.
				// ** return 하면 안된다.  아래로 진행해야 한다.
			}
		}
	}

	
	/* SSL 아닌 일반 경우 */	
	{
		if (findHeader < 0)
		{
			// 헤더 부분이 패킷이 분리되어 수신되는 경우이다.  수신을 한번 더 해야 한다.
			PAS_DEBUG1("ClientHandler::waiting more packet to complete Http Header. curr leng=%d", recvBuffer.length());
			PHTR_DEBUG1("ClientHandler::waiting more packet to complete Http Header. curr leng=%d", recvBuffer.length());
			return 0;
		}

		PAS_TRACE2("ClientHandler:: findHeader=%d, header.size()=%d", findHeader, header.size());
		PHTR_DEBUG2("ClientHandler:: findHeader=%d, header.size()=%d", findHeader, header.size());
		
		// advance read pointer
		if (static_cast<int>(header.size()) <= orgRecvLeng)
		{
			recvBuffer.rd_ptr(header.size());
		}
		else
		{
			recvBuffer.rd_ptr(orgRecvLeng);
			PAS_INFO4("ClientHandler:: Odd Length Buffer=%d header=%d Qsize=%d %s", 
				orgRecvLeng, header.size(), transactionQueue.size(), getMyInfo());
			PHTR_INFO4("ClientHandler:: Odd Length Buffer=%d header=%d Qsize=%d %s", 
				orgRecvLeng, header.size(), transactionQueue.size(), getMyInfo());
		}

		PAS_TRACE4("ClientHandler::after header parsing: Buffer=%d header=%d Qsize=%d %s", 
				recvBuffer.length(), header.size(), transactionQueue.size(), getMyInfo());
		PHTR_DEBUG4("ClientHandler::after header parsing: Buffer=%d header=%d Qsize=%d %s", 
				recvBuffer.length(), header.size(), transactionQueue.size(), getMyInfo());
				
		// 새 Transaction 이 개시될려고 한다.
		// 새로 Transaction 을 할당받아  currTransaction,  currHttpRequest 을 설정한다.
		if (currTransaction==NULL)
		{
			allocTransaction();
			trAllocated = true;
			if (currTransaction==NULL)
			{
				PAS_ERROR1("ClientHandler::onReceivedRequestHeader >> Memory alloc failed: Transaction - %s", setMyInfo());
				return -1;
			}
		}

	
		setHeader = currHttpRequest->setHeader(header);
		
		if(setHeader < 0)
		{
			PAS_INFO4("ClientHandler:: Strange Header from PHONE: Not ssl - %s Buffer=%d header=%d Qsize=%d", 
					getMyInfo(), recvBuffer.length(), header.size(), transactionQueue.size());
			PHTR_INFO4("ClientHandler:: Strange Header from PHONE: Not ssl - %s Buffer=%d header=%d Qsize=%d",
					getMyInfo(), recvBuffer.length(), header.size(), transactionQueue.size());
			PAS_INFO_DUMP("Strange Header from PHONE: Not ssl", header.toStr(), header.size());

			if (trAllocated)
				freeTransaction(FREE_BACK, currTransaction);

			if (transactionQueue.size() == 0)
			{
				// 이상하게 동작한 단말은 잘라 버리자. 단 현재 처리 중인 transaction 이 없는 경우에만. 2006.12.16
				PAS_INFO1("ClientHandler:: CLOSE PHONE because of Strange Header", getMyInfo());
				return -1;
			}
			else
			{
				printRecentTransaction();
				return 0;
			}
			
		}

		else if(currHttpRequest->getHeader()->getMethod() == HTTP::RM_CONNECT)
		{
			PAS_INFO_DUMP("SSL CONNECT", header.toStr(), header.size());
			startSSL(currTransaction);
			// 아마도 free 하는게 낫다. 그러나 테스트 필요. 여기서 free 하지 않아도  close() 에서 일괄 처리. 2006.12.15
			// freeTransaction(FREE_BACK, currTransaction);
			return 0;
		}
		else if(currHttpRequest->getHeader()->getMethod() == HTTP::RM_RESULT)
		{
			// just ignore
			PAS_INFO1("ClientHandler::RECV SSL RESULT NOT in SSL mode - %s", getMyInfo());
			PHTR_INFO1("ClientHandler::RECV SSL RESULT NOT in SSL mode - %s", getMyInfo());
			PAS_INFO_DUMP("SSL RESULT", header.toStr(), header.size());
			
			if (trAllocated)
				freeTransaction(FREE_BACK, currTransaction);
			return 0;
		}

		// 일반 HTTP 데이터 요청		
		else
		{			
			//PAS_DEBUG_DUMP("PHONE REQ HEAD", currHttpRequest->getRawHeader()->rd_ptr(), currHttpRequest->getHeadLeng());

			// content 가 10메가를 넘어가면, 로그를 남기자
			if(currHttpRequest->getContentLength() >= 1024*1024)
			{
				PAS_INFO1("Big content is requested. URL is [%s]", currHttpRequest->getHeader()->getUrl().toStr());
			}
			
			// has body?
			if(currHttpRequest->getContentLength() > 0)
			{
				PAS_DEBUG2("ClientHandler::RECV REQ HEAD - Head=%d, ContentLength=%d", header.size(), currHttpRequest->getContentLength());
				PHTR_DEBUG2("ClientHandler::RECV REQ HEAD - Head=%d, ContentLength=%d", header.size(), currHttpRequest->getContentLength());

				changeState ( CS_WAIT_REQUEST_BODY);
			}
			else
				changeState ( CS_RECEIVED_REQUEST);
		}
	}

	return 0;
	
}


bool ClientHandler::isCompletedReceiveRequestBody()
{
	return (static_cast<int>(currHttpRequest->getContentLength()) == currHttpRequest->getBodyLeng());
}

int ClientHandler::onReceivedRequestBody()
{
	static int appendCount = 0; //  currHttpRequest->appendBody() 가 얼마나 호출되는지 보기 위해. 로그용.
	
	ACE_ASSERT(currHttpRequest != NULL);
	ACE_ASSERT(currHttpRequest->getContentLength() > 0);
	ACE_ASSERT(state == CS_WAIT_REQUEST_BODY);

	// 수신된 데이터가 없다면 skip
	if(recvBuffer.length() == 0)
		return 0;

	// copy recvBuffer to requestBody
	int remainSize = currHttpRequest->getContentLength() - currHttpRequest->getBodyLeng();
	int appendSize = std::min(remainSize, (int)recvBuffer.length());
	int resultAppend = currHttpRequest->appendBody(recvBuffer.rd_ptr(), appendSize);
	appendCount++;
	
	PAS_DEBUG2("ClientHandler::RECV REQ BODY - recv=%d, remain=%d", recvBuffer.length(), remainSize);
	PHTR_DEBUG2("ClientHandler::RECV REQ BODY - recv=%d, remain=%d", recvBuffer.length(), remainSize);
	
	if(resultAppend < 0)
	{
		PAS_ERROR("ClientHandler::onReceivedRequestBody >> recvBuffer 복사 실패");
		PHTR_NOTICE("ClientHandler::onReceivedRequestBody >> recvBuffer 복사 실패");
		return -1;
	}

	recvBuffer.rd_ptr(appendSize);

	if(isCompletedReceiveRequestBody())
	{
		PAS_DEBUG2("ClientHandler: appendBody() called %d times for %d bytes", 
			appendCount, currHttpRequest->getBodyLeng());
			
		appendCount = 0;
				
		changeState ( CS_RECEIVED_REQUEST );
		ACE_ASSERT(currHttpRequest != NULL);

		PAS_DEBUG2("ClientHandler::RECV REQ BODY Complete - recv=%d, ContentLength=%d", 
			currHttpRequest->getBodyLeng(), currHttpRequest->getContentLength() );
		PHTR_DEBUG2("ClientHandler::RECV REQ BODY Complete - recv=%d, ContentLength=%d", 
			currHttpRequest->getBodyLeng(), currHttpRequest->getContentLength() );

		if (currHttpRequest->getRawBody() != NULL) {
		//int dump_len = MIN(currHttpRequest->getBodyLeng(), 160);
		//PAS_DEBUG_DUMP("PHONE REQ BODY", currHttpRequest->getRawBody()->rd_ptr(), dump_len);
		}
		
	}

	return 0;
}




int ClientHandler::onCompletedReceiveRequest()
{
	ACE_ASSERT(state == CS_RECEIVED_REQUEST);

	PAS_DEBUG("+-----------------------------------------------------------------------+");
	PAS_DEBUG("|                         Transaction start                             |");
	PAS_DEBUG("+-----------------------------------------------------------------------+");

	PHTR_DEBUG("+-----------------------------------------------------------------------+");
	PHTR_DEBUG("|                         Transaction start                             |");
	PHTR_DEBUG("+-----------------------------------------------------------------------+");

	_sentBodySizeByStream = 0;
	
	PAS_DEBUG3("Session Req(%d) Res(%d) New Request. Request URL is [%s]", 
		session.reqNums, session.respNums, currHttpRequest->getHeader()->getUrl().toStr());
	PHTR_DEBUG3("Session Req(%d) Res(%d) New Request. Request URL is [%s]", 
		session.reqNums, session.respNums, currHttpRequest->getHeader()->getUrl().toStr());	

	session.lastRequestURL = currHttpRequest->getHeader()->getUrl();

	logHttpReq("[PHONE REQ]", currTransaction, true);

	// 하나의 Transaction 이 개시된 것.  Transaction Q 에 추가.
	if (startTransaction(currTransaction) < 0)
		return -1;

	// currTransaction 은 한 사이클의 recv 가 완료되면 세팅 NULL 이 필요하다.
	currTransaction = NULL;
	currHttpRequest = NULL;
	return 0;

}


/**
SSL 연결 성공/ 실패 경우 모두  여기서 처리 
*/
void ClientHandler::onCompletedCPConnection(CpHandler* pCP, int isSucc)
{
	if (state == CS_SSL_CONNECTING)
		changeState ( CS_WAIT_REQUEST_HEADER );

	if(session.isSSL())
	{
		PAS_DEBUG2("CONNECT complete, Succ=%d; %s",  isSucc, setMyInfo());
		PHTR_DEBUG2("CONNECT complete, Succ=%d; %s", isSucc,  getMyInfo());
		
		if ( !isSucc )
		{
			responseSSLConnectionFail();
			return;
		}
		// SSL통신을 하기 위한 CP인지 확인
		else if(session.getSSLHost() == pCP->getHost() && session.getSSLPort() == pCP->getPort())
		{
			// SSL통신용 CP 접속이 완료 됐음을 Client에게 알림
			responseSSLConnectionSuccess();
			return;
		}
		
	}

		/*
		// 일반 (SSL 아닌 것) 도  connect 결과 통보된다.
	PAS_INFO1("Unexpected CONNECT complete; %s",  setMyInfo());
	PHTR_INFO1("Unexpected: CONNECT complete; %s",  getMyInfo());
		*/
}


void ClientHandler::onCompletedCPResponse(CpHandler* pCP, Transaction *tr)
{
	ACE_ASSERT(pCP != NULL);
	ACE_ASSERT(tr != NULL);

	HTTP::Response* pRes  = tr->getResponse();

	if(pCP->getReceiveBodySize() != pRes->getContentLength())
	{
		PAS_NOTICE2("Size mismatch. ReceiveBodySize[%d] != ContentLength[%d]", pCP->getReceiveBodySize(), pRes->getContentLength());
		PAS_NOTICE_DUMP("CP RESP HEAD", pRes->getRawHeader()->rd_ptr(), pRes->getHeadLeng());
		PAS_NOTICE_DUMP("CP RESP BODY", pRes->getRawBody()->rd_ptr(), pRes->getBodyLeng());
	}

	PAS_DEBUG4("Received Response from CPProxy. HeadSize[%d] StoreBody[%d] StreamBody[%d] %s", pRes->getHeadLeng(), pRes->getBodyLeng(), _sentBodySizeByStream, getMyInfo());
	PHTR_DEBUG4("ClientHandler::onCompletedCPResponse >> RECV CP RESP - Head[%d] StoreBody[%d] StreamBody[%d] %s", pRes->getHeadLeng(), pRes->getBodyLeng(), _sentBodySizeByStream, getMyInfo());

	if(pRes->getContentLength() >= 1024*1024)
	{
		PAS_INFO3("Big contents found. Contents size is %d Bytes. URL is [%s] %s", 
			pRes->getContentLength(), tr->getRequest()->getHeader()->getUrl().toStr(), getMyInfo());
	}
	
	PHTR_HEXDUMP(pRes->getRawHeader()->rd_ptr(), pRes->getHeadLeng(), "CP RESP HEAD");
	if (pRes->getRawBody()) {
		int dump_len = MIN(pRes->getBodyLeng(), 160);
		//PAS_TRACE_DUMP("CP RESP BODY", pRes->getRawBody()->rd_ptr(), dump_len);
		PHTR_HEXDUMP(pRes->getRawBody()->rd_ptr(), dump_len, "CP RESP BODY");
	}
	
	#ifdef HTTP_LOG
	logHttpResp("[CP RESP]", tr, false);
	#endif

	afterCpTransaction(tr);
}

void ClientHandler::onCommand(CommandID cid, PasHandler* pEH, void* arg1, void* arg2)
{
	PAS_TRACE1("ClientHandler::onCommand >> fd[%d]", get_handle());
	PHTR_DEBUG("ClientHandler::onCommand");

	ACE_ASSERT(pEH != NULL);

	switch(cid)
	{
	// CP receive complete
	case CID_CP_Completed:
	{
		PAS_TRACE1("CID_CP_Completed, fd[%d]", get_handle());
		PHTR_DEBUG("ClientHandler::onCommand >> CID_CP_Completed");
		CpHandler* pCP = dynamic_cast<CpHandler*>(pEH);
		onCommandCPCompleted(pCP, (Transaction *)arg1);
		break;
	}

	// CP Receive SSL Data
	case CID_CP_SSLData:
	{
		PAS_TRACE1("CID_CP_SSLData, fd[%d]", get_handle());
		PHTR_DEBUG("ClientHandler::onCommand >> CID_CP_SSLData");
		CpHandler* pCP = dynamic_cast<CpHandler*>(pEH);
		onCommandCPReceivedSSLData(pCP, (char*)arg1, *(size_t*)arg2);
		break;
	}

	// CP Connection established
	case CID_CP_Connected:
	{
		PAS_TRACE1("CID_CP_Connected, fd[%d]", get_handle());
		PHTR_DEBUG("ClientHandler::onCommand >> CID_CP_Connected");
		CpHandler* pCP = dynamic_cast<CpHandler*>(pEH);
		
		ACE_ASSERT(arg1 != NULL);
		int	succFlag = *(static_cast<int*>(arg1));
		onCommandCPConnected(pCP, succFlag);
		break;
	}

	// CP Close
	case CID_CP_Closed:
	{
		PAS_TRACE1("CID_CP_Closed, fd[%d]", get_handle());
		PHTR_DEBUG("ClientHandler::onCommand >> CID_CP_Closed");
		CpHandler* pCP = dynamic_cast<CpHandler*>(pEH);
		onCommandCPClosed(pCP);
		break;
	}

	case CID_CP_Received_Header:
	{
		PAS_TRACE1("CID_CP_Received_Header, fd[%d]", get_handle());
		PHTR_DEBUG("ClientHandler::onCommand >> CID_CP_Received_Header");
		CpHandler* pCP = dynamic_cast<CpHandler*>(pEH);
		CpResponseData* pData = static_cast<CpResponseData*>(arg1);
		onReceivedHeaderFromCP(pCP, pData);
		break;
	}

	case CID_CP_Received_PartOfBody:
	{
		PAS_TRACE1("CID_CP_Received_Header, fd[%d]", get_handle());
		PHTR_DEBUG("ClientHandler::onCommand >> CID_CP_Received_Header");
		CpHandler* pCP = dynamic_cast<CpHandler*>(pEH);
		CpResponseData* pData = static_cast<CpResponseData*>(arg1);
		onReceivedPartialBodyFromCP(pCP, pData);
		break;
	}

	// Santa Complete
	case CID_Santa_Completed:
	{
		PAS_TRACE1("CID_Santa_Completed, fd[%d]", get_handle());
		SantaHandler* pSanta = dynamic_cast<SantaHandler*>(pEH);
		onCommandSantaCompleted(pSanta, (const SANTA::Response*)arg1);
		break;
	}

	// Santa Close
	case CID_Santa_Closed:
	{
		PAS_TRACE1("CID_Santa_Closed, fd[%d]", get_handle());
		SantaHandler* pSanta = dynamic_cast<SantaHandler*>(pEH);
		onCommandSantaClosed(pSanta);
		break;
	}

	case CID_Santa_TimeOut:
	{
		PAS_TRACE1("CID_Santa_TimeOut, fd[%d]", get_handle());
		SantaHandler* pSanta = dynamic_cast<SantaHandler*>(pEH);
		onCommandSantaTimeOut(pSanta);
		break;
	}

	// Unknown
	default:
		PAS_ERROR2("onCompleted() >> Unknown CommandID[%d], fd[%d]", (int)cid, get_handle());
		PHTR_DEBUG2("ClientHandler::onCompleted >> Unknown CommandID[%d], fd[%d]", (int)cid, get_handle());
	}
}

/** 기존에 Request/Response 쌍을 응답하던 것을 Transaction 하나로 수정함. */
int ClientHandler::onCommandCPCompleted(CpHandler* pCP, Transaction *resTransaction)
{
	PAS_TRACE("onCommandCPCompleted");
	PHTR_DEBUG("ClientHandler::onCommandCPCompleted");

	ACE_ASSERT(pCP != NULL);
	//ACE_ASSERT(pCP == &_cp);
	//ACE_ASSERT(cpList.isExist(pCP));
	ACE_ASSERT(resTransaction != NULL);

	onCompletedCPResponse(pCP, resTransaction);

	return 0;
}

int ClientHandler::onCommandCPConnected(CpHandler* pCP, int isSucc)
{
	PAS_TRACE("onCommandCPConnected");
	PHTR_DEBUG("ClientHandler::onCommandCPConnected");

	ACE_ASSERT(pCP != NULL);
	//ACE_ASSERT(pCP == &_cp);
	//ACE_ASSERT(cpList.isExist(pCP));

	onCompletedCPConnection(pCP, isSucc);

	return 0;
}

int ClientHandler::onCommandCPClosed(CpHandler* pCP)
{
	PAS_TRACE("onCommandCPClosed");
	PHTR_DEBUG("ClientHandler::onCommandCPClosed");

	ACE_ASSERT(pCP != NULL);
	//ACE_ASSERT(pCP == &_cp);
	//ACE_ASSERT(cpList.isExist(pCP));

	return 0;
}



/**
단말로부터 SSL 데이타 받은 것을 CP로 전송.
*/

int ClientHandler::onReceivedSSLData(Transaction *tr)
{
	PAS_TRACE2("SSL Data from phone - sock[%d] %d bytes", get_handle(), recvBuffer.length());
	PHTR_DEBUG2("SSL Data from phone - sock[%d] %d bytes", get_handle(), recvBuffer.length());

	// 수신된 데이터가 없다면 skip
	if(recvBuffer.length() == 0)
		return 0;

	// SSL Data Relay
	// send to CP
	CpHandler* cp = pickCpHandler(session.getSSLHost(), session.getSSLPort());

	if (cp == NULL)
	{
		PHTR_NOTICE1("ClientHandler::onReceivedSSLData - SSL CP Closed [%s]", getMyInfo());
		return -1;
	}
	#ifdef HTTP_DUMP
	filedump->init("PHONE-REQU-SSL", 1);
	filedump->write(recvBuffer.rd_ptr(), recvBuffer.length());
	#endif
	
	if (userInfo)
		userInfo->updateReqTime();
	
	if (cp->sendSSLData(recvBuffer.rd_ptr(), recvBuffer.length()) < 0)
	{
		PHTR_NOTICE1("ClientHandler::onReceivedSSLData - SSL CP Closed [%s]", getMyInfo());
		return -1;
	}
		

	if (session.sslReqBytes==0) {
		session.beginSSL(); // SSL connect/response 를 하나의 tr 로 간주하고, SSL 데이타 시점부터 SSL 새로 시작한 것으로 간주.
		tr->recvPhoneReq();
	}
		
	tr->sendCpReq();
	tr->onSslDataRecv(recvBuffer.length());
	session.onSslDataRecv(recvBuffer.length());

	
	
	// 로그 작성.
	paslog->writeSslDataReq(userInfo, &session, recvBuffer.length());
	recvBuffer.reset();

	return 0;
}

/**
CP 로부터 받은 SSL 데이타를 단말로 전송.
*/
int ClientHandler::onCommandCPReceivedSSLData(CpHandler* pCP, const char* buf, size_t bufSize)
{
	PAS_TRACE2("SSL Data from CP - sock[%d] %d bytes", pCP->get_handle(), bufSize);
	PHTR_DEBUG2("SSL Data from CP - sock[%d] %d bytes", pCP->get_handle(), bufSize);


	ACE_ASSERT(pCP != NULL);
	//ACE_ASSERT(pCP == &_cp);
	//ACE_ASSERT(cpList.isExist(pCP));

	enSendQueue(buf, bufSize);

	session.onSslDataSend(bufSize);

	#ifdef HTTP_DUMP
	filedump->init("PHONE-RESP-SSL", 4);
	filedump->write(buf, bufSize);
	#endif
	
	if (currTransaction)
		currTransaction->recvCpResp();
	// 로그 작성.
	paslog->writeSslDataResp(userInfo, &session, bufSize);
	return 0;
}

int ClientHandler::onCommandSantaCompleted(SantaHandler* pSanta, const SANTA::Response* /* pResponse */)
{
	Transaction	*tr = NULL;
	
	ACE_ASSERT(pSanta != NULL);

	SANTA::MDN_t santaMDN;
	SANTA::IMSI_t santaIMSI;

	santaMDN = pSanta->getMDN();
	santaIMSI = pSanta->getIMSI();

	PAS_TRACE3("SANTA query result MDN[%s] IMSI[%s] - %s", santaMDN.toStr(), santaIMSI.toStr(), getMyInfo());
	PHTR_DEBUG3("SANTA query result MDN[%s] IMSI[%s]- %s", santaMDN.toStr(), santaIMSI.toStr(), getMyInfo());
	
	if (! transactionQueue.empty())
	{
		tr = transactionQueue.front();
	}
	else
	{
		tr = currTransaction;
	}

	ACE_ASSERT(tr != NULL);
		
	tr->santaEndTime = ACE_OS::gettimeofday();
		
	// SANTA 에서 결과가 정상적으로 왔는지 체크
	if( pSanta->isPassed() == false )
	{
	
		// SANTA 조회 실패 페이지 전송
		ResponseBuilder::SantaFailed( tr->getResponse() );

		// Over10 로그 출력을 위한 에러 코드
		tr->setErrorRespCode( RESCODE_SANTA );

		// @todo 모든 transaction 을 종료하여야 한다.
		afterCpTransaction(tr);

		return 0;
	}
	else 
	{
		session.isSantaNumber = true; // SANTA 조회 결과를 session 에 저장하였음을 표현.
		// MDN 과 IMSI 를 세션 정보에  저장한다.
		STRNCPY( session.phoneNumber, santaMDN.toStr(), LEN_PHONE_NUM );
		STRNCPY( session.IMSI, santaIMSI.toStr(), LEN_IMSI );
		session.santaTime = time(NULL);

		// SANTA_WAITING 상태를 해제.
		changeState(CS_WAIT_REQUEST_HEADER);

		/// SANTA 조회이 끝나고 나면 나머지 작업들(Auth 인증, CP에 페이지 요청등)을 진행한다.
		midTransaction(tr);
	}
	

	return 0;
}

int ClientHandler::onCommandSantaClosed(SantaHandler* /* pSanta */)
{
	PAS_TRACE("ClientHandler::onCommandSantaClosed");

	changeState(CS_WAIT_REQUEST_HEADER);

	return 0;
}

int ClientHandler::onCommandSantaTimeOut(SantaHandler* /* pSanta */)
{
	Transaction	*tr = NULL;

	PAS_INFO1("SANTA query failed (TimeOut). %s", getMyInfo());
	PHTR_DEBUG1("ClientHandler::SANTA query failed (TimeOut)- %s", setMyInfo());
	
	changeState(CS_WAIT_REQUEST_HEADER);
	
	if (! transactionQueue.empty())
	{
		tr = transactionQueue.front();
	}
	else
	{
	// @todo
		tr = currTransaction;
	}

	ACE_ASSERT(tr != NULL);
	
	/// SANTA DB 조회 실패 메시지 전송
	ResponseBuilder::SantaFailed( tr->getResponse() );

	// Over10 로그 출력을 위한 에러 코드
	tr->setErrorRespCode( RESCODE_SANTA );

	afterCpTransaction(tr);

	return 0;
}

int ClientHandler::handle_timeout(const ACE_Time_Value& current_time, const void* /* act */)
{
	PAS_TRACE1("ClientHandler::handle_timeout >> fd[%d]", get_handle());
	PHTR_DEBUG1("ClientHandler::handle_timeout >> fd[%d]", get_handle());

	if (!everRecvPacket)
	{
		/*
		아무 메시지없이 바로 끊어진 단말을 timeout값까지 기다리지 않고 10초 만에 close 하기
		2006.12.19
		*/
		time_t now = time(NULL);
		int idle = now - session.startSec;
		if (idle >= 20)
		{
			PAS_INFO2("Phone idle without any mesg for %d secs. %s", idle, getMyInfo());

			/* TB에서 kunmulti.py 로 테스트 한 결과 core 발생 생김. */
			if(isConnected())
				pSysStat->clientCloseByHost(1);
			close();

			return 0;
		}
	}
	
	ACE_Time_Value recvTimeout(Config::instance()->client.receiveTimeout);

	if( isIdle(current_time, recvTimeout) )
	{
		if (recvBuffer.length() > 0)
		{
			PAS_INFO2("Phone is idle with %d bytes mesg. Force to close. %s", recvBuffer.length(), getMyInfo());
			PAS_INFO_DUMP("Mesg left in recvBuffer", recvBuffer.rd_ptr(), recvBuffer.length());
		}
		else
			PAS_INFO1("Phone is idle. Force to close. %s", getMyInfo());

		// increase statistic
		if(isConnected())
			pSysStat->clientCloseByHost(1);		
		close();
	}

	return 0;
}

int ClientHandler::handle_exception(ACE_HANDLE fd)
{
	PAS_TRACE1("ClientHandler::handle_exception >> fd[%d]", fd);
	PHTR_DEBUG1("ClientHandler::handle_exception >> fd[%d]", get_handle());
	
	return 0;
}



/**
MNC 값이 04, 08 이 아닌 경우 (아마도 00) 에 가상 번호대역인지 검사하고 
Santa 연동을 수행한다. -- 2006.10.13

@brief SANTA 에 질의하기 전에 몇가지 조건을 검사한다.
       만약 SANTA 인증이 필요없는 상태라면 true 를 리턴하여 인증 완료를 알려준다.
	   그 외에 false 인 경우는 인증을 실행한다.

@return 인증완료(true), 인증시작(false)

*/
int  ClientHandler::checkSanta(Transaction *tr)
{
	#ifdef TEST_VERSION
	const Config* pConfig = Config::instance();
	if(pConfig->santa.enable == false)
	{
		PAS_DEBUG1("SANTA check disabled - %s", getMyInfo());
		santaHandler.setPassed();
		return 0;
	}
	#endif

	if (strcmp(tr->MNC, "04") == 0 || strcmp(tr->MNC, "08") == 0)
	{
		santaHandler.setPassed();
		return 0;
	}

	// MIN이 Anonymous 이면 SANTA 조회가 필요없다.
	if( !strcmp(tr->MINnumber, "Anonymous") || !strcmp(tr->MINnumber, "N/A") )
	{
		santaHandler.setPassed();
		return 0;
	}

	// 가상 번호 대역이 아니면 SANTA 조회가 필요 없다.
	if (KtfInfo::isVirtualNumber(tr->phoneNumber) == false)
	{
		santaHandler.setPassed();
		return 0;
	}

	PAS_DEBUG1("ClientHandler::SANTA query start - %s", getMyInfo());
	PHTR_DEBUG1("ClientHandler::SANTA query start - %s", getMyInfo());

	// Santa 핸들러 등록
	santaHandler.set( reactor(), this );
	santaHandler.setTraceLog( this->tracelog );

	if (santaHandler.start(tr) < 0)
	{
		PAS_WARNING1("ClientHandler::SANTA connection failed - %s", getMyInfo());
		PHTR_WARN1("ClientHandler::SANTA connection failed - %s", getMyInfo());
		return -1;
	}
	return 0;
}


/**
한 session 에서 한번만 수행하면 된다.
*/
int ClientHandler::checkAuth(Transaction *tr)
{
	const Config* pConfig = Config::instance();

	#ifdef TEST_VERSION
	if (pConfig->auth.enable == false) {
		return 0;
	}
	#endif
	if( authHandler.getUserInfo() == NULL )
		authHandler.set( userInfo );

	// 인증성공인 경우, n초 후에 재인증을 받아야 한다.
	if( userInfo->getAuthState() == AS_RESPONSED )
	{
		time_t currTime;
		time( &currTime );

		// 마지막 인증 시간이 n초 이상이면 AuthState를 AS_NONE으로 셋팅 함으로써
		// 재인증을 받도록 유도한다.
		time_t delayTime = currTime - userInfo->getLatestAuthTime();
		if( delayTime >= pConfig->auth.authentication )	// 10초
			userInfo->changeAuthState( AS_NONE );
	}

	// 최초 한번 인증 시작
	if( authHandler.getState() == AS_NONE )
	{
		const host_t& host = pConfig->auth.host;
		const int port = pConfig->auth.port;

		/// Auth 에 접속요청, AuthEventHandler 생성 및 접속된 소켓 핸들 셋팅,
		/// Auth 에 요청할 데이터 셋팅
		requestAuth(tr, host, port);

		return 0;
	}

	// Auth 응답이 인증실패면 진행 중단.
	if( authHandler.isPassed() == false )
	{
		// 인증 실패시 다음에 재인증을 받기위해 AS_NONE 으로 셋팅
		userInfo->changeAuthState( AS_NONE );
		return -1;
	}

	return 0;
}


int ClientHandler::requestAuth(Transaction *tr, const host_t& /* host */, int /* port */)
{
	// Auth에 접속 요청

	// Auth에게 요청할 데이터 셋팅
	if( tr->phoneNumber[0] == '\0' )
	{
		PAS_NOTICE1("ClientHandler::AUTH >> Not Found HTTP_PHONE_NUMBER, fd[%d]", get_handle());
		PHTR_NOTICE("ClientHandler::AUTH >> Not Found HTTP_PHONE_NUMBER");
		return -1;
	}

	/// 여기서 부터 Auth 에 요청할 데이터 셋팅
	int isCaching = 0;
	if( KtfInfo::isNewBrowser(tr->browser) == false )
		isCaching = 1;

	/// PasGW 포트
	Config *pConf = Config::instance();
	int npasgwPort = pConf->network.listenPort;

	/// 3G 단말기 여부
	/*
	3G 단말 판단 조건 추가
	2007.5.7
	*/
	int n3G = (tr->is3G);

	/// Auth 에 요청할 데이터 셋팅 완료

	authHandler.setTraceLog( this->tracelog );

	AUTH::RequestBody reqBody;

	/// Auth 에 데이터 요청
	if( n3G )
	{
		makeRequestAuth(reqBody, 1, tr->MINnumber, tr->nIPAddr, npasgwPort, 0, isCaching, n3G, tr->MDN, tr->msModel);

		PAS_DEBUG5( "ClientHandler::AUTH >> MIN[%s] IP[%d] PORT[%d] Caching[%d] 3G[%d]",
			tr->MINnumber, tr->nIPAddr, npasgwPort, isCaching, n3G );
	}

	else
	{
		makeRequestAuth(reqBody, 1, tr->IMSI, tr->nIPAddr, npasgwPort, 0, isCaching, n3G, tr->MDN, tr->msModel);

		PAS_DEBUG5( "ClientHandler::AUTH >> IMSI[%s] IP[%d] PORT[%d] Caching[%d] 3G[%d]",
			tr->IMSI, tr->nIPAddr, npasgwPort, isCaching, n3G );
	}

	// 인증 요청 상태로 변경
	userInfo->changeAuthState( AS_REQUESTED );

	// AuthAgent 에 인증 요청
	AuthAgent *authAgent = AuthAgent::instance();
	int res = authAgent->putWork(reqBody, userInfo);
	session.pasauthTime = time(NULL);

	return res;
}

void ClientHandler::makeRequestAuth(AUTH::RequestBody &body, const int seq, const char* pMin, const unsigned int ip, const int port, const int startConn, const int newBrowser, int g3GCode, const char* pMdn, const char* pMsModel)
{
	// init body
	body.type = AUTH::REQUEST_AUTHORIZE_CODE;
	body.seq = seq;
	STRNCPY(body.min, pMin, sizeof(body.min));
	body.accessIP = ip;
	body.port = port;
	body.startConn = startConn;
	body.newBrowser = newBrowser;
	body.g3GCode = g3GCode;

	if(pMdn)
	{
		STRNCPY(body.mdn, pMdn, sizeof(body.mdn));
	}
	else
		memset(body.mdn, 0x00, sizeof(body.mdn));


	if(pMsModel)
	{
		STRNCPY(body.msModel, pMsModel, sizeof(body.msModel));
	}
	else
		memset(body.msModel, 0x00, sizeof(body.msModel));
}


/**
CP pRequest 전에.
*/

bool ClientHandler::applyHotNumber(Transaction *tr)
{
	HTTP::Request* pRequest = tr->getRequest();

	ACE_ASSERT(pRequest != NULL);
	ACE_ASSERT(!pRequest->getHeader()->getUrl().isEmpty());

	HotNumber hotNumber;
	const url_t orgurl = pRequest->getHeader()->getOrgUrl();

	
	int hit = hotNumber.convert(orgurl, orgurl.size());
	if(hit)
	{
		int ret = pRequest->setUrl(hotNumber.getConverted());
		const url_t newurl = pRequest->getHeader()->getUrl();
		
		PAS_DEBUG2("HotNumber applied: ORG=%s, URL=%s", orgurl.toStr(), newurl.toStr());
		PHTR_DEBUG2("HotNumber applied: ORG=%s, URL=%s", orgurl.toStr(), newurl.toStr());
		
		if(ret < 0)
		{
			PAS_ERROR("ClientHandler::applyHotNumber >> HotNumber 적용 실패");
			return false;
		}

		STRCPY(tr->realUrl, newurl.toStr(), MAX_URL_LEN);
		tr->hotNumberConverted = true;
		return true;
		
	}
	return false;
}


/**
CP pRequest 전에. Hotnumber 후에 
*/
ClientHandler::AclResult ClientHandler::applyACL(HTTP::Request* pRequest, host_t& proxyHost, int& proxyPort)
{
	ACE_ASSERT(pRequest != NULL);
	ACE_ASSERT(!pRequest->getHeader()->getHost().isEmpty());

	const host_t host = pRequest->getHeader()->getHost();
	const int port = pRequest->getHeader()->getPort();

	AclRouter* pAcl = AclRouter::instance();

	char newHost[MAX_HOST_LEN+1] = "\0";
	int newPort;

	PAS_DEBUG2("ACL Query >> Query about %s:%d", host.toStr(), port);

	// ACL SWITCH 가 OFF 면 searchDNS 만 수행하고
	// ON 이면 searchALL 을 수행한다.
	// ACL_FIND_DNS 혹은 ACL_NOT_FOUND 의 경우 현재의 서버 ID가 DEFAULT(KUN00)가 아니면
	// KUN00 으로 MULTIPROXY 하라고 알려준다.
	int searchResult;
	if( Config::instance()->acl.multiproxy == true )
	{
		searchResult = pAcl->searchALL(host, port, newHost, sizeof(newHost), newPort);
		if( searchResult == ACL_FIND_DNS || searchResult == ACL_NOT_FOUND )
		{
			if( Config::instance()->process.serverID != DEFAULT_MULTI_PROXY )
			{
				AclRouter::instance()->getHost( newHost, sizeof(newHost), newPort, DEFAULT_MULTI_PROXY );

				// MultiProxy 해야함
				searchResult = ACL_DENY_ACL;
			}
		}
	}

	else
	{
		// searchDNS 수행
		searchResult = pAcl->searchDNS(host, port, newHost, sizeof(newHost), newPort);
	}

	const bool changedHostAndPort = (searchResult == ACL_FIND_DNS || searchResult == ACL_ALLOW_ACL);

	PAS_TRACE2("ACL searchResult[%d] changedHostAndPort[%d]", searchResult, changedHostAndPort);
	PHTR_DEBUG2("ACL searchResult[%d] changedHostAndPort[%d]", searchResult, changedHostAndPort);
		
	switch(searchResult)
	{
	case ACL_NOT_FOUND:
		PAS_TRACE("applyACL >> ACL_NOT_FOUND");
		break;

	case ACL_FIND_DNS:
		PAS_TRACE("applyACL >> ACL_FIND_DNS");
		break;

	case ACL_ALLOW_ACL:
		PAS_TRACE("applyACL >> ACL_ALLOW_ACL");
		break;

	case ACL_DENY_ACL:
		PAS_TRACE("applyACL >> ACL_DENY_ACL");
		break;

	case ACL_INPUT_ERR:
		PAS_TRACE("applyACL >> ACL_INPUT_ERR");
		break;

	default:
		PAS_NOTICE("applyACL >> Unknown ACL Response Code");
		break;
	}

	// 변경된 host와 port 적용
	if(changedHostAndPort)
	{
		/***
		int setResult = pRequest->setHostPort(newHost, newPort);
		const url_t url  = pRequest->getHeader()->getUrl();
		PAS_DEBUG3("%s ACL applied TR[%d], URL=%s", setMyInfo(), currTransaction->id(), url.toStr());
		PHTR_DEBUG3("%s ACL applied TR[%d], URL=%s", getMyInfo(), currTransaction->id(), url.toStr());
		****/
		proxyHost = newHost;
		proxyPort = newPort;
		return ACL_DNS_APPLIED;
	}

	// proxy 변경 필요
	if(searchResult == ACL_DENY_ACL)
	{
		proxyHost = newHost;
		proxyPort = newPort;
		return AR_CHANGE_PROXY;
	}

	// proxy 변경 필요 없음
	return AR_HOLD_PROXY;
}

/**
공지 처리

FILTER_BY_URL -- 엄격한 스트링 매치 검사.   http://localhost:80  와 http://localhost 는 서로 다른 것으로 간주된다.
FILTER_BY_DOMAIN -- http://localhost:80  와 http://localhost 는 서로 같은 것으로 간주된다.
*/
bool	ClientHandler::checkStatFilter(Transaction *tr, char *notimesg )
{
	
	const HTTP::RequestHeader* h = tr->getRequest()->getHeader();
	StatFilterMng *pStatFilterMng = StatFilterMng::instance();

	char *urlValue = 0;

	if (tr->hotNumberConverted)
		urlValue = tr->realUrl;
	else
		urlValue = tr->orgUrl;
		
	if (pStatFilterMng->isBlocked(FILTER_BY_URL,  urlValue, 0, notimesg))
		return true;
		
	if (pStatFilterMng->isBlocked(FILTER_BY_DOMAIN,  (char*)h->getHost().toStr(), h->getPort(), notimesg))
		return true;
		
	if (pStatFilterMng->isBlocked(FILTER_BY_MDN,  tr->phoneNumber,  0, notimesg))
		return true;

	return false;
}


/**
CP 응답 후에.
*/
int ClientHandler::addHashKey(HTTP::ResponseHeader* pDestHeader, const url_t& requestUrl, const size_t requestSize, int responseBodyLeng)
{
	PAS_TRACE1("ClientHandler::addHashKey >> fd[%d]", get_handle());

	ACE_ASSERT(pDestHeader != NULL);
	ACE_ASSERT(requestSize > 0);

	// hashkey 생성
	char hashKey[MaxHashKeySize+4];
	HashKey::getKtfHashKey(hashKey, requestUrl.toStr());
	

	//!!! Contents-Length 와 실제 데이타 사이즉 다를 수  있다. 
	//!!! 실제 데이타 길이 기준으로 작성하여야 한다.  2006.10.17
	
	int expectedRSSIZE =  responseBodyLeng;

	// 임시 KTF_HTTP_KEY 데이터 생성
	line_t line;
	line.sprintf("%s;RQSIZE=%u;RSSIZE=%d", hashKey, requestSize, expectedRSSIZE);

	// KTF_HTTP_KEY 추가
	int ret = pDestHeader->addElementAtFront("KTF_HTTP_KEY", line);
	if(ret < 0)
	{
		PAS_ERROR1("ClientHandler::addHashKey >> Http 헤더에 KTF_HTTP_KEY 추가 실패, fd[%d]", get_handle());
		return -1;
	}

	// 임시 송신용 http header 생성
	HTTP::header_t header;
	ret = pDestHeader->build(&header);
	if(ret < 0)
	{
		PAS_ERROR1("ClientHandler::addHashKey >> 임시 Http 헤더 데이터 생성 실패, fd[%d]", get_handle());
		return -1;
	}

	// KTF_HTTP_KEY 추가에 따른 RSSIZE 변화 오차 수정	
	for(int loop=0; loop<3; loop++) // while(true) 에서 수정함. 혹시 무한 루프되면 안되니까.
	{
		int diffSize = (header.size() +responseBodyLeng) - expectedRSSIZE;

		// 오차가 없다면 종료
		if(diffSize == 0)
			break;

		expectedRSSIZE += diffSize;
		line.sprintf("%s;RQSIZE=%u;RSSIZE=%d", hashKey, requestSize, expectedRSSIZE);

		pDestHeader->replaceElement("KTF_HTTP_KEY", line);
		ret = pDestHeader->build(&header);
		if(ret < 0)
		{
			PAS_ERROR1("ClientHandler::addHashKey >> 오차 보정 Http 헤더 데이터 생성 실패, fd[%d]", get_handle());
			return -1;
		}
	}
	HashKey::writeLog(session.phoneNumber, session.ipAddr, hashKey, requestUrl.toStr(), requestSize, expectedRSSIZE);

	return 0;
}



CpHandler* ClientHandler::pickCpHandler(const host_t& host, int port)
{
	/*
	_cp.setTraceLog( tracelog );

	if (session.isSSL())
	{
		return &_cp;
	}

	if(Config::instance()->cp.ReuseCpConnection)
	{
		if(_cp.getHost() != host || _cp.getPort() != port)
			_cp.reset(host, port);
	}
	else
	{
		_cp.reset(host, port);
	}


	return &_cp;
	*/

	if(session.isSSL() && _pCP != NULL)
	{
		if(_pCP->getHost() == host || _pCP->getPort() == port)
		{
			return _pCP;
		}
	}

	if(_pCP != NULL)
	{
		delete _pCP;
		_pCP = NULL;
	}

	if(_pCP == NULL)
	{
		// CP 핸들러 생성 & 등록
		_pCP = new CpHandler(reactor(), this);
		_pCP->init();
		_pCP->setTraceLog( this->tracelog );

		// 정상 소스
		_pCP->setHost(host);	
		_pCP->setPort(port);

		return _pCP;
	}

	

/*
	CpHandler* pCP = NULL;

	// 2006-12-01 현근창
	// ReuseCpConnection 버그로 인하여 ReuseCpConnection 기능 제거
	//if (session.isSSL() || Config::instance()->cp.ReuseCpConnection)
	if (session.isSSL())
	{
		pCP = cpList.get(host, port);
	}

	if(pCP != NULL)
	{
		PAS_DEBUG3("REUSE CP[%X] %s:%d", pCP, host.toStr(), port);
		PHTR_DEBUG3("REUSE CP[%X] %s:%d", pCP, host.toStr(), port);
		pCP->setTraceLog( this->tracelog );
		return pCP;
	}

	// CP 핸들러 생성 & 등록
	pCP = new CpHandler(reactor(), this);
	pCP->init();
	pCP->setTraceLog( this->tracelog );

	#ifdef VERIFY_PAS
	port  = Config::instance()->cp.fakeCpPort;
	
	PAS_INFO1("VERIFY_PAS (COMPILED FOR TEST):  CP= localhost:%d",  port);
	pCP->setHost("127.0.0.1");	
	pCP->setPort(port);	
	#else
	// 정상 소스
	pCP->setHost(host);	
	pCP->setPort(port);
	#endif
	

	cpList.add(pCP);

	PAS_TRACE3("NEW CP[%X] %s:%d", pCP, host.toStr(), port);
	PHTR_DEBUG3("NEW CP[%X] %s:%d", pCP, host.toStr(), port);
	
	return pCP;
*/
}


/**
CP handler를 새로 생성하거나, 기존 CP handler를 이용하여 새로운 요청 처리를 개시하도록 한다.
CP 서버 연결 실패 등의 예외 처리를 고려하여야 한다.
*/
void ClientHandler::requestToCP(Transaction *tr)
{
	ACE_ASSERT(tr != NULL);
	
	preRequestToCP(tr);

	HTTP:: Request *pRequest = tr->getRequest();

	// DNS Query 가 실패했을 경우 에러처리한다.
	if ( strcmp(tr->cpIpAddr,"0.0.0.0")==0)
	{
		PAS_INFO("Can't request to CP. Because of DNS query failure.");

		ResponseBuilder::DnsFail( tr->getResponse() );
		tr->setErrorRespCode( RESCODE_CP_TIMEOUT );
		afterCpTransaction(tr);
		return ;
	}

	PHTR_DEBUG3("CP REQ: Host=%s, IP=%s, Port=%d", tr->cpHostName, tr->cpIpAddr, tr->cpPort);
	

#ifdef DEBUG_LOG_HTTP_HEADER
	{
		BigString httpReqHeader(pRequest->getRawHeader()->rd_ptr(), pRequest->getHeadLeng());
		PAS_INFO1("Contents of header which request to CP is\n%s", httpReqHeader.c_str());
	}
#endif
	
	PHTR_HEXDUMP(pRequest->getRawHeader()->rd_ptr(), pRequest->getHeadLeng(), "CP REQ HEAD"); 
	
	CpHandler* pCP = pickCpHandler(tr->cpIpAddr, tr->cpPort);

	logHttpReq("[CP REQ]", tr, false);
	
	int resultRequest = pCP->start(tr);
	if(resultRequest < 0)
	{
		// Connection 실패 --> 적당한 오류 메시지를 단말로 전달.
		PAS_INFO3("Can't connect to CP{%s, %s:%d}", tr->cpHostName, tr->cpIpAddr, tr->cpPort);
		PHTR_INFO3("Can't connect to CP{%s, %s:%d}", tr->cpHostName, tr->cpIpAddr, tr->cpPort);

		ResponseBuilder::CpConnFailed( tr->getResponse() );
		tr->setErrorRespCode( RESCODE_CP_CONN );
		afterCpTransaction(tr);
		return ;
	}

	PAS_TRACE3("CP CONN TRYING: Host=%s, IP=%s, Port=%d", tr->cpHostName, tr->cpIpAddr, tr->cpPort);
	PHTR_DEBUG3("CP CONN TRYING: Host=%s, IP=%s, Port=%d", tr->cpHostName, tr->cpIpAddr, tr->cpPort);
}


/**
CP 응답 후에. 
현재는 Bill Info 를 CP 측에서 HTTP header 에 포함해서 주므로 이 기능은 config 에 ON/OFF 설정하여야 한다.
*/
int ClientHandler::addBillInfo(HTTP::ResponseHeader* pDestHeader)
{
	PAS_TRACE1("ClientHandler::addBillInfo >> fd[%d]", get_handle());

	ACE_ASSERT(pDestHeader != NULL);

	char key[MaxBillInfoKeySize+4];

	if(Util::CreateBillInfoKey(key) < 0)
	{
		PAS_ERROR1("ClientHandler::addBillInfo >> Bill Info Key 생성 실패, fd[%d]", get_handle());
		return -1;
	}

	pDestHeader->addElementAtFront("BILL_INFO", key);

	return 0;
}

/**
안내 페이지 관련 요청에 대해서는 BILL_INFO에 KTF_BILL_INFO_PAGE를 입력하여 무과금 처리되도록 한다.
*/
int ClientHandler::addGuidePageBillInfo(HTTP::ResponseHeader* pDestHeader)
{
	PAS_TRACE1("ClientHandler::addGuidePageBillInfo >> fd[%d]", get_handle());

	ACE_ASSERT(pDestHeader != NULL);

	const char* key = "KTF_BILL_INFO_PAGE";

	pDestHeader->addElementAtFront("BILL_INFO", key);

	return 0;
}

void ClientHandler::addProxyInfo(HTTP::RequestHeader* pDestHeader)
{
	PAS_TRACE("addProxyInfo");

	ACE_ASSERT(pDestHeader != NULL);

	const Config* pConfig = Config::instance();

	//MULTI_PROXY: KUN00
	//HTTP_PROXY_INFO:PNAME:t-upas-02;PTIME:20060926001736

	char hostName[32] = "\0";
	gethostname(hostName, sizeof(hostName)-1);

	char curTime[32] = "\0";
	Util2::get_nowtime_str_simple(curTime);

	char proxyInfo[256] = "\0";
	snprintf(proxyInfo, sizeof(proxyInfo), "PNAME:%s;PTIME:%s", hostName, curTime);

	pDestHeader->addElementAtFront("HTTP_PROXY_INFO", proxyInfo);
	pDestHeader->addElementAtFront("MULTI_PROXY", pConfig->process.serverID);
}

/**
단말로 Http Response 메시지를 송신하는 것이다.
실제적로 socket send 가 아니라 sendQueue  에 넣는 것이다.
*/
int	ClientHandler::sendRespToPhoneHeadbody(HTTP::Response* resToPhone)
{
	// 송신큐에 헤더 입력
	int allLen = resToPhone->getHeadLeng() + resToPhone->getBodyLeng();
	if(allLen > 0)
	{
		ACE_Message_Block *allmesg = MemoryPoolManager::instance()->alloc(allLen);

		allmesg->copy(resToPhone->getRawHeader()->rd_ptr(), resToPhone->getHeadLeng());
		if (resToPhone->getRawBody())
			allmesg->copy(resToPhone->getRawBody()->rd_ptr(), resToPhone->getBodyLeng());

		int resultEnqueue = enSendQueue(allmesg->rd_ptr(), allmesg->length());

		MemoryPoolManager::instance()->free(allmesg);

		if (resultEnqueue==0)
			sentDirectCnt++;
		else if (resultEnqueue==1)
			sentByQueCnt++;
		
		if(resultEnqueue >= 0)
		{
			PAS_TRACE2("sendRespToPhone_headbody, SEND PHONE RESP - %d bytes, %s", allLen, getMyInfo());
			PHTR_DEBUG2("ClientHandler::sendRespToPhone_headbody, SEND PHONE RESP - %d bytes, %s", allLen, getMyInfo());
		}

		// enQueue fail
		else
		{
			PAS_INFO1("sendRespToPhone_headbody, send fail - fd[%d]", get_handle());
			PHTR_INFO1("ClientHandler::sendRespToPhone_headbody, send fail - fd[%d]", get_handle());
			return -1;
		}
	}

	// no exist send data
	else 
	{
		PAS_INFO2("PHONE RESP size wrong - %d bytes, %s", allLen, getMyInfo());
		PHTR_INFO2("ClientHandler:: PHONE RESP size wrong - %d bytes, %s", allLen, getMyInfo());	
	}

	return 0;
}


/**
단말로 Http Response 메시지를 송신하는 것이다.
실제적로 socket send 가 아니라 sendQueue  에 넣는 것이다.
*/
int	ClientHandler::sendRespToPhone(HTTP::Response* resToPhone)
{
	// 송신큐에 헤더 입력
	const int headerLen = resToPhone->getRawHeader()->length();

	if (headerLen  <= 0)
	{
		PAS_ERROR2("ClientHandler:: header length error (%d), fd[%d]", headerLen, get_handle());
		PHTR_DEBUG2("ClientHandler:: header length error (%d), fd[%d]", headerLen, get_handle());
	}
	
	int resultEnqueue = enSendQueue(resToPhone->getRawHeader()->rd_ptr(), resToPhone->getRawHeader()->length());
	
	#ifdef HTTP_DUMP
	filedump->init("PHONE-RESP-HEAD", 4);
	filedump->write(resToPhone->getRawHeader()->rd_ptr(), resToPhone->getRawHeader()->length());
	#endif
	
	if(resultEnqueue < 0)
	{
		PAS_ERROR1("ClientHandler:: 응답 헤더 송신 실패, fd[%d]", get_handle());
		PHTR_DEBUG1("ClientHandler:: 응답 헤더 송신 실패, fd[%d]", get_handle());
		return -1;
	}
	else
	{
		PAS_DEBUG2("ClientHandler::SEND RESP HEAD - %d bytes, %s", headerLen, getMyInfo());
		PHTR_DEBUG2("ClientHandler::SEND RESP HEAD - %d bytes, %s", headerLen, getMyInfo());
	}

	// 송신큐에 바디 입력
	const int bodyLen = resToPhone->getBodyLeng();

	#ifdef HTTP_DUMP
	filedump->init("PHONE-RESP-BODY", 4);
	if (resToPhone->getRawBody() != NULL)
		filedump->write(resToPhone->getRawBody()->rd_ptr(), bodyLen);
	#endif
	
	if(bodyLen > 0)
	{
		ACE_ASSERT(resToPhone->getRawBody() != NULL);
		
		resultEnqueue = enSendQueue(resToPhone->getRawBody()->rd_ptr(), resToPhone->getRawBody()->length());
		if (resultEnqueue==0)
			sentDirectCnt++;
		else if (resultEnqueue==1)
			sentByQueCnt++;
		
		if(resultEnqueue < 0)
		{
			PAS_ERROR1("ClientHandler:: 응답 바디 송신 실패, fd[%d]", get_handle());
			PHTR_DEBUG1("ClientHandler:: 응답 바디 송신 실패, fd[%d]", get_handle());
			return -1;
		}
		else
		{
			PAS_DEBUG2("ClientHandler::SEND RESP BODY - %d bytes, %s", bodyLen, getMyInfo());
			PHTR_DEBUG2("ClientHandler::SEND RESP BODY - %d bytes, %s", bodyLen, getMyInfo());
		}
	}
	return 0;
}



/**
*/
int	 ClientHandler::getUserInfo(Transaction *tr)
{
	UserInfoMng *userInfoMng = UserInfoMng::instance();
	if (userInfo == NULL && tr->phoneNumber[0] != '\0')
	{
		userInfo = userInfoMng->searchByMdn(tr->phoneNumber, tr->phoneIpAddr);
		if (userInfo==NULL)
		{  // 처음 접속한 경우이다.
			PAS_DEBUG1("Here comes new user. PhoneNumber[%s]",  tr->phoneNumber);
			userInfo = userInfoMng->add(tr->phoneNumber, tr->phoneIpAddr);
			
			if (userInfo==NULL)
				return -1;

			userInfo->onClientConnection();
		}
		else
		{  // 최근에  접속한  적이 있는 사용자.
			// @todo 최근 접속 시간 확인.
			// 마지막 접속 후 시간 경과가 크면 Auth,  Santa 재수행. 
			// @todo browser 등이 변경되었는지 check
			PAS_TRACE1("UserInfo is already exist. Phone number is %s",  tr->phoneNumber);	
			userInfo->onClientConnection();

			// 첫 접속시에는 기존 사용자 정보가 있으면 , 값을 세팅.
			session.setLastTransactionTime(userInfo->lastRespTime);
		}
	}
	else
	{
		userInfo->updateReqTime();
	}
	
	return 0;

}

/**
*/
int	 ClientHandler::getUserInfo_SSL()
{
	UserInfoMng *userInfoMng = UserInfoMng::instance();
	if (userInfo == NULL)
	{
		userInfo = userInfoMng->searchByAddr(session.getNumber(), session.getIpAddr());
		if (userInfo==NULL)
		{  // 처음 접속한 경우이다.
			PAS_DEBUG1("UserInfoMng:: New SSL User %s",  session.getIpAddr());
			userInfo = userInfoMng->add(session.getNumber(), session.getIpAddr());
			
			if (userInfo==NULL)
				return -1;

			userInfo->onClientConnection();
			
		}
		else
		{  // 최근에  접속한  적이 있는 사용자.
			// @todo 최근 접속 시간 확인.
			// 마지막 접속 후 시간 경과가 크면 Auth,  Santa 재수행. 
			// @todo browser 등이 변경되었는지 check
			PAS_DEBUG2("UserInfoMng:: Old User (SSL) %s, %s",  userInfo->getPhoneNumber(), session.getIpAddr());

			userInfo->copySession(&session);
			userInfo->onClientConnection();
			
			// 첫 접속시에는 기존 사용자 정보가 있으면 , 값을 세팅.
			session.setLastTransactionTime(userInfo->lastRespTime);
		}
	}
	else
	{
		userInfo->updateReqTime();
	}
	
	return 0;

}


/**
단말의 요청 헤더까지 받은 경우. 새 Transaction 을 할당.
정식으로 Transaction 이 시작된 것이 아니다.. 
*/
int ClientHandler::allocTransaction()
{
	ACE_ASSERT(currTransaction == NULL);
	

	currTransaction = new Transaction();
	currHttpRequest = currTransaction->getRequest();
	currTransaction->setLog(tracelog);

	transactionQueue.push(currTransaction);
	
	numTransactionAlloc++;
	numRequests++;
	
	PAS_TRACE2("Alloc Transaction (%d) [%X]", numTransactionAlloc, currTransaction);

	PAS_TRACE2("%s alloc TR[%d]", setMyInfo(), currTransaction->id());
	PHTR_DEBUG2("%s alloc TR[%d]", getMyInfo(), currTransaction->id());
	return 0;
}

/**
flag_pos == FREE_FRONT, FREE_BACK
tr_to_free == NULL or transaction-to-delete

tr_to_free 이 null 이 아니면 일치하는 경우에만 free.

단말 requst 메시지가 이상한 경우, 바로 freeTransaction() 을 하는데,
front 에서 하면 안되고, back 에서 free 하여야 한다.
초도 적용에서 발견한 버그 수정을 위해.
2006.12.16 수정.
*/
void ClientHandler::freeTransaction(int flag_pos, Transaction *tr_to_free)
{
	if (transactionQueue.empty())
	{
		PAS_INFO2("freeTransaction() called too many times [%X] [%s]", tr_to_free, setMyInfo());
		PHTR_INFO2("freeTransaction() called too many times [%X] [%s]", tr_to_free, getMyInfo());
		return;
	}
		
	Transaction *todelete = NULL;

	int popResult = 0;
	
	if (flag_pos == FREE_FRONT)
	{
		if (tr_to_free != NULL)
		{
			// tr_to_free 이 null 이 아니면 일치하는 경우에만 free.
			if (transactionQueue.front()==tr_to_free)
				popResult = transactionQueue.pop_front(todelete);
		}
		else			
			popResult = transactionQueue.pop_front(todelete);
	}
	
	else if (flag_pos == FREE_BACK)
	{
		if (tr_to_free != NULL)
		{
			// tr_to_free 이 null 이 아니면 일치하는 경우에만 free.
			if (transactionQueue.back()==tr_to_free)
				popResult = transactionQueue.pop_back(todelete);
		}
		else			
			popResult = transactionQueue.pop_back(todelete);
	}
	
	else
	{
		PAS_NOTICE1("ERROR freeTransaction()  flag wrong [%s]", setMyInfo());
		PHTR_NOTICE1("ERROR freeTransaction() flag wrong [%s]", getMyInfo());
		return;
	}
	
	if( popResult == -1 )
	{
		PAS_NOTICE2("ERROR Transaction Q pop() failed, TR [%X] [%s]", tr_to_free, setMyInfo());
		PHTR_NOTICE2("ERROR Transaction Q pop() failed, TR [%X] [%s]", tr_to_free, getMyInfo());
		return;
	}

	if (numTransactionAlloc > 0)
	{
		numTransactionAlloc--;
		numResponses++;
	}

	PAS_TRACE3("ClientHandler::Free Transaction - Deleted=[%X], ToFree=[%X] Current=[%X]", 
		todelete, tr_to_free,  currTransaction);			
	PHTR_DEBUG3("ClientHandler:: Free Transaction - Deleted=[%X], ToFree=[%X] Current=[%X]", 
		todelete, tr_to_free,  currTransaction);	
		
	PAS_TRACE2("ClientHandler::Free Transaction -Qsize=%d TrAlloc=%d", transactionQueue.size(), numTransactionAlloc);
	PHTR_DEBUG2("ClientHandler::Free Transaction -Qsize=%d TrAlloc=%d", transactionQueue.size(), numTransactionAlloc);

	if (tr_to_free != NULL && todelete != tr_to_free)
		PAS_NOTICE1("ClientHandler:: Free Transaction - ERROR todelete != tr_to_free %s", getMyInfo());
		
	if(todelete == currTransaction)
	{
		currTransaction = NULL;
		currHttpRequest = NULL;
	}


	delete todelete;

}



/**
단말의 요청 헤더/바디 전체 까지 받은 경우.
정식으로 Transaction 이 시작된 것이다. Transaction Q 에 추가한다.
*/
int ClientHandler::startTransaction(Transaction *tr)
{
	ACE_ASSERT(tr != NULL);

	preStartTransaction(tr);

	HTTP::Request *pRequest = tr->getRequest();
			
	// santa 처리가 완료 되었거나, santa 없이 CP로 진행되는 경우에
	// 또 다른 Request (단말 )를 처리할 수 있는 상태가 된다.
	changeState ( CS_WAIT_REQUEST_HEADER );

	// increase statistic
	pSysStat->clientRequest(1);
	if(pRequest->getContentLength() > 0)
		pSysStat->clientUpload(1);

	if(pRequest->getHeader()->getHost().isEmpty())
	{
		PAS_NOTICE("Host in request header is empty.");
		PAS_NOTICE_DUMP("Request Header", pRequest->getRawHeader()->rd_ptr(), pRequest->getHeadLeng());
		return -1;
	}

	// send mwatch (mIDC system monitoring)
	WatchReporter::IncreaseMsgWatchSvr();

	// pasmonitor (logsvr, KUNLOG) 로 1 초 마다 호출. 실제로는 20초 마다 전송.
	MonitorReporter::increasePasMon();

	session.setThreadId();
	
	// HTTP 헤더 파싱, 결과값은 tr 에 저장
	tr->setTransactionInfo();

	this->tracelog = PhoneTraceMng::instance()->getTraceLog(tr->phoneNumber);
	PAS_TRACE1("tracelog[%X]", tracelog);

	PHTR_HEXDUMP(pRequest->getRawHeader()->rd_ptr(), pRequest->getHeadLeng(), "PHONE REQ HEAD");
	if (pRequest->getRawBody() != NULL) 
	{
		int dump_len = MIN(pRequest->getBodyLeng(), 160);
		PHTR_HEXDUMP(pRequest->getRawBody()->rd_ptr(), dump_len, "PHONE REQ BODY");
	}
			
	// session 정보 변경, seqNum 설정 등의 작업 수행.
	session.beginTransaction(tr);
	session.setCPname(tr);

	if (getUserInfo(tr) < 0)
	{
		PAS_ERROR("Memory full: cannot alloc userinfo");
		PHTR_ERROR("Memory full: cannot alloc userinfo");
		return -1;
	}

	
	// 디버그 정보 출력
	//session.print();
	
	// request를 단말로부터 받은 경우의 설정 처리
	tr->recvPhoneReq();

	 // 혹시 연결 실패할 경우에도 시간값이 제대로 출력되게 하기 위해 여기서 설정. 
	 // CpHandler 에서 실제 연결되는 경우에 다시 업데이트 하므로 여기서 중복해서 불러줘도 된다.
	tr->connectCp(); 
	tr->sendCpReq();

	// PAS 공식 로그 직성
	paslog->startOfTransaction(userInfo, &session, tr);
	
	if (session.isFirst()) // 첫 transaction
	{
		// session 에 전화번호  및 각종 정보 저장.
		session.setFirstInfo(tr);

		if (userInfo)
			userInfo->storeSession(&session);
	}
	else
	{
		// 가상번호가 아니면서,  기존에 session 에 저장된 전화번호와 새 transaction 의 전화번호가 다른 경우 로그 출력.
		if ( ! session.isSantaNumber &&
			strcmp(session.getNumber(), tr->phoneNumber) != 0)
		{
			PAS_WARNING3("Session[%s], Transaction[%s]. 이상한 사용자. 접속 중 전화번호 변경 - %s", 
				session.getNumber(), tr->phoneNumber, setMyInfo());
			PHTR_WARN3("Session[%s], Transaction[%s]. 이상한 사용자. 접속 중 전화번호 변경 - %s",
				session.getNumber(), tr->phoneNumber,  getMyInfo());
		}
	}

	if(Config::instance()->process.browserTypeCheck)
	{
		if(browserTypeCheck(tr) < 0)
			return 0;
	}
	
	if( santaHandler.getState() == SS_NONE) 
	{
		// IMSI 검사, Santa 연동
		// MNC 값이 04, 08 이 아닌 경우 (아마도 00) 에 가상 번호대역인지 검사하고 
		// Santa 연동을 수행한다. -- 2006.10.13

		tr->santaStartTime = ACE_OS::gettimeofday();
		checkSanta(tr);
		tr->santaEndTime = ACE_OS::gettimeofday();

		// 아래는 로그릉 위해 추가한 것.
		if(santaHandler.isPassed())
		{
			PAS_DEBUG2("NO SANTA required - %s  TR[%d]", getMyInfo(), tr->id());
			PHTR_DEBUG2("NO SANTA required - %s  TR[%d]", getMyInfo(), tr->id());
		}
	}
		
	// 인증을 받아야 한다.
	if( santaHandler.getState() == SS_REQUESTED )
	{
		PAS_DEBUG2("SANTA requested - %s  TR[%d]", getMyInfo(), tr->id());
		PHTR_DEBUG2("SANTA requested - %s  TR[%d]", getMyInfo(), tr->id());
		changeState( CS_SANTA_WAITING);
		return 0;
	}

	// Santa 연결이 바로  실패한  경우: 인증 받을 수 있는 상황이 아니므로 에러 메시지 출력
	else if( santaHandler.getState() == SS_FAILED )
	{
		PAS_DEBUG2("SANTA failed - %s  TR[%d]", getMyInfo(), tr->id());
		PHTR_DEBUG2("SANTA failed - %s  TR[%d]", getMyInfo(), tr->id());
		
		// SANTA 조회 실패 페이지 전송
		ResponseBuilder::SantaFailed( tr->getResponse() );

		// Over10 로그 출력을 위한 에러 코드
		tr->setErrorRespCode( RESCODE_SANTA );

		afterCpTransaction(tr);

		return 0;
	}
	else if(santaHandler.isPassed())
	{
		// Santa  연동이 필요 없는 경우는 아래에서 처리된다.
		// santa 연동을 안한 경우 바로 midTransaction() 수행

		midTransaction(tr);
	}
	

	return 0;
}

GuideCode ClientHandler::getFimmSpecificGuideCode(const url_t& url)
{
	// Fimm 전용 안내페이지 요청일 경우 다음과 같은 URL 이 수신된다.
	// "http://ktfproxy.magicn.com:9090/?reqURL=fimm.co.kr"

	url_t schemeHostPort("http://");
	schemeHostPort += Config::instance()->network.kunHost;
	if(Config::instance()->network.listenPort != 80)
	{
		schemeHostPort += ":";
		schemeHostPort += Config::instance()->network.listenPort;
	}

	if(url.incaseFind(schemeHostPort + "/?reqURL=") == 0) 
		return GCODE_Fimm;
	else
		return GCODE_Unknown;
}

url_t ClientHandler::getReqURL(const url_t& url)
{
	// Fimm 전용 안내페이지 요청일 경우 다음과 같은 URL 이 수신된다.
	// "http://ktfkunproxy.magicn.com:9090/?reqURL=fimm.co.kr"

	int pos = url.incaseFind("reqURL=");
	if(pos < 0)
		return url_t();

	return url.substr(pos+7);
}

int ClientHandler::procGuide(Transaction* tr)
{
	PAS_TRACE("Check for guide page.");
	PHTR_DEBUG("Check for guide page.");

	// Sisor와의 통신 키는 MDN 이므로, MDN을 알 수 없을 경우에는 
	// Sisor와 통신 할 수 없다. 그러므로 안내페이지 표시 여부를
	// 확인하지 않고, 무조건 표시하지 않는 것으로 한다.
	if(tr->MDN[0] == '\0')
	{
		PAS_NOTICE("Skip guide checking. Because not found MDN.");
		PHTR_NOTICE("Skip guide checking. Because not found MDN.");
		return 0; // pass
	}

	try
	{
		// 안내페이지 skip 을 셋팅했다.
		if(procSetSkipGuide(tr) == 1)
			return 1; // redirected

		// Fimm Specific 안내페이지를 표시할지 여부
		if(procFimmSpecificShowGuidePage(tr) == 1)
			return 1; // redirected

		// 안내 페이지를 표시하도록 단말사용자를 안내페이지로 redirect 시켰다.
		if(procShowGuidePage(tr) == 1)
			return 1; // redirected
	
		url_t kunSchemeHost("http://");
		kunSchemeHost += Config::instance()->network.kunHost;
		if(tr->getRequest()->getHeader()->getHost().incaseFind(kunSchemeHost) == 0)
		{
			//--------
			// 에러
			//--------
			// PAS 에게 특정 작업을 요청하기 위한 URL 이라면, 요청작업을 처리 후 항상
			// 다른 URL 로 redirect 되어야 한다.
			// 위 요청 처리 작업에서 redirect 되지 않았다는 것은, 작업 도중에 문제가 
			// 발생했음을 의미한다.
			// 계속 진행하면 무한루프가 발생하므로 작업을 중단한다.

			sendHTTPForbidden(tr);
			PAS_INFO1("Access forbidden for %s", tr->getRequest()->getHeader()->getHost().toStr());
			return 1; // stop, because error occur
		}
	}
	catch (Exception e)
	{
		PAS_NOTICE1("Guide page skip. Because of %s", e.toString());
		PHTR_NOTICE1("Guide page skip. Because of %s", e.toString());
	}

	return 0;
}

void ClientHandler::sendHTTPForbidden(Transaction* tr)
{
	ResponseBuilder::Forbidden(tr->getResponse());
	tr->setErrorRespCode(RESCODE_URL_INVALID);

	afterCpTransaction(tr);
}

int ClientHandler::procShowGuidePage(Transaction* tr)
{
	PAS_TRACE("Check for ShowGuidePage");
	PHTR_DEBUG("Check for ShowGuidePage");

	HTTP::value_t strCounter = tr->getRequest()->getHeader()->getElement("COUNTER");
	if(strCounter.isEmpty())
	{
		PAS_INFO("Skip guide checking. Can't find COUNTER field in Http Request Header.");
		PHTR_INFO("Skip guide checking. Can't find COUNTER field in Http Request Header.");
		return 0; // pass
	}
	else
	{
		// 첫 요청일 때만 안내페이지 표시 여부를 판단하고
		// 처음이 아닌 경우는 무조건 안내페이지를 표시 하지 않는다.
		if(strCounter.toInt() > 1)
		{
			PAS_DEBUG("Skip guide page. Because this is not first request.");
			PHTR_DEBUG("Skip guide page. Because this is not first request.");
			return 0; // pass
		}
	}

	// Sisor와의 통신 키는 MDN 이므로, MDN을 알 수 없을 경우에는 
	// Sisor와 통신 할 수 없다. 그러므로 안내페이지 표시 여부를
	// 확인하지 않고, 무조건 표시하지 않는 것으로 한다.
	if(tr->MDN[0] == '\0')
	{
		PAS_NOTICE("Skip guide checking. Because not found MDN.");
		PHTR_NOTICE("Skip guide checking. Because not found MDN.");
		return 0; // pass
	}

	try
	{
		url_t reqURL = tr->getRequest()->getHeader()->getUrl();
		GuideReadResponse res = getSkipGuide(tr, tr->MDN, reqURL);

		// redirect to guide page
		if(res.skipGuide == false)
		{
			PAS_DEBUG1("Show guide page. MDN[%s]", tr->MDN);
			PHTR_DEBUG1("Show guide page. MDN[%s]", tr->MDN);

			int redirectResult = redirectToGuidePage(tr, res.guideCode);

			// redirect 가 실패 했다면, 안내페이지 표시를 무시하고 그냥 계속 진행한다.
			if(redirectResult < 0)
				return 0;

			// redirect 가 성공하였으므로, 추가 진행을 중단한다.
			else
				return 1; // redirected
		}

		// skip
		else
		{
			PAS_DEBUG("Guide page skip. Because skipGuide is on.");
			PHTR_DEBUG("Guide page skip. Because skipGuide is on.");
			return 0;
		}		
	}
	catch (Exception e)
	{
		// 장애시에는 안내페이지를 무조건 보여주지 않고, 그냥 통과한다.

		PAS_NOTICE2("Guide page skip. Because of %s. MDN[%s]", e.toString(), tr->MDN);
		PHTR_NOTICE2("Guide page skip. Because of %s. MDN[%s]", e.toString(), tr->MDN);

		return 0;
	}
}

int ClientHandler::procFimmSpecificShowGuidePage(Transaction* tr)
{
	PAS_TRACE("Check for FimmSpecificShowGuidePage");
	PHTR_DEBUG("Check for FimmSpecificShowGuidePage");

	// Fimm 전용 안내페이지 요청일 경우 다음과 같은 URL 이 수신된다.
	// "http://ktfkunproxy.magicn.com:9090/?reqURL=fimm.co.kr"

	try
	{
		PAS_TRACE2("ReqHost[%s] KunHost[%s]", 
			tr->getRequest()->getHeader()->getHost().toStr(),
			Config::instance()->network.kunHost.toStr());

		if(tr->getRequest()->getHeader()->getHost() != Config::instance()->network.kunHost)
		{
			PAS_TRACE("Skip guide page. Because Host in header is not kun host");
			PHTR_DEBUG("Skip guide page. Because Host in header is not kun host");
			return 0; // pass
		}

		HTTP::value_t strCounter = tr->getRequest()->getHeader()->getElement("COUNTER");
		if(strCounter.isEmpty())
		{
			PAS_INFO("Skip guide page. Because can't find COUNTER field in Http Request Header.");
			PHTR_INFO("Skip guide page. Because Can't find COUNTER field in Http Request Header.");
			return 0; // pass
		}

		// 첫 요청일 때만 안내페이지 표시 여부를 판단하고
		// 처음이 아닌 경우는 무조건 안내페이지를 표시 하지 않는다.
		// Fimm 집적접속폰의 경우 Fimm 서버를 갔다가 PAS 로 redirect 
		// 되는 과정과 또다른 추가적인 복잡한 과정에 의해 COUNTER 값이 처음에 1 이 아니다.
		PAS_DEBUG2("COUNTER[%d] in Http Header, FimmSpecificFirstCount[%d] in Config", strCounter.toInt(), Config::instance()->guide.fimmSpecificFirstCounter);
		if(strCounter.toInt() > Config::instance()->guide.fimmSpecificFirstCounter)
			return redirectToReqURL(tr);

		url_t reqURL = tr->getRequest()->getHeader()->getUrl();
		url_t redirectURL = getReqURL(reqURL);

		if (redirectURL.isEmpty() == true)
			return 0; // pass

		GuideReadResponse res = getSkipGuide(tr, tr->MDN, redirectURL);

		// redirect to guide page
		if(res.skipGuide == false)
		{
			PAS_INFO("Show guide page. Because SkipGuide is off.");
			PHTR_INFO("Show guide page. Because SkipGuide is off.");

			redirectToFimmSpecificGuidePage(tr);
			return 1; // redirected
		}

		// skip => 요청한 페이지로 redirect를 시켜준다.
		else
		{
			PAS_INFO("Guide page skip. Because SkipGuide is on.");
			PHTR_INFO("Guide page skip. Because SkipGuide is on.");

			return redirectToReqURL(tr);
		}		
	}
	catch (Exception e)
	{
		// 장애시에는 안내페이지를 무조건 보여주지 않고, 그냥 통과한다.

		PAS_NOTICE1("Skip guide page. Because of %s", e.toString());
		PHTR_NOTICE1("Skip guide page. Because of %s", e.toString());

		return redirectToReqURL(tr);
	}
}

GuideReadResponse ClientHandler::getSkipGuide(Transaction* tr, const MDN& mdn, const url_t& reqURL )
{
	SisorProxy* pSisor = NULL;

	try
	{
		tr->guideStartTime = ACE_OS::gettimeofday();

		// 질의
		GuideReadRequest req;
		req.mdn = mdn;
		req.reqURL = reqURL;

		PAS_TRACE3("GetSkipGuide >> Request : MDN[%s] URL[%s] Host[%s]", mdn.toString().toStr(), reqURL.toStr(), req.reqURL.toStr());
		PHTR_DEBUG3("GetSkipGuide >> Request : MDN[%s] URL[%s] Host[%s]", mdn.toString().toStr(), reqURL.toStr(), req.reqURL.toStr());

		pSisor = SisorQueue::instance()->get();
		GuideReadResponse res = pSisor->query(req);
		SisorQueue::instance()->put(pSisor);

		PAS_TRACE3("GetSkipGuide >> Response : MDN[%s], GuideCode[%d], skipGuide[%d]", 
			res.mdn.toString().toStr(), static_cast<int>(res.guideCode), static_cast<int>(res.skipGuide));
		PHTR_DEBUG3("GetSkipGuide >> Response : MDN[%s], GuideCode[%d], skipGuide[%d]", 
			res.mdn.toString().toStr(), static_cast<int>(res.guideCode), static_cast<int>(res.skipGuide));	

		tr->guideEndTime = ACE_OS::gettimeofday();

		writeGuideCommunicationLog(tr->guideEndTime - tr->guideStartTime, res.guideCode);

		return res;
	}
	catch (Exception e)
	{
		if(pSisor != NULL)
			delete pSisor;

		PAS_NOTICE1("Query to SISOR is fail. Because of %s. Force to skip guide page.", e.toString());

		GuideReadResponse res;
		res.guideCode = GCODE_Unknown;
		res.skipGuide = true;
		return res;
	}
}

int ClientHandler::procSetSkipGuide(Transaction* tr)
{
	PAS_TRACE("Check for SetSkipGuide.");

	// Sisor와의 통신 키는 MDN 이므로, MDN을 알 수 없을 경우에는 
	// Sisor와 통신 할 수 없다. 그러므로 안내페이지 표시 여부를
	// 확인하지 않고, 무조건 표시하지 않는 것으로 한다.
	if(tr->MDN[0] == '\0')
	{
		PAS_NOTICE("Skip procSetSkipGuide. Because MDN is empty.");
		PHTR_NOTICE("Skip procSetSkipGuide. Because MDN is empty.");
		return 0; // pass
	}

	url_t reqURL = tr->getRequest()->getHeader()->getUrl();
	GuideCode gcode = getSkipGuideCodeFromURL(reqURL);
	
	if(gcode == GCODE_Unknown)
	{
		PAS_TRACE("Request is not SetSkipGuide.");
		PHTR_DEBUG("Request is not SetSkipGuide.");
		return 0; // pass
	}

	// Sisor에게 skip 값을 on 시키도록 알리고, 
	// 단말사용자를 각 서비스의 첫페이지로 redirect 시킨다.
	try
	{
		bool resultOfSetSkipGuide = setSkipGuide(tr, tr->MDN, gcode);	
		if(resultOfSetSkipGuide == false)
			ThrowException(ECODE_UNKNOWN);

		redirectToGuideResultPage(tr, gcode);
	}
	catch (Exception e)
	{
		PAS_INFO1("SetSkipGuide fail. Because of %s", e.toString());
		PHTR_INFO1("SetSkipGuide fail. Because of %s", e.toString());

		redirectToReqURL(tr);
	}
	
	return 1; // redirected
}

bool ClientHandler::setSkipGuide(Transaction* tr, const MDN& mdn, const GuideCode& gcode )
{
	// MDN에 문제가 있거나 gcode 에 문제가 있다면, 아무것도 안한다.
	if(gcode == GCODE_Unknown || mdn.valid() == false)
	{
		PAS_NOTICE2("Can't set skip guide. Because of invalid gcode[%d] or invalid MDN[%s].", gcode, mdn.toString().toStr());
		PHTR_NOTICE2("Can't set skip guide. Because of invalid gcode[%d] or invalid MDN[%s].", gcode, mdn.toString().toStr());
		return false;
	}

	GuideWriteRequest req;
	req.mdn = mdn;
	req.guideCode = gcode;
	req.skipGuide = true;

	PAS_INFO2("Set SkipGuide. MDN[%s] GuideCode[%d]", mdn.toString().toStr(), static_cast<int>(gcode));
	PHTR_INFO2("Set SkipGuide. MDN[%s] GuideCode[%d]", mdn.toString().toStr(), static_cast<int>(gcode));

	GuideWriteResponse res;

	tr->guideStartTime = ACE_OS::gettimeofday();

	SisorProxy* pSisor = NULL;
	
	try
	{
		pSisor = SisorQueue::instance()->get();
		res = pSisor->query(req);
		SisorQueue::instance()->put(pSisor);
	}
	catch (Exception e)
	{
		if(pSisor != NULL)
			delete pSisor;

		PAS_NOTICE1("Guide write fail. Because of %s.", e.toString());

		res.mdn = req.mdn;
		res.resultState = false;
	}

	tr->guideEndTime = ACE_OS::gettimeofday();

	if(res.mdn != req.mdn)
	{
		PAS_NOTICE2("GuideWriteRequest MDN[%s] is miss match with GuideWriteResponse MDN[%s].", 
			req.mdn.toString().c_str(), res.mdn.toString().c_str());
		PHTR_NOTICE2("GuideWriteRequest MDN[%s] is miss match with GuideWriteResponse MDN[%s].", 
			req.mdn.toString().c_str(), res.mdn.toString().c_str());
	}

	if(res.resultState == false)
	{
		PAS_NOTICE("GuideWriteRequest is fail.");
		PHTR_NOTICE("GuideWriteRequest is fail.");
	}

	writeGuideCommunicationLog(tr->guideEndTime - tr->guideStartTime, gcode);

	return res.resultState;
}

GuideCode ClientHandler::getSkipGuideCodeFromURL(const url_t& reqURL)
{
	PAS_TRACE1("ClientHandler::getSkipGuideCodeFromURL >> ReqURL[%s]", reqURL.toStr());
	PHTR_DEBUG1("ClientHandler::getSkipGuideCodeFromURL >> ReqURL[%s]", reqURL.toStr());

	if(reqURL.incaseFind("/?GuideCode=MagicN&SkipGuide=1") > 0)
	{
		return GCODE_MagicN;
	}
	else if(reqURL.incaseFind("/?GuideCode=Fimm&SkipGuide=1") > 0)
	{
		return GCODE_Fimm;
	}
	else if(reqURL.incaseFind("/?GuideCode=MultiPack&SkipGuide=1") > 0)
	{
		return GCODE_MultiPack;
	}

	PAS_TRACE1("Can't recognize GuideCode from Request URL[%s]", reqURL.toStr());

	return GCODE_Unknown;
}

int ClientHandler::redirectToGuidePage(Transaction* tr, const GuideCode gcode)
{
	url_t reqURL = tr->getRequest()->getHeader()->getUrl();
	url_t redirectURL = getGuideURL(gcode, reqURL);
	ResponseBuilder::Redirect(tr->getResponse(), redirectURL);

	afterCpTransaction(tr);

	return 0;
}

int ClientHandler::redirectToFimmSpecificGuidePage(Transaction* tr)
{
	url_t reqURL = tr->getRequest()->getHeader()->getUrl();
	url_t redirectURL = getReqURL(reqURL);
	redirectURL = getGuideURL(GCODE_Fimm, redirectURL);
	ResponseBuilder::Redirect(tr->getResponse(), redirectURL);

	afterCpTransaction(tr);

	return 0;
}

int ClientHandler::redirectToGuideResultPage( Transaction *tr, const GuideCode gcode)
{
	url_t reqURL = tr->getRequest()->getHeader()->getUrl();
	reqURL = getReqURL(reqURL);

	PAS_INFO1("RedirectURL[%s]", getGuideResultURL(gcode, reqURL).toStr());
	ResponseBuilder::Redirect(tr->getResponse(), getGuideResultURL(gcode, reqURL));

	afterCpTransaction(tr);

	return 0;
}

/**
Santa, 처리 후의 진행 과정.
*/
void ClientHandler::midTransaction(Transaction *tr)
{
	ACE_ASSERT(tr != NULL);

	const Config* pConfig = Config::instance();

	// 가상번호 단말기는 SANTA 에서 받은 폰번호를 헤더에 보내야 한다.
	// 핸드폰 번호 최소자리 수 10자리 검사
	if(session.isSantaNumber)
	{
		tr->setSantaResult(session.phoneNumber, session.IMSI);

		if (session.isFirst()) // 첫 transaction
			userInfo->set(tr->phoneNumber, tr->phoneIpAddr); // 2006.12.14 
	}

	int checkResult = checkAuth(tr);
	if(checkResult < 0)
	{
		PAS_NOTICE2("AUTH failed TR[%d] %s", tr->id(), getMyInfo());
		PHTR_NOTICE2("AUTH failed TR[%d] %s", tr->id(), getMyInfo());
		// Auth 인증 실패시 에러 메시지 전송
		ResponseBuilder::AuthFailed( tr->getResponse() );
		tr->setErrorRespCode( RESCODE_AUTH ); // Over10 로그 출력을 위한 에러 코드
		afterCpTransaction(tr);
		return;
	}

	PAS_TRACE2("AUTH OK TR[%d] %s", tr->id(), getMyInfo());
	PHTR_DEBUG2("AUTH OK TR[%d] %s", tr->id(), getMyInfo());

	// 핫 넘버 적용
	if(pConfig->hotnumber.enable)
		applyHotNumber(tr);

	// ACL 적용전에 호스트명 기록.
	tr->setCpConnInfo_first();

	// ACL 적용
	if(pConfig->acl.enable)
	{
		if(procACL(tr) < 0)
			return;
	}

	// 안내페이지
	if(pConfig->guide.enable)
	{
		if(procGuide(tr) == 1)
			return; // 안내페이지와 관련된 작업을 했으므로, 더이상 진행을 하지 않는다.
	}

	// 서비스 인증
	if(pConfig->service.enable)
	{
		if(procService(tr))
			return;
	}

	char notimesg[512];
	if(checkStatFilter(tr, notimesg))
	{
		PAS_DEBUG1("StatFilter Blocked %s ", session.getNumber());
		PHTR_DEBUG1("StatFilter Blocked %s", session.getNumber());
		
		tr->setErrorRespCode(RESCODE_STATFILTER);
		ResponseBuilder::StatFilterBlocked(tr->getResponse(), notimesg, strlen(notimesg));
		afterCpTransaction(tr);
		return;
	}

	tr->setCpConnInfo_second();

	// 헤더에 추가 정보 입력
	if(additionalInfoToReqHeader(tr) < 0)
	{
		PAS_INFO("Fail additional info insert to header.");
		return;
	}

	// CP에게 데이터 요청
	PAS_DEBUG("Request to CPProxy.");
	PHTR_DEBUG("Request to CPProxy.");
	requestToCP(tr);
}

/**
CP 응답 후의 처리 내용. HashKey 등을 추가. 
CP 요청을 하지 않은 경우에도 여기를 거치게 한다.
*/
void ClientHandler::afterCpTransaction(Transaction *tr)
{
	ACE_ASSERT(tr != NULL);

	preAfterCpTransaction(tr);

	tr->recvCpResp();
	tr->setCpTime(); // CP 와의 연동 없이 끝난 경우에도 시간값 설정하기 위해.
	tr->setDone();

	HTTP::Request* pReq = tr->getRequest();
	HTTP::Response* pRes = tr->getResponse();

	ACE_ASSERT(pReq != NULL);
	ACE_ASSERT(pRes != NULL);

	const HTTP::RequestHeader* h = pReq->getHeader();
	
	// 헤더에 추가적인 정보 기록
	HTTP::ResponseHeader modifyHeader = additionalInfoToResHeader((*pRes->getHeader()), tr->phoneNumber, tr->orgUrl, 
		tr->cpReqBytes, pRes->getContentLength(), h->getHost(), tr->correlationKey);
	pRes->setHeader(modifyHeader);
	
	//PAS_DEBUG_DUMP("PHONE RESP HEAD", resTrResponse->getRawHeader()->rd_ptr(), resTrResponse->getHeadLeng());
	PHTR_HEXDUMP(pRes->getRawHeader()->rd_ptr(), pRes->getHeadLeng(), "PHONE RESP HEAD");
	
	if (pRes->getRawBody()) {
		int dump_len = MIN(pRes->getBodyLeng(), 160);
		//PAS_TRACE_DUMP("PHONE RESP BODY", pRes->getRawBody()->rd_ptr(), dump_len);
		PHTR_HEXDUMP(pRes->getRawBody()->rd_ptr(), dump_len, "PHONE RESP BODY");
	}

	// 단말단으로 실제 전송 
	finishTransaction(tr);
}


/**
CP의 응답을 받거나 오류 상황에서 Transaction 이 종료되는 경우 처리.
Transaction 완료를 수행한다. 파이프라이닝 처리.
Q의 front()와 일치하면 바로 단말로 전송. 아니면 연기. front()를 처리할 때 Q 에 남아 있는 일이 있으면 같이 처리.
*/
int ClientHandler::finishTransaction(Transaction *tr)
{
	ACE_ASSERT(tr != NULL);

	session.setCPname(tr);

	if (userInfo)
		userInfo->storeSession(&session);
			
	if (transactionQueue.empty())
	{
		PAS_NOTICE1("Transaction finished with NO Q: %s", getMyInfo());
		PHTR_NOTICE1("Transaction finished with NO Q: %s", getMyInfo());

		// let's just send the result to phone
	}
	else
	{
		if (transactionQueue.front() != tr)
		{
			// 나중에 보내야 한다.
			PAS_DEBUG3("Transaction finish delayed - TR[%d], front TR[%d], %s", 
				tr->id(), transactionQueue.front()->id(), getMyInfo());
			PHTR_DEBUG3("Transaction finish delayed - TR[%d], front TR[%d], %s", 
				tr->id(), transactionQueue.front()->id(), getMyInfo());
			return -1;
		}
	}

	while (true)
	{
		HTTP::Response *response = tr->getResponse();

		PAS_DEBUG2("Transaction finished. TRID[%d] %s", tr->id(), setMyInfo());
		PHTR_DEBUG2("Transaction finished. TRID[%d] %s", tr->id(), getMyInfo());
		
		// response t를 CP 로부터 받은  경우의 설정 처리
		tr->sendPhoneResp();

		// 단말로 응답 보내기		
		// 스트리밍일 경우는 여기서 송신하지 않고, 수신 즉시 실시간으로 송신한다.
		if( !tr->streaming() )
		{
			//sendRespToPhone( response ); // -- head/body 분리 전송 방식
			sendRespToPhoneHeadbody(response); // -- head/body 통합 전송 방식 -- 주소록 테스트에서 문제 발생.
		}

		// increase statistic
		pSysStat->clientResponse(1);	

		#ifdef HTTP_LOG
		logHttpResp("[PHONE RESP]", tr, false);
		#endif
		
		// Transaction 종료시 정보 설정 처리.
		session.endTransaction(tr);

		if (userInfo)
			userInfo->updateRespTime();
			
		// PAS 공식 로그 직성
		paslog->endOfTransaction(userInfo, &session,  tr);
		
		// 로그 작성 후에 
		session.setLastTransactionTime();		

		if (tr->connCloseRequested)
		{
			//!!! opera browser 업로드시에 단말 요청 헤더에 Connection: close 가 포함된 경우.
			requestClose(); // 부모 클래스 함수 -- PasHandler
			PAS_INFO1("Connection:Close - Request Close  [%s]", tr->phoneNumber);
		}

		// 완료된 transaction 을 Q 에서 제거.
		freeTransaction(FREE_FRONT, tr);

		// 완료되었으나  파이프라이닝 순서로 Queue 에 남아 있는 응답들을  처리한다.
		if (transactionQueue.empty())
			break;
			
		tr = transactionQueue.front();
		if (! tr->isDone())
			break;

		PAS_DEBUG2("Transaction: delayed TR [%d] now finishing, %s", tr->id(), getMyInfo() );
		PHTR_DEBUG2("Transaction: delayed TR [%d] now finishing, %s", tr->id(), getMyInfo() );
	}
	
	return 0;
}


void ClientHandler::startSSL(Transaction *tr)
{
	// SSL 도 report 에 추가.  2006.12.28
	// send mwatch (mIDC system monitoring)
	WatchReporter::IncreaseMsgWatchSvr();

	// pasmonitor (logsvr, KUNLOG) 로 1 초 마다 호출. 실제로는 20초 마다 전송.
	MonitorReporter::increasePasMon();
	
	tr->setCpConnInfo_first();
	
	host_t sslHost(tr->cpHostName);
	int sslPort(tr->cpPort);


	// session, tr  설정 순서는 잘 지켜야 한다. 주의 필요.
	session.setThreadId();
	session.beginSSL();
	
	tr->beginSSL();
	tr->setTransactionInfo();
	tr->recvPhoneReq();
	tr->sendCpReq();
	
	session.setSSLHost(sslHost);
	session.setSSLPort(sslPort);

	// user 정보에서 session 으로 가져 온다.
	getUserInfo_SSL();

	// session 정보에서 tr 으로 가져 온다.
	session.copyTransaction(tr);

	if (tr->phoneNumber[0] == '\0')  // 처음부터 SSL 로 올라오라, 사용자 정보가 UserInfo 에 없는 경우. 
	{
		tr->setAnonymous();
	}
	
	PAS_DEBUG3("[%s] SSL CONNECT : %s %d", session.getNumber(), sslHost.toStr(), sslPort);
	PHTR_DEBUG3("[%s] SSL CONNECT : %s %d", session.getNumber(), sslHost.toStr(), sslPort);
	logHttpReq("[SSL CONNECT PHONE REQ]", tr, true);
	
	session.beginTransaction(tr);
	
	
	paslog->startOfTransaction(userInfo, &session, tr);

	tr->setCpConnInfo_second();

	 // 혹시 연결 실패할 경우에도 시간값이 제대로 출력되게 하기 위해 여기서 설정. 
	 // CpHandler 에서 실제 연결되는 경우에 다시 업데이트 하므로 여기서 중복해서 불러줘도 된다.
	tr->connectCp(); 
	
	host_t sslAddr(tr->cpIpAddr); // IP address 로 연결하면 잘 안되는 문제로 , 도메인을 사용. ( ent.wooribank.com:443)

	requestSSLConnection(tr, sslHost, sslPort);
}

/*----------
[SSL CONNECT PHONE REQ] : Leng=71, Header=71, Body=0
12:06:52 CONNECT ent.wooribank.com:443 HTTP/1.1^M
Host: ent.wooribank.com:443^M
^M

[SSL RESULT PHONE REQ] : Leng=243, Header=243, Body=0
12:06:55 RESULT^M
CPData: cpname=woori;svccode=wooribanking000^M
User-Agent: MobileExplorer/1.2 (Mozilla/1.22; compatible; KUNF12; ^M
HTTP_PHONE_NUMBER: 8201073989200^M
HTTP_PHONE_SYSTEM_PARAMETER: BASE_ID:326, NID:36, SID:2189, BASE_LAT:0, BASE_LONG:0^M
^M

--------*/

/**
*/
void ClientHandler::finishSSL(Transaction *tr,  bool resultRecved /* = false */)
{
	// SSL 종료
	//storeSSLTransaction(sslResult);
	if (resultRecved)
	{
		PAS_DEBUG1("ClientHandler::finishSSL() - SSL RESULT : %s", setMyInfo());
		PHTR_DEBUG1("ClientHandler::finishSSL() - SSL RESULT : %s", getMyInfo());
		
		tr->beginSSL();
		tr->recvPhoneReq();
		tr->setTransactionInfo();
		session.setFirstInfo(tr);
		session.setCPname(tr);
		session.copyTransaction(tr);
	}
	else
	{
		PAS_DEBUG1("ClientHandler::finishSSL() - SSL FINISH : %s", setMyInfo());
		PHTR_DEBUG1("ClientHandler::finishSSL() - SSL FINISH : %s", getMyInfo());
		
		session.copyTransaction(tr);
	}

	if (userInfo) //2006-12-13
		userInfo->storeSession(&session);
		
	if (tr->phoneNumber[0] == '\0')  // 처음부터 SSL 로 올라오라, 사용자 정보가 UserInfo 에 없는 경우. 
	{
		tr->setAnonymous();
	}
	
	
	logHttpReq("[SSL RESULT PHONE REQ]", tr, true);

	tr->sendPhoneResp();
	tr->setCpTime(); // CP 와의 연동 없이 끝난 경우에도 시간값 설정하기 위해.
	tr->setDone();
	tr->endSSL();
	snprintf(tr->realUrl, MAX_URL_LEN, "%s:%d", session.sslHost.toStr(), session.sslPort);
	session.endTransaction(tr);
	session.endSSL();


	if (userInfo)
		userInfo->updateRespTime();

	paslog->openFiles();
	paslog->writeSslPasResponse(userInfo, &session, tr, resultRecved);
	paslog->writeSslPasStat(userInfo, &session, tr);
	paslog->writeSslPasIdr(userInfo, &session, tr);

	if (userInfo)
		userInfo->updateReqTime();
		
	// 로그 작성 후에 	
	session.setLastTransactionTime();
	session.clearSSL();
	
	
}

void ClientHandler::storeSSLTransaction(const HTTP::KTFSSLResult& /* sslResult */)
{
	// @todo : Http 헤더 수신 alloc 한 트랜잭션 데이터 처리후 제거
}



void ClientHandler::requestSSLConnection(Transaction * /* tr */, const host_t& host, const int port)
{
	CpHandler* cp = pickCpHandler(host, port);
	
	// SSL 접속 요청
	if(!cp->isConnected())
	{
		int conn_res = cp->connectSSL(host, port);
		if (conn_res < 0)
		{
			PAS_DEBUG3("SSL Client: CONNECT Failed instantly : %s %d [%s]", host.toStr(), port, session.getNumber());
			PHTR_DEBUG3("SSL Client: CONNECT Failed instantly : %s %d [%s]", host.toStr(), port, session.getNumber());
			responseSSLConnectionFail();
		}
		else
		{
			 /// SSL 서버에 연결 중이면 수신 대기 
			changeState (CS_SSL_CONNECTING );
			PAS_DEBUG3("SSL Client: CONNECTING : %s %d ; %s", host.toStr(), port, session.getNumber());
			PHTR_DEBUG3("SSL Client: CONNECTING : %s %d ; %s", host.toStr(), port,  session.getNumber());
		}
	}

	// 이미 CP와 연결되어 있음
	else
	{
		PAS_DEBUG3("SSL Client: Already CONNECTED : %s %d ; %s", host.toStr(), port, session.getNumber());
		PHTR_DEBUG3("SSL Client: Already CONNECTED : %s %d ; %s", host.toStr(), port,  session.getNumber());
		// SSL통신용 CP 접속이 완료 됐음을 Client에게 알림
		responseSSLConnectionSuccess();
	}
}

void ClientHandler::responseSSLConnectionSuccess()
{
	
	Transaction *tr = NULL;
	
	if (! transactionQueue.empty())
	{
		tr = transactionQueue.front();
	}
	else
	{
	// @todo
		tr = currTransaction;
	}

	HTTP::Response *pResponse = tr->getResponse();
		
	ResponseBuilder::CpSSLConnSuccessed(pResponse);
	
	PAS_DEBUG1("SSL Client: sending CONNECT SUCC to phone; %s",  setMyInfo());
	PHTR_DEBUG1("SSL Client: sending CONNECT SUCC to phone; %s",  getMyInfo());

	tr->recvCpResp(); 
	tr->sendPhoneResp(); 
	tr->setCpTime(); 
	
	enSendQueue(pResponse->getRawHeader()->rd_ptr(), pResponse->getRawHeader()->length());
	if (pResponse->getRawBody() != NULL)
		enSendQueue(pResponse->getRawBody()->rd_ptr(), pResponse->getRawBody()->length());
		
	paslog->endOfTransaction(userInfo, &session, tr);
	session.setLastTransactionTime();
	tr->clearSizeAndTime();
	
}

void ClientHandler::responseSSLConnectionFail()
{
	Transaction *tr = NULL;
	
	if (! transactionQueue.empty())
		tr = transactionQueue.front();
	else
		tr = currTransaction;
	

	HTTP::Response *pResponse = tr->getResponse();
		
	ResponseBuilder::CpSSLConnFailed(pResponse);
	

	PAS_DEBUG1("SSL Client: sending CONNECT FAIL to phone; %s",  setMyInfo());
	PHTR_DEBUG1("SSL Client: sending CONNECT FAIL to phone; %s",  getMyInfo());

	tr->recvCpResp(); 
	tr->sendPhoneResp(); 
	tr->setCpTime(); 
	
	enSendQueue(pResponse->getRawHeader()->rd_ptr(), pResponse->getRawHeader()->length());
	if (pResponse->getRawBody() != NULL)
		enSendQueue(pResponse->getRawBody()->rd_ptr(), pResponse->getRawBody()->length());
	
	paslog->endOfTransaction(userInfo, &session, tr);
	session.setLastTransactionTime();
	tr->clearSizeAndTime();
}





char* ClientHandler::setMyInfo()
{
	snprintf(myinfo, MYID_LEN, "PhoneNumber[%s] Phone[%s:%d] Sock[%d] Seq[%d]",
		session.phoneNumber, session.ipAddr, session.port, session.sockfd, session.getSeqNum());
	return myinfo;
}

#ifdef HTTP_LOG
void ClientHandler::logHttpReq(const char *mesg, Transaction *tr, bool printBody)
#else
void ClientHandler::logHttpReq(const char* /* mesg */, Transaction *tr, bool /* printBody */)
#endif
{
	if (tr==NULL) return;

	HTTP::Request* pRequest = tr->getRequest();
	const ACE_Message_Block*header = pRequest->getRawHeader();

	if (header==NULL)
	{
		PAS_NOTICE1("ODD happend - HTTP::Request==NULL - %s", getMyInfo());
		return;
	}
	
	char	headerBuff[1024*2];
	int	len = MIN(sizeof(headerBuff)-1, header->length());
	memcpy(headerBuff, header->rd_ptr(), len);
	headerBuff[len] = 0;

	#ifdef HTTP_LOG
	httplog->logprint(LVL_INFO, "=== %s : Leng=%d, Header=%d, Body=%d\n", 
			mesg,  pRequest->getHeadLeng() + pRequest->getBodyLeng(), pRequest->getHeadLeng(), pRequest->getBodyLeng());
	httplog->logprint(LVL_INFO, "%s", headerBuff);
	httplog->logprint(LVL_INFO, "---\n" );
	
	const ACE_Message_Block* body = pRequest->getRawBody();
	if (body && printBody && body->length())
	{
		len = MIN(body->length(), 256);
		httplog->hexdump(LVL_INFO,  body->rd_ptr(), len,  "Body ");
	}
	#endif
}

/**
*/
#ifdef HTTP_LOG
void ClientHandler::logHttpResp(const char *mesg, Transaction *tr, bool printBody)
#else
void ClientHandler::logHttpResp(const char * /* mesg */, Transaction *tr, bool /* printBody */)
#endif
{
	if (tr==NULL) return;
	HTTP::Response* resp = tr->getResponse();
	const ACE_Message_Block*header = resp->getRawHeader();
	
	if (header==NULL)
	{
		PAS_NOTICE1("ODD happend - HTTP::Response==NULL - %s", getMyInfo());
		return;
	}
	
	char	headerBuff[1024*2];
	int	len = MIN(sizeof(headerBuff)-1, header->length());
	memcpy(headerBuff, header->rd_ptr(), len);
	headerBuff[len] = 0;

	#ifdef HTTP_LOG
	httplog->logprint(LVL_INFO, "=== %s : Leng=%d, Header=%d, Body=%d\n", 
			mesg,  resp->getHeadLeng() + resp->getBodyLeng(), resp->getHeadLeng(), resp->getBodyLeng());
			
	httplog->logprint(LVL_INFO, "%s", headerBuff);
	httplog->logprint(LVL_INFO, "---\n" );

	const ACE_Message_Block*body = resp->getRawBody();
	if (body && printBody && resp->getBodyLeng())
	{
		len = MIN(body->length(), 32);
		httplog->hexdump(LVL_INFO,  body->rd_ptr(), len,  "Body ");
	}
	#endif
}


/**
현재 처리 중에 있는 transaction 의 단말 request header 를 덤프 한다.
단말로부터 이상한 패킷을 수신한 경우, 그 직전에 어떤 패킷을 수신하였는지 보기 위함이다.
2006.12.16
*/
void ClientHandler::printRecentTransaction()
{
	if (transactionQueue.empty())
		return;
	
	Transaction *recentTr = transactionQueue.back();
	HTTP:: Request *pRequest = recentTr->getRequest();
	PAS_INFO1("ClientHandler:: print the recent HTTP header from phone - %s", getMyInfo());
	PAS_INFO_DUMP("Recent PHONE HEAD", pRequest->getRawHeader()->rd_ptr(), pRequest->getHeadLeng());	
}


void ClientHandler::updateSessionInfo()
{
	bool emptyPhoneNumber = (session.phoneNumber[0] == '\0');
	if(emptyPhoneNumber)
	{
		PAS_DEBUG("Session update skip. Because phoneNumber is empty.");
		return;
	}

	if(strlen(session.phoneNumber) < 10)
	{
		PAS_INFO1("Session update skip. Because phoneNumber[%s] is small.", session.phoneNumber);
		return;
	}

	if(memcmp(session.phoneNumber, "010", 3) != 0)
	{
		PAS_INFO1("Session update skip. Because phoneNumber[%s] is not started with \"010\".", session.phoneNumber);
		return;
	}

	SisorProxy* pSisor = NULL;

	try
	{
		SessionWriteRequest req;
		req.mdn = session.phoneNumber;
		req.connCount = 1;
		req.lastAddr = session.ipAddr;
		req.lastAuthTime = session.pasauthTime;
		req.lastClose = time(NULL);
		req.lastConn = session.startSec;
		req.lastSantaTime = session.santaTime;
		req.tranCount = session.respNums;

		pSisor = SisorQueue::instance()->get();
		pSisor->query(req);
		SisorQueue::instance()->put(pSisor);

		PAS_DEBUG3("Session updated. MDN[%s] ConnCount[%d] TranCount[%d]",
			req.mdn.toString().toStr(), req.connCount, req.tranCount);
	}
	catch (Exception e)
	{
		if(pSisor != NULL)
			delete pSisor;

		PAS_NOTICE1("Session update fail. Because of %s", e.toString());
	}
}

url_t ClientHandler::addQueryToURL(const url_t& srcURL, const char* key, const char* value)
{
	url_t result;

	// exist query
	if(srcURL.find('?') >= 0)
	{
		result.sprintf("%s&%s=%s", srcURL.toStr(), key, value);
	}
	else
	{
		// end of host or directory
		if(srcURL[srcURL.size()-1] == '/')
		{
			result.sprintf("%s?%s=%s", srcURL.toStr(), key, value);
		}

		else
		{
			int schemePos = srcURL.incaseFind("http://");
			int schemeNextPos = (schemePos >= 0) ? (schemePos + 7) : 0;
			
			// end of file
			if(srcURL.find('/', schemeNextPos) >= 0)
			{
				result.sprintf("%s?%s=%s", srcURL.toStr(), key, value);
			}

			// end of host
			else
			{
				result.sprintf("%s/?%s=%s", srcURL.toStr(), key, value);
			}
		}
	}

	return result;
}

url_t ClientHandler::getGuideURL( const GuideCode gcode, const url_t& reqURL )
{
	url_t redirectURL;

	const GuideConfig& guideConf = Config::instance()->guide;
	switch(gcode)
	{
	case GCODE_MagicN:
		redirectURL = guideConf.guideUrlMagicn;
		break;

	case GCODE_Fimm:
		redirectURL = guideConf.guideUrlFimm;
		break;

	case GCODE_MultiPack:
		redirectURL = guideConf.guideUrlMultipack;
		break;

	default:
		redirectURL = guideConf.guideUrlMagicn;
		break;
	}

	if(redirectURL.isEmpty())
		return url_t();

	return addQueryToURL(redirectURL, "reqURL", reqURL);
}

url_t ClientHandler::getGuideResultURL( const GuideCode gcode, const url_t& reqURL )
{
	url_t resultURL;
	const GuideConfig& guideConf = Config::instance()->guide;

	switch(gcode)
	{
	case GCODE_MagicN:
		resultURL= guideConf.guideResultUrlMagicn;
		break;

	case GCODE_Fimm:
		resultURL= guideConf.guideResultUrlFimm;
		break;

	case GCODE_MultiPack:
		resultURL = guideConf.guideResultUrlMultipack;
		break;

	default:
		PAS_INFO1("Unknown gcode[%d]", gcode);
		resultURL = guideConf.guideResultUrlMagicn;
		break;
	}

	return addQueryToURL(resultURL, "reqURL", reqURL);
}

int ClientHandler::redirectToReqURL( Transaction* tr )
{
	url_t reqURL = tr->getRequest()->getHeader()->getUrl();
	url_t redirectURL = getReqURL(reqURL);
	if(redirectURL.isEmpty())
		return 0; // no redirected

	// Pas 로 부터 redirection 됐다는 것을 알리기 위해 URL 뒤에 추가 정보를 붙인다.
	// add slash to trail

	PAS_INFO1("Redirect to reqURL[%s]", redirectURL.toStr());
	PHTR_INFO1("Redirect to reqURL[%s]", redirectURL.toStr());

	// redirect
	ResponseBuilder::Redirect(tr->getResponse(), redirectURL);

	afterCpTransaction(tr);	

	return 1; // redirected
}

bool ClientHandler::procService( Transaction* tr )
{
	PAS_TRACE("Check for service.");

	url_t reqURL = tr->getRequest()->getHeader()->getUrl();
	int pos = reqURL.find("service.");

	if(pos < 0)
		return false;

	url_t serviceStr = reqURL.substr(pos);
	urls_t parts = explode(serviceStr, '.');

	if(parts.size() != 3)
		return false;

	if(parts[0] != "service" || parts[2] != "html")
		return false;

	int productCode = parts[1].toInt();

	ServiceReadRequest req;
	req.mdn = tr->MDN;
	req.productCode = productCode;
	
	SisorProxy* pSisorProxy = NULL;

	bool blockProduct;

	try
	{
		pSisorProxy = SisorQueue::instance()->get();
		ServiceReadResponse res = pSisorProxy->query(req);				
		SisorQueue::instance()->put(pSisorProxy);

		blockProduct = res.blockProduct;
		if(blockProduct)
		{
			PAS_DEBUG("Service was blocked.");
		}
	}
	catch (Exception e)
	{
		PAS_NOTICE1("Query to sisor fail. Because of %s. Force to block.", e.toString());

		blockProduct = true;
		delete pSisorProxy;
	}

	if(blockProduct)
	{
		// redirect
		ResponseBuilder::Redirect(tr->getResponse(), "http:://172.23.35.87/~ssehoony/service/blocked.html");

		tr->setErrorRespCode(RESCODE_STATFILTER);
		afterCpTransaction(tr);	
		return true;
	}

	return false;
}

bool ClientHandler::addCorrelationKey( HTTP::ResponseHeader& destHeader, const CorrelationKey& key )
{
	PAS_TRACE("Add correlation key to response header");
	PAS_TRACE1("Correlation key is %s", key.toString().toStr());

	HTTP::value_t bInfo = destHeader.getElement("BILL_INFO");
	bInfo += ";";
	bInfo += key.toString();
	destHeader.replaceElement("BILL_INFO", bInfo);

	return true;
}

bool ClientHandler::addCorrelationKey( HTTP::RequestHeader& destHeader, const CorrelationKey& key )
{
	PAS_TRACE("Add correlation key to request header");
	PAS_TRACE1("Correlation key is %s", key.toString().c_str());

	destHeader.addElement("CORRELATION_KEY", key.toString());

	return true;
}

void ClientHandler::onReceivedHeaderFromCP(CpHandler* pCP, CpResponseData* pData)
{
	//ACE_ASSERT(pCP == &_cp);
	ACE_ASSERT(pData != NULL);

	const Transaction* tr = pData->getTransaction();
	ACE_ASSERT(tr != NULL);

	if(!tr->streaming())
		return;

	_sentBodySizeByStream = 0;

	HTTP::ResponseHeader header;

	header.parse(pData->getData(), pData->getDataSize());

	// 헤더에 추가적인 정보 기록
	HTTP::ResponseHeader modifyHeader = additionalInfoToResHeader(header, tr->phoneNumber, tr->orgUrl, 
		tr->cpReqBytes, header.getContentLength(), tr->getRequest()->getHeader()->getHost(), tr->correlationKey);
	
	HTTP::header_t rawHeader;
	modifyHeader.build(&rawHeader);
	
	enSendQueue(rawHeader.toStr(), rawHeader.size());

	if (userInfo)
		userInfo->updateRespTime();	
}

void ClientHandler::onReceivedPartialBodyFromCP(CpHandler* pCP, CpResponseData* pData)
{
	//ACE_ASSERT(pCP == &_cp);
	ACE_ASSERT(pData != NULL);

	const Transaction* tr = pData->getTransaction();
	ACE_ASSERT(tr != NULL);

	if(!tr->streaming())
		return;

	_sentBodySizeByStream += pData->getDataSize();

	int res = enSendQueue(pData->getData(), pData->getDataSize());
	
	// 만약 모두 송신하지 못했다면
	// handset으로 송신이 가능 할 때까지 cp의 수신을 대기 시킨다.
	if(res == 1)
		pCP->removeEvent(ACE_Event_Handler::READ_MASK);

	if (userInfo)
		userInfo->updateRespTime();
}

void ClientHandler::onSendQueueEmpty()
{
	// streaming을 위해 client의 send queue에 데이터가 있을 경우
	// CP의 수신 이벤트 MASK를 제거한게 된다.
	// 그러므로 client의 send queue가 empty가 되면, CP가 데이터를 계속 수신 할 수 있도록
	// READ_MASK를 등록해야 한다.
	//_cp.addEvent(ACE_Event_Handler::READ_MASK);
	if(_pCP != NULL)
		_pCP->addEvent(ACE_Event_Handler::READ_MASK);
}

HTTP::ResponseHeader ClientHandler::additionalInfoToResHeader( const HTTP::ResponseHeader& header, const char* phoneNumber, 
	const url_t& orignalRequestURL, const int requestDataSize, const int responseBodySize, const host_t& requestHost, 
	const CorrelationKey& correlationKey )
{
	HTTP::ResponseHeader modifyHeader = header;

	// billinfo 가 ON 이면서 전화번호가 billinfoTarget 에 부합하는 경우에 포함.
	Config *conf = Config::instance();
	if (conf->process.billinfo 
		&&	(conf->process.billinfoTarget.c_str()[0] == '*'  || Util::mdnStartswith(phoneNumber, conf->process.billinfoTarget.c_str() )))
	{
		if(addBillInfo(&modifyHeader) < 0)
		{
			PAS_ERROR1("일반 페이지 BillInfo 추가 실패, fd[%d]", get_handle());
			PHTR_DEBUG("일반 페이지 BillInfo 추가 실패");
		}
	}

	// 안내 페이지 BILL_INFO 삽입
	if (!strcasecmp(conf->network.kunHost, requestHost))
	{
		if (addGuidePageBillInfo(&modifyHeader) < 0)
		{
			PAS_ERROR1("안내 페이지 BillInfo 추가 실패, fd[%d]", get_handle());
			PHTR_DEBUG("안내 페이지 BillInfo 추가 실패");
		}
	}

	// correlation key
	//addCorrelationKey(modifyHeader, correlationKey);

	// hash key
	// @ 주의 :  hash key 를 구할때는 원본 URL 값을 바탕으로 구하여야 한다. 
	// ACL, Hotnumber 등의 결과 URL을 사용하는 것이 아니다.
	if(addHashKey(&modifyHeader, orignalRequestURL, requestDataSize, responseBodySize) < 0)
	{
		PAS_ERROR1("HashKey 추가 실패, fd[%d]", get_handle());
		PHTR_DEBUG("HashKey 추가 실패");
	}

	return modifyHeader;
}

void ClientHandler::writeGuideCommunicationLog( const ACE_Time_Value& duration, const GuideCode& gcode ) const
{
	MyLog log;
	filename_t filename = Config::instance()->getLogPrefix();
	filename += "guide";
	log.openWithYear("./", filename);

	// MDN Duration GCode MSModel Browser URL
	log.logprint(LVL_INFO, "%s %d.%06d %d %s %s %s\n", 
		session.phoneNumber, duration.sec(), duration.usec(), 
		static_cast<int>(gcode), session.msModel, session.browser, 
		session.lastRequestURL.c_str());
	log.close();
}

void ClientHandler::writeAbnormalCloseLog()
{
	if(transactionQueue.size() > 0 || sendQueue.size() > 0)
	{
		if(transactionQueue.size() > 0)
			PAS_INFO("Abnormal close. Transaction queue is not empty.");
		
		if(sendQueue.size() > 0)
			PAS_INFO("Abnormal close. Send queue is not empty.");

		MyLog log;
		filename_t filename = Config::instance()->getLogPrefix();
		filename += "abnormalclose";
		log.openWithYear("./", filename);

		// MDN msModel Browser URL
		log.logprint(LVL_INFO, "%-11s %03d/%03d %-11s %-11s %s\n", 
			session.phoneNumber, session.getRespNum(), session.getSeqNum(),
			session.msModel, session.browser, session.lastRequestURL.c_str());

		log.close();
	}
}


int ClientHandler::additionalInfoToReqHeader( Transaction * tr )
{
	HTTP::Request* pRequest = tr->getRequest();
	HTTP::RequestHeader modifyHeader = (*pRequest->getHeader());

	// Proxy Info 추가
	addProxyInfo( &modifyHeader );

	// Correlation key 추가
	//addCorrelationKey(modifyHeader, tr->correlationKey);

	// 새로운 헤더 생성
	HTTP::header_t newRawHeader;
	if(modifyHeader.build(&newRawHeader) < 0)
	{
		PAS_ERROR1("additionalInfoToReqHeader >> HTTP header building fail. fd[%d]", get_handle());
		PHTR_ERROR("additionalInfoToReqHeader >> HTTP header building fail.");
		return -1;
	}

	// 새로운 헤더 저장
	if(pRequest->setHeader(newRawHeader) < 0)
	{
		PAS_ERROR1("additionalInfoToReqHeader >> HTTP header setting fail. fd[%d]", get_handle());
		PHTR_ERROR("additionalInfoToReqHeader >> HTTP header setting fail.");

		PAS_ERROR1("newRawHeader =>\n%s", newRawHeader.c_str());
		PHTR_ERROR1("newRawHeader =>\n%s", newRawHeader.c_str());

		PAS_ERROR_DUMP("newRawHeader tail", newRawHeader.c_str() + newRawHeader.size() - std::min(newRawHeader.size(), 128U), std::min(newRawHeader.size(), 128U));
		return -1;
	}

	return 0;
}

