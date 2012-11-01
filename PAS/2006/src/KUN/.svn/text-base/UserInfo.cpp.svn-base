#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
 
#include "UserInfo.h"
#include "Util.h"
#include "PasLog.h"
#include "CookieDB.h"



UserInfo::UserInfo()
{
	sock = 0;
	authState = AS_NONE;
	santaState = SS_NONE;
	numSessions = 0;
	numRequests = 0;
	numResponses = 0;
	numConnTimes = 0;
	firstConnTime = lastConnTime = 0;
	intPhoneNumber = 0;
	intIpAddr = 0;

	lastCloseTime = 0;
	lastReqTime = 0;
	lastRespTime = 0;
	lastAuthSantaTime = 0;
	latestAuthTime = 0;
	
	ipAddr[0] = NULLCHAR;
	phoneNumber[0] = NULLCHAR;
	IMSI[0] = NULLCHAR;
	msModel[0] = NULLCHAR;
	browser[0] = NULLCHAR;
	cpName[0] = NULLCHAR;
	svcCode[0] = NULLCHAR;
	
}


void UserInfo::set(char *phonenum, char *ipaddr)
{
	STRNCPY(phoneNumber, phonenum, sizeof(phoneNumber)-1);

	STRNCPY(ipAddr, ipaddr, sizeof(ipAddr)-1);


	intPhoneNumber = Util::phonenum2int(phoneNumber);
	//PAS_TRACE2("UserInfo::set(): MDN %s --> %d", phoneNumber, intPhoneNumber);
	
	intIpAddr = Util::ipaddr2int(ipaddr);

	lastConnTime = time(NULL);
	lastReqTime = time(NULL);
}


void	UserInfo::set(intMDN_t phonenum, intIP_t ipaddr)
{
	intPhoneNumber = phonenum;
	intIpAddr = ipaddr;

	lastConnTime = time(NULL);
}

void	UserInfo::storeSession(Session * _sess)
{
	if (ISDIGIT(_sess->phoneNumber[0]) && phoneNumber[0]==0)
		STRCPY(phoneNumber, _sess->phoneNumber, LEN_PHONE_NUM);
		
	if (ISDIGIT(_sess->IMSI[0]) && IMSI[0]==0)
		STRCPY(IMSI, _sess->IMSI, LEN_IMSI);
		
	if (_sess->msModel[0] && strcmp(_sess->msModel, "Unknown") != 0) 
		STRCPY(msModel, _sess->msModel, LEN_MS_MODEL);
		
	if (_sess->browser[0] && strcmp(_sess->browser, "Unknown") != 0)
		STRCPY(browser, _sess->browser, LEN_BROWSER);
		
	if (_sess->cpName[0])
		STRCPY(cpName, _sess->cpName, LEN_CP_NAME);
		
	if (_sess->svcCode[0])
		STRCPY(svcCode, _sess->svcCode, LEN_SVC_CODE);
}

void UserInfo::copySession(Session * sess_)
{
	if (ISDIGIT(phoneNumber[0]))
		STRCPY(sess_->phoneNumber, phoneNumber, LEN_PHONE_NUM);
		
	if (ISDIGIT(IMSI[0]))
		STRCPY(sess_->IMSI, IMSI, LEN_IMSI);
		
	if (msModel[0])
		STRCPY(sess_->msModel, msModel, LEN_MS_MODEL);

	if (browser[0])
		STRCPY(sess_->browser, browser, LEN_BROWSER);
		
	if (cpName[0])
		STRCPY(sess_->cpName, cpName, LEN_CP_NAME);
		
	if (svcCode[0])
		STRCPY(sess_->svcCode, svcCode, LEN_SVC_CODE);
}

void UserInfo::onClientConnection()
{
	lastConnTime = time(NULL);
	lastReqTime = lastConnTime;
	if (firstConnTime==0)
		firstConnTime = lastConnTime;

	numConnTimes++;
	numSessions++;
	PAS_TRACE5("UserInfo::Connection - %s - FirstConn=%d, LastConn=%d, numConnTimes=%d numSessions=%d", 
		phoneNumber, firstConnTime, lastConnTime, numConnTimes, numSessions);
}

void UserInfo::onClientClose()
{
	numSessions--;
	lastCloseTime = time(NULL);
	PAS_TRACE5("UserInfo::Close - %s - FirstConn=%d, LastConn=%d, numConnTimes=%d numSessions=%d", 
		phoneNumber, firstConnTime, lastConnTime, numConnTimes, numSessions);
}

void UserInfo::updateConnTime()
{
	lastConnTime = time(NULL);
}

void UserInfo::updateReqTime()
{
	numRequests++;
	lastReqTime = time(NULL);
	PAS_TRACE3("UserInfo::updateReqTime(): %s - First=%d, last=%d", 
		phoneNumber, firstConnTime, lastReqTime);
}

void UserInfo::updateRespTime()
{
	numResponses++;
	lastRespTime = time(NULL);
}

int	UserInfo::getIdleTime(time_t now)
{
	return static_cast<int>(std::min((now - lastReqTime), (now - lastRespTime)));
}

bool UserInfo::isIdle(time_t now, int maxidlesec)
{
	if (numSessions != 0)
		return false;
		
	if (getIdleTime(now) >= maxidlesec)
	{
		char timestr[32];
		struct tm dateTM;
		localtime_r(&lastReqTime, &dateTM);
		strftime( timestr, 30, "%H:%M:%S", &dateTM);
		PAS_DEBUG3("PhoneNumber[%s] IdleSec[%d] LastReqTime[%s]", 
			phoneNumber, getIdleTime(now), timestr);
		return true;
	}
	else
	{
		return false;
	}
}

void UserInfo::changeIpAddr(char *ipaddr)
{
	intIpAddr = Util::ipaddr2int(ipaddr);
	changeIpAddr(intIpAddr);
}

void UserInfo::changeIpAddr(intIP_t ipaddr)
{
	intIpAddr = ipaddr;
	lastConnTime = time(NULL);
}


void UserInfo::changeAuthState(AuthState state)
{
	authState = state;
	lastAuthSantaTime = time(NULL);
}

AuthState UserInfo::getAuthState()
{
	return authState;
}

// 가장 최근에 인증받은 시간
time_t UserInfo::getLatestAuthTime()
{
	return latestAuthTime;
}

// 인증받은 시간 설정
void UserInfo::setLatestAuthTime(time_t &authTime)
{
	latestAuthTime = authTime;
}

void UserInfo::print()
{
	/*
	printf("STR: MDN=%s,  IP=%s\n", phoneNumber, ipAddr);
	printf("INT: MDN=0%d,  IP=%08X\n", intPhoneNumber, intIpAddr);
	printf("sock= %d, lasttime= %d\n", sock, lastConnTime);
	*/
}

cookie_t UserInfo::getCookie( const host_t& host, const HTTP::path_t& path )
{
	// remove query part in path
	HTTP::path_t tmpPath = path;
	int pos = tmpPath.find('?');
	if(pos >= 0)
		tmpPath.erase(pos);

	return _cookieDB.get(host, tmpPath);
}

void UserInfo::setCookie( const cookie_t& cookie )
{
	_cookieDB.set(cookie);
}



#ifdef TEST_MAIN


int main(int argc, char *argv[])
{
	UserInfo a;
	a.set("820114300258", "123.10.20.30");
	a.print();
}

#endif
