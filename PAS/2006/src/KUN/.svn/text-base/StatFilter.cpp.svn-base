
#include <stdio.h>
#include <stdlib.h>

#include "StatFilter.h"
#include "StrUtil.h"
#include "StrSplit.h"
#include "Util.h"
#include "Util2.h"

#include "Config.h"
#include "PasLog.h"
#include "MyLog.h"

#include <iostream>
#include <fstream>


#ifdef KUN
#define CFG_FILE_NAME   "./k_stat.cfg"
#else
#define CFG_FILE_NAME   "./stat.cfg"
#endif

#define SWITCH_INFO_STRING              "^SWITCH"

#define URL_INFO_STRING                 "^START_URL"
#define DOMAIN_INFO_STRING              "^START_DOMAIN"
#define MESSAGE_INFO_STRING             "^START_MESSAGE"

#define URL_INFO_END_STRING             "^END_URL"
#define DOMAIN_INFO_END_STRING          "^END_DOMAIN"
#define MESSAGE_INFO_END_STRING         "^END_MESSAGE"

// 한글 처리시에 isspace 함수를 사용하게 되면 문제가 발생한다.
#ifndef ISSPACE
#define	ISSPACE(X)	((X)==' ' || (X)=='\t' || (X)=='\n' || (X)=='\r')
#endif

#ifndef ISDIGIT
#define	ISDIGIT(X) ((X) >= '0' && (X) <= '9')
#endif

#ifndef TRUE
#define	TRUE		1
#endif

#ifndef FALSE
#define	FALSE		1
#endif

StatFilter::StatFilter()
{
	tracelog = NULL;
	itemCount = 0;
	
	if (Config::instance()->process.StatFilterLog)
	{
		tracelog = new MyLog();
		tracelog->open((char*)"./", (char*)"statfilter");
	}
}


StatFilter::~StatFilter()
{
	if (tracelog) delete tracelog;
	clear();
}


void StatFilter::clear()
{
	itemCount = 0;
	urlList.clear();
	domainList.clear();
	mdnList.clear();
}



/**
주어진 URL 의 "http://" 부분을 제거. URL 뒷두분의 '/' 제거. 알파벳은 소문자로 변환.
@param org 원본 URL  (input)
@param normalized 변환된 URL (input)
@param isDomain  주어진 URL 이 Domain 인지 나타내는 플래그. Domain 이면 "D "를 URL 앞에 붙이기로 함.
@return 변환된 URL 의길이.
*/

int StatFilter::normalizeUrl(char *org, int iType, int portNum, char *normalized, int maxlen)
{
	int	urlleng = 0;
	char *pos=0, *dest=0;
	int	i=0;
	int	hasPort=0; // 원본 URL('org')이 포트 번호(':')를 포함하면 TURE
	while(1) {
		if (strncmp(org, "http://", 7)==0) org += 7;
		else break;
	}
	urlleng = strlen(org);
	for(pos = org+urlleng-1; *pos=='/'; pos--) {
		urlleng--;
	}

	dest = normalized;
	if (iType == FILTER_BY_DOMAIN) {
		strcpy(dest, "D ");
		dest += 2;
		maxlen -= 2;
	}

	if (urlleng > maxlen) urlleng = maxlen;

	for(pos=org,  i=0; i<urlleng; pos++, i++, dest++) {
		if (BETWEEN(*pos, 'A', 'Z'))
			*dest = *pos + 'a' - 'A';
		else
			*dest = *pos;
		if (*pos==':') hasPort = 1;
	}

	if (iType == FILTER_BY_DOMAIN && ! hasPort) {
	// 도메인에 포트번호가 없는 경우 디폴트 80 포트를 덧붙인다.
		if (portNum==0) portNum = 80;
		sprintf(dest, ":%d", portNum);
	}
	else {
		*dest = 0;
	}

	return urlleng;
}


