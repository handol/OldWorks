#include "SantaTypes.h"
#include "SantaEventHandler.h"
#include "HttpTypes.h"
#include "HttpResponse.h"
#include "HttpRequest.h"
#include "PasLog.h"
#include "SantaResponse.h"
#include "Config.h"
#include <ace/SOCK_Connector.h>
#include "MyLog.h"

const int SANTA_RECEIVE_TIMEOUT = 10;

using namespace SANTA;
MyLog SantaHandler::santalog;

SantaHandler::SantaHandler(ACE_Reactor* pReactor, PasHandler* pRequester_)
: PasHandler(pReactor, HT_SantaHandler)
{
	pRequester = pRequester_;
	requeterTr = NULL;

	init();
}


SantaHandler::~SantaHandler(void)
{
	close();
}

void SantaHandler::init()
{
	SmallString fname = Config::instance()->getLogPrefix();
	fname += "santa";
	santalog.openWithYear("./", fname.c_str());

	// 인증 절차 상태
	// Santa 를 사용하지 않으면 jobDone 을 true 로 셋팅
	Config *pConf = Config::instance();
	if( pConf->santa.enable )
		setState( SS_NONE );
	else
	{
		setState( SS_RESPONSED );
		setJobDone();
	}

	// 패킷 송수신 상태
	state = SS_WAIT_RESPONSE_HEADER;
}

void SantaHandler::set(ACE_Reactor* pReactor, PasHandler* _pRequester)
{
	reactor(pReactor);	
	pRequester = _pRequester;
}

void SantaHandler::onReceived()
{
	PAS_TRACE1("SantaHandler::onReceived >> fd[%d]", get_handle());

	PAS_DEBUG1("SantaHandler::onReceived >> Data Received %d bytes", recvBuffer.length());
	PHTR_DEBUG1("SantaHandler::onReceived >> Data Received %d bytes", recvBuffer.length());

	ACE_ASSERT(recvBuffer.length() > 0);

	PAS_DEBUG_DUMP("Santa recvBuffer", recvBuffer.rd_ptr(), recvBuffer.length());
	PHTR_HEXDUMP(recvBuffer.rd_ptr(), recvBuffer.length(), "SANTA RESULT");

	// 이벤트 함수 내에서 state 가 변경 된다.
	while(true)
	{
		const size_t oldRecvBufLength = recvBuffer.length();

		// 상태에 따른 이벤트 함수 호출
		switch(state)
		{
		case SS_WAIT_RESPONSE_HEADER:
			onReceivedResponseHeader();
			break;

		case SS_WAIT_RESPONSE_BODY:
			onReceivedResponseBody();
			break;

		case SS_RECEIVED_RESPONSE:
			onCompletedReceiveResponse();
			break;

		default:
			ACE_ASSERT(false);
		}

		const size_t newRecvBufLength = recvBuffer.length();

		// 더이상 처리할 데이터가 없다면
		bool dataProcessed = (oldRecvBufLength != newRecvBufLength);
		if(!dataProcessed)
			break;
	}

	recvBuffer.crunch();

	// 장애 발생
	bool recvBufferFull = (recvBuffer.space() == 0);
	if(recvBufferFull)
	{
		PHTR_ERROR1( "SantaHandler::onReceived >> Receive Buffer Full, %s", setMyInfo() );
		ACE_ASSERT(false);

		// 수신 초기화
		recvBuffer.reset();
		state = SS_WAIT_RESPONSE_HEADER;
	}
}

void SantaHandler::close()
{
	PAS_TRACE1("SantaHandler::close >> fd[%d]", get_handle());
	PHTR_DEBUG1( "SantaHandler::close >> %s", setMyInfo() );

	PasHandler::close();
	setJobDone();
}

