#include "basicDef.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#if defined(WIN32) || defined(WINDOWS) || defined(_WIN32) || defined(__WIN32__)
	#ifndef WIN32
	#define WIN32
	#endif 

	#include <windows.h>
	#include <direct.h>
	#include <io.h>
	#include <fcntl.h>
	#include <time.h>
	#include <sys/types.h>
	#include <sys/stat.h>


	#define	strcasecmp	_stricmp
	#define	strncasecmp	_strnicmp

#else
	#include <time.h>
	#include <netdb.h>
	#include <unistd.h>
	#include <fcntl.h>
	#include <sys/types.h>
	#include <sys/stat.h>
	#include <sys/socket.h>
#endif

#include <ace/SOCK_Stream.h>
#include <ace/SOCK_Connector.h>
#include "Config.h"

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <strings.h>
#include <sys/param.h>
//#include <sysent.h>

#include "Util2.h"

using namespace std;

int Util2::check_file_exist(char *fname)
{
#ifndef WIN32
	struct stat buf;
	if (stat(fname, &buf) < 0) return 0;
	else return 1;
#else
	return 0;
#endif
}

// if this is NOT a directory, but a file, delete the file and make this directory
// if this does NOT exist, make this new directory
// return 0 if good
// return -1 if fail
int Util2::check_this_directory(char *fname)
{
	#ifndef WIN32
	struct stat buf;
	if (stat(fname, &buf) < 0 || ! S_ISDIR(buf.st_mode))
		return (mkdir(fname, 0755) );
	#else
		mkdir(fname);
	#endif	
	return 0;
}


/**

_fstat64 <sys/stat.h> and <sys/types.h> -- Windows
http://msdn.microsoft.com/library/default.asp?url=/library/en-us/vclib/html/_crt__chsize.asp
*/
int Util2::get_file_size(char *fname)
{
#ifndef WIN32
	struct stat buf;	
	if (stat(fname, &buf) < 0) return 0;
#else
	struct _stat buf;	
	if (_stat(fname, &buf) < 0) return 0;
#endif
	return buf.st_size;

}

/**
화일의 변경 시간 구하기
*/
int Util2::get_file_mtime(char *fname)
{
	#ifndef WIN32
	struct stat buf;
	if (stat(fname, &buf) < 0) return 0;
	#else
	struct _stat buf;
	if (_stat(fname, &buf) < 0) return 0;
	#endif
	return buf.st_mtime;

}

char * Util2::get_mmdd(char *nowtime)
{
	time_t		t_val;
	struct tm	t;
	
	time(&t_val);
	localtime_r(&t_val, &t);
	
	sprintf(nowtime, "%02d%02d", 
		t.tm_mon+1,	t.tm_mday);
	return nowtime;
}


char * Util2::get_yymmdd(char *nowtime)
{
	time_t		t_val;
	struct tm	t;
	
	time(&t_val);
	 localtime_r(&t_val, &t);
	
	sprintf(nowtime, "%04d%02d%02d", 
		t.tm_year+1900, t.tm_mon+1,
		t.tm_mday);
	return nowtime;
}

void Util2::get_nowtime_str_simple(char *nowtime)
{
	time_t		t_val;
	struct tm	t;
	
	time(&t_val);
	localtime_r(&t_val, &t);
	sprintf(nowtime, "%04d%02d%02d%02d%02d%02d", 
		t.tm_year+1900, t.tm_mon+1,
		t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec);
}

void Util2::get_curr_time_str(char *nowtime)
{
	time_t		t_val;
	struct tm	t;
	
	time(&t_val);
	localtime_r(&t_val, &t);
	sprintf(nowtime, "%04d/%02d/%02d %02d:%02d:%02d", 
		t.tm_year+1900, t.tm_mon+1,
		t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec);
}

void Util2::get_curr_time_str2(char *nowtime)
{
	time_t		t_val;
	time(&t_val);
	struct tm dateTM;
	localtime_r(&t_val, &dateTM);
	strftime( nowtime, 24, "%Y/%m/%d %H:%M:%S", &dateTM);
}

void Util2::get_time_str(char *timeval, long t_val)
{
	struct tm dateTM;
	localtime_r((time_t *)&t_val, &dateTM);
	strftime( timeval, 24, "%Y/%m/%d %H:%M:%S", &dateTM);
}

int	Util2::is_weekends()
{
	time_t		t_val;
	struct tm	t;
	time(&t_val);
	localtime_r(&t_val, &t);

	if (t.tm_wday == 0 || t.tm_wday == 6) /* Sunday or Saturday */
		return 1;
	else	return 0;
}

