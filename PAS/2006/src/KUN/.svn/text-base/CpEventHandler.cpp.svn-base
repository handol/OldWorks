#include "CpEventHandler.h"
#include <ace/Reactor.h>
#include "Common.h"
#include "Mutex.h"
#include "HttpResponseHeader.h"
#include "HttpRequest.h"
#include "Config.h"
#include "HttpCodes.h"
#include "ResponseBuilder.h"
#include <stdio.h>
#include <strings.h>

unsigned int CpHandler::sentDirectCnt = 0;
unsigned int CpHandler::sentByQueCnt = 0;

CpHandler::CpHandler(ACE_Reactor* pReactor, PasHandler* pRequester_)
: PasHandler(pReactor, HT_CpHandler)
{
	myinfo[0] = '\0';

	closeFuncExecuted = false;	
	this->pRequester = pRequester_;
	isSSL = false;

	#ifdef HTTP_DUMP
	filedump = new FileDump();
	#endif

	state = CPS_NONE;
	lastCpRecvSec = 0;
	_port = 0;
}

// construct 이후에 실행하기 위해.
void CpHandler::init()
{
	ACE_ASSERT(reactor()==pRequester->reactor());

	// PasEventHandler 에서 처리하도록 수정. 2007.1.12
	int BufferSize = Config::instance()->process.RecvBufferBytes;
	if (BufferSize > 4 * 1024)
		recvBuffer.size(BufferSize); 

	setMyInfo();
		
	PAS_TRACE2("CP Init:BufferSize[%d] %s", BufferSize, getMyInfo());
	PHTR_DEBUG2("CP Init:BufferSize[%d] %s", BufferSize, getMyInfo());
}

CpHandler::~CpHandler(void)
{
	if(isConnected())
		pSysStat->cpCloseByHost(1);

	close();

	#ifdef HTTP_DUMP
	if( filedump )
		delete filedump;
	#endif
}


void CpHandler::close()
{
	if (lastCpRecvSec != 0)
	{
		PAS_DEBUG1("CP Complete odd body. %s", getMyInfo());
		PHTR_INFO1("CP Complete odd body. %s", getMyInfo());
		changeState ( CPS_RECEIVED_RESPONSE );
		onCompletedReceiveResponse();
	}

	deleteSendQueue();
	
	if (closeFuncExecuted)
	{
		PAS_TRACE1("Already closed. %s", getMyInfo());	
		return;
	}

	closeFuncExecuted = true;
	
	PAS_TRACE1("CP closed. %s", getMyInfo());
	PHTR_DEBUG1("CP closed. %s", getMyInfo());

	if(!requestedQueue.empty())
	{
		PAS_INFO2("CP close(). STILL %d requests left - %s", requestedQueue.size(), getMyInfo());
		PHTR_INFO2("CP close(). STILL %d requests left - %s", requestedQueue.size(), getMyInfo());
		
		if (!pRequester->isConnected())
		{
		// 이미 단말이 close 된 경우.
			PAS_INFO2("CP finish [%s]: phone is closed already. [%s]", getMyInfo(), pRequester->getMyInfo());
			PHTR_INFO2("CP finish [%s]: phone is closed already. [%s]", getMyInfo(), pRequester->getMyInfo());
		}
		else
		{
			PAS_INFO3("CP finish [%s] Finishing %d jobs. [%s]", getMyInfo(), requestedQueue.size(), pRequester->getMyInfo());
			PHTR_INFO3("CP finish [%s] Finishing %d jobs. [%s]", getMyInfo(), requestedQueue.size(), pRequester->getMyInfo());
			finishAll(RESCODE_CP_TIMEOUT);
		}
	}

	PasHandler::close();
	
	setJobDone();
}

void CpHandler::changeState(CPState _state)
{
	PAS_TRACE2("CpHandler::changeState: %d --> %d", state,  _state);
	PHTR_DEBUG2("CpHandler::changeState: %d --> %d", state,  _state);
	state = _state;
}

/**
PAS - CP 서버 사이의 한  transaction의 개시를 의미한다.
개시가 성공적인지 못한 경우 return -1. 
transaction 이 잘 개시된 경우에는 finish() 로 transaction완료를 처리하여야 한다.

@return -1: connect 시도가 실패한 경우. (실제 CP에 접속하기도 전에 실패한 경우이다. 내부 socket 오류 가若?
 CP에 연결 실패는 이후에 event로 알 수 있다.
*/
int CpHandler::start(Transaction *pTransaction)
{
	setMyInfo();

	PAS_TRACE1("CpHandler:: start() %s", getMyInfo());
	PHTR_DEBUG1("CpHandler:: start() %s", getMyInfo());

	startTimeTick(Config::instance()->cp.timeoutCheckInterval);  // Timer 시작 
	
	// CP WEB 에 접속
	if( !isConnected() )
	{
		if (isConnecting())
		{
			PAS_INFO2("Connect requested while connecting %s:%d", _host.toStr(), _port);
			PHTR_INFO2("CpHandler:: start() - connect requested while connecting %s:%d", _host.toStr(), _port);
		}
		else if( connect(_host, _port) == -1 )
		{
			// 2007.1.4 don't close()
			//close();
			return -1;
		}

		changeState (CPS_CONNECTING);
		
		pSysStat->cpConnectionRequest(1);

		startConnectTimer(); // 연결 시도 중이므로 Timer 개시

		// onConnect() 에서 처리. sendToCp() 호출.
		requestedQueue.push(pTransaction);
		setMyInfo();
	}

	// CP WEB 에 이미 접속된 상태
	else
	{
		pTransaction->connectCp();
		int sendres = sendToCp(pTransaction);
		PAS_DEBUG2("Already connected. sendres[%d] %s", sendres, getMyInfo());
		PHTR_DEBUG2("CpHandler:: already connected. sendres[%d] %s", sendres, getMyInfo());
		if (sendres >= 0)
			requestedQueue.push(pTransaction);
	}

	return 0;
}