/// recv 중에 connection close 를 감지한 경우 호출됨
void SantaHandler::onRecvFail()
{
	if (santaState != SS_FAILED && santaState != SS_RESPONSED)
	{
		setState( SS_FAILED );

		// 여기서 SS_FAILED 일 경우 단말기에게 에러 메시지를 보낸다.
		pRequester->onCommand( CID_Santa_Completed, this );
	}
	
	close();
}

/// send 중에 connection close 를 감지한 경우 호출됨
void SantaHandler::onSendFail()
{
	PAS_DEBUG1( "SantaHandler::onSendFail >> fd[%d]", get_handle() );
	PHTR_DEBUG1( "SantaHandler::onSendFail >> fd[%d]", get_handle() );

	if (santaState != SS_FAILED && santaState != SS_RESPONSED)
	{
		setState( SS_FAILED );

		// 여기서 SS_FAILED 일 경우 단말기에게 에러 메시지를 보낸다.
		pRequester->onCommand( CID_Santa_Completed, this );
	}

	close();
}

void SantaHandler::onReceivedResponseHeader()
{
	PAS_TRACE1("SantaHandler::onReceivedResponseHeader >> fd[%d]", get_handle());

	ACE_ASSERT(state == SS_WAIT_RESPONSE_HEADER);

	// 수신된 데이터가 없다면 skip
	if(recvBuffer.length() == 0)
		return;

	// 수신 데이터중 http header 추출
	HTTP::ResponseHeader resHeader;
	HTTP::header_t header;

	int ret = resHeader.getHeader(&header, recvBuffer.rd_ptr(), recvBuffer.length());
	if(ret < 0)
	{
		PAS_DEBUG1("Http Header Not Found in Received Data, fd[%d]", get_handle());
		PHTR_DEBUG1("Http Header Not Found in Received Data, fd[%d]", get_handle());
		return;
	}

	// advance read pointer
	recvBuffer.rd_ptr(header.size());

	int resultSetHeader = httpResponse.setHeader(header);
	if(resultSetHeader < 0)
	{
		PAS_ERROR1("SantaHandler::onReceivedResponseHeader >> 헤더 셋팅 실패, fd[%d]", get_handle());
		PHTR_ERROR1("SantaHandler::onReceivedResponseHeader >> 헤더 셋팅 실패, %s", setMyInfo());
		ACE_ASSERT(false);
		return;
	}

	PAS_DEBUG1("SantaHandler::onReceivedResponseHeader >> Response Header\n%s", header.toStr());
	PHTR_DEBUG1("SantaHandler::onReceivedResponseHeader >> Response Header\n%s", header.toStr());

	// content-length ?
	bool hasContentData = (httpResponse.getContentLength() > 0);
	if(hasContentData)
	{
		state = SS_WAIT_RESPONSE_BODY;
	}
	else
	{
		PAS_ERROR1("SantaHandler::onReceivedResponseHeader >> Not Exist Body, fd[%d]", get_handle());
		PHTR_ERROR1("SantaHandler::onReceivedResponseHeader >> Not Exist Body, %s", getMyInfo());
		recvBuffer.reset();
	}
}

void SantaHandler::onReceivedResponseBody()
{
	PAS_TRACE1("SantaHandler::onReceivedResponseBody >> fd[%d]", get_handle());

	ACE_ASSERT(state == SS_WAIT_RESPONSE_BODY);
	ACE_ASSERT(httpResponse.getContentLength() > 0);

	// 수신된 데이터가 없다면 skip
	if(recvBuffer.length() == 0)
		return;

	// content length
	const int remainSize = httpResponse.getContentLength() - httpResponse.getBodyLeng();
	const int appendSize = std::min(remainSize, (int)recvBuffer.length());

	ACE_ASSERT(appendSize > 0);

	// 수신한 데이터를 responseBody에 복사
	int resultAppend = httpResponse.appendBody(recvBuffer.rd_ptr(), appendSize);
	if(resultAppend < 0)
	{
		PAS_ERROR1("SantaHandler::onReceivedResponseBody >> 데이터 붙여 넣기 실패, appendSize[%d]", appendSize);
		PAS_ERROR1("SantaHandler::onReceivedResponseBody >> 데이터 붙여 넣기 실패, appendSize[%d]", appendSize);
		return;
	}

	// advance read pointer
	recvBuffer.rd_ptr(appendSize);

	// 바디 수신 완료?
	if(isCompletedReceiveResponseBody())
	{
		PAS_DEBUG1("SantaHandler::onReceivedResponseBody >> 수신 완료, fd[%d]", get_handle());
		state = SS_RECEIVED_RESPONSE;
	}
	else
	{
		PAS_DEBUG1("SantaHandler::onReceivedResponseBody >> 수신 미완료, fd[%d]", get_handle());
	}

	//@todo 결과 메시지 파싱해서 IMSI, MDN 값 구한다.
}

