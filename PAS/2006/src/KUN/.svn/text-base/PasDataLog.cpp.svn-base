#include "PasDataLog.h"
#include "UserInfoMng.h"

char	PasDataLog::pasIpAddr[32];

PasDataLog *PasDataLog::oneInstance =NULL;

PasDataLog *PasDataLog::instance()
{
	if (oneInstance == NULL)
	{
		oneInstance = new PasDataLog();
		oneInstance->openFiles();
	}
	return oneInstance;
}

PasDataLog::PasDataLog()
{
	// time
	lastSec = lastMinute = lastHour = lastDay = 0;
}

PasDataLog::~PasDataLog()
{
}
	
/**
lineTime == YYYYMMDDhhmm 형태의 시간값 문자열. 로그 파일의 이름 지정에 쓸 값.

fnameTime == YYYY/MM/DD hh:mm:ss 형태의 시간값 문자열. 로그 파일의 각 라인의 앞 부분에 쓸 값.
*/
void PasDataLog::openFiles(bool reopenFlag /*= false */)
{
	time_t	t_val;
	time(&t_val);

	if (t_val == lastSec) return;
	lastSec = t_val;
	localtime_r(&t_val, &t);
	snprintf(lineTime, sizeof(lineTime)-1,  "%04d/%02d/%02d %02d:%02d:%02d",
		t.tm_year+1900, t.tm_mon+1,
		t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec);

	// 1일 단위 파일 갱신
	if (reopenFlag || lastDay != t.tm_mday || myFilePas.empty() || myFilePasStat.empty())
	{
		openPas();
		openPasStat();
		lastDay = t.tm_mday;
	}

	// 1시간 단위 파일 갱신
	if (reopenFlag || lastHour != t.tm_hour || myFileOver10.empty() || myFileDuration.empty())
	{
		openOver10();
		openDuration();
		lastHour = t.tm_hour;
	}

	// 1분 단위 파일 갱신
	if (reopenFlag || lastMinute != t.tm_min || myFilePasIdr.empty() || myFilePasIss.empty())
	{
		openPasIdr();
		openPasIss();
		lastMinute = t.tm_min;
	}
}

/**

flush를 얼마나 자주 할것인가/

1. 매번 ?  -- 너무 자주
2. 1초 마다 ?
3. over10 은 실시간 flush 필요.
*/

void PasDataLog::startOfTransaction(UserInfo *user, Session *sess, Transaction *tr)
{
	#ifdef BENCH_TEST
	return;
	#endif
	
	openFiles();
	if (sess->isSSL())
		writeSslPasRequest(user, sess, tr);
	else
		writePasRequest(user, sess, tr);
}

void PasDataLog::endOfTransaction(UserInfo *user, Session *sess, Transaction *tr)
{
	#ifdef BENCH_TEST
	return;
	#endif
	
	openFiles();

	// 접속 로그 기록
	if (sess->isSSL())
		writeSslConnectResult(user, sess, tr);
	else
		writePasResponse(user, sess, tr);

	// 통계로그
	writePasStat(user, sess, tr);

	// 과금로그
	writePasIdr(user, sess, tr);

	// 오류로그
	writeOver10(user, sess, tr);

	// 통합통계로그
	writePasIss(user, sess, tr);

	// 시간로그
	writeDuration(user, sess, tr);
}

/** 접속 로그 - ./k_pas.MMDD.log - 1 일 단위 */
void PasDataLog::openPas()
{
	char fname[64];

	snprintf(fname, sizeof(fname)-1, "%spas.%02d%02d.log", Config::instance()->getLogPrefix().c_str(), t.tm_mon+1, t.tm_mday);
	
	// default open parameter is O_CREAT|O_APPEND|OWRONLY
	int ret = myFilePas.open( fname );
	if( ret == -1 )
		printErr( fname, errno );
}

/** 통계 로그 - ./k_passtat.MMDD.log  - 1 일 단위 */
void PasDataLog::openPasStat()
{
	char fname[64];

	snprintf(fname, sizeof(fname)-1, "%spasstat.%02d%02d.log", Config::instance()->getLogPrefix().c_str(),  t.tm_mon+1, t.tm_mday);

	int ret = myFilePasStat.open( fname );
	if( ret == -1 )
		printErr(fname,  errno );
}