/**
목록을 저장하는 메모리를 초기화 (free 등) 새로 k_stat.cfg 를 로드한다.

Stat.cfg 화일을 읽고 파싱하여 공지 처리하여할 대상이 되는 Domain, URL, MDN 목록에 저장하고,
공지 메시지로 목록에 저장한다.

@param fpath [k_]stat.cfg 화일의 path 이름.
*/
int StatFilter::load(char *fpath)
{
	char buf[1024];
	ifstream    in;
	int lineno = 0;
	itemCount = 0;
	
	in.open(fpath, ios::in);
	if (in.rdbuf()->is_open() == 0) {
		PAS_NOTICE1("Cannot read Stat file : %s", fpath);
		return (-1);
	}

	PAS_NOTICE1("Loading Stat file : %s", fpath);

	while (in.getline(buf, sizeof(buf)-1)) {	
		lineno++;
		itemCount += this->parseLine(buf);
	}  /* while : file read */

	in.close();

	PAS_NOTICE1("Stat items: %d", itemCount);
	return itemCount;
}

/**
(k_)stat.cfg 의 한 라인을 파싱하고 처리하는 중요 함수.
readStatCfg()  에 있던 것을 하나의 함수로 분리함. 2006/8/11
*/
int StatFilter::parseLine(char *oneline)
{
	StrSplit split(3, 255);
	char *ch = 0;
	char url[256];
	
	ch = StrUtil::skipSpace(oneline);
	if (*ch == '#' || *ch == 0) return 0;

	//printf("%s\n", oneline);
	split.split(ch);
	//split.print();
	
	if (split.numFlds() < 3) return 0;
	
	char *first = split.fldVal(0);

	if (first[0] == 'D') {	/*-- filtering by Domain */
		
		normalizeUrl(split.fldVal(2), FILTER_BY_DOMAIN, 0, url, sizeof(url)-1);
		string urlkey(url);		
		int msgid =  (int)strtol(split.fldVal(1), 0, 10);
		domainList[urlkey] = msgid;
			
		//printf("Domain: %s %d\n", url, msgid);

	}
	else    if (first[0] == 'U') { /*-- filtering by URL */

		normalizeUrl(split.fldVal(2), FILTER_BY_URL, 0, url, sizeof(url)-1);

		string domainkey(url);		
		int msgid =  (int)strtol(split.fldVal(1), 0, 10);
		urlList[domainkey] = msgid;
			
		//printf("URL: %s %d\n", url, msgid);
	}
	else    if (first[0] == 'M') { /*-- filtering by MDN */

		intMDN_t mdnKey = Util::phonenum2int(split.fldVal(2));
		int msgid =  (int)strtol(split.fldVal(1), 0, 10);
		mdnList[mdnKey] = msgid;
		
		//printf("MDN: %d %d\n", mdnKey, msgid);
	}
	
	else    if (ISDIGIT(first[0])) {
		/*-- 공지 메시지 목록 저장  */
		int idx = (int)strtol(first, 0, 10);
		char *mesg = StrUtil::skipDigit(ch);
		mesg = StrUtil::skipSpace(mesg);

		if (idx <= MAX_NOTIMESG)
			notiMesgs[idx] = string(mesg);

	}
	else {
		return 0;
	}
	
	return 1;
}