int SantaHandler::onCompletedReceiveResponse()
{
	PAS_DEBUG1("SantaHandler::onCompletedReceiveResponse >> fd[%d]", get_handle());
	PHTR_DEBUG1("SantaHandler::onCompletedReceiveResponse >> %s", setMyInfo());

	ACE_ASSERT(pRequester);
	ACE_ASSERT(static_cast<int>(httpResponse.getContentLength()) == httpResponse.getBodyLeng());

	stopTimeTick();

	int result = 0;

	const ACE_Message_Block* pBody = httpResponse.getRawBody();

	if (pBody==NULL)
	{
		setState( SS_FAILED );
		return result;
	}
	
	// 응답 내용 파싱
	int ret = santaResponse.parse(pBody->rd_ptr(), pBody->length());
	if(ret < 0)
	{
		setState( SS_FAILED );

		PAS_ERROR1("SantaHandler::onCompletedReceiveResponse >> 응답 내용 파싱 실패, fd[%d]", get_handle());
		PHTR_ERROR1("SantaHandler::onCompletedReceiveResponse >> 응답 내용 파싱 실패, %s", setMyInfo());
	}

	SANTA::ResponseCode_t rescode = santaResponse.getResponseCode();
	if( rescode.find("err_") != -1 )
	{
		setState( SS_FAILED );
	}

	if (getState() != SS_FAILED)
		setState( SS_RESPONSED );

	// Santa 로그 파일에 출력
	responseLog( NULL, santaResponse.getResponseCode(), santaResponse.getIMSI(), santaResponse.getMDN() );

	// 여기서 SS_FAILED 일 경우 단말기에게 에러 메시지를 보낸다.
	pRequester->onCommand( CID_Santa_Completed, this );

	state = SS_WAIT_RESPONSE_HEADER;

	requestClose();
	setJobDone();
	
	//@todo 2006.9.16 ACE_Reactor::notify () 를 이용하여  ClientHandler 에 알려 주는 방안.
	// notify ()두번째  파라미터는 masks = ACE_Event_Handler::EXCEPT_MASK
	// 따라서 ClientHandler::handler_exception() 에서 santa 결과를 처리하여야 한다.
	// reactor()->notify(pRequester);  

	return result;
}

int SantaHandler::request(const host_t& host, const int port, const SANTA::id_t& id, const SANTA::password_t& password, const SANTA::svcID_t& svcID, const SANTA::MDN_t& MDN)
{
	ACE_ASSERT(host.isEmpty() == false);
	ACE_ASSERT(0 <= port && port <= 0xffff);

	// 요청 URL 생성
	url_t requestURL;
	requestURL.sprintf("http://%s:%d/apps/sus?SVCID=%s&ID=%s&PASSWORD=%s&FUNC=GET(MDN=%s,MDN+IMSI)",
				host.toStr(), port, svcID.toStr(), id.toStr(), password.toStr(), MDN.toStr());

	// 요청 Http 헤더 생성
	HTTP::header_t header;
	int ret = createRequestHeader(&header, requestURL);
	if(ret < 0)
	{
		santaState = SS_FAILED;
		PAS_ERROR1("SantaHandler::request >> 요청 Http 헤더 생성 실패, fd[%d]", get_handle());
		PHTR_ERROR1("SantaHandler::request >> 요청 Http 헤더 생성 실패, %s", setMyInfo());
		return -1;
	}

	// 헤더 송신
	ret = enSendQueue(header, header.size());
	if(ret < 0)
	{
		santaState = SS_FAILED;
		PAS_ERROR1("SantaHandler::request >> 요청 송신 실패, fd[%d]", get_handle());
		PHTR_ERROR1("SantaHandler::request >> 요청 송신 실패, %s", setMyInfo());
		return -1;
	}

	santaState = SS_REQUESTED;
	return 0;
}

