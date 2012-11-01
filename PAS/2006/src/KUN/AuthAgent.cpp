#include "AuthAgent.h"
#include "AuthHandler.h"
#include "Config.h"
#include "UserInfo.h"
#include "UserInfoMng.h"
#include "AuthInfo.h"

#include <ace/SOCK_Connector.h>

AuthAgent *AuthAgent::oneInstance = NULL;

AuthAgent::AuthAgent(ACE_Thread_Manager *threadManager)
: ACE_Task<PAS_SYNCH>(threadManager)
{
	run = true;
	isconnected = false;
	sock = -1;
	errCount = 0;
	errTime = 0;

	SmallString fname = Config::instance()->getLogPrefix();
	fname += "auth";

	authlog = new MyLog;
	authlog->open("./", fname);
}

AuthAgent::~AuthAgent()
{
	if( authlog )
		delete authlog;
}

AuthAgent *AuthAgent::instance(ACE_Thread_Manager* threadManager)
{
	if( oneInstance == NULL )
		oneInstance = new AuthAgent( threadManager );

	return oneInstance;
}

void AuthAgent::stop()
{
	isconnected = false;
	sock.close();
}

int AuthAgent::svc()
{
	PAS_NOTICE("AuthAgent::svc start");

	ACE_Message_Block *mesg = NULL;

	while(run)
	{
		// 이전 MessageBlock 제거
		if( mesg )
			delete mesg;

		mesg = NULL;

		// waiting 3 second for getq
		ACE_Time_Value timeout(ACE_OS::gettimeofday());
		timeout += ACE_Time_Value(3);
		if( getq(mesg, &timeout) < 0 )
		{
			sleep(1);
			continue;
		}

		else
		if( mesg->length() == 0 )
		{
			delete mesg;
			break;
		}

		// ACE_Message_Block 을 AuthInfoMessageBlock 으로 타입 케스팅
		AuthInfoMessageBlock *pAuthInfoMB = NULL;
		pAuthInfoMB = getAuthInfo( mesg );

		if( pAuthInfoMB == NULL )
		{
			continue;
		}

		// Auth 인증 결과를 담을 userInfo 셋팅
		UserInfo *userInfo = NULL;
		userInfo = &pAuthInfoMB->userInfo;

		// Auth 에 요청을 보낼 내용
		AUTH::RequestBody *reqBody = NULL;
		reqBody = &pAuthInfoMB->reqBody;

		// Auth 에 응답 받은 내용
		AUTH::ResponseBody *respBody = NULL;
		respBody = &pAuthInfoMB->respBody;

		// 이미 인증을 거친 상태라면 인증을 건너뛴다.
		UserInfoMng *userInfoMng = UserInfoMng::instance();
		UserInfo *userFound = NULL;

		userFound = userInfoMng->search( userInfo->getPhoneNumber(), userInfo->ipAddr );
		if( userFound )
		{
			// 폰 번호와 IP가 동일하고 인증을 마친 상태라면 인증을 건너뛴다.
			if( userFound->getIntPhoneNumber() == userInfo->getIntPhoneNumber() )
				if( userFound->getIntIpAddr() == userInfo->getIntIpAddr() )
					if( userFound->getAuthState() >= AS_RESPONSED )
					{
						continue;
					}
		}

		if( isconnected == false )
			isconnected = connecttoAuth( Config::instance()->auth.host, Config::instance()->auth.port );

		// Auth와 접속이 종료된 경우, 인증을 건너뛴다.
		if( isconnected == false )
		{
			userInfo->changeAuthState( AS_RESPONSED );
			stop();
			continue;
		}

		// send
		int nsend = sendtoAuth( reqBody, sizeof(AUTH::RequestBody) );
		if( nsend <= 0 )
		{
			incErrCount();
			requestLog( pAuthInfoMB->reqBody, "Connection Closed" );
			userInfo->changeAuthState( AS_RESPONSED );

			// 재접속 시도
			stop();
			continue;
		}

		requestLog( pAuthInfoMB->reqBody );

		// recv
		int nrecv = recvfromAuth( respBody, sizeof(AUTH::ResponseBody) );
		if( nrecv <= 0 )
		{
			incErrCount();
			responseLog( pAuthInfoMB->respBody, "Connection Closed" );
			userInfo->changeAuthState( AS_RESPONSED );

			// 재접속 시도
			stop();
			continue;
		}

		responseLog( pAuthInfoMB->respBody );

		decErrCount();

		// 인증결과
		AuthState state = getAuthState( respBody );

		// 인증결과를 UserInfoMng 에 셋팅
		if( userFound == NULL )
		{
			userFound = userInfoMng->add( userInfo->getPhoneNumber(), userInfo->ipAddr );
			if( userFound == NULL )
			{
				// 유저를 찾아도 없고, 추가 할 수도 없다면 메시지를 삭제(인증요청 무시)
				continue;
			}
		}

		// 인증 성공 시간 셋팅
		if( state == AS_RESPONSED )
		{
			time_t currTime;
			time( &currTime );
			userFound->setLatestAuthTime( currTime );
		}

		userFound->changeAuthState( state );
	}

	PAS_NOTICE("AuthAgent::svc stop");

	return 0;
}