void PasDataLog::printErr(char *fname, int err)
{
	char *sysErrMsg = NULL;
	sysErrMsg = strerror( err );

	PAS_WARNING2("file open error - %s: %s", fname, sysErrMsg);
}

/** 원시 과금 로그 - bill/k_n_pasidr.YYYYMMDDhhmm.log  - 10 분 단위 */
void PasDataLog::openPasIdr()
{
	char	fname[64];
	char	timeval[32];
	int	minute=0;

	minute = ((int)(t.tm_min / 10)) * 10;
	snprintf(timeval, sizeof(timeval)-1,  "%04d%02d%02d%02d%02d",
		t.tm_year+1900, t.tm_mon+1,
		t.tm_mday, t.tm_hour, minute);

	SmallString prefix = Config::instance()->getLogPrefix();
	
	// 과금로그는 ME의 경우 prefix 가 "a_" 이다.
	// ME 에서는 과금로그를 제외한 다른 로그의 prefix 가 없지만, 과금로그만 예외적인다.
	// (이런 예외는 정말 짱난다. >_<)
	if(Config::instance()->process.serverID.incaseFind("ME") >= 0)
		prefix = "a_";

	snprintf(fname, sizeof(fname)-1, "bill/%sn_pasidr.%s.log", prefix.c_str(), timeval);
	int ret = myFilePasIdr.open( fname );
	if( ret == -1 )
		printErr(fname,  errno );
}

/** 오류(over10) 로그 - bill/k_over10.YYYYMMDDhh.log  - 1 시간 단위 */
void PasDataLog::openOver10()
{
	char	fname[64];
	char	timeval[32];

	snprintf(timeval, sizeof(timeval)-1, "%04d%02d%02d%02d",
		t.tm_year+1900, t.tm_mon+1,
		t.tm_mday, t.tm_hour);
	snprintf(fname, sizeof(fname)-1,  "bill/%sover10.%s.log", Config::instance()->getLogPrefix().c_str(), timeval);
	int ret = myFileOver10.open( fname );
	if( ret == -1 )
		printErr(fname,  errno );
}

/** 통합 통계 로그 - cmslog/k_pasiss.YYYYMMDDhhmm.log  - 5 분 단위 */
void PasDataLog::openPasIss()
{
	char	fname[64];
	char	timeval[32];
	int	minute=0;

	minute = ((int)(t.tm_min / 5)) * 5;
	snprintf(timeval, sizeof(timeval)-1, "%04d%02d%02d%02d%02d",
		t.tm_year+1900, t.tm_mon+1,
		t.tm_mday, t.tm_hour, minute);
	snprintf(fname, sizeof(fname)-1,  "cmslog/%spasiss.%s.log", Config::instance()->getLogPrefix().c_str(), timeval);

	int ret = myFilePasIss.open( fname );
	if( ret == -1 )
		printErr(fname,  errno );
}

/**
SSL CONNECT 수신시 로그
*/
void	PasDataLog::writeSslPasRequest(UserInfo* /* user */, Session *sess, Transaction *tr)
{
	const HTTP::RequestHeader *reqHeader = tr->getRequest()->getHeader();
	const ProcessConfig& procConf = Config::instance()->process;

	myFilePas.print( "%s [SSL] %s --> %d %s %s %s, %d bytes, %s/%d ssock:%d, %s, (CKEY:%s)\n",
		lineTime,
		sess->phoneNumber,
		UserInfoMng::instance()->getCount(),
		reqHeader->getMethodStr().toStr(), reqHeader->getOrgUrl().toStr(), reqHeader->getVersion().toStr(),  // HTTP Req
		tr->phoneReqBytes,
		sess->ipAddr, sess->port, sess->sockfd, procConf.serverID.toStr(),
		tr->cKey
		);
}

/**
SSL CONNECT 응답 송신시 로그
*/
void	PasDataLog::writeSslConnectResult(UserInfo* /* user */, Session *sess, Transaction *tr)
{
	const HTTP::ResponseHeader *respHeader = tr->getResponse()->getHeader();
	const ProcessConfig& procConf = Config::instance()->process;
	
	myFilePas.print( "%s [SSL] %s <-- %d %s %d %s, %d bytes, %s/%d ssock:%d, %s, (CKEY:%s)\n",
		lineTime,
		sess->phoneNumber,
		UserInfoMng::instance()->getCount(),
		respHeader->getVersion().toStr(), respHeader->getStatusCode(), respHeader->getStatusString().toStr(),
		tr->getResponse()->getHeadLeng(),
		sess->ipAddr, sess->port, sess->sockfd, procConf.serverID.toStr(),
		(tr) ?  tr->cKey : "0"
		);
}