/**
@briesf 주어진 Domain/URL 이 statfilter 목록에 포함되는지 확인하고,
포함되는 경우 단말에 보낼 메시지를 버퍼에 저장하고, return 1.

@param keystr  DOMAIN/URL/MDN 값. 입력치.
@param notimesg 이 함수의 결과로 얻는 텍스트 메시지. 단말기에 전달되어야 할 공지 내용.
@param iport DOMAIN 의 port ?
@param iType keystr이 DOMAIN/URL/MDN 중 어느 타입인지 표시하는 것. URL-1, DOMAIN-2, MDN-3

@return 0 statfilter 목록에 포함되지 않는 경우.
*/
bool StatFilter::isBlocked(int iType , char *keystr, int iport,  char* notimesg)
{

	char strhashkey[256];

	notimesg[0] = 0;


	/**
	DOMAIN type 인 경우에는 keystr 값은  포트값이 포함되지 않는다. $HOST
	URL type 경우에는  keystr 값은  포트값이 포함된 URL 이다.  $EURL

	테스트 결과, 지오텔의 소스에서
	$HOST (즉, 도메인, type==2) 에는 포트값이 포함되지 않는다.  포트값이 80이든 아니든.
	$URL (즉, URL type==1) 에는 포트값이 포함된다 (당연).

	*/


	#ifdef DEBUG_NEWSTAT
	printf ("##STAT: org=%s type=%d port =%d : %s\n", keystr, iType, iport, hashkey);
	#endif

	int	notiIndex = -1;
	if (iType==FILTER_BY_MDN)
	{
		intMDN_t mdnKey = Util::phonenum2int(keystr);
		
		map<intMDN_t,  int>::const_iterator pos = mdnList.find(mdnKey);
		if (pos != mdnList.end())
		{
			notiIndex = pos->second;
		}
		if (notiIndex != -1) PAS_INFO3("StatFilter: MDN %s, %d, Res=%d", keystr, mdnKey, notiIndex);
		if (tracelog) tracelog->logprint(LVL_DEBUG, "MDN %s, %d, Res=%d\n", keystr, mdnKey, notiIndex);
		
	}
	else if (iType==FILTER_BY_URL)
	{
		//  Domain 이면 "호스트:포트" 형태의 키값을 만들어야 한다.
		// URL, DOMAIN 의 http:// 를 제거한다.
		normalizeUrl(keystr, iType, iport, strhashkey, sizeof(strhashkey)-1);
		string hashkey(strhashkey);
		
		map<string,  int>::const_iterator pos = urlList.find(hashkey);
		if (pos != urlList.end())
		{
			notiIndex = pos->second;
		}

		if (notiIndex != -1) PAS_INFO3("StatFilter: URL %s, %s, Res=%d", keystr, strhashkey, notiIndex);
		if (tracelog) tracelog->logprint(LVL_DEBUG, "URL %s, %s, Res=%d\n", keystr, strhashkey, notiIndex);
	}
	else if (iType==FILTER_BY_DOMAIN)
	{
		normalizeUrl(keystr, iType, iport, strhashkey, sizeof(strhashkey)-1);
		string hashkey(strhashkey);
		
		map<string,  int>::const_iterator pos = domainList.find(hashkey);
		if (pos != domainList.end())
		{
			notiIndex = pos->second;
		}
		if (notiIndex != -1) PAS_INFO3("StatFilter: DOMAIN %s, %s, Res=%d", keystr, strhashkey, notiIndex);
		if (tracelog) tracelog->logprint(LVL_DEBUG, "DOMAIN %s, %s, Res=%d\n", keystr, strhashkey, notiIndex);
	
	}
	

	if (notiIndex < 0)
		return false;
		
	if (BETWEEN(notiIndex, 0,  MAX_NOTIMESG))
	{
		strcpy(notimesg, notiMesgs[notiIndex].c_str());
	}

	if (notimesg[0] ) 
		return true;
	else 
		return false;
}


void StatFilter::prn()
{
	int i=0;
	map<string,  int>::const_iterator pos_d = domainList.begin();

	if (tracelog) tracelog->logprint(LVL_DEBUG, "====== DOMAIN [%d], URL [%d], MDN [%d] ======\n", 
		domainList.size(), urlList.size(), mdnList.size() );
		
	for (i=0; pos_d != domainList.end(); pos_d++, i++)
	{
	if (tracelog) tracelog->logprint(LVL_DEBUG, "[D-%02d]  %s  [%d]\n", i, pos_d->first.c_str(), pos_d->second);
	}

	map<string,  int>::const_iterator pos_u = urlList.begin();
	
	for (i=0; pos_u != urlList.end(); pos_u++, i++)
	{
	if (tracelog) tracelog->logprint(LVL_DEBUG, "[U-%02d]  %s  [%d]\n", i, pos_u->first.c_str(), pos_u->second);
	}

	map<intMDN_t,  int>::const_iterator pos_m = mdnList.begin();
	
	for (i=0; pos_m != mdnList.end(); pos_m++, i++)
	{
	if (tracelog) tracelog->logprint(LVL_DEBUG, "[M-%02d]  0%d  [%d]\n", i, pos_m->first, pos_m->second);
	}
	if (tracelog) tracelog->logprint(LVL_DEBUG, "====================================\n");

}

void StatFilter::test()
{
	test_normalizeUrl();
	test_parseLine();
}

