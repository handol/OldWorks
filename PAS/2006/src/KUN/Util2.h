#ifndef	UTIL2_H
#define	UTIL2_H

/**
@brief 각종 유틸리티

mwatch 및 빈번히 사용되는 간단한 함수들 클래스
맴버 함수는 모두 static 으로 되어 있으므로 인스턴스 생성 없이 사용 가능하다.

주요 기능은 아래와 같다.
mwatch 서버로 데이터 송신
File 및 디렉토리 의 각종정보 추출
현재 시간 추출
호스트명 추출
*/

#ifdef USE_MY_UDP
#include "mwatchcli.h"
#include "udpcli.h"
#endif

class Util2
{
public:
	static	int get_file_size(char *fname);
	static	int get_file_mtime(char *fname);
	static	int check_file_exist(char *fname);
	static	int check_this_directory(char *fname);
	static	char *get_mmdd(char *nowtime);
	static	char *get_yymmdd(char *nowtime=0);

	static	void get_nowtime_str_simple(char *nowtime);
	static	void get_curr_time_str(char *nowtime);
	static	void get_curr_time_str2(char *nowtime);
	static	void get_time_str(char *timeval, long t_val);

	static	int is_weekends();
	static	int count_days(int year, int month, int day);
	

	static	void setMwatchMsg(int svrport);
	static	void	sendMwatch();
	static	void	sendToPasmonitor(int flag_realcnt=1);

	static	bool getHostByName_threadsafe(const char* domainName, char* dotstrIP, size_t destlen);
	static	int	getHeaderSize(const char* src, size_t srcSize);
	static	time_t	sleepToSecChange();
	static	time_t	sleepToSync(int syncInterval);
	static	int getCpnameSvcCode(const char *srcstr, char *deststr, int destlen);
	static	const char * strcasestr (const char *haystack, const char *needle);
	
private:
	#ifdef USE_MY_UDP
	static	mwatchcli msgwatch;
	static	udpcli  	udpclient;
	#endif
	
	static	char mwatchMsg[256];		
	static	int	pasmonitor_msgcnt;
	static	int	pasmonitor_lastsec;
	static	char	myhostname[64];
	static	int	mysvrport ;

};
#endif