/** 접속 로그 - ./k_pas.MMDD.log - 1 일 단위 */
void	PasDataLog::writeSslPasResponse(UserInfo* /* user */, Session *sess, Transaction *tr, bool resultRecved /* = false */)
{
	int reqBytes = 0;
	char method[32];
	
	if (resultRecved) 
	{
		const HTTP::RequestHeader *reqHeader = tr->getRequest()->getHeader();
		
		reqBytes = tr->phoneReqBytes;
		STRNCPY(method, reqHeader->getMethodStr(), 16);
	}
	else
	{
		strcpy(method, "FINISH");
	}
	
	//int res =
	myFilePas.print( "%s [SSL] %s --> %d %s  (size:%d bytes, thr:%d, sock:%d) (cpname=%s;svccode=%s)\n",
		lineTime,
		sess->phoneNumber,
		UserInfoMng::instance()->getCount(),
		method,
		reqBytes,
		sess->threadId,
		sess->sockfd,
		sess->cpName,
		sess->svcCode);
}

/** 접속 로그 - ./k_pas.MMDD.log - 1 일 단위 */
void	PasDataLog::writeSslDataReq(UserInfo* /* user */, Session *sess, int currBytes)
{
	//int res =
	myFilePas.print( "%s [SSL] %s --> %s:%d  (size:%d bytes, total:%d bytes,  %s:%d, thr:%d, sock:%d) (cpname=%s;svccode=%s)\n",
		lineTime,
		sess->phoneNumber,
		sess->sslHost.toStr(),
		sess->sslPort,
		currBytes,
		sess->sslReqBytes,
		sess->sslHost.toStr(),
		sess->sslPort,
		sess->threadId,
		sess->sockfd,
		sess->cpName,
		sess->svcCode);
}

/** 접속 로그 - ./k_pas.MMDD.log - 1 일 단위 */
void	PasDataLog::writeSslDataResp(UserInfo* /* user */, Session *sess, int currBytes)
{
	//int res =
	myFilePas.print( "%s [SSL] %s <-- %s:%d  (size:%d bytes, total:%d bytes,  thr:%d, sock:%d) (cpname=%s;svccode=%s)\n",
		lineTime,
		sess->phoneNumber,
		sess->sslHost.toStr(),
		sess->sslPort,
		currBytes,
		sess->sslRespBytes,
		sess->threadId,
		sess->sockfd,
		sess->cpName,
		sess->svcCode);
}


/** 접속 로그 - ./k_pas.MMDD.log - 1 일 단위 */
void	PasDataLog::writePasRequest(UserInfo* /* user */, Session* /* sess */, Transaction *tr)
{
	const HTTP::RequestHeader *reqHeader = tr->getRequest()->getHeader();
	const ProcessConfig& procConf = Config::instance()->process;

	//int res =
	myFilePas.print( "%s %s --> %d %s %s %s, %d bytes, %s/%d ssock:%d, %s, (CKEY:%s)\n",
		lineTime,
		tr->phoneNumber,
		UserInfoMng::instance()->getCount(),
		reqHeader->getMethodStr().toStr(), reqHeader->getOrgUrl().toStr(), reqHeader->getVersion().toStr(),  // HTTP Req
		tr->phoneReqBytes,
		tr->phoneIpAddr, tr->phonePort, tr->sockfd, procConf.serverID.toStr(),
		tr->cKey
		);
}