int SantaHandler::request(const host_t& host, const int port, const SANTA::id_t& id, const SANTA::password_t& password, const SANTA::svcID_t& svcID, const SANTA::MSIN_t& MSIN)
{
	// 요청 URL 생성
	url_t requestURL;
	requestURL.sprintf("http://%s:%d/apps/sus?SVCID=%s&ID=%s&PASSWORD=%s&FUNC=GET(MSIN=%s,MDN+IMSI)",
				host.toStr(), port, svcID.toStr(), id.toStr(), password.toStr(), MSIN.toStr());

	// 요청 Http 헤더 생성
	HTTP::header_t header;
	int ret = createRequestHeader(&header, requestURL);
	if(ret < 0)
	{
		santaState = SS_FAILED;
		PAS_ERROR1("SantaHandler::request >> 요청 Http 헤더 생성 실패, fd[%d]", get_handle());
		PHTR_ERROR1("SantaHandler::request >> 요청 Http 헤더 생성 실패, %s", setMyInfo());
		return -1;
	}

	// 헤더 송신
	ret = enSendQueue(header, header.size());
	if(ret < 0)
	{
		santaState = SS_FAILED;
		PAS_ERROR1("SantaHandler::request >> 요청 송신 실패, fd[%d]", get_handle());
		PHTR_ERROR1("SantaHandler::request >> 요청 송신 실패, %s", setMyInfo());
		return -1;
	}

	santaState = SS_REQUESTED;
	return 0;
}

int SantaHandler::request(const host_t& host, const int port, const SANTA::id_t& id, const SANTA::password_t& password, const SANTA::svcID_t& svcID, const SANTA::IMSI_t& IMSI)
{
	// 요청 URL 생성
	url_t requestURL;
	requestURL.sprintf("http://%s:%d/apps/sus?SVCID=%s&ID=%s&PASSWORD=%s&FUNC=GET(IMSI=%s,MDN+IMSI)",
				host.toStr(), port, svcID.toStr(), id.toStr(), password.toStr(), IMSI.toStr());

	// 요청 Http 헤더 생성
	HTTP::header_t header;
	int ret = createRequestHeader(&header, requestURL);
	if(ret < 0)
	{
		santaState = SS_FAILED;
		PAS_ERROR1("SantaHandler::request >> 요청 Http 헤더 생성 실패, fd[%d]", get_handle());
		PHTR_ERROR1("SantaHandler::request >> 요청 Http 헤더 생성 실패, %s", setMyInfo());
		return -1;
	}

	// 헤더 송신
	ret = enSendQueue(header, header.size());
	if(ret < 0)
	{
		santaState = SS_FAILED;
		PAS_ERROR1("SantaHandler::request >> 요청 송신 실패, fd[%d]", get_handle());
		PHTR_ERROR1("SantaHandler::request >> 요청 송신 실패, %s", setMyInfo());
		return -1;
	}

	santaState = SS_REQUESTED;
	return 0;
}

