// -*- C++ -*-
 
/**
@file UserInfo.h

@author DMS

@brief 사용자 정보 저장

유저정보는 UserInfoMng 에서 관리한다.
유저의 접속이 종료 되더라도 일정시간(config 값)동안 정보가 유지된다.

유저정보는 아래의 내용을 포함한다.

[상태 정보]
Auth 인증 상태
Santa 인증 상태
가장 처음 접속한 시간
가장 최근 접속한 시간
최근 종료한 시간
최근 Request 를 보낸 시간
최근 Response 를 받은 시간
최근 Auth, Santa 를 조회한 시간
접속한 횟수(새로운 socket, IP 로 접속한 경우)
현재 연결 개수
Request 받은 수
Response 받은 수

[단말기 정보]
Phone Number
IMSI
MsModel
Browser
CP Name
SVC Code
단말 IP
*/


#ifndef USERINFO_H
#define	USERINFO_H

#include "Common.h"
#include "Session.h"
#include "CookieDB.h"

//#include "basicDef.h"


//#include "ace/Thread_Mutex.h"

enum SantaState
{
	SS_NONE,			///< Santa 인증 요청 이전 초기 상태
	SS_REQUESTED,		///< Santa에게 인증 요청을 한 상태
	SS_RESPONSED,		///< Santa로 부터 인증 응답을 받은 상태
	SS_FAILED,			///< Santa 인증 실패
	SS_DIE				///< Santa 서버가 죽어서 접속할 수 없는 상태
};

enum SantaRecvState
{
	SS_WAIT_RESPONSE_HEADER,
	SS_WAIT_RESPONSE_BODY,
	SS_RECEIVED_RESPONSE
};

enum AuthState
{
	AS_NONE,			///< 인증 요청 이전 초기 상태
	AS_REQUESTED,		///< Auth에게 인증 요청을 한 상태
	AS_RESPONSED,		///< Auth로 부터 인증 응답을 받은 상태
	AS_FAILED			///< Auth 인증 실패
};

class UserInfo
{
public:

	UserInfo();

	void	set(char *phonenum, char *ipaddr);
	void	set(intMDN_t phonenum, intIP_t ipaddr);
	void	changeIpAddr(char *ipaddr);
	void	changeIpAddr(intIP_t ipaddr);

	void	onClientConnection();
	void	onClientClose();

	int	getIdleTime(time_t now);
	
	intIP_t getIntIpAddr() {
		return intIpAddr;
	}

	intMDN_t getIntPhoneNumber() {
		return intPhoneNumber;
	}

	char *getPhoneNumber() {
		return phoneNumber;
	}

	void print();

	void updateConnTime();
	void updateReqTime();
	void updateRespTime();
	
	void changeState();
	void changeAuthState(AuthState state);
	AuthState getAuthState();

	bool isIdle(time_t now, int maxidlesec);
	
	time_t getLastTime()
	{
		return lastReqTime;
	}

	void storeSession(Session * _sess); // input
	void copySession(Session *sess_); // output

	// 가장 최근에 인증받은 시간
	time_t getLatestAuthTime();

	// 인증받은 시간 설정
	void setLatestAuthTime(time_t &authTime);

	cookie_t getCookie(const host_t& host, const HTTP::path_t& path);
	void setCookie(const cookie_t& cookie);
	
public:

	/*-- User 정보에 해당하는 것들 --*/
	AuthState authState;
	int santaState;
	time_t firstConnTime; // 단말이 처음 PAS 접속한 시각
	time_t lastConnTime; // 단말이 최근 PAS 접속한 시각 (새로운 socket, IP 로 접속한 경우)
	time_t lastCloseTime; // 
	time_t lastReqTime; // 단말이 최근 request 보낸 시각
	time_t lastRespTime; // 단말이 최근 request 보낸 시각
	time_t lastAuthSantaTime; // 최근에 Auth, Santa 조회한 시간.
	int	numConnTimes; // 단말이 접속한 횟수 (새로운 socket, IP 로 접속한 경우)
	int	numSessions; // 현재 연결 개수.
	int	numRequests;
	int	numResponses;
	
	intMDN_t intPhoneNumber;
	char phoneNumber[LEN_PHONE_NUM+1];
	char IMSI[LEN_IMSI+1];
	char msModel[LEN_MS_MODEL+1];
	char browser[LEN_BROWSER+1];
	char cpName[LEN_CP_NAME+1];
	char svcCode[LEN_SVC_CODE+1];
	
	/*-- Session 정보에 해당하는 것들 --*/
	int sock;  /**< 단말 연결 socket */
	intIP_t intIpAddr; ///< 단말 IP 주소	
	char ipAddr[LEN_IPADDR+1];
	
	//ACE_Thread_Mutex mutex;
	//int userStates;

	time_t latestAuthTime;

private:
	CookieDB _cookieDB;
};

#endif