/** 접속 로그 - ./k_pas.MMDD.log - 1 일 단위 */
void	PasDataLog::writePasResponse(UserInfo *user, Session *sess, Transaction *tr)
{
	const HTTP::ResponseHeader *respHeader = tr->getResponse()->getHeader();

	int reqCount = 0;
	if (user)
		reqCount = user->numRequests;
	else
		reqCount = sess->getSeqNum();

	// 2007-04-03 한 번에 라인 작성하기
	char	initPath[128];
	if( tr->counter == 1 )
	{
		// InitPath 는 맨 첫 request 에만 브라우저 req 헤더에 포함된다.
		snprintf(initPath, sizeof(initPath), "(KTF-INIT-PATH:%s) ", tr->ktfInitPath );
	}
	else
	{
		initPath[0] = 0;
	}
	
	//int res =
	myFilePas.print( "%s %s <-- %d %s %d %s %s (size:%d bytes, thr:%d, sock:%d) (cpname=%s;svccode=%s)(RequestCount:%d) (RequestMIN:%s) (RequestMNC:%s) (ChannelInfo:%s) (Counter:%s) %s (CKEY:%s)\n",
		lineTime,
		tr->phoneNumber,
		UserInfoMng::instance()->getCount(),
		respHeader->getVersion().toStr(), respHeader->getStatusCode(), respHeader->getStatusString().toStr(), 
		tr->userAgentLine,
		tr->phoneRespBytes,
		tr->threadId,
		tr->sockfd,
		tr->cpName,
		tr->svcCode,
		reqCount, 
		tr->MINnumber,
		tr->MNC,
		tr->chInfo,
		tr->counterStr,
		initPath,
		tr->cKey);

}

/** 통계 로그 - ./k_passtat.MMDD.log  - 1 일 단위 */
void	PasDataLog::writeSslPasStat(UserInfo* /* user */, Session *sess, Transaction *tr)
{
	// 호 처리 종료 시각 기준으로 하자 -- 2006.10.24 -- 양동기 차장님 협의 사항. - OLD PAS 와 다름.
	char timestr[32];
	struct tm dateTM;
	localtime_r(&sess->sslEndTm, &dateTM);
	strftime( timestr, 30, "%Y/%m/%d %H:%M:%S", &dateTM);
	
	// 2007-04-03 한 라인을 한 번에 쓰도록 변경.
	char	cpnameNsvccode[128];

	if( sess->cpName[0] != '\0' )
	{
		snprintf(cpnameNsvccode, sizeof(cpnameNsvccode),  "(cpname=%s;svccode=%s)", sess->cpName, sess->svcCode);
	}

	else
	{
		snprintf(cpnameNsvccode, sizeof(cpnameNsvccode),  "(N/A)");
	}
	
	myFilePasStat.print( "%s %-11s %-15s %0.4f %4d %5d %5d %s %s %s 200 %s:%d\n",
		timestr,
		sess->phoneNumber, 
		sess->IMSI,  
		tr->cpRespMicrosec,  // CP req ~ resp 소요 시간.
		//sess->sslEndTm - sess->sslStartTm,
		tr->phoneEndSec - sess->getLastTransactionTime(),  // 직전 transaction 과의 차이 
		sess->sslReqBytes, sess->sslRespBytes, 
		//sess->msModel, sess->browser
		tr->msModel, tr->browser,
		cpnameNsvccode,
		sess->sslHost.toStr(), sess->sslPort
		);
		
}