void CpHandler::onConnect()
{
	PAS_TRACE1("Connected. %s", getMyInfo());
	PHTR_DEBUG1("CpHandler:: connected %s", getMyInfo());

	PasHandler::onConnect();
	
	changeState (CPS_WAIT_RESPONSE_HEADER);

	closeFuncExecuted = false;

	int connFlag = 1;	
	pRequester->onCommand(CID_CP_Connected, this, &connFlag); // 접속 완료 했음을 ClientHandler 알림, 1은 성공을 의미
	pSysStat->cpConnectionEstablished(1); // increase statistic

	if (!requestedQueue.empty())
	{
		Transaction *pTr = requestedQueue.front();
		pTr->connectCp();
		int sendres = sendToCp(pTr);
		PAS_TRACE2("Sent on connect. SendRes[%d] %s", sendres, getMyInfo());
		PHTR_DEBUG2("CpHandler:: sent on connect. SendRes[%d] %s", sendres, getMyInfo());
	}
	
}

/**
sendToCp() 추가: Connect와 send를 분리하는 것. 기존에는 start() 에 포함되었다.
*/
int CpHandler::sendToCp(Transaction *pTransaction)
{
	startReceiveTimer();

	HTTP::Request* pNewRequest = pTransaction->getRequest();
		
	ACE_ASSERT(pNewRequest != NULL);
	ACE_ASSERT(pNewRequest->getRawHeader() != NULL);

	// increase statistic
	pSysStat->cpRequest(1);

	// 헤더 송신
	const ACE_Message_Block* pSendData = pNewRequest->getRawHeader();
	int resultEnqueue = enSendQueue(pSendData->rd_ptr(), pSendData->length());
		
	#ifdef HTTP_DUMP
	filedump->init("CP-REQU-ALL", 2);
	filedump->write(pSendData->rd_ptr(), pSendData->length());
	#endif
	
	if(resultEnqueue < 0)
	{
		PAS_INFO1("CP enSendQueue failed. %s", getMyInfo());
		PHTR_INFO1("CP enSendQueue failed. %s", getMyInfo());

		return -1;
	}

	// body 송신
	const size_t bodyLen = pNewRequest->getBodyLeng();
	if(bodyLen > 0)
	{
		ACE_ASSERT(pNewRequest->getRawBody() != NULL);
		// increase statistic
		pSysStat->cpUpload(1);

		pSendData = pNewRequest->getRawBody();
		resultEnqueue = enSendQueue(pSendData->rd_ptr(), pSendData->length());

		if (resultEnqueue==0)
			sentDirectCnt++;
		else if (resultEnqueue==1)
			sentByQueCnt++;
			
		if(resultEnqueue < 0)
		{
			PAS_NOTICE1("CP enSendQueue failed [%s]", getMyInfo());
			PHTR_NOTICE1("CP enSendQueue failed [%s]", getMyInfo());

			return -1;
		}
	}

	// request를 CP 로 보낸 경우의 설정 처리
	pTransaction->sendCpReq();
	_receivedBodySize = 0;

	return 0;
}

int	CpHandler::finishAll(int errorCode)
{
	int	count = 0;
	while (! requestedQueue.empty())
	{
		count++;
		PAS_INFO2("CpHandler::finishAll() - Count=%d, Code=%d", count, errorCode);
		PHTR_INFO2("CpHandler::finishAll() - Count=%d, Code=%d", count, errorCode);
		finish(errorCode);	
	}
	return 0;
}

/*
하나의 transaction 이 완료되었다고 판단될 때 호출.
transaction 이 잘 개시된 경우에는 finish() 로 transaction완료를 처리하여야 한다.
@param errorCode  default값은 RESCODE_OK. RESCODE_OK은 오류없음을 의미. timeout 시나  오류시에 errorCode를 설정하여 내부에서 호출.
*/
int	CpHandler::finish(int errorCode)
{
	Transaction* finishTransaction;

	if ( !pRequester->isConnected() )
	{
		// 이미 단말이 close 된 경우.
		PAS_INFO1("Phone is closed already. %s", getMyInfo());
		PHTR_INFO1("Phone is closed already. %s", getMyInfo());
		return -1;
	}
	
	if( requestedQueue.empty() || requestedQueue.pop_front(finishTransaction) < 0 )
	{
		PAS_NOTICE1("%s CP finish: 요청하지 않은 결과를 받음.", getMyInfo());
		PHTR_NOTICE1("%s CP finish: 요청하지 않은 결과를 받음.", getMyInfo());
		return  -1;
	}

	// increase statistic
	pSysStat->cpResponse(1);

	PAS_TRACE2("errorCode[%d] %s", errorCode, getMyInfo());
	PHTR_DEBUG2("errorCode[%d] %s", errorCode, getMyInfo());

	// response 를 CP 로부터 받은  경우의 설정 처리
	
	if (errorCode == RESCODE_OK)
	{
		if (currHttpResponse != finishTransaction->getResponse())
		{
			// 오류 상황. 파이프라이닝에서 꼬인 경우라 할 수 있다.   recv에서 mutex 하기 때문에 발생 불가한 상황.
			currHttpResponse = NULL;
			PAS_NOTICE1("%s CP finish: currHttpResponse is wrong.", getMyInfo());
			PHTR_NOTICE1("%s CP finish: currHttpResponse is wrong.", getMyInfo());
			return -1;
		}
		else
		{
			//기존에 Request/Response 쌍을 응답하던 것을 Transaction 하나로 수정함.
			PAS_TRACE1("CP finish: notify to Client. %s", getMyInfo());
			PHTR_DEBUG1("CP finish: notify to Client. %s", getMyInfo());
			pRequester->onCommand(CID_CP_Completed, this, (void*)finishTransaction);
			currHttpResponse = NULL;
		}

	}
	else
	{
		if (errorCode==RESCODE_CP_CONN)
			ResponseBuilder::CpConnFailed( finishTransaction->getResponse() );
		else if (errorCode==RESCODE_CP_TIMEOUT)
			ResponseBuilder::CpTimeout( finishTransaction->getResponse() );

		_receivedBodySize = finishTransaction->getResponse()->getBodyLeng();
			
		finishTransaction->setErrorRespCode( errorCode );

		// ClientHandler 에게 결과 통보.
		pRequester->onCommand(CID_CP_Completed, this, (void*)finishTransaction);
		currHttpResponse = NULL;
	}

	//!!! CP 와의 connection 을 유지 하지 않는다. cp reuse 를 하지 않기 때문.  2006.12.08
	if(isConnected())
		pSysStat->cpCloseByHost(1);
	this->close();
	return 0;
}