int SantaHandler::createRequestHeader(HTTP::header_t* pDestHeader, const url_t& url)
{
	ACE_ASSERT(pDestHeader != NULL);
	ACE_ASSERT(url.isEmpty() == false);

	HTTP::UrlParser urlParser;
	int ret = urlParser.parse(url);
	if(ret < 0)
	{
		PAS_ERROR1("SantaHandler::createRequestHeader >> URL 파싱 실패, fd[%d]", get_handle());
		PHTR_ERROR1("SantaHandler::createRequestHeader >> URL 파싱 실패, %s", setMyInfo());
		return -1;
	}

	host_t hostAndPort;
	hostAndPort.sprintf("%s:%d", urlParser.getHost().toStr(), urlParser.getPort());

	pDestHeader->sprintf("GET %s HTTP/1.1\r\n"
		"Accept : */*\r\n"
		"Accept-Language: ko\r\n"
		"Host: %s\r\n"
		"\r\n"
		, url.toStr()
		, hostAndPort.toStr());

	return 0;
}

bool SantaHandler::isCompletedReceiveResponseBody()
{
	return (static_cast<int>(httpResponse.getContentLength()) == httpResponse.getBodyLeng());
}

int SantaHandler::handle_timeout(const ACE_Time_Value &current_time, const void* /* act */)
{
	PAS_TRACE1("SantaHandler::handle_timeout >> fd[%d]", get_handle());
	PHTR_DEBUG1("SantaHandler::handle_timeout >> %s", setMyInfo());

	const SantaConfig& santaConfig = Config::instance()->santa;
	ACE_Time_Value connTimeout(santaConfig.connectionTimeout);
	ACE_Time_Value recvTimeout(santaConfig.receiveTimeout);

	if(isConnectTimeOut(current_time, connTimeout))
	{
		
		setState( SS_FAILED );
		PAS_DEBUG1("SantaHandler::handle_timeout >> 접속요청후 접속대기 시간 초과, fd[%d]", get_handle());
		pRequester->onCommand( CID_Santa_TimeOut, this );
		close();
		
	}
	else	if( isIdle(current_time, recvTimeout))
	{
	
		setState( SS_FAILED );
		PAS_DEBUG1("SantaHandler::handle_timeout >> 수신후 수신대기 시간 초과, fd[%d]", get_handle());
		pRequester->onCommand( CID_Santa_TimeOut, this );
		close();		
	}	

	return 0;
}

bool SantaHandler::isRemovable()
{
	return jobDone;
}

void SantaHandler::setPassed()
{
	setState(SS_RESPONSED);
}

bool SantaHandler::isPassed()
{
	return ( santaState == SS_RESPONSED );
}

SantaState SantaHandler::getState()
{
	return santaState;
}

int SantaHandler::connect(const host_t& host, int port)
{
	// Santa에 접속 요청
	PAS_DEBUG2("SantaHandler::connect >> Try connect to Santa[%s:%d]", host.toStr(), port);
	PHTR_DEBUG3( "SantaHandler::connect >> Try connect to Santa[%s:%d] %s", host.toStr(), port, setMyInfo() );

	// onConnect 에서 사용한다.
	santahost = host;
	santaport = port;

	ACE_INET_Addr addr(port, host.toStr());
	ACE_SOCK_Connector connector;
	ACE_SOCK_Stream newSock;
	if(connector.connect(newSock, addr, &ACE_Time_Value::zero) < 0)
	{
		if(errno != EWOULDBLOCK)
		{
			santaState = SS_FAILED;

			PAS_WARNING1("SantaHandler::connect >> Fail Connect to Santa, fd[%d]", get_handle());
			PHTR_WARN1( "SantaHandler::connect >> Fail Connect to Santa, %s", setMyInfo() );
			responseLog( "Connection failed" );

			return -1;
		}
	}

	PHTR_DEBUG1( "SantaHandler::connect >> Connected Santa, %s", getMyInfo() );

	//newSock.enable(ACE_NONBLOCK);

	set_handle(newSock.get_handle());
	
	startTimeTick(Config::instance()->santa.timeoutCheckInterval);
	startConnectTimer();

	// Santa 에 접속했을 때와 송신이 가능할 때 이벤트 받도록 셋팅
	addEvent(CONNECT_MASK);
	return 0;
}