/** 통계 로그 - ./k_passtat.MMDD.log  - 1 일 단위 */
void	PasDataLog::writePasStat(UserInfo* /* user */, Session *sess, Transaction *tr)
{
/*
접속 일시	접속한 날짜/시간	2005-06-09 15:55
전화번호(MIN)	016xxxx, 018xxxx	162011138
IMSI번호	단말 식별을 위한 IMSI 번호	4.5E+14   -----> 3G 이면 "WCDMA"
처리 시간	CP Web 서버 접속 처리 시간 + PAS 내부 처리 시간	0.104
응답시간	단말기 실제 응답 시간	4
요청 Size	단말로부터 요청 받은 Size(parsed by pasgw)	407
전송 Size	응답한 페이지 사이즈	2400
단말기 정보	단말기모델명/단말기 브라우저명	SPHX5900 MSMB13
CPName/ServiceCode	CP Code / Service Code	(cpname=ktf;svccode=ktfimg000)
HTTP 상태	HTTP 상태 코드	200
요청 URL	요청한 URL	http://www.magicn.com/main.asp

Ex) 2005/06/09 15:55:11 01692087244 450001692087244 0.0533 5  407  2400 SPHX7700 MSMB13 (cpname=daum;svccode=daumportal000) 200 http://m.daum.net/
*/
	const HTTP::ResponseHeader *respHeader = tr->getResponse()->getHeader();

	char timestr[32];
	struct tm dateTM;
	localtime_r(&tr->phoneEndSec, &dateTM);
	// 호 처리 종료 시각 기준으로 하자 -- 2006.10.24 -- 양동기 차장님 협의 사항. - OLD PAS 와 다름.
	strftime( timestr, 30, "%Y/%m/%d %H:%M:%S", &dateTM); 

	char imsi[LEN_IMSI+1];
	if (strncmp(tr->IMSI, "3G", 2)==0)
		strcpy(imsi, "WCDMA");
	else
		STRCPY(imsi, tr->IMSI, LEN_IMSI);

	// 2007-04-03 한 라인을 한 번에 쓰도록 변경.
	char	cpnameNsvccode[128];
	
	if( tr->cpName[0]  &&  tr->svcCode[0])
	{
		snprintf(cpnameNsvccode, sizeof(cpnameNsvccode),  "(cpname=%s;svccode=%s)", tr->cpName, tr->svcCode);
	}
	else if( tr->cpName[0] )
	{
		snprintf(cpnameNsvccode, sizeof(cpnameNsvccode),  "(cpname=%s)", tr->cpName);
	}
	else if( tr->svcCode[0])
	{
		snprintf(cpnameNsvccode, sizeof(cpnameNsvccode),  "(svccode=%s)", tr->svcCode);
	}
	else
	{
		snprintf(cpnameNsvccode, sizeof(cpnameNsvccode),  "(N/A)");
	}

	myFilePasStat.print( "%s %-11s %-15s %0.4f %4d %5d %5d %s %s %s %d %s\n",
		timestr,
		tr->phoneNumber, 
		imsi,  
		tr->cpRespMicrosec,  // CP req ~ resp 소요 시간.
		tr->phoneEndSec - sess->getLastTransactionTime(),  // 직전 transaction 과의 차이 
		tr->phoneReqBytes, tr->phoneRespBytes, 
		tr->msModel, tr->browser,
		cpnameNsvccode,
		respHeader->getStatusCode(), tr->realUrl
		);

}