/**
SSL CP 서버에 접속 시작. isSSL 값을 true로 세팅한다.
connection fail (timeout) 시에 SSL 이면 다르게 처리하게 하기 위한 것이다.\
SSL 처리는 transaction 정보 없이 처리하므로 다르게 처리하여야 한다.
*/
int CpHandler::connectSSL(const host_t &host, int port)
{
	startTimeTick(Config::instance()->cp.timeoutCheckInterval);  // Timer 시작 
	
	this->setHost(host);
	this->setPort(port);

	// SSL CP 에 접속
	if( !isConnected() )
	{
		if (isConnecting())
			return 0;
			
		if( connect(host, port) == -1 )
		{
			if(isConnected())
				pSysStat->cpCloseByHost(1);
			close();
			return -1;
		}

		changeState (CPS_CONNECTING);
		startConnectTimer(); // 연결 시도 중이므로 Timer 개시
		
		isSSL = true;

	}
	return 0;
}

int CpHandler::sendSSLData(char* buf, size_t bufSize)
{
	if (get_handle() < 0)
		return -1;

	changeState( CPS_SSL );
	enSendQueue(buf, bufSize);
	#ifdef HTTP_DUMP
	filedump->init("CP-REQ-SSL", 2);
	filedump->write(buf, bufSize);
	#endif
	return 0;
}

int CpHandler::finishSSL(int error_code)
{
	if (error_code ==  RESCODE_CP_CONN)
	{
		// '접속 실패 했음'을 ClientHandler에게 알림, 0 은  실패를 의미
		int connFlag = 0;
		pRequester->onCommand(CID_CP_Connected, this, &connFlag);		
	}
	else if(error_code == RESCODE_CP_TIMEOUT)
	{

	}

	isSSL = false;
	return 0;
}

void CpHandler::onReceived()
{
	if (get_handle() < 0)
	{
		PAS_NOTICE1("CpHandler::onReceived - SOCK closed - %s", getMyInfo());
		if(isConnected())
			pSysStat->cpCloseByHost(1);
		close();
		return;
	}

	// re-start timer ( == init timer)
	startReceiveTimer();

	connectedFlag = true;
	
	PAS_TRACE2("CP recv %d bytes %s", recvBuffer.length(), getMyInfo());
	ACE_ASSERT(recvBuffer.length() > 0);
	//PAS_DEBUG_DUMP("recvBuffer", recvBuffer.rd_ptr(), recvBuffer.length());

	// 이벤트 함수 내에서 state 가 변경 된다.
	while(true)
	{
		// 작업 전 버퍼 사이즈
		const size_t oldRecvBufLength = recvBuffer.length();

		// 상태에 따른 이벤트 함수 호출
		switch(state)
		{
		case CPS_WAIT_RESPONSE_HEADER:
			#ifdef HTTP_DUMP
			filedump->init("CP-RESP-HEAD", 3);
			filedump->before(recvBuffer.rd_ptr(), recvBuffer.length());
			#endif
			
			onReceivedResponseHeader();

			#ifdef HTTP_DUMP
			filedump->after(recvBuffer.length());
			#endif
			break;

		case CPS_WAIT_RESPONSE_BODY:
			#ifdef HTTP_DUMP
			filedump->init("CP-RESP-BODY", 3);
			filedump->before(recvBuffer.rd_ptr(), recvBuffer.length());
			#endif
			
			onReceivedResponseBody();

			#ifdef HTTP_DUMP
			filedump->after(recvBuffer.length());
			#endif
			break;

		case CPS_WAIT_RESPONSE_CHUNK_SIZE:
			#ifdef HTTP_DUMP
			filedump->init("CP-RESP-CHUNK", 3);
			filedump->before(recvBuffer.rd_ptr(), recvBuffer.length());
			#endif
			
			onReceivedResponseChunkSize();

			#ifdef HTTP_DUMP
			filedump->after(recvBuffer.length());
			#endif
			
			break;

		case CPS_WAIT_RESPONSE_CHUNK_DATA:
			#ifdef HTTP_DUMP
			filedump->before(recvBuffer.rd_ptr(), recvBuffer.length());
			#endif
			
			onReceivedResponseChunkData();

			#ifdef HTTP_DUMP
			filedump->after(recvBuffer.length());
			#endif
			
			break;

		case CPS_SSL:
			#ifdef HTTP_DUMP
			filedump->init("CP-RESP-SSL", 3);
			filedump->write(recvBuffer.rd_ptr(), recvBuffer.length());
			#endif
			
			onReceivedSSLData();

			break;

		default:
			ACE_ASSERT(false);
		}

		// Header/Body 수신 완료 됐다면
		if(state == CPS_RECEIVED_RESPONSE)
		{
			onCompletedReceiveResponse();
		}

		// 작업 후 버퍼 사이즈
		const size_t newRecvBufLength = recvBuffer.length();

		// 더이상 처리할 데이터가 없다면
		bool noDataProcessed = (oldRecvBufLength == newRecvBufLength);
		if(newRecvBufLength == 0 || noDataProcessed)
			break;
	}

	// 수신 버퍼 정리
	recvBuffer.crunch();

	// 장애 발생
	bool recvBufferFull = (recvBuffer.space() == 0);
	if(recvBufferFull)
	{
		// 수신 초기화
		recvBuffer.reset();
		changeState (CPS_WAIT_RESPONSE_HEADER );
	}
}



void CpHandler::onReceivedSSLData()
{
	// re-start timer ( == init timer)
	startReceiveTimer();
	
	size_t bufSize = recvBuffer.length();
	pRequester->onCommand(CID_CP_SSLData, this, recvBuffer.rd_ptr(), (void*)&bufSize);
	recvBuffer.reset();
}

/// recv 중에 connection close 를 감지한 경우 호출됨
void CpHandler::onRecvFail()
{
	PAS_DEBUG1("Connection closed by CP WEB. %s", getMyInfo());

	if(isConnected())
		pSysStat->cpCloseByHost(1);
	close();
}

/// send 중에 connection close 를 감지한 경우 호출됨
void CpHandler::onSendFail()
{
	// 2006.12.19
	// send 중 오류난 경우 socket close() 하자.
	PAS_NOTICE1("Send fail. %s", getMyInfo());
	// 2007.1.4 don't close()
	//close();
}