/**
@brief AuthAgent에 Job을 할당한다.
*/
int AuthAgent::putWork(AUTH::RequestBody &reqBody, UserInfo *userinfo)
{
	ACE_ASSERT( userinfo != NULL );

	AuthInfoMessageBlock* pMB = new AuthInfoMessageBlock( &reqBody, userinfo );

	PAS_TRACE1( "AuthAgent:: putWork MB[%X]", pMB );

	// PAS AUTH 의 행업 현상에 의해 queue 가 가득차는 문제가 발생
	// 문제를 우회하기 위해 큐입력을 blocking 에서 non-blocking 으로 수정 - 2007.04.21
	ACE_Time_Value timeout = ACE_Time_Value::zero;
	if(this->putq((ACE_Message_Block*)pMB, &timeout) < 0)
	{
		PAS_ERROR1("AuthAgent::putWork >> Work 큐에 입력 실패 [userinfo:%X]", userinfo);
		delete pMB;
		return -1;
	}

	return 0;
}

bool AuthAgent::connecttoAuth(const char* ip, const int port)
{
	if( connCheck() == false )
		return false;

	// setting Connection timeout
	ACE_Time_Value conntime;
	conntime.set( Config::instance()->auth.connectionTimeout, 0 );

	ACE_INET_Addr addr(port, ip);
	ACE_SOCK_Connector connector;

	if(connector.connect(sock, addr, &conntime) < 0)
	{
		if(errno != EWOULDBLOCK)
		{
			incErrCount();

			PAS_TRACE("AuthAgent::connect >> Fail Connect to PasAuth");
			return false;
		}
	}

	return true;

/*	sock = socket( AF_INET, SOCK_STREAM, 0 );

	struct sockaddr_in saddr;

	memset( &saddr, 0x00, sizeof(sockaddr_in) );

	saddr.sin_family = AF_INET;
	saddr.sin_addr.s_addr = inet_addr( ip );
	saddr.sin_port = htons( port );

	// 타임아웃 시간 재설정
	linger lingtime;
	lingtime.l_onoff = 1;
	lingtime.l_linger = Config::instance()->auth.connectionTimeout;
	setsockopt(sock, SOL_SOCKET, SO_LINGER, (void*)&lingtime, sizeof(linger));

	if( connect(sock, (struct sockaddr*)&saddr, sizeof(sockaddr_in)) < 0 )
	{
		if( errno != EWOULDBLOCK )
			return false;
	}

	return true;
*/
}

void AuthAgent::closesocket()
{
	sock.close();
	isconnected = false;
}

AuthInfoMessageBlock *AuthAgent::getAuthInfo(ACE_Message_Block *mesg)
{
	AuthInfoMessageBlock *pAuthInfoMB = NULL;
	pAuthInfoMB = dynamic_cast<AuthInfoMessageBlock*>(mesg);

	return pAuthInfoMB;
}

int AuthAgent::sendtoAuth(AUTH::RequestBody *sendbuff, int size)
{
	int sendsize = 0;
	int cursend = 0;

	while( sendsize < size )
	{
		cursend = sock.send(&sendbuff[sendsize], size-sendsize, &ACE_Time_Value::zero);
//		cursend = send( sock, &sendbuff[sendsize], size-sendsize, 0 );

		if( cursend < 0 )
		{
			// 송신버퍼 만땅(발생하면 이상한거다)
			if( errno == ETIME )
			{
				sleep(1);
				continue;
			}

			// 잠시 블럭되어 보낼 수 없음(논블럭 소켓만 해당)
			else
			if( errno == EWOULDBLOCK )
			{
				sleep(1);
				continue;
			}

			// 기타...접속종료로 판단
			else
			{
				closesocket();
				return -1;
			}
		}

		else
		if( cursend == 0 )
			return 0;

		sendsize += cursend;
	}

	return sendsize;
}

int AuthAgent::recvfromAuth( AUTH::ResponseBody *recvbuff, int size )
{
	ACE_Time_Value recvtime;
	recvtime.set( 5, 0 );

	int recvsize = 0;

	while( recvsize < size )
	{
		int currecv = sock.recv(&recvbuff[recvsize], size-recvsize, &recvtime);
		if( currecv < 0 )
		{
			if( errno == EWOULDBLOCK )
			{
				PAS_DEBUG("AuthAgent::recv => EWOULDBLOCK");
				continue;
			}

			// 접속 종료
			else
			{
				closesocket();
				break;
			}
		}

		else
		if( currecv == 0 )
			return 0;

		recvsize += currecv;
	}

	return recvsize;
}