/* 
count the days from a given date to today
*/
int Util2::count_days(int year, int month, int day)
{
	time_t		old;
	struct tm	s;
	time_t		t_val;
	
	bzero(&s, sizeof(s));
	s.tm_year = year - 1900;
	s.tm_mon = month - 1;
	s.tm_mday = day;
	old = (long) mktime(&s);

	time(&t_val);
	return ( (int) ((long)t_val - (long)old) / (60*60*24) );
}

#ifdef USE_MY_UDP
mwatchcli Util2::msgwatch;
#endif
char 	Util2::mwatchMsg[256]={0};

// send UDP mesg to KUNLOG's UDP port 8090 (pasmonitor)
#ifdef USE_MY_UDP
udpcli  	Util2::udpclient;
#endif

int	Util2::pasmonitor_msgcnt = 0;
int	Util2::pasmonitor_lastsec = 0;
char	Util2::myhostname[64];
int	Util2::mysvrport = 0;

void	Util2::setMwatchMsg(int svrport)
{
	mysvrport = svrport;
	gethostname(myhostname, sizeof(myhostname)-1);
	sprintf(mwatchMsg, "pasgw_%d(%s)", svrport, myhostname);

#ifdef USE_MY_UDP
	udpclient.init();
#endif
}

#ifdef USE_MY_UDP
void	Util2::sendMwatch()
{
	int result = msgwatch.addMsgCnt(mwatchMsg);
	if (result==0)
		PAS_NOTICE("sending to msgwatchsvr failed");
	else if (result==1)
		PAS_DEBUG("sending to msgwatchsvr succ");
	else // result == -1
	{
		//	PAS_DEBUG("sending to msgwatchsvr delayed");	
	}	
}
 
/**
ClientHandler 에서 호출하면 (flag_realcnt == 1), msgcnt 만 증가 시키고,
PasAcceptor 에서 호출하면 (flag_realcnt == 0), pas monitor 로 전송한다.
이렇게 하면 socket send() 를 다른 thread 에서 동시에 수행하지 않게 된다.
*/
void	Util2::sendToPasmonitor(int flag_realcnt)
{
	if( flag_realcnt )
	{
		pasmonitor_msgcnt++;
		return;
	}

	time_t now = time(NULL);
	int	currsec =0;
	currsec = now / 20; // 20 초 마다
	if (currsec != pasmonitor_lastsec) 
	{
		pasmonitor_lastsec = currsec;
		udpclient.send(myhostname, mysvrport, pasmonitor_msgcnt);
		PAS_DEBUG3("sending to pasmonitor: %s %d %d", 
			myhostname, mysvrport, pasmonitor_msgcnt);
		pasmonitor_msgcnt = 0;
	}	
}
#endif

/**
기존 PAS 소스 에서 빌려 온 것  -- tcpcli.cpp

*/

#ifdef LINUX
bool Util2::getHostByName_threadsafe(const char* domainName, char* dotstrIP, size_t destlen)
{
	struct in_addr InAddr;
	char** ppAddrList;

	struct hostent hp_allocated;
	struct hostent * h_result;
	
	char buf[1001];
	size_t buf_len = (sizeof buf) - 1;
	int errnop;
	
	dotstrIP[0] = '\0';

	ACE_Time_Value before = ACE_OS::gettimeofday();
	int ret = gethostbyname_r(domainName, &hp_allocated, buf, buf_len, (hostent **) &h_result, &errnop);

	ACE_Time_Value after = ACE_OS::gettimeofday();

	ACE_Time_Value diff = after - before;

	// DNS lookup 에 1초 이상 소요된 경우 로그 출력.
	if (diff.sec() >= 1) 
	{
		PAS_NOTICE3("DNS DELAY :  %d.%d sec, %s", diff.sec(), diff.usec(), domainName);
	}
	
	if ( ret < 0 ) return false;
	if ( h_result->h_addrtype != AF_INET ) return false;
	for ( ppAddrList = h_result->h_addr_list; *ppAddrList != NULL; ppAddrList++ ) {
		/* if several IP addresses exist..
		 ** return the last IP */
		memcpy(&InAddr, *ppAddrList, sizeof(InAddr.s_addr));
		STRNCPY(dotstrIP, inet_ntoa(InAddr), destlen);
	}
	return true;
}
#else
bool Util2::getHostByName_threadsafe(const char* domainName, char* dotstrIP, size_t destlen)
{
	hostent* pHE;
	struct in_addr InAddr;
	char** ppAddrList;

	struct hostent hp_allocated;
	
	char buf[1001];
	size_t buf_len = (sizeof buf) - 1;
	int errnop;
	
	dotstrIP[0] = '\0';

	ACE_Time_Value before = ACE_OS::gettimeofday();

#if defined(linux)
	gethostbyname_r(domainName, &hp_allocated, buf, buf_len, &pHE, &errnop);
#else
	pHE = gethostbyname_r(domainName, &hp_allocated, buf, buf_len, &errnop);
#endif

	/*
	// test code
	static int count = 0;
	count++;
	if(count % 50 == 0)
		sleep(30);
	*/

	ACE_Time_Value after = ACE_OS::gettimeofday();

	ACE_Time_Value diff = after - before;

	// DNS lookup 에 1초 이상 소요된 경우 로그 출력.
	if (diff.sec() >= 1) 
	{
		PAS_NOTICE3("DNS DELAY : %d.%06d sec, %s", diff.sec(), diff.usec(), domainName);
	}
	
	if ( pHE == NULL )
	{
		return false;
	}

	if ( pHE->h_addrtype != AF_INET ) return false;

	ppAddrList = pHE->h_addr_list;

	
	if (ppAddrList !=NULL && *ppAddrList != NULL)
	{ 
	// 2007.1.29 - return the first result IP	
		memcpy(&InAddr, *ppAddrList, sizeof(InAddr.s_addr));
		STRNCPY(dotstrIP, inet_ntoa(InAddr), destlen);
		return true;
	}
	else
	{
	//
		return false;
	}

	#ifdef USE_LAST_IPADDR	
	for ( ppAddrList = pHE->h_addr_list; *ppAddrList != NULL; ppAddrList++ ) {
		memcpy(&InAddr, *ppAddrList, sizeof(InAddr.s_addr));
		STRNCPY(dotstrIP, inet_ntoa(InAddr), destlen);
	}
	return true;
	#endif
}
#endif