/**

@todo 파싱 오류인 경우 단말로 적당한 응답 전송. 현재는 abort 처리.
*/
void CpHandler::onReceivedResponseHeader()
{
	ACE_ASSERT(state == CPS_WAIT_RESPONSE_HEADER);

	// 수신된 데이터가 없다면 skip
	if(recvBuffer.length() == 0)
		return;

	if (requestedQueue.empty())
	{
		// CP에 요청을 보내지도 않았는데  응답이 온 경우 이거나, 프로그램 버그 . ^^
		// advance read pointer -- 다 처리한 것 처럼 포인터 이동.
		PAS_INFO1("CpHandler - Recv unexpected http mesg. %s", getMyInfo());
		PHTR_INFO1("CpHandler - Recv unexpected http mesg. %s", getMyInfo());
		PAS_NOTICE_DUMP("Unexpected response header from CP", recvBuffer.rd_ptr(), recvBuffer.length());

		recvBuffer.rd_ptr(recvBuffer.length());
		return;
	}
	
	Transaction *currTransaction = requestedQueue.front();
	ACE_ASSERT(currTransaction != NULL);

	// currHttpResponse의 포인터는 request 송신시에 세팅하면 안되고, response 수신 시에  세팅하는 것이 맞다. 
	// 파이프라이닝을 고려하기 때문이다.
	currHttpResponse = currTransaction->getResponse();

	if (currHttpResponse == NULL)
	{
		PAS_NOTICE("CpHandler::onReceivedResponseHeader >> currHttpResponse is NULL.");
		return;
	}
	
	// 수신 데이터중 http header 추출
	HTTP::ResponseHeader resHeader;
	HTTP::header_t header;

	// raw buffer에서 http header 부분까지만 잘라서 FastString (header_t) 에 복사한다.
	int ret = resHeader.getHeader(&header, recvBuffer.rd_ptr(), recvBuffer.length());
	if(ret < 0)
	{
		PAS_DEBUG2("CpHandler::waiting more packet to complete Http Header. curr leng=%d. %s", recvBuffer.length(), getMyInfo());
		PHTR_DEBUG2("CpHandler::waiting more packet to complete Http Header. curr leng=%d. %s", recvBuffer.length(), getMyInfo());
		PAS_DEBUG_DUMP("CpHandler::waiting more packet", recvBuffer.rd_ptr(), recvBuffer.length());
		return;
	}

	// advance read pointer
	ACE_ASSERT(header.size() <= recvBuffer.length());
	recvBuffer.rd_ptr(header.size());

	int resultSetHeader = currHttpResponse->setHeader(header);
	if(resultSetHeader < 0)
	{
		PAS_NOTICE1("CpHandler:: Strange Header from CP -%s", getMyInfo());
		PHTR_NOTICE1("CpHandler:: Strange Header from CP -%s", getMyInfo());
		PAS_NOTICE_DUMP("Strange Header from CP", recvBuffer.rd_ptr(), recvBuffer.length());
		return;
	}

	// content-length ?
	bool hasContentData = currHttpResponse->getContentLength() > 0;
	bool hasChunkData = currHttpResponse->hasChunked();
	
	// content-length 기준으로 처리.
	// content-length 와 chunked 가 동시에 존재하더라도 content-length 기준으로 처리.
	// 헤더에 Transfer-Encoding: chunked 도 포함하여 전송. --- old PAS 처리 기준.
	if(hasContentData)
	{
		PAS_DEBUG("Response type is Content-Length.");

		const int loggingContentSize = 1024*1024;
		if(loggingContentSize <= currHttpResponse->getContentLength())
		{
			PAS_INFO3("Big content found. Content size is %d bytes. URL is [%s] %s",
				currHttpResponse->getContentLength(), currTransaction->getRequest()->getHeader()->getUrl().toStr(), setMyInfo());
		}


		// Config 설정을 확인하여 스트리밍 사용 여부를 설정한다.
		if(Config::instance()->network.streaming)
		{
			if(Config::instance()->network.streamingMinSize <= currHttpResponse->getContentLength())
				currTransaction->streaming(true);
			else
				currTransaction->streaming(false);
		}
		else
			currTransaction->streaming(false);

		pSysStat->cpNormal(1);

		if(currTransaction->streaming())
			pSysStat->cpStream(1);

		changeState ( CPS_WAIT_RESPONSE_BODY );
	}
	else if(hasChunkData)
	{
		PAS_DEBUG("Response type is Chunked.");

		// 청크 데이터는 해쉬키 생성문제로 인해 스트리밍을 사용할 수 없다.
		currTransaction->streaming(false);
		pSysStat->cpChunked(1);

		changeState ( CPS_WAIT_RESPONSE_CHUNK_SIZE );
	}	
	else
	{
		PAS_DEBUG("Response type is Non Content-Length.");

		// content-length가 없다면 해쉬키 생성문제로 인해 스트리밍을 사용할 수 없다.
		currTransaction->streaming(false);
		pSysStat->cpOdd(1);

		const HTTP::ResponseHeader *currHeader = currHttpResponse->getHeader();
		PAS_TRACE2("ContentLengthEmpty[%d] StatusCode[%d]",
			currHeader->getElement("Content-Length").isEmpty() ,
			currHeader->getStatusCode());

		// !! 예외 상황 content-length 없고 Chunked 도 아닌 경우. 그러면서 200 인 경우.
		if (currHeader->getElement("Content-Length").isEmpty() && currHeader->getStatusCode() == 200)
		{
			if (recvBuffer.length() > 0)  
				// 수신할 바디가 있는 경우이면 바디 수신하도록 한다.		
				changeState ( CPS_WAIT_RESPONSE_BODY );
			else  
				// 수신할 바디가 바로 없더라도 한번더 socket read 해서 바디 수신하도록 한다.		
				changeState ( CPS_WAIT_RESPONSE_BODY );

			lastCpRecvSec = 0;
		}

		// 헤더만 존재 하는 경우이다.
		else
		{
			changeState ( CPS_RECEIVED_RESPONSE );		
		}		
	}
	
	PAS_TRACE2("CP recv header. %s\n%s", getMyInfo(), header.toStr());
	PHTR_DEBUG2("CP recv header. %s\n%s", getMyInfo(), header.toStr());

	// client handler 측에서 currTransaction 의 streaming 값을 이용하므로, streaming 값이 상황에 맞게 설정된 후에
	// onCommand 를 호출해야 하므로 이 코드를 상단으로 이동 시키면 안된다.
	CpResponseData dataInfo(header.front(), header.size(), currTransaction);
	pRequester->onCommand(CID_CP_Received_Header, this, &dataInfo);
}