AuthState AuthAgent::getAuthState( AUTH::ResponseBody *respBody )
{
	AuthState state;

	switch(respBody->status)
	{
	case PAS_AUTH_FAIL_NOT_ADDR:
		PAS_DEBUG1("AuthAgent::getAuthState => PAS_AUTH_FAIL_NOT_ADDR [%X]", this);
		state = AS_FAILED;
		break;

	case PAS_AUTH_FAIL_AT_NOT_FOUND:
		PAS_DEBUG1("AuthAgent::getAuthState => PAS_AUTH_FAIL_AT_NOT_FOUND [%X]", this);
		state = AS_FAILED;
		break;

	case PAS_AUTH_FAIL_MSISDN:
		PAS_DEBUG1("AuthAgent::getAuthState => PAS_AUTH_FAIL_MSISDN [%X]", this);
		state = AS_FAILED;
		break;

	case PAS_AUTH_FAIL_IPADDR:
		PAS_DEBUG1("AuthAgent::getAuthState => PAS_AUTH_FAIL_IPADDR [%X]", this);
		state = AS_FAILED;
		break;

	case PAS_AUTH_FAIL_INVALID_IP_A_CLASS:
		PAS_DEBUG1("AuthAgent::getAuthState => PAS_AUTH_FAIL_INVALID_IP_A_CLASS [%X]", this);
		state = AS_FAILED;
		break;

	default:
		PAS_DEBUG2("AuthAgent::getAuthState => Success Auth Code [0x%X] [%X]", respBody->status, this);
		state = AS_RESPONSED;
		break;
	}

	return state;
}

/**
@brief
Auth 와 연결상태를 체크한다.
@return true, false
*/
bool AuthAgent::connCheck()
{
	Config *pConf = Config::instance();

	// 마지막 장애 시간과 현재 시간차가 10분 이상이면 재시도 한다.
	long currTime = 0;
	time( &currTime );

	long delaysec = currTime - errTime;

	// 장애 카운트가 10 이상이고, 장애 상황이 10분 이하면 인증생략
	if( errCount >= pConf->auth.errCount && delaysec <= pConf->auth.retryDelay )
	{
//		authlog->logprint( LVL_DEBUG, "장애 카운트[%d] errTime[%d] currTime[%d] delaysec[%d]\n",
//										errCount, errTime, currTime, delaysec );

		return false;
	}

//	authlog->logprint( LVL_DEBUG, "Auth 접속 시도  장애 카운트[%d] errTime[%d] currTime[%d] delaysec[%d]\n",
//										errCount, errTime, currTime, delaysec );

	return true;
}

void AuthAgent::incErrCount()
{
	Config *pConf = Config::instance();

	long currTime = 0;
	time( &currTime );

	long delaysec = currTime - errTime;

	if( errCount < pConf->auth.errCount )
	{
		errCount++;
//		authlog->logprint( LVL_DEBUG, "장애 카운트 증가[%d] errTime[%d] currTime[%d] delaysec[%d]\n",
//										errCount, errTime, currTime, delaysec );

		if( errCount == pConf->auth.errCount )
		{
			authlog->logprint( LVL_DEBUG, "Auth 장애발생 Cnt[%d] errTime[%d] currTime[%d] delaysec[%d]\n",
										errCount, errTime, currTime, delaysec );
		}
	}

	errTime = currTime;
}

void AuthAgent::decErrCount()
{
	if( errCount > 0 )
	{
		errCount--;

		if( errCount == 0 )
		{
			long currTime = 0;
			time( &currTime );

			long delaysec = currTime - errTime;

			authlog->logprint( LVL_DEBUG, "Auth 장애복구 Cnt[%d] errTime[%d] currTime[%d] delaysec[%d]\n",
										errCount, errTime, currTime, delaysec );
		}
	}

//	authlog->logprint( LVL_DEBUG, "장애 카운트감소 [%d]\n", errCount );
}

void AuthAgent::requestLog(const AUTH::RequestBody &requestBody, const char *pment)
{
	char terminator = '\0';

	if( pment == NULL )
		pment = &terminator;

	authlog->logprint( LVL_DEBUG, "REQ::type[%d] seq[%d] MIN[%s] IP[%d] Port[%d] conn[%d] newbrowser[%d] 3G[%d] MDN[%s] MsModel[%s] %s\n",
		requestBody.type, requestBody.seq, requestBody.min, requestBody.accessIP, requestBody.port, requestBody.startConn, requestBody.newBrowser, requestBody.g3GCode, requestBody.mdn, requestBody.msModel, pment );
}

void AuthAgent::responseLog(const AUTH::ResponseBody &responseBody, const char *pment)
{
	char terminator = '\0';

	if( pment == NULL )
		pment = &terminator;

	authlog->logprint( LVL_DEBUG, "REP::type[%d] status[%d] MIN[%s] %s\n",
		responseBody.type, responseBody.status, responseBody.ackMin, pment );
}