void SantaHandler::onConnect()
{
	PAS_DEBUG2("SantaHandler::onConnect fd[%d]  [%X]", get_handle(), this);
	PHTR_DEBUG1("SantaHandler::onConnect fd[%d]", get_handle());

	startReceiveTimer();

	if( santaRequest(requeterTr->phoneNumber) == -1 )
	{
		PAS_DEBUG("SantaHandler::onConnect santaRequest Failed");
		PHTR_DEBUG("SantaHandler::onConnect santaRequest Failed");

		//setState( SS_FAILED );
		//setJobDone();
	}

	PasHandler::onConnect();
}

int SantaHandler::santaRequest(const char* phoneNum)
{
	Config *pConf = Config::instance();

	// Santa에게 요청할 데이터 셋팅
	SANTA::id_t id = pConf->santa.ID;
	SANTA::password_t pass = pConf->santa.Password;
	SANTA::svcID_t svc = pConf->santa.svcID;
	SANTA::MSIN_t MSIN = phoneNum;

	// Santa 로그 파일에 출력
	requestLog( santahost, santaport, svc, MSIN );

	// Santa 에 패킷 전송
	int Error = request( santahost, santaport, id, pass, svc, MSIN );
	if( Error == -1 )
		return -1;

	return 0;
}

void SantaHandler::setState(SantaState _state)
{
	santaState = _state;
}


/**
return -1 : 연동이 실패한 경우
return 0 : 연동이 필요 없거나, 연동이 개시된 경우.
*/
int SantaHandler::start(Transaction *trInfo)
{
	const Config* pConfig = Config::instance();
	// Santa 인증
	const host_t& host = pConfig->santa.host;
	const int port = pConfig->santa.port;

	requeterTr = trInfo;

	
	if (santaState == SS_REQUESTED)
	{
		return 0;
	}
	
	// Santa 에 접속이 안되면, 인증 실패
	if( connect(host, port) == -1 )
	{
		setState( SS_FAILED );
		setJobDone();
		return -1;
	}

	setState( SS_REQUESTED );

	return 0;
}

void SantaHandler::requestLog(const host_t& host, const int port, const SANTA::svcID_t& svcID, const SANTA::MSIN_t &MSIN)
{
	santalog.logprint( LVL_DEBUG, "REQ::host[%s] port[%d] svcid[%s] MSIN[%s]\n", host.toStr(), port, svcID.toStr(), MSIN.toStr() );
}

/*
접속 일시	접속한 날짜/시간	59:56.5
전화번호(MIN)	조회 요청 전화번호(MIN)	184348583
처리 상태	SANTA 연동 결과 코드	S1/E0
접속 결과	SANTA DB 접속 결과(성공 여부)	0/-1
Request size	요청한 데이터 크기	178
Request size	응답 받은 데이터 크기	130
Network duration	연동 시 네트워크 수행 시간	0.006143
Parsing duration	응답 받은 데이터 parsing 수행 시간	0.000022
SANTA 조회 결과 값	SANTA 조회 결과 값	s/err_noreply
MDN	SANTA 조회 MDN 결과 값	107490638
IMSI	SANTA 조회 IMSI 결과 값	4.50002E+14
*/
void SantaHandler::responseLog(const char *pment, SANTA::ResponseCode_t resp, SANTA::IMSI_t imsi, SANTA::MDN_t mdn)
{
	char terminator='\0';

	if( pment == NULL )
		pment = &terminator;

	santalog.logprint( LVL_DEBUG, "RES::code[%s] IMSI[%s] MDN[%s] %s\n", resp.toStr(), imsi.toStr(), mdn.toStr(), pment );
}

char* SantaHandler::setMyInfo()
{
	snprintf(myinfo, MYID_LEN, "SantaHandler[%p], State[%d], jobDone[%d] fd[%d]",
		this, getState(), jobDone, get_handle());

	return myinfo;
}
