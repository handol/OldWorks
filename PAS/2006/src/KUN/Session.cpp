#include <time.h>

#include "PasLog.h"
#include "CookieDB.h"
#include "Session.h"


void Session::clear()
{
	reqNums = 0;
	respNums = 0;
	
	isSantaNumber =  false; // SANTA 조회 결과를 session 에 저장하였음을 표현.
	is_SSL = false;
	sslPort = 0;
	ipAddr[0] = '\0';
	phoneNumber[0] = '\0';
	IMSI[0] = '\0';
	msModel[0] = '\0';
	browser[0] = '\0';
	cpName[0] = '\0';
	svcCode[0] = '\0';
	sslReqBytes = 0;
	sslRespBytes = 0;
	lastHttpTransactionReqnum = 0;
	sslStartTm = 0;
	sslEndTm = 0;
	threadId = 0;
	startSec = 0;
	lastTrSec = 0;

	pasauthTime = 0;
	santaTime = 0;
}

void Session::start(const char *phone_ip, int phone_port, int sock)
{
	
	STRNCPY(ipAddr, phone_ip, LEN_IPADDR);
	port = phone_port;
	sockfd = sock;
	startSec = time(NULL);
	PAS_DEBUG3("Session start. Client from %s:%d, sock[%d]", ipAddr, port, sockfd);
	
}

void Session::setThreadId()
{
	threadId = ACE_OS::thr_self();
}

void Session::finish()
{
	PAS_TRACE("Session::finish()");
	//print();
}

void Session::print()
{
	PAS_DEBUG4("Session::Client[%s:%d] Thread[%d] Sock[%d]", ipAddr, port, threadId, sockfd);
	PAS_DEBUG4("Session::ReqNums[%d] RespNums[%d] StartSec[%d] LastTrSec[%d]", reqNums, respNums, startSec, lastTrSec);
	PAS_DEBUG4("Session::PhoneNumber[%s] IMSI[%s] MSModel[%s] Browser[%s]", phoneNumber, IMSI, msModel, browser);
	PAS_DEBUG4("Session::CPName[%s] SvcCode[%s] LastTransNum[%d] isSSL[%d]", cpName, svcCode, lastHttpTransactionReqnum, is_SSL);
}

void Session::setNumber(char *phone_num)
{
	if (phone_num[0] != '\0')
		STRNCPY(phoneNumber, phone_num, LEN_PHONE_NUM);
}


/**
첫 Transaction 시에 각종 정보를 기억해둔다.
*/
void Session::setFirstInfo(Transaction *tr)
{
	//if (ISDIGIT(tr->phoneNumber[0]) && phoneNumber[0]=='\0')
	if (phoneNumber[0]=='\0')
		STRNCPY(phoneNumber, tr->phoneNumber, LEN_PHONE_NUM);
		
	//if (ISDIGIT(tr->IMSI[0]) && IMSI[0]=='\0')
	if (IMSI[0]=='\0')
		STRNCPY(IMSI, tr->IMSI, LEN_IMSI);
		
	if (strcmp(tr->msModel, "Unknown") != 0  && msModel[0]=='\0')	
		STRNCPY(msModel, tr->msModel, LEN_MS_MODEL);

	if (strcmp(tr->browser, "Unknown") != 0  && browser[0]=='\0')
		STRNCPY(browser, tr->browser, LEN_BROWSER);

	PAS_TRACE4("Session::setFirstInfo() - %s %s %s %s", phoneNumber, IMSI, msModel, browser);
}


/**
하나의 transaction 이 시작되었을 때 세션에서  해야할 일을 처리.
변경되어야 할 정보를 변경 - seqNum 등
한 transaction의 시작. 
*/
int	Session::beginTransaction(Transaction *tr)
{
	incSeqNum();
	if (threadId)
		tr->threadId = threadId;
	tr->sockfd = this->sockfd;
	tr->id( this->getSeqNum() );   // 세션내에서 이 transaction 이 할당받은 번호 설정.

	STRNCPY(tr->phoneIpAddr, this->ipAddr, LEN_IPADDR);
	tr->nIPAddr = Util::ipaddr2int(tr->phoneIpAddr);
	tr->phonePort =  this->port;
	return 0;
}

int	Session::endTransaction(Transaction * /* tr */)
{
	respNums++;
	return 0;
}

void Session::copyTransaction(Transaction * tr)
{
	if (ISDIGIT(phoneNumber[0]))
		STRNCPY(tr->phoneNumber, phoneNumber, LEN_PHONE_NUM);
		
	if (ISDIGIT(IMSI[0]))
		STRNCPY(tr->IMSI, IMSI, LEN_IMSI);
		
	if (msModel[0])
		STRNCPY(tr->msModel, msModel, LEN_MS_MODEL);
		
	if (browser[0])
		STRNCPY(tr->browser, browser, LEN_BROWSER);
		
	if (cpName[0])
		STRNCPY(tr->cpName, cpName, LEN_CP_NAME);
		
	if (svcCode[0])
		STRNCPY(tr->svcCode, svcCode, LEN_SVC_CODE);
}

void Session::setLastTransactionTime(time_t  given_time /* == 0 */)
{
	if (given_time != 0)
		lastTrSec = given_time ;
	else
		lastTrSec = time(NULL);
}


void Session::setCPname(Transaction *tr)
{
	if (tr->cpName[0])
		STRNCPY(cpName, tr->cpName, LEN_CP_NAME);

	if (tr->svcCode[0])
		STRNCPY(svcCode, tr->svcCode, LEN_SVC_CODE);
	
	lastHttpTransactionReqnum = tr->id();
	PAS_TRACE3("Session::setCPname() - %s %s , lastTr=%d", cpName, svcCode, lastHttpTransactionReqnum);
}

int	Session::incSeqNum()
{
	this->reqNums++; 
	//@todo 너무 커지면 모듈라. 송수신이 완료된 경우 다시 초기화
	if (this->reqNums > 100*100*100 && this->isDone() )
		this->reqNums =  1;
	return reqNums;
}

void Session::beginSSL()
{
	is_SSL = true;
	sslReqBytes = 0;
	sslRespBytes = 0;
	sslStartTm= time(NULL);
}

void Session::endSSL()
{
	is_SSL = false;
	sslEndTm= time(NULL);
}

void Session::clearSSL()
{
	sslReqBytes = 0;
	sslRespBytes = 0;
}


/*
단말에서 받은 SSL 데이타 사이즈
*/
void Session::onSslDataRecv(int recvBytes)
{
	sslReqBytes += recvBytes;
}

/*
단말로 보낸 SSL 데이타 사이즈
*/
void Session::onSslDataSend(int sendBytes)
{
	sslRespBytes += sendBytes;
}

bool Session::needCookieHandler() const
{
	if(strcasecmp(msModel, "SPHX1000") == 0)
		return true;

	if(strcasecmp(msModel, "SPHX1100") == 0)
		return true;

	if(strcasecmp(msModel, "SPHX1200") == 0)
		return true;

	if(strcasecmp(msModel, "SPHX1300") == 0)
		return true;

	if(strcasecmp(msModel, "SPHX2000") == 0)
		return true;

	if(strcasecmp(msModel, "SPHX2500") == 0)
		return true;

	if(strcasecmp(msModel, "SPHX2700") == 0)
		return true;

	if(strcasecmp(msModel, "CX-300KB") == 0)
		return true;

	return false;
}

