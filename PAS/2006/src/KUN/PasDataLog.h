#ifndef	PASDATALOG_H
#define	PASDATALOG_H

/**
@brief 로그 출력용 클래스

Over10, 통계, 통합통계, 접속, SSL, Transaction 시작/종료 로그
*/

#ifdef WIN32
#include <Time.h>
#include <Winsock2.h>
#else
#include <sys/time.h>
#include <sys/times.h>
#include <unistd.h>
#endif

#include <stdio.h>
#include <strings.h>

#include "Common.h"
#include "UserInfo.h"
#include "Session.h"
#include "TransactionInfo.h"
#include "Config.h"
#include "ActiveObjectChecker.h"
#include "MyFile.h"

class PasDataLog : public ActiveObjectChecker
{
public:
	static PasDataLog *instance();
	~PasDataLog();

	void	openFiles(bool reopenFlag=false);

	/** transaction 개시에 따른 로그 - pas.log */
	void startOfTransaction(UserInfo *user, Session *sess,Transaction *tr);

	/** transaction 종료에 따른 로그 - pas.log, passtat.log, pasidr.log, pasiss.log, over10.log */
	void endOfTransaction(UserInfo *user, Session *sess, Transaction *tr);

	static void	setPasAddr(const char* _pasaddr)
	{
		strncpy(pasIpAddr, _pasaddr, sizeof(pasIpAddr)-1);
		pasIpAddr[sizeof(pasIpAddr)-1] = 0;
	}

	void	writePasRequest(UserInfo *user, Session *sess, Transaction *tr);
	void	writePasResponse(UserInfo *user, Session *sess, Transaction *tr);
	
	void	writeSslPasRequest(UserInfo *user, Session *sess, Transaction *tr);
	void	writeSslConnectResult(UserInfo *user, Session *sess, Transaction *tr);
	void	writeSslPasResponse(UserInfo *user, Session *sess, Transaction *tr, bool resultRecved);
	void	writeSslDataReq(UserInfo *user, Session *sess, int currBytes);
	void	writeSslDataResp(UserInfo *user, Session *sess, int currBytes);
	
	
	void	writePasStat(UserInfo *user, Session *sess, Transaction *tr);
	void	writeSslPasStat(UserInfo *user, Session *sess, Transaction *tr);

	void	writeSslPasIdr(UserInfo *user, Session *sess, Transaction *tr);
	void	writePasIdr(UserInfo *user, Session *sess, Transaction *tr);
	void	writeOver10(UserInfo *user, Session *sess, Transaction *tr);
	void	writePasIss(UserInfo *user, Session *sess, Transaction *tr);

	void writeDuration(UserInfo* user, Session* sess, Transaction *tr);

	static double getDiffTime(const ACE_Time_Value& start, const ACE_Time_Value& end);
	

private:
	PasDataLog();
	static PasDataLog *oneInstance;

	/** 접속 로그 - ./k_pas.MMDD.log - 1 일 단위 */
	void	openPas();


	/** 통계 로그 - ./k_passtat.MMDD.log  - 1 일 단위 */
	void	openPasStat();
	

	/** 원시 과금 로그 - bill/k_n_pasidr.YYYYMMDDhhmm.log  - 10 분 단위 */
	void	openPasIdr();
	

	/** 오류(over10) 로그 - bill/k_over10.YYYYMMDDhh.log  - 1 시간 단위 */
	void	openOver10();
	

	/** 통합 통계 로그 - cmslog/k_pasiss.YYYYMMDDhhmm.log  - 5 분 단위 */
	void	openPasIss();

	void openDuration();

	/** 시스템 에러 번호를 넘겨주면 시스템 메시지를 PAS_DEBUG 로 출력한다. */
	void	printErr(char *fname,int err);

	MyFile myFilePas;
	MyFile myFilePasStat;
	MyFile myFilePasIdr;
	MyFile myFileOver10;
	MyFile myFilePasIss;
	MyFile myFileDuration;

	char	lineTime[32];

	time_t lastSec;
	int	lastMinute;
	int	lastHour;
	int	lastDay;

	static	char	pasIpAddr[32];
	struct tm	t;
};
#endif