/** 원시 과금 로그 - bill/k_n_pasidr.YYYYMMDDhhmm.log  - 10 분 단위 */
void	PasDataLog::writeSslPasIdr(UserInfo* /* user */, Session *sess, Transaction *tr)
{
/*
1	Record_length	레코드 전체길이	Alpha Numeric	1	5
2	Service_type	무선구분 코드	Alpha Numeric	6	2	무선('A'), 유선('F'), 기본값은 'A'
3	Calling_MDN	사용자전화번호(MDN)	Alpha Numeric	8	12
4	Connect_start_time	사용자 접속요청 시간	Alpha Numeric	20	15
5	Page_duration	페이지간 이동시간	Alpha Numeric	35	10
6	Packet_count	사용바이트	Alpha Numeric	45	10
7		사용시간	Alpha Numeric	55	15
8	Call_phone_type	단말기모델	Alpha Numeric	70	16
9	Call_browser_version	단말기 브라우져 종류	Alpha Numeric	86	16
10	단말 IP	접속한 IP	Alpha Numeric	102	16
11	Response_code	HTTP 결과코드	Alpha Numeric	118	7
12	G/W IP	PAS G/W L4 IP	Alpha Numeric	125	16
13	Call_start_time	Callstarttime	Alpha Numeric	141	15
14	Cp_IP	CP 서버 IP	Alpha Numeric	156	16
15	Cp_port	CP 서버 Port	Alpha Numeric	172	7
16	Cp_name	CP Name	Alpha Numeric	179	21
17	service_code	Service Code	Alpha Numeric	200	21	Header에 Setting되는 Service Code
18	URL	접속 URL	Alpha Numeric	221	variable	사용 url
19	Line Feed		Alpha Numeric	variable	1	ASCII Code 10 ( 0x0a )

*/
	char trEndTime[32];
	char callstart[32];
	char response_time[32];
	time_t last_tr_time = sess->getLastTransactionTime();
	struct tm dateTM;

	// 호 처리 종료 시각 기준으로 하자 -- 2006.10.24 -- 양동기 차장님 협의 사항. - OLD PAS 와 다름.
	localtime_r(&tr->phoneEndSec, &dateTM);
	strftime( trEndTime, 30, "%Y%m%d%H%M%S", &dateTM);

	localtime_r(&tr->callstarttime, &dateTM);
	strftime( callstart, 30, "%Y%m%d%H%M%S", &dateTM); // CP connect 직후 시간

	localtime_r(&tr->cpEndSec, &dateTM);
	strftime( response_time, 30, "%Y%m%d%H%M%S", &dateTM);

	// 기존 소스에는 CP connect 직후의 시간값.
	
	//int res =
	myFilePasIdr.print( "%04d A %-11s %14s %09d %09d %-14s %-15s %-15s %-15s %-6d %-15s %-14s %-15s %-6d %-20s %-20s %s\n",
		125+9+68+strlen(tr->realUrl),
		tr->phoneNumber, 			// 3	Calling_MDN	사용자전화번호(MDN)	Alpha Numeric	8	12
		
		trEndTime,					 // 4	Connect_start_time	사용자 접속요청 시간	Alpha Numeric	20	15
		
		// 직전 transaction 과 현재 transaction 완료 시간과이 차이 
		(int)(tr->phoneEndSec -last_tr_time) ,	// 5	Page_duration	페이지간 이동시간	Alpha Numeric	35	10

		sess->sslReqBytes + sess->sslRespBytes, 		// 6	Packet_count	사용바이트	Alpha Numeric	45	10
		// CP 응답 시간 -- 기존 소스에는 CP 요청 시간과 같게 되어 있다.  (오류로 보임)
		callstart,					//7	사용  시간 Alpha Numeric	55	15
		
		tr->msModel,			//8	Call_phone_type	단말기모델	Alpha Numeric	70	16
		tr->browser,			//9	Call_browser_version	단말기 브라우져 종류	Alpha Numeric	86	16
		tr->phoneIpAddr,		//10	단말 IP	접속한 IP	Alpha Numeric	102	16
		tr->errorRespCode,		//11	Response_code	HTTP 결과코드	Alpha Numeric	118	7
		pasIpAddr,			//12	G/W IP	PAS G/W L4 IP	Alpha Numeric	125	16
		
		callstart,			//13	Call_start_time	Callstarttime	Alpha Numeric	141	15
		tr->cpIpAddr,	//14	Cp_IP	CP 서버 IP	Alpha Numeric	156	16
		tr->cpPort,	//15	Cp_port	CP 서버 Port	Alpha Numeric	172	7
		tr->cpName,	//16	Cp_name	CP Name	Alpha Numeric	179	21
		tr->svcCode,	//17	service_code	Service Code	Alpha Numeric	200	21	Header에 Setting되는 Service Code
		tr->realUrl	//18	URL	접속 URL	Alpha Numeric	221	variable	사용 url
	);	 
}