bool CpHandler::isCompletedReceiveResponseBody()
{
	if (currHttpResponse == NULL)
		return false;

	ACE_ASSERT(currHttpResponse->valid());

	// 2007.1.3 수신한 바이트가 더 많아도 OK 처리.
	// onReceivedResponseBody() 에서 appendSize 가 minus 경우 발생하여 수정함.
	return (static_cast<int>(currHttpResponse->getContentLength()) <= _receivedBodySize);
}

/**
!! 예외 상황 content-length 없고 Chunked 도 아닌 경우를 처리하기 위해.

handle_timeout() 에서 더 이상 response 메시지의 수신 없음을 판단하고,
메시지 수신을 완료 처리하도록 되어 있다.
*/
void CpHandler::onOddResponseBody()
{
	// lastCpRecvSec != 0 이면  CP 응답을 수신 중임을 의미한다. !! 
	lastCpRecvSec = time(NULL);
	
	_receivedBodySize += recvBuffer.length();

	const int loggingContentSize = 1024*1024;
	if((_receivedBodySize - recvBuffer.length()) < loggingContentSize && loggingContentSize <= _receivedBodySize)
	{
		ACE_ASSERT( !requestedQueue.empty() );
		Transaction* tr = requestedQueue.front();

		PAS_NOTICE3("Big content found. URL[%s] is over %d bytes. Transfer method is Non-Content-Length. %s", 
			tr->getRequest()->getHeader()->getUrl().toStr(), loggingContentSize, setMyInfo());
	}
	
	if(!requestedQueue.front()->streaming())
	{
		int resultAppend = currHttpResponse->appendBody(recvBuffer.rd_ptr(), recvBuffer.length());	
		if(resultAppend < 0)
		{
			PAS_NOTICE2("onOddResponseBody >> Append error, appendSize[%d], %s", recvBuffer.length(), getMyInfo());
			PHTR_NOTICE2("CpHandler::onOddResponseBody >> Append error, appendSize[%d], %s", recvBuffer.length(), getMyInfo());
			// 이런 예외 경우에도 받은 만큼 단말로 응답하자.
		}
	}

	PAS_TRACE3("Received odd body from CP. body %d/%d bytes. %s", recvBuffer.length(), _receivedBodySize, getMyInfo());
	PHTR_DEBUG3("Received odd body from CP. body %d/%d bytes. %s", recvBuffer.length(), _receivedBodySize, getMyInfo());

	recvBuffer.reset();
}

void CpHandler::onReceivedResponseBody()
{
	PAS_TRACE1("CpHandler::onReceivedResponseBody >> fd[%d]", get_handle());
	PHTR_DEBUG("CpHandler::onReceivedResponseBody");

	ACE_ASSERT(state == CPS_WAIT_RESPONSE_BODY);
	ACE_ASSERT(currHttpResponse != NULL);
	ACE_ASSERT(currHttpResponse->valid());

	// 수신된 데이터가 없다면 skip
	if(recvBuffer.length() == 0)
		return;

	if (currHttpResponse->getContentLength() == 0)
	{
		// content-length 없고 Chunked 도 아닌 경우
		onOddResponseBody();
	}
	else
	{
		onNonOddResponseBody();
	}
}

void CpHandler::onReceivedResponseChunkSize()
{
	PAS_TRACE1("CpHandler::onReceivedResponseChunkSize >> fd[%d]", get_handle());
	PHTR_DEBUG("CpHandler::onReceivedResponseChunkSize");

	ACE_ASSERT(state == CPS_WAIT_RESPONSE_CHUNK_SIZE);
	ACE_ASSERT(currHttpResponse != NULL);
	ACE_ASSERT(currHttpResponse->valid());

	// 수신된 추가 데이터가 없다면 skip
	if(recvBuffer.length() == 0)
		return;

	const char* pos = (char*)memchr(recvBuffer.rd_ptr(), '\n', recvBuffer.length());

	// 수신 미완료
	if(pos == NULL)
		return;

	const char* startPos = recvBuffer.rd_ptr();

	size_t sizeOfChunkSizeStringWithTrailer = pos - startPos + 1;

	// \r\n일 경우 pos 를 \r 위치로 셋팅
	if(pos - startPos > 0)
	{
		if(*(pos-1) == '\r')
			--pos;
	}

	//PAS_DEBUG_DUMP("chunk data", recvBuffer.rd_ptr(), recvBuffer.length());

	HTTP::chunkSize_t chunkSizeStr(startPos, pos - startPos);
	recvBuffer.rd_ptr(sizeOfChunkSizeStringWithTrailer);

	//PAS_DEBUG_DUMP("chunk data", recvBuffer.rd_ptr(), recvBuffer.length());

	int chunkSize = strtol(chunkSizeStr, (char **)NULL, 16);

	PAS_TRACE2("onReceivedResponseChunkSize >> chunkSizeStr[%s] chunkSize[%d]", chunkSizeStr.toStr(), chunkSize);
	PHTR_DEBUG2("CpHandler::onReceivedResponseChunkSize >> chunkSizeStr[%s] chunkSize[%d]", chunkSizeStr.toStr(), chunkSize);

	if(chunkSize < 0)
	{
		PAS_ERROR1("CpHandler::onReceivedResponseChunkSize >> Chunk Size 변환 실패, fd[%d]", get_handle());
		PHTR_DEBUG("CpHandler::onReceivedResponseChunkSize >> Chunk Size 변환 실패");
		return;
	}

	currHttpResponse->setChunkSize(chunkSize);
	currHttpResponse->setRecevicedChunkSize(0);

	if(chunkSize > 0)
		changeState ( CPS_WAIT_RESPONSE_CHUNK_DATA );
	else {
		/*
		2006.09.24 - handol
		chunkSize 가 0 인 것을 마지막에 주는 경우가 있다.
		이 경우에도 \r\n 을 읽어 주어야 한다.
		*/
		if (recvBuffer.length() >= 2) {
			PAS_TRACE1("Before absorbing last chunk trailer. Remain buffer size is %d", recvBuffer.length());
			const char *endline = recvBuffer.rd_ptr();
			if (endline[0]=='\n') 
				recvBuffer.rd_ptr(1);
			if (endline[0]=='\r' && endline[1]=='\n') 
				recvBuffer.rd_ptr(2);
			PAS_TRACE1("After absorbing last chunk trailer. Remain buffer size is %d", recvBuffer.length());
		}

		PAS_DEBUG("Chuncked data receive completed.");
			
		changeState ( CPS_RECEIVED_RESPONSE );
	}
}