/**
HTTP header와 body의 구분 위치를 찾아, header의 길이를 return.
header와 body의 구분문자열: "\r\n\r\n", "\n\n", "\r\n\n" 등 모두 처리할 수 있도록 한다. 
header의 길에에는 구분 문자열 길이까지 포함한다.
@return -1 if not found
*/
int	Util2::getHeaderSize(const char* src, size_t srcSize)
{
	const char* srcPtr = src;
	size_t	n=0;
	while (n < srcSize) 
	{
		if (srcPtr[0]=='\n')
		{
			if (srcPtr[1]=='\n') 
			{
				return (int)n + 2;
			}
			else if (srcPtr[1]=='\r'  && srcPtr[2]=='\n') 
			{
				return (int)n + 3;
			}
			else 
			{
				// 다음 루프에서 srcPtr[0]!='\n' 임을 이미 알 수 있다. 
				// 한 루프 아낀다.
				srcPtr++;
				n++;
			}
		}
		srcPtr++;
		n++;
	}
	return 0;
}

/**
초값이 바뀌는 순간 return 하는 함수
*/
time_t	Util2::sleepToSecChange()
{
	time_t startSec = time(NULL);
	time_t now = startSec;
	 while (startSec == now)
	{
		// 초값이 바뀌는 순간 루프를 나간다.
		usleep(10000);
		now = time(NULL);
	}
	
	return now;
}

/**
현재 시간의 초값이 0초, 10초 등으로 sync 가 맞을 때 까지 sleep 한다.
현재 초값 SEC % syncInterval == 0 이 될 때 까지 잔다.
이 구현에 localtime() 함수를 사용할 필요가 없다. echo time() 을 modular 한 값도 localtime() 의 sec 를 modular 한 값과 동일하다.
*/
time_t	Util2::sleepToSync(int syncInterval)
{
	if (syncInterval == 0) 
		return 0;
		
	time_t start = time(NULL);
	int remainSec = syncInterval - (start % syncInterval);
	
	for(int cnt =0; cnt < remainSec -2; cnt++)
		sleep(1);

	time_t now = 0;
	while (1)
	{	
		now = sleepToSecChange();
		if (now % syncInterval == 0)
			break;
	}
	
	return now;
}


/**
2007.1.25
KTF cpname, svccode 처리
*/
int Util2::getCpnameSvcCode(const char *srcstr, char *deststr, int destlen)
{
	int	dest = 0;
	while (*srcstr && dest < destlen) {
		if (*srcstr == '&' || *srcstr == ';' || *srcstr == '(' || *srcstr == ')'  || *srcstr == '\n' || *srcstr == '\r')
			break;

		*deststr = *srcstr;

		srcstr++;
		
		dest++;
		deststr++;
	}
	
	*deststr = 0;
	return (dest);
}

const char * Util2::strcasestr (const char *haystack, const char *needle)
{
	const char *p, *startn = 0, *np = 0;

	for (p = haystack; *p; p++) {
		if (np) {
			if (toupper(*p) == toupper(*np)) {
				if (!*++np)
					return startn;
			} else
				np = 0;
		} else if (toupper(*p) == toupper(*needle)) {
			np = needle + 1;
			startn = p;
		}
	}

	return 0;
}