/** 원시 과금 로그 - bill/k_n_pasidr.YYYYMMDDhhmm.log  - 10 분 단위 */
void	PasDataLog::writePasIdr(UserInfo* /* user */, Session* sess, Transaction *tr)
{
/*
1	Record_length	레코드 전체길이	Alpha Numeric	1	5
2	Service_type	무선구분 코드	Alpha Numeric	6	2	무선('A'), 유선('F'), 기본값은 'A'
3	Calling_MDN	사용자전화번호(MDN)	Alpha Numeric	8	12
4	Connect_start_time	사용자 접속요청 시간	Alpha Numeric	20	15
5	Page_duration	페이지간 이동시간	Alpha Numeric	35	10
6	Packet_count	사용바이트	Alpha Numeric	45	10
7		사용시간	Alpha Numeric	55	15
8	Call_phone_type	단말기모델	Alpha Numeric	70	16
9	Call_browser_version	단말기 브라우져 종류	Alpha Numeric	86	16
10	단말 IP	접속한 IP	Alpha Numeric	102	16
11	Response_code	HTTP 결과코드	Alpha Numeric	118	7
12	G/W IP	PAS G/W L4 IP	Alpha Numeric	125	16
13	Call_start_time	Callstarttime	Alpha Numeric	141	15
14	Cp_IP	CP 서버 IP	Alpha Numeric	156	16
15	Cp_port	CP 서버 Port	Alpha Numeric	172	7
16	Cp_name	CP Name	Alpha Numeric	179	21
17	service_code	Service Code	Alpha Numeric	200	21	Header에 Setting되는 Service Code
18	URL	접속 URL	Alpha Numeric	221	variable	사용 url
19	Line Feed		Alpha Numeric	variable	1	ASCII Code 10 ( 0x0a )

*/
	char trEndTime[32];
	char callstart[32];
	char response_time[32];
	time_t last_tr_time = sess->getLastTransactionTime();
	struct tm dateTM;

	// 호 처리 종료 시각 기준으로 하자 -- 2006.10.24 -- 양동기 차장님 협의 사항. - OLD PAS 와 다름.
	localtime_r(&tr->phoneEndSec, &dateTM);
	strftime( trEndTime, 30, "%Y%m%d%H%M%S", &dateTM);

	localtime_r(&tr->callstarttime, &dateTM);
	strftime( callstart, 30, "%Y%m%d%H%M%S", &dateTM); // CP connect 직후 시간

	localtime_r(&tr->cpEndSec, &dateTM);
	strftime( response_time, 30, "%Y%m%d%H%M%S", &dateTM);

	// 기존 소스에는 CP connect 직후의 시간값.
	
	//int res =
	myFilePasIdr.print( "%04d A %-11s %14s %09d %09d %-14s %-15s %-15s %-15s %-6d %-15s %-14s %-15s %-6d %-20s %-20s %s\n",
		125+9+68+strlen(tr->realUrl),
//		220+strlen(tr->realUrl),	// 2006-11-28 레코드 길이 수정(하지만, 기존 oldpas도 레코드 길이를 잘못 계산하므로 그대로 둔다)
		tr->phoneNumber, 			// 3	Calling_MDN	사용자전화번호(MDN)	Alpha Numeric	8	12
		
		trEndTime,					 // 4	Connect_start_time	사용자 접속요청 시간	Alpha Numeric	20	15
		
		// 직전 transaction 과 현재 transaction 완료 시간과이 차이 
		(int)(tr->phoneEndSec -last_tr_time) ,	// 5	Page_duration	페이지간 이동시간	Alpha Numeric	35	10
		
		tr->phoneReqBytes + tr->phoneRespBytes, 		// 6	Packet_count	사용바이트	Alpha Numeric	45	10
		// CP 응답 시간 -- 기존 소스에는 CP 요청 시간과 같게 되어 있다.  (오류로 보임)
		callstart,					//7	사용  시간 Alpha Numeric	55	15
		
		tr->msModel,			//8	Call_phone_type	단말기모델	Alpha Numeric	70	16
		tr->browser,			//9	Call_browser_version	단말기 브라우져 종류	Alpha Numeric	86	16
		tr->phoneIpAddr,		//10	단말 IP	접속한 IP	Alpha Numeric	102	16
		tr->errorRespCode,		//11	Response_code	HTTP 결과코드	Alpha Numeric	118	7
		pasIpAddr,			//12	G/W IP	PAS G/W L4 IP	Alpha Numeric	125	16
		
		callstart,			//13	Call_start_time	Callstarttime	Alpha Numeric	141	15
		tr->cpIpAddr,	//14	Cp_IP	CP 서버 IP	Alpha Numeric	156	16
		tr->cpPort,	//15	Cp_port	CP 서버 Port	Alpha Numeric	172	7
		tr->cpName,	//16	Cp_name	CP Name	Alpha Numeric	179	21
		tr->svcCode,	//17	service_code	Service Code	Alpha Numeric	200	21	Header에 Setting되는 Service Code
		tr->realUrl	//18	URL	접속 URL	Alpha Numeric	221	variable	사용 url
	);	 
}

// 5 초
#define OVER10_CP_DELAY	(5)  

/** 오류(over10) 로그 - bill/k_over10.YYYYMMDDhh.log  - 1 시간 단위 */
void	PasDataLog::writeOver10(UserInfo* /* user */, Session* /* sess */, Transaction *tr)
{
	if (tr->cpRespSec  >= 5  || tr->errorRespCode  >= 400 ||  tr->errorRespCode == 299)
	{
		//int res =

		myFileOver10.print( "%s %-11s %-14s %.4f %d %s\n",
			lineTime,
			tr->phoneNumber, tr->msModel, 
			tr->cpRespMicrosec,
			tr->errorRespCode,
			tr->realUrl  // HotNumber, ACL 등을 거친 후
			//tr->orgUrl  // 원본 URL
			);
	}
}