void CpHandler::onReceivedResponseChunkData()
{
	PAS_TRACE1("onReceivedResponseChunkData >> fd[%d]", get_handle());
	PHTR_DEBUG("CpHandler::onReceivedResponseChunkData");

	ACE_ASSERT(state == CPS_WAIT_RESPONSE_CHUNK_DATA);
	ACE_ASSERT(currHttpResponse != NULL);
	ACE_ASSERT(currHttpResponse->valid());

	// 수신된 추가 데이터가 없다면 skip
	if(recvBuffer.length() == 0)
		return;

	size_t remainSize = currHttpResponse->getChunkSize() - currHttpResponse->getReceivedChunkSize();

	// 더 수신해야할 chunk 데이터가 있다면
	if(remainSize > 0)
	{
		size_t appendSize = std::min(recvBuffer.length(), remainSize);
		int resultAppend = currHttpResponse->appendBody(recvBuffer.rd_ptr(), appendSize);
		if(resultAppend < 0)
		{
			PAS_ERROR1("CpHandler::onReceivedResponseChunkData >> 데이터 붙여 넣기 실패, appendSize[%d]", appendSize);
			PHTR_ERROR1("CpHandler::onReceivedResponseChunkData >> 데이터 붙여 넣기 실패, appendSize[%d]", appendSize);
		}

		_receivedBodySize += appendSize;

		currHttpResponse->setRecevicedChunkSize(currHttpResponse->getReceivedChunkSize() + appendSize);

		ACE_ASSERT(appendSize <= recvBuffer.length());
		recvBuffer.rd_ptr(appendSize);

		const int loggingContentSize = 1024*1024;
		if((currHttpResponse->getBodyLeng() - appendSize) < loggingContentSize && loggingContentSize <= _receivedBodySize)
		{
			ACE_ASSERT( !requestedQueue.empty() );
			Transaction* tr = requestedQueue.front();

			PAS_NOTICE3("Big content found. URL[%s] is over %d bytes. Transfer method is chunked data. %s", 
				tr->getRequest()->getHeader()->getUrl().toStr(), loggingContentSize, setMyInfo());
		}
	}

	//PAS_DEBUG_DUMP("chunk data", recvBuffer.rd_ptr(), recvBuffer.length());

	remainSize = currHttpResponse->getChunkSize() - currHttpResponse->getReceivedChunkSize();
	bool completeChunkDataReceive = (remainSize == 0);

	if(completeChunkDataReceive)
	{
		// chunk 데이터 이후에 따라오는 "\r\n" skip 하기
		if(recvBuffer.length() >= 1)
		{
			if(*recvBuffer.rd_ptr() == '\n')
			{
				recvBuffer.rd_ptr(1);
				changeState ( CPS_WAIT_RESPONSE_CHUNK_SIZE );
			}
			else if(recvBuffer.length() >= 2)
			{
				if(*recvBuffer.rd_ptr() == '\r' && *(recvBuffer.rd_ptr()+1) == '\n')
				{
					recvBuffer.rd_ptr(2);
					changeState ( CPS_WAIT_RESPONSE_CHUNK_SIZE );
				}
				else
				{
					// chunk data가 끝났다면, 꼭 "\r\n"이 있어야 한다.
					// 받은 만큼만 만들어서 보내자.
					// !!! 2006.10.20
					changeState ( CPS_RECEIVED_RESPONSE );
					if (requestedQueue.size() <= 1)
					{
						// 버퍼에 남은 가비지 읽어 버린다.
						PAS_INFO2("CpHandler::onReceivedResponseChunkData() - CONSUME garbage after chunk data. [%d] bytes - %s",
							recvBuffer.length(), getMyInfo());
						recvBuffer.rd_ptr(recvBuffer.length());
					}
				}
			}
		}
	}
}

/**
Contents-Length 보다 더 많이 온 경우 예외 처리.
*/
int CpHandler::handleOddLengthResponse()
{
	if (requestedQueue.size() > 1) // pipelining 인 경우는 예외 처리하지 않는다.
		return 0;

	if (recvBuffer.length() == 0)
		return 0;
	
	PAS_INFO1("CpHandler::handleOddLengthResponse - Receive more than Contents-Length. Recv size is [%d]", recvBuffer.length());
	PHTR_INFO1("CpHandler::handleOddLengthResponse - Receive more than Contents-Length. Recv size is [%d]", recvBuffer.length());

	if(requestedQueue.front()->streaming())
	{
		// 남아 있는 데이터를 그냥 삭제
		recvBuffer.reset();
	}
	else
	{
		// 남아 있는 데이터를 responseBody에 복사
		int orgBodyLeng = currHttpResponse->getBodyLeng();
		int resultAppend = currHttpResponse->appendBody(recvBuffer.rd_ptr(), recvBuffer.length());
		if(resultAppend < 0)
		{
			PAS_ERROR1("CpHandler::handleOddLengthResponse >> 데이터 붙여 넣기 실패, appendSize[%d]", recvBuffer.length());
			PHTR_ERROR1("CpHandler::handleOddLengthResponse >> 데이터 붙여 넣기 실패, appendSize[%d]", recvBuffer.length());
		}

		_receivedBodySize += recvBuffer.length();

		// advance read pointer
		recvBuffer.rd_ptr(recvBuffer.length());

		PAS_INFO3("CpHandler::handleOddLengthResponse - LENG: org=%d contents=%d new=%d",
			orgBodyLeng, currHttpResponse->getContentLength(), currHttpResponse->getBodyLeng());
		PHTR_INFO3("CpHandler::handleOddLengthResponse - LENG: org=%d contents=%d new=%d",
			orgBodyLeng, currHttpResponse->getContentLength(), currHttpResponse->getBodyLeng());
	}

	return recvBuffer.length();
}