void StatFilter::test_normalizeUrl()
{
	typedef struct _normalizeTestCase {
		/*-- 입력 데이타 */
		const char *orgUrl; /** 원본 Url  */
		/*-- 검사 테이타 : 확인 --*/
		const char *nomalizedUrl;  /** nomalizeUrl() 을 수행한 결과 */

	} normalizeTestCase;

	normalizeTestCase testcases[] = {
		{ "211.174.49.200", "211.174.49.200"},
		{ "http://211.174.49.200", "211.174.49.200"},
		{ "http://http://211.174.49.200", "211.174.49.200"},
		{ "http://211.174.49.200/", "211.174.49.200"},
		{ "http://211.174.49.200//", "211.174.49.200"},
		{ "http://211.174.49.200:7000/", "211.174.49.200:7000"},

		{ "hello.magicn.com", "hello.magicn.com"},
		{ "hELLO.안녕-().Com", "hello.안녕-().com"},
		{ "http://Hello.magicn-100.com", "hello.magicn-100.com"},
		{ "http://http://hello.Magicn.접속하신 서비스는 제공되지 않는 서비스", "hello.magicn.접속하신 서비스는 제공되지 않는 서비스"},
		{ "http://hello.magicn.com/", "hello.magicn.com"},
		{ "http://hello.magicn.com//", "hello.magicn.com"},
		{ "http://hello.magicn.com:7000/", "hello.magicn.com:7000"},
		{ NULL, NULL}
	};

	StatFilter sfilter;
	normalizeTestCase *testcase = 0;
	int casenumber=0;
	char	normalUrlBuf[256];

	/*-- 데이타 입력 : stat.cfg 의 내용을 입력 --*/
	for (testcase = testcases ,casenumber=0;
			casenumber < static_cast<int>(sizeof(testcases)/sizeof(normalizeTestCase));
			casenumber++, testcase++)
	{
		int cmpres = 0;
		if (testcase->orgUrl ==NULL ) break;
		sfilter.normalizeUrl((char*)testcase->orgUrl, FILTER_BY_URL,  0, normalUrlBuf, sizeof(normalUrlBuf)-1);
		cmpres = strcmp(normalUrlBuf, testcase->nomalizedUrl);
		if (cmpres==0)
			printf("normalizeUrl() SUCC: %s --> %s\n", testcase->orgUrl, normalUrlBuf);
		else
			printf("normalizeUrl() FAIL: %s --> %s\n", testcase->orgUrl, normalUrlBuf);
	}
}