/** 통합 통계 로그 - cmslog/k_pasiss.YYYYMMDDhhmm.log  - 5 분 단위 */
void	PasDataLog::writePasIss(UserInfo* /* user */, Session* /* sess */, Transaction *tr)
{
/*
Data	접속 날짜	ex) 2004/01/08
Time	접속 시간	ex) 16:53:17
MIN	접속 전화번호	ex) 01690109782	10~11자리 숫자
Process Time	처리시간	단위: 초  / ex) 2.15	가변길이 숫자
Process Byte	처리 데이터량	단위: Byte  / ex) 5120	가변길이 숫자
msModel 단말 모델 
cpname	CP 업체 코드	ex) k_dms	가변 문자열
svccode	Service 코드	ex) k_dmspers100	가변 문자열
HTTP Status	HTTP 상태 값	ex) 200	100 ~ 999 숫자값
HTTP channel info	망 정보 값	Ex) B	가변 문자열
URL	접속URL	ex) http://www.magicn.com/a...	10~11자리 숫자

2004/08/05^15:54:09^01033380059^0.036685^2182^PGK2500^polycube^polycubetm820^200^B^http://tmktf.polycube.co.kr/MYIidealsel.asp?chk1=3&retURL=/MYMmain.asp&ilook=53&ipurp=1&ichrt=1&istyle=1
*/
	char cpname[LEN_CP_NAME+1];
	char svccode[LEN_SVC_CODE+1];

	if (tr->cpName[0]=='\0')
		strcpy(cpname, "(N/A)");
	else
		STRCPY(cpname, tr->cpName, LEN_CP_NAME);

	if (tr->svcCode[0]=='\0')
		strcpy(svccode, "(N/A)");
	else
		STRCPY(svccode, tr->svcCode, LEN_SVC_CODE);

	if ( tr->chInfo[0] == '\0' || tr->chInfo[0] == ' ' )
	{
		strcpy(tr->chInfo, "N/A");
	}

	// 2007-04-03 : 통계로그의 lineTime 값에 영향을 주지 않기 위해 issLineTime 를 별도로 사용.
	char issLineTime[32];
	strcpy(issLineTime, lineTime);
	issLineTime[10] = '^';
	
	//int res =
	myFilePasIss.print( "%s^%s^%f^%d^%s^%s^%s^%d^%s^%s\n",
		issLineTime,
		tr->phoneNumber, 
		tr->cpRespMicrosec,
		tr->phoneReqBytes + tr->phoneRespBytes,
		tr->msModel, 
		cpname, 
		svccode,
		tr->errorRespCode, 
		tr->chInfo, 
		tr->realUrl);

}

void PasDataLog::openDuration()
{
	char fname[64];
	char timeval[32];

	snprintf(timeval, sizeof(timeval)-1, "%04d%02d%02d%02d",
		t.tm_year+1900, t.tm_mon+1,
		t.tm_mday, t.tm_hour);

	snprintf(fname, sizeof(fname)-1, "%sduration.%s.log", Config::instance()->getLogPrefix().c_str(), timeval);

	int ret = myFileDuration.open( fname );
	if( ret == -1 )
		printErr(fname,  errno );
}

void PasDataLog::writeDuration( UserInfo* /* user */, Session* /* sess */, Transaction *tr )
{
	// time MDN IMSI msmodel browser santa guide cp total TransactionStart COUNTER ResCode URL
	myFileDuration.print("%s %-11s %s %-10s %-10s %.4f %.4f %.4f %.4f %d.%06d %3s %d %s\n",
		lineTime,
		tr->phoneNumber, 
		tr->IMSI,
		tr->msModel, 
		tr->browser,
		getDiffTime(tr->santaStartTime, tr->santaEndTime),
		getDiffTime(tr->guideStartTime, tr->guideEndTime),
		getDiffTime(tr->cpStartTime, tr->cpEndTime),
		getDiffTime(tr->phoneStartTime, tr->phoneEndTime),
		tr->phoneStartTime.sec(), tr->phoneStartTime.usec(),
		tr->counterStr,
		tr->cpRespCode,
		tr->orgUrl);
}

double PasDataLog::getDiffTime( const ACE_Time_Value& start, const ACE_Time_Value& end )
{
	if(end < start)
	{
		PAS_INFO4("Time screw. StartTime[%d.%06d] EndTime[%d.%06d]", start.sec(), start.usec(), end.sec(), end.usec());
		return -1.0;
	}

	ACE_Time_Value diff = end - start;

	return (double)diff.sec() + (diff.usec() / 1000000.0);
}