/**
 onReceived() 에서 CP 응답을 수신 완료한 경우 호출한다. CP 응답이 chunked 인 경우와 아닌 경우 모두 처리한다.
*/
int CpHandler::onCompletedReceiveResponse()
{
	ACE_ASSERT(pRequester != NULL);
	ACE_ASSERT(currHttpResponse != NULL);
	ACE_ASSERT(currHttpResponse->valid());

	lastCpRecvSec = 0;	
	
	if(currHttpResponse->getBodyBufferExpandCount() > 5)
	{
		PAS_INFO4("Body buffer expanded %d times. contents[%d] body[%d] %s",
			currHttpResponse->getBodyBufferExpandCount(), currHttpResponse->getContentLength(),
			currHttpResponse->getBodyLeng(), getMyInfo());
		PHTR_INFO4("Body buffer expanded %d times. contents[%d] body[%d] %s",
			currHttpResponse->getBodyBufferExpandCount(), currHttpResponse->getContentLength(),
			currHttpResponse->getBodyLeng(), getMyInfo());
	}
	
	changeState ( CPS_WAIT_RESPONSE_HEADER );

	int	http_res_code = currHttpResponse->getHeader()->getStatusCode();
	if (http_res_code == HTTP_CODE_100_CONTINUE || http_res_code == HTTP_CODE_101_SWITCH)
	{
		// 이러한 종류의 HTTP response는 단말로 전송하지 않고, PAS 내부적으로 처리하고 
		// 그 다음 Response 를 기다려야 한다.

		PAS_DEBUG1("%s CP HTTP CONTINUE recved", getMyInfo());
		PHTR_DEBUG1("%s CP HTTP CONTINUE recved", getMyInfo());

		// 새로운 데이타를 받아야 하므로 클리어 해 주자. -- 2006.10.17
		currHttpResponse->clear();
		return 0;
	}

	// Chunked 이면서 동시에 Contents-Length 가 있는 경우, Contents-Length 보다 더 많이 온 경우 등의 예외 처리.
	handleOddLengthResponse();
	
	// 스트리밍이 아닌 경우는 수신 데이터를 가공한다.
	// create Content-Length
	ACE_ASSERT( !requestedQueue.empty() );
	if( !requestedQueue.front()->streaming() )
	{
		// chunk to Content-Length
		if( currHttpResponse->hasChunked() )
		{
			if ( !currHttpResponse->getHeader()->getElement("Content-Length").isEmpty() )
			{	
				// Chunked 이면서 Content-Length 가 있는 경우 -- web server가 좀 이상한 경우이죠 ^^
				// CGI 프로그램을 이상하게 작성하면 발생 가능.
				PAS_NOTICE2("CP %s:%d replied odd Response. Content-Length in Chunked", _host.toStr(), _port);
				PHTR_NOTICE2("CP %s:%d replied odd Response. Content-Length in Chunked", _host.toStr(), _port);
			}
			
			reformatHeader(currHttpResponse);
		}

		// create Content-Length for old web server
		else if ( currHttpResponse->getContentLength() == 0 )
		{
			reformatHeader(currHttpResponse);
		}
	}

	PAS_DEBUG3("Received Response from CP. HeadSize[%d] ContentLength[%d] Body[%d]", 
			currHttpResponse->getHeadLeng(), currHttpResponse->getContentLength(), _receivedBodySize);

	// !!! finish normally
	finish();
	
	return 0;
}

/**
Content-Length 헤더를 업데이트 한다.
이 함수는 Chunked 인 경우에  호출된다. - "Transfer-Encoding" 를 제거하고, Content-Length 를 추가.
Chunked 아니면서 Content-Length 가 없는 경우에도 호출된다. 이상한 웹서버.  - Content-Length 를 0 으로 설정한다.
2006-11-28
*/
int	CpHandler::reformatHeader(HTTP::Response* resultHttpResponse)
{

	HTTP::ResponseHeader resHeader(*resultHttpResponse->getHeader());

	if (! resHeader.getElement("Transfer-Encoding").isEmpty())
	{
		if (resHeader.delElement("Transfer-Encoding"))
			PAS_INFO1("CpHandler::reformatHeader() - delElement(\"Transfer-Encoding\") FAIL - %s",
				getMyInfo());
	}

	
	if(resHeader.getElement("Content-Length").isEmpty())
	{
		// 정상 케이스. 당연히 "Content-Length" 없어야 한다.
		resHeader.addElement("Content-Length", resultHttpResponse->getBodyLeng());
	}
	else
	{
		// Chunked 이면서 Content-Length 가 있는 경우 -- web server가 좀 이상한 경우이죠 ^^
		//resHeader.replaceElement("Content-Length", resultHttpResponse->getBodyLeng());

		//replace 하지 않고 헤더 추가.
		resHeader.addElement("Content-Length", resultHttpResponse->getBodyLeng());
	}
	
	

	HTTP::header_t newRawHeader;
	int ret = resHeader.build(&newRawHeader);
	if(ret < 0)
	{
		PAS_ERROR1("CpHandler::onCompletedReceiveResponse >> 헤더 생성 실패, fd[%d]", get_handle());
		PHTR_ERROR("CpHandler::onCompletedReceiveResponse >> 헤더 생성 실패");
		return -1;
	}

	ret = currHttpResponse->setHeader(newRawHeader);
	if(ret < 0)
	{
		PAS_ERROR1("CpHandler::onCompletedReceiveResponse >> 헤더 셋팅 실패, fd[%d]", get_handle());
		PHTR_ERROR("CpHandler::onCompletedReceiveResponse >> 헤더 셋팅 실패");
		return -1;
	}

	PAS_TRACE1("CpHandler::reformatHeader\n%s", newRawHeader.toStr());

	return 0;
}

host_t CpHandler::getHost()
{
	return _host;
}

int CpHandler::getPort()
{
	return _port;
}

int CpHandler::setHost(host_t host)
{
	ACE_ASSERT(host.size() > 0);

	_host = host;
	return 0;
}

int CpHandler::setPort(const int port)
{
	// 포트가 80 ~ 65535 범위를 넘어가면 기본포트(80)로 셋팅
	if(80 > port || port > 0xFFFF)
		_port = 80;

	else
		_port = port;

	return 0;
}