void StatFilter::test_parseLine()
{
	StatFilter sfilter;

	sfilter.parseLine((char*) "1 서비스 점검중 입니다. 서비스 이용에 불편을 드려 대단히 죄송합니다.  \n");
	sfilter.parseLine((char*) "2 접속량이 많습니다. 서비스 이용에 불편을 드려 대단히 죄송합니다. ");
	sfilter.parseLine((char*) "3 접속하신 서비스는 제공되지 않는 서비스입니다.");
	sfilter.parseLine((char*) "4 접속하신 서비스는 이벤트가 종료 되었습니다.\n");

	typedef struct _statfilterTestCase {
		/*-- 입력 데이타 */
		int	keytype; /** URL-1, DOMAIN-2, MDN-3 */
		int	msgId; /** 공지 메시지 번호 */
		const char *keyval; /** 공지 적용 대상  Domain or Url or Mdn 값  */
		/*-- 검사 테이타 : 확인 --*/
		const char *url; /* GetSTATInfo 에 전달하는 검사 데이타 ( $HOST, $EURL, $MDN  의 값 ) */
		int	port;
		bool	blocked; /** 공지 처리 여부 : True -1, False = 0; */
	} statfilterTestCase;

	statfilterTestCase testcases[] = {
		{2, 1, "211.174.49.100", "211.174.49.100", 80, 1},
		{2, 1, NULL,          "http://211.174.49.100", 80,  1},   // 바로 위의 테스트에서 입력한 값에 대한 다른 테스트. keyval 가 NULL 이면 입력은 없음

		{2, 3, "http://211.174.49.200", "211.174.49.200", 80,  1},
		{2, 3, NULL,          "http://211.174.49.200/", 80,  1},
		{2, 3, NULL,          "http://http://211.174.49.200//", 80,  1},

		{2, 3, "http://211.174.49.210//", "211.174.49.210", 80,  1},


		{2, 2, "211.174.49.100:7000", "211.174.49.100", 7000,  1},
		{2, 2, NULL,                    "http://211.174.49.100", 7000,  1},
		{2, 2, NULL,                    "http://211.174.49.100", 8000,  0},


		{2, 1, "http://211.174.49.150:7000//", "211.174.49.150", 7000,1},
		{2, 1, NULL,                    "http://211.174.49.150", 7000, 1},
		{2, 1, NULL,                    "http://211.174.49.150/", 7000, 1},
		{2, 1, NULL,                    "http://211.174.49.150", 8000, 0},

		{2, 3, "http://sexy.magicn.com", "sexy.magicn.com", 80,  1},
		{2, 3, NULL,          "http://http://sexy.magicn.com//", 80,  1},
		{2, 3, "sexy.magicn.com:5000",          "http://http://sexy.magicn.com//", 5000,  1},

		{1, 3, "http://슐(무료체험):0/", "슐(무료체험):0", 80, 1},
		{1, 3, NULL, "슐(무료체험):0/", 80,  1},
		{1, 3, NULL, "http://슐(무료체험):0//", 80,  1},
		{1, 2, "http://[dni]연애극비유출★얼짱수영장SEXy희귀노컷사고", "http://[dni]연애극비유출★얼짱수영장SEXy희귀노컷사고/", 80,  1},
		{1, 2, NULL, "http://[dni]연애극비유출★얼짱수영장SEXy/", 80,  0},


		{3, 3, "0114300258", "0114300258", 0, 1},
		{3, 3, "0162003000", "0162003000", 0, 1},
		{3, 3, "01690103000", "01690103000", 0, 1},

		{0,0,NULL, NULL, 80,  0}
	};

	const char *keyTypeName[3] = { "URL", "DOMAIN", "MDN" };

	statfilterTestCase *testcase = 0;
	int casenumber=0;
	/*-- 데이타 입력 : stat.cfg 의 내용을 입력 --*/
	for (testcase = testcases ,casenumber=0;
			casenumber < static_cast<int>(sizeof(testcases)/sizeof(statfilterTestCase));
			casenumber++, testcase++)
	{
		if (testcase->keytype < 1 || testcase->keytype > 3 ) break;
		if (testcase->keyval != NULL) {
			char line[256];
			sprintf(line, "%s %d %s \n", keyTypeName[testcase->keytype-1], testcase->msgId, testcase->keyval);
			sfilter.parseLine(line);
		}
	}

	sfilter.prn();

	/*-- 검색 테스트 --*/
	const char *prevKeyval = 0;
	for (casenumber=0, testcase = testcases  ;
		casenumber < static_cast<int>(sizeof(testcases)/sizeof(statfilterTestCase));
		casenumber++, testcase++)
	{
		char msgBuf[256]; // 공지 메시지 버퍼 - 받아오는 것.
		bool	f_result = false;
		if (testcase->keytype < 1 || testcase->keytype > 3 ) break;
		if (testcase->keyval != NULL) prevKeyval = testcase->keyval;

		f_result = sfilter.isBlocked(testcase->keytype, (char*)testcase->url, testcase->port, msgBuf );

		if (f_result == testcase->blocked)
			printf("%s SUCC\t", keyTypeName[testcase->keytype-1]);
		else
			printf("%s FAIL\t", keyTypeName[testcase->keytype-1]);

		printf(" - stat.cfg=%s search=%s res=%d\n",	prevKeyval, testcase->url, f_result);
	}


}

#ifdef TEST_STATFILTER

int main()
{
	StatFilter sfilter;

	sfilter.test();

	//exit(0);
	sfilter.StartServer();
	sleep(1);
	while(1) {
		char keystr[128];
		char result[512];
		printf("Enter key: ");
		scanf("%s", keystr);
		if (strcmp(keystr, "quit")==0) break;
		sfilter.GetSTATInfo((char*)keystr, result, 0, 0);
		printf("%s\n", result);
	}

	StatFilter::StopServer();

}
#endif
