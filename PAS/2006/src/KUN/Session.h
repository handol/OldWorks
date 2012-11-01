// -*- C++ -*-

/**
@file Session.h

@author DMS

@brief 각 사용자별 세션 정보 관리

세션 정보는 아래와 같다.
요청 횟수
응답 횟수

단말 IP
단말 Port

단말 번호
IMSI
MsModel
Browser
CP Name
SVC Code

세션 시작 시간
마지막 Transaction 완료 시간

SSL 시작 전의 Sequence 번호
SSL 호스트
SSL Port
SSL 시작 시간
SSL 종료 시간
SSL Request Bytes
SSL Response Bytes
*/


#ifndef SESSION_H
#define	SESSION_H

#include "Common.h"

#include "TransactionInfo.h"
#include "Util.h"
#include "MDN.h"
#include "CookieDB.h"

class Session
{
public:
	Session()
	{
		clear();
	}

	void clear();

	void start(const char *phone_ip, int phone_port, int sock);
	void setThreadId();
	void finish();
	
	void print();
	
	int	getSeqNum()
	{
		return reqNums;
	}

	int getRespNum()
	{
		return respNums;
	}

	bool isDone()
	{
		return reqNums == respNums;
	}
	
	int	beginTransaction(Transaction *tr);
	int	endTransaction(Transaction *tr);

	void copyTransaction(Transaction * tr);
	
	int	incSeqNum();
	
	bool isFirst()
	{
		return (reqNums == 1);
	}

	bool isSSL()
	{
		return is_SSL;
	}
	
	void beginSSL();
	void endSSL();
	void clearSSL();

	

	void setNumber(char *phone_num);
	
	void setFirstInfo(Transaction *tr);

	char* getIpAddr()
	{
		return ipAddr;
	}
	
	char* getNumber()
	{
		return phoneNumber;
	}
	
	int	getSock()
	{
		return sockfd;
	}

	int	getPort()
	{
		return port;
	}
	
	void setSSLHost(const host_t& host)
	{
		sslHost = host;
	}

	host_t getSSLHost()
	{
		return sslHost;
	}

	void setSSLPort(int _port)
	{
		sslPort = _port;
	}

	int getSSLPort()
	{
		return sslPort;
	}

	void setLastTransactionTime(time_t given_time=0);
	time_t getLastTransactionTime()
	{
		return lastTrSec;
	}

	void onSslDataRecv(int recvBytes);
	void onSslDataSend(int sendBytes);

	void setCPname(Transaction *tr);

	bool needCookieHandler() const;

public:
	
	int reqNums; // 현재 세션에서의 단말 request 회수
	int respNums; // 현재 세션에서의 단말 response 회수

	bool isSantaNumber; // SANTA 조회 결과를 session 에 저장하였음을 표현.
	int threadId; // thread 번호
	int sockfd; // 소켓 번호
	char ipAddr[LEN_IPADDR+1]; // 단말기의 IP 주소 문자열(123.123.123.123)
	int	port; // 단말기 port
	unsigned int intIpAddr;	// 단말기의 IP 주소 unsigned int 형

	url_t lastRequestURL;

	time_t pasauthTime;	// PasAUTH 인증시간 (인증 요청 시각)
	time_t santaTime; // Santa 인증시간 (인증 결과 받은 시각)

	char phoneNumber[LEN_PHONE_NUM+1]; // MIN or MDN
	char IMSI[LEN_IMSI+1];
	char msModel[LEN_MS_MODEL+1];
	char browser[LEN_BROWSER+1];
	char cpName[LEN_CP_NAME+1];
	char svcCode[LEN_SVC_CODE+1];

	time_t	startSec; // session start time
	time_t	lastTrSec; // 마지막 transaction 완료 시간.
	int	lastHttpTransactionReqnum;  // SSL 직전의 마지막 HTTP request 의 seq num

	bool is_SSL; // 이 세션이 현재 SSL 상태인지를 표현.
	host_t sslHost; // SSL 대상 호스트 이름 또는 주소.
	int sslPort;  //  SSL 대상 포트
	time_t sslStartTm; //SSL 호 시작 시각 
	time_t sslEndTm; // SSL 호 종료 시각

	
	int	sslReqBytes; // SSL 호 동안 단말에서 수신한  바이트 누적치.
	int	sslRespBytes; // SSL 호 동안 단말로 송신한 바이트 누적치.
};


#endif