int CpHandler::handle_timeout(const ACE_Time_Value &current_time, const void* /* act */)
{
	PAS_TRACE1("CpHandler::handle_timeout >> fd[%d]", get_handle());
	PHTR_INFO("CpHandler::handle_timeout");

	const CPConfig& cpConfig = Config::instance()->cp;
	ACE_Time_Value connTimeout(cpConfig.connectionTimeout);
	ACE_Time_Value recvTimeout(cpConfig.receiveTimeout);

	if (lastCpRecvSec != 0)
	{
		time_t now = time(NULL);
	 	int diff = now - lastCpRecvSec;
	 	if (diff >= 2)
		{
	 		PAS_DEBUG3("CP COMPLETE ODD BODY [%s:%d] %s", _host.toStr(), _port, getMyInfo());
	 		PHTR_INFO3("CP COMPLETE ODD BODY [%s:%d] %s", _host.toStr(), _port, getMyInfo());
	 		changeState ( CPS_RECEIVED_RESPONSE );
	 		onCompletedReceiveResponse();
	 		return 0;
	 	}
	}
	
	// 시간이 만료되면 종료
	if(isConnectTimeOut(current_time, connTimeout))
	{
		PAS_INFO1("CP connect timeout %s", getMyInfo());
		PHTR_INFO1("CP connect timeout %s", getMyInfo());

		if (isSSL)
			finishSSL(RESCODE_CP_CONN);
		else
			finishAll(RESCODE_CP_CONN);

		if(isConnected())
			pSysStat->cpCloseByHost(1);
		close();

	}
	else if(isIdle(current_time, recvTimeout))
	{
		if (lastCpRecvSec != 0)
		{
	 		PAS_INFO1("CP COMPLETE ODD BODY %s", getMyInfo());
			PHTR_INFO1("CP COMPLETE ODD BODY %s", getMyInfo());
	 		changeState ( CPS_RECEIVED_RESPONSE );
	 		onCompletedReceiveResponse();
	 		return 0;
	 	}

		PAS_INFO2("CP recv timeout. Q=%d %s",  requestedQueue.size(), getMyInfo());
		PHTR_INFO2("CP recv timeout. Q=%d %s", requestedQueue.size(), getMyInfo());

		
		// timeout 임을 ClientHandler 에 알려주어야 한다. 
		if (isSSL) 
		{
			finishSSL(RESCODE_CP_TIMEOUT);
		}
		else 
		{
			// 이미 처리되었는데 어떻게 timeout 발생인가?  -- 이상 상황
			if (requestedQueue.empty())
			{
				PAS_INFO1("CP recv timeout with Nothing in Q %s", getMyInfo());
				PHTR_INFO1("CP recv timeout with Nothing in Q %s", getMyInfo());
			}
			else
			{
				finishAll(RESCODE_CP_TIMEOUT);
			}			
		}

		if(isConnected())
			pSysStat->cpCloseByHost(1);
		close();
	}
	
	return 0;
}

bool CpHandler::isRemovable()
{
	if( requestedQueue.size() > 0 )
		jobDone = false;

	else
		jobDone = true;

	return jobDone;
}

char* CpHandler::setMyInfo()
{
	Transaction *tr = NULL;
	if (requestedQueue.size() > 0)
		tr = requestedQueue.front();
		
	if (tr)
		snprintf(myinfo, MYID_LEN, "MDN[%s] CPHost[%s:%d] Sock[%d]", 
			tr->phoneNumber,  _host.toStr(), _port, sock.get_handle());
	else
		snprintf(myinfo, MYID_LEN, "CPHost[%s:%d] Sock[%d]", 
			_host.toStr(), _port, sock.get_handle());

	return myinfo;
}

bool CpHandler::isConnecting()
{
	return (state == CPS_CONNECTING);
}

void CpHandler::onNonOddResponseBody()
{
	if(recvBuffer.length() == 0)
		return;

	// content length	
	const int remainSize = currHttpResponse->getContentLength() - _receivedBodySize;

	// 2007.1.3 수신한 바이트가 contentLength 보다 더 많아도 OK 처리.
	// onReceivedResponseBody() 에서 appendSize 가 minus 경우 발생하여 수정함.
	if (remainSize < 0)
	{
		PAS_NOTICE3("CpHandler: remainSize[%d] recv[%d] %s", remainSize, recvBuffer.length(), getMyInfo());
		PHTR_NOTICE3("CpHandler: remainSize[%d] recv[%d] %s", remainSize, recvBuffer.length(), getMyInfo());

		// 2007.1.3 이런 예외 경우에도 받은 만큼 단말로 응답하자.
		changeState ( CPS_RECEIVED_RESPONSE );
		return;
	}

	const int partOfBodySize = std::min(remainSize, (int)recvBuffer.length());

	ACE_ASSERT(0 < partOfBodySize && partOfBodySize <= recvBuffer.length());
	onReceivedPartOfBody(recvBuffer.rd_ptr(), partOfBodySize);

	if(!requestedQueue.front()->streaming())
	{
		// 수신한 데이터를 responseBody에 복사
		int resultAppend = currHttpResponse->appendBody(recvBuffer.rd_ptr(), partOfBodySize);
		if(resultAppend < 0)
		{
			PAS_NOTICE2("CpHandler::onReceivedResponseBody >> Append error, appendSize[%d] %s", partOfBodySize, getMyInfo());
			PHTR_NOTICE2("CpHandler::onReceivedResponseBody >> Append error, appendSize[%d] %s", partOfBodySize, getMyInfo());
			// 이런 예외 경우에도 받은 만큼 단말로 응답하자.
		}
	}

	// advance read pointer
	recvBuffer.rd_ptr(partOfBodySize);

	// 바디 수신 완료?
	if(isCompletedReceiveResponseBody())
	{
		PAS_TRACE2("Body receive complete. Body is %d bytes. ContentLength is %d bytes.",
			_receivedBodySize, currHttpResponse->getContentLength());
		PHTR_DEBUG2("CP recv body %d bytes. %s", _receivedBodySize, getMyInfo());

		changeState ( CPS_RECEIVED_RESPONSE );
	}
	else
	{
		PAS_TRACE("Body receive incomplete.");
	}
}

void CpHandler::onReceivedPartOfBody( const char* srcBuf, const int srcSize )
{
	_receivedBodySize += srcSize;

	if (!requestedQueue.empty())
	{
		CpResponseData dataInfo(srcBuf, srcSize, requestedQueue.front());
		pRequester->onCommand(CID_CP_Received_PartOfBody, this, &dataInfo);
	}
}

void CpHandler::reset( const host_t& host, const int port )
{
	if(isConnected())
		pSysStat->cpCloseByHost(1);

	close();
	stopTimeTick();
	stopConnectTimer();

	setHost(host);
	setPort(port);
}

int CpHandler::getReceiveBodySize()
{
	return _receivedBodySize;
}

void CpHandler::onCloseByPeer()
{
	pSysStat->cpCloseByPeer(1);
	close();
}

