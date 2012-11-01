#include <time.h>
#include <strings.h>

#include <ace/Time_Value.h>

#include "Util.h"
#include "Util2.h"
#include "StrUtil.h"
#include "DNSManager.h"
#include "PasLog.h"
#include "MyLog.h"
#include "Config.h"

#include "TransactionInfo.h"

Transaction::Transaction()
{
	clear();

	seqNum = 0;
}

Transaction::Transaction( const Transaction &old ) : ActiveObjectChecker()
{
	*this = old;
}



Transaction::~Transaction()
{

}

void Transaction::clear()
{
	is3G = false;
	connCloseRequested = false;
	hotNumberConverted =  false;
	sockfd = 0;
	jobDone = false;
	_isSSL = false;
	aclApplied = false;
	hotNumberApplied = false;
	_streaming = false;

	errorRespCode = 0;
	threadId = 0;
	phoneNumber[0] = '\0';
	phonePort = 0;
	MNC[0] = '\0';
	MDN[0] = '\0';
	MINnumber[0] = '\0';
	IMSI[0] = '\0';
	chInfo[0] = '\0';
	baseId[0] = '\0';
	msModel[0] = '\0';
	browser[0] = '\0';
	orgUrl[0] = '\0';
	realUrl[0] = '\0';

	cpName[0] = '\0';
	svcCode[0] = '\0';

	hashKey[0] = '\0';
	billInfo[0] = '\0';

	phoneIpAddr[0] = '\0';
	cpIpAddr[0] = '\0';
	cpPort = 0;
	
	counter = 0;
	ktfInitPath[0] = '\0';
	cpHostName[0] = '\0';
	userAgentLine[0] = '\0';
	cKey[0] = '\0';
	headerNameForPhoneNum[0] = '\0';
	
	trStartSec = 0; // sec from epoch. time() 으로 구하는 값.
	nIPAddr = 0;

	clearSizeAndTime();
}									

void Transaction::clearSizeAndTime()
{
	phoneReqBytes = 0;
	phoneRespBytes = 0;
	phoneRespCode = 0;
	phoneStartSec = 0;
	phoneEndSec = 0;
	phoneRespSec = 0;

	callstarttime = 0;
	
	cpReqBytes = 0;
	cpRespBytes = 0;
	cpRespCode = 0;
	cpRespMicrosec = 0;
	cpStartSec = 0;
	cpEndSec = 0;
	cpRespSec = 0;

	phoneStartTime.set(0,0);
	phoneEndTime.set(0,0);
	cpStartTime.set(0,0);
	cpEndTime.set(0,0);
	cpRespTime.set(0,0);

	sslReqBytes = 0;
	sslRespBytes = 0;
	sslStartTm= 0;
	sslEndTm = 0;
}


void Transaction::beginSSL()
{
	_isSSL = true;
	sslReqBytes = 0;
	sslRespBytes = 0;
	sslStartTm= time(NULL);
}

void Transaction::endSSL()
{
	_isSSL = false;
	sslEndTm= time(NULL);
}

/*
단말에서 받은 SSL 데이타 사이즈
*/
void Transaction::onSslDataRecv(int recvBytes)
{
	sslReqBytes += recvBytes;
}

/*
단말로 보낸 SSL 데이타 사이즈
*/
void Transaction::onSslDataSend(int sendBytes)
{
	sslRespBytes += sendBytes;
}

/** 
set id : Transaction id (seqNum)을 설정하면서  Request/Response 메시지의 seqNum 도 같이 설정한다.
*/
void	Transaction::id(int _id)
{
	seqNum = _id;
	request.setSeqNum(seqNum);
	response.setSeqNum(seqNum);
}

int Transaction::id()
{
	return seqNum;
}


/**
단말에서 요청이 온 직후에 계산.
*/
void Transaction::recvPhoneReq()
{
	phoneReqBytes = request.getHeadLeng() + request.getBodyLeng(); 
	phoneStartSec = time(NULL);
	phoneStartTime = ACE_OS::gettimeofday();


	PAS_TRACE2("Transaction::recvPhoneReq(): phoneReqBytes=%d, phoneStartSec=%d", phoneReqBytes, phoneStartSec);
}

/**
단말로 응답 보내기 직전에 계산..
*/
void Transaction::sendPhoneResp()
{
	phoneRespBytes = response.getHeadLeng() + response.getBodyLeng();
	phoneEndSec = time(NULL);
	phoneRespSec = phoneEndSec - phoneStartSec;
	
	phoneEndTime = ACE_OS::gettimeofday();
}

/**
CP 연결 직후 시간 저장.
*/
void Transaction::connectCp()
{
	callstarttime = time(NULL);
}

/**
CP 로  요청 보내기 직전에 계산..
CP 연결 실패 등으로 보내지 못한 경우에는 zero 상태로 남아 있다.
*/
void Transaction::sendCpReq()
{
	cpReqBytes = request.getHeadLeng() + request.getBodyLeng();
	cpStartSec = time(NULL);
	cpStartTime = ACE_OS::gettimeofday();

	if (callstarttime == 0)
		callstarttime = cpStartSec;
}

/**
CP 에서 응답이 온 직후에 계산.
*/
void Transaction::recvCpResp()
{
	cpRespBytes = response.getHeadLeng() + response.getBodyLeng();
	cpEndSec = time(NULL);
	cpRespSec = cpEndSec - cpStartSec;
	cpEndTime = ACE_OS::gettimeofday();

	cpRespTime = cpEndTime - cpStartTime;
	cpRespMicrosec += (double)cpRespTime.sec()   + (double)cpRespTime.usec() / 1000000 ;
	

	if ( !_isSSL )
	{
		const HTTP::ResponseHeader* h = response.getHeader();
		if (h==0)		return;
		
		cpRespCode = h->getStatusCode();

		if (errorRespCode == 0)
			errorRespCode = RESCODE_OK;

		// CP 응답 없이 예외 처리된 경우 (인증 실패, santa 실패,  연결 실패 등) 가 아니면서 
		// CP 로부터 200 외의  응답 코드 받은 경우 이 값으로 로그 출력해야 한다.
		if (errorRespCode == RESCODE_OK && h->getStatusCode() != RESCODE_OK)
			errorRespCode = h->getStatusCode();

		HTTP::HeaderElement::value_t cpdata = h->getElement( "CPdata" );
		if( cpdata.size() > 0 )
		{
			parseCpName_New(cpdata.toStr()); //2007.1.25
			//parseCpName(cpdata, ";", this->cpName, this->svcCode);
		}
			

	}
	else
	{
		cpRespCode = RESCODE_OK;
		errorRespCode = RESCODE_OK;
	}

}

/**
실제로 CP 와 연동 하지 않은 경우에도 값을 설정해 놓기 위해.
*/
void Transaction::setCpTime()
{
	if (callstarttime==0)
		callstarttime = time(NULL);
	if (cpStartSec==0)
		sendCpReq();
	if (cpEndSec==0)
		recvCpResp();	
}

void Transaction::setSantaResult(const char *santaMDN, const char *santaIMSI)
{
	// setPhoneNumber() 가 santa 조회전에 호출되어 현재  phoneNumber 에는 MIN 값이 있는 상태이다.	
	STRNCPY(MINnumber, phoneNumber, LEN_PHONE_NUM);

	// phoneNumber, MDN, IMSI 는 santa 조회 결과값으로 저장한다.
	STRNCPY( IMSI, santaIMSI, LEN_IMSI );
	STRNCPY( phoneNumber, santaMDN, LEN_PHONE_NUM );
	STRNCPY( MDN, santaMDN, LEN_PHONE_NUM );

	// set correlation key
	correlationKey.set(MDN, ACE_OS::gettimeofday());
	
	PAS_DEBUG3("Using SANTA result: MDN=%s MIN=%s IMSI=%s", phoneNumber, MINnumber, IMSI);
	PHTR_DEBUG3("Using SANTA result: MDN=%s MIN=%s IMSI=%s", phoneNumber, MINnumber, IMSI);

	HTTP::key_t keyName(headerNameForPhoneNum); // 2006.12.8 handol
	HTTP::value_t newMDN = phoneNumber;


	// SANTA 인증 번호에 국가번호(82)가 없으면 추가한다.
	if( newMDN[0] != '8' || newMDN[1] != '2' )
	{
		HTTP::value_t ccode = "82";
		newMDN = ccode + newMDN;
	}

	HTTP::RequestHeader* newHeader = request.getHeader();
	if (newHeader)
		newHeader->replaceElement( keyName, newMDN );
		
}


/**
return 0 if phone number is good
return -1 if not good
good_phoneNumber == 82 빼고 , 0 붙인 전화 번호가 저장된다.
전화번호 기준.
길이 : 10 또는 11
사업자번호 : 010, 011, 016, 017, 018, 019
*/
int	Transaction::validatePhoneNumber(const char *http_phoneNumber, char *good_phoneNumber)
{
	/* 숫자 아닌 문자 제거 
	기존 PAS 소스에도 전화번호 뒤에 이상한 문자가 있는 경우 처리함.  -- OLD PAS -- HttpRequest::setMin()
	*/
	char cleanHttpPhoneNumber[LEN_PHONE_MIN+1];
	StrUtil::copyNumber(cleanHttpPhoneNumber, http_phoneNumber, LEN_PHONE_MIN);

	// 82 빼고 , 0 붙인 값
	char *removePrefix = Util::normalizeMDN(cleanHttpPhoneNumber);
	snprintf(good_phoneNumber, LEN_PHONE_MIN+1, "0%s", removePrefix);

	int	leng_phoneNumber = strlen(good_phoneNumber);

	if (leng_phoneNumber != 10  && leng_phoneNumber  != 11)
	{
		if ( !_isSSL )
		{
			// 번호 길이가 짧은  경우 anonymous 처리 -- 기존 PAS
			return -1;
		}
	}
	
	/* 이상한 번호 로그 추가 -- 2007.03.12 
	016, 018, 010, 011 처럼 '0' 다음에 '1' 있어야 한다.
	*/

	const char *findKoreanCarrier = strchr("016789", good_phoneNumber[2]); // 한국 이통사업자 번호 찾기.
	if (good_phoneNumber[0] != '0'  ||
		good_phoneNumber[1] != '1' ||
		findKoreanCarrier == NULL)
	{
			// 번호가 이상한 경우 anonymous 처리 -- 기존 PAS
			return -1;
	}
	
		

	/* 정상인 경우 */
	return 0;
}


void Transaction::setPhoneNumber(const char *good_phoneNumber)
{

	STRCPY(phoneNumber, good_phoneNumber, LEN_PHONE_NUM);
	// MDN, phoneNumber 동일 
	STRCPY(MDN, good_phoneNumber, LEN_MDN);

	correlationKey.set(MDN, ACE_OS::gettimeofday());
	
	// MIN 값 세팅  -- 무조건  10자리 
	/// MIN ( 총 10자리 전화번호, 11자리 전화번호는 앞에 0을 뺀다 )
	/// 1012345678 = 010-1234-5678
	if (strlen(MDN) > 10)
		STRCPY(MINnumber, MDN+1, LEN_MDN)
	else
		STRCPY(MINnumber, MDN, LEN_MDN)
		

	HTTP::key_t keyName(headerNameForPhoneNum); // 2008.12.8 handol
	HTTP::value_t newMDN = phoneNumber;

	// SANTA 인증 번호에 국가번호(82)가 없으면 추가한다.
	if( newMDN[0] != '8' || newMDN[1] != '2' )
	{
		HTTP::value_t ccode = "82";
		newMDN = ccode + newMDN;
	}


	HTTP::RequestHeader* newHeader = request.getHeader();
	if (newHeader)
		newHeader->replaceElement( keyName, newMDN );
	
}

void Transaction::setAnonymous()
{
	strcpy(phoneNumber, "Anonymous");
	strcpy(IMSI, "Anonymous");
	strcpy(MINnumber, "Anonymous");
}
		
/**
Phone 으로부터 받은 Request 메시지에서 각종 정보를 추출하여 멤버 변수에 저장.
*/
void Transaction::parsePhoneNumberInfo()
{
	//char tmp_phoneNumber[LEN_PHONE_NUM+1];
	HTTP::RequestHeader* h = request.getHeader();
	if (h==0)		return;

	
	FastString<256> value;

	// MDN, MNC
	value = h->getElement( "HTTP_MDN_INFO");
	if(value.size() > 0) {
	// HTTP_MDN_INFO 는 활용하지 않는다.  (
	}

	value = h->getElement( "HTTP_MNC_INFO" );
	if( value.size()==0 )
		value = h->getElement( "MNC" );
		
	if( value.size() > 0 )
		STRNCPY( this->MNC, value, LEN_MNC )
	else
		strcpy( this->MNC, "00" );

	// PHONE_NUMBER ( real MDN)
	// 전화번호에 해당하는 헤더의 이름을 기억하여야 한다. 2006.12.8
	value = h->getElement( "HTTP_PHONE_NUMBER" );
	if (value.size() > 0) {
		strcpy(headerNameForPhoneNum, "HTTP_PHONE_NUMBER");
	}
	else  {
		value = h->getElement( "HTTP_PHONE-NUMBER" );
		if (value.size() > 0)
			strcpy(headerNameForPhoneNum, "HTTP_PHONE-NUMBER");
		
		if (value.size()==0)
		value = h->wildSearchElement_getkey( "PHONE_NUMBER", headerNameForPhoneNum, HEADERNAME_LEN );
		
		if (value.size()==0)
		value = h->wildSearchElement_getkey( "PHONE_NUMBER", headerNameForPhoneNum, HEADERNAME_LEN );
	}


	char good_phoneNumber[LEN_PHONE_MIN+1];
	int validationRes = validatePhoneNumber(value.toStr(), good_phoneNumber);


	if (validationRes==0) // 정상인 경우
	{
		setPhoneNumber(good_phoneNumber);

		/*
		3G 단말 판단 조건 추가
		2007.5.7
		*/
		if (strncmp(this->MNC, "08", 2)==0)
		{
			this->is3G = true;
			PAS_TRACE2("MDN=%s 3G=%d By MNC", MDN, is3G);
		}

		FastString<256> phonesysparam;
		phonesysparam = h->getElement( "HTTP_PHONE_SYSTEM_PARAMETER");
		if ( phonesysparam.size() > 0  && phonesysparam.incaseFind("WCDMA") >= 0)
		{
			this->is3G = true;
			PAS_TRACE2("MDN=%s 3G=%d By PHONE_SYSTEM_PARAMETER", MDN, is3G);
		}

		/// IMSI = MCC + MNC + MIN
		/// 450080182304250 = 450 + 08 + 0182304250
		if (this->is3G)
			strcpy(this->IMSI, "3G");
		else
			snprintf( this->IMSI, sizeof(IMSI),  "%s%s%s", "450", this->MNC, this->MINnumber );
	}
	else // 비정상 번호
	{
		logStrangeNumber(value.toStr());
		setAnonymous();
	}

	PAS_TRACE4("ORG=%s MDN=%s MNC=%s IMSI=%s", value.toStr(), MDN, MNC, IMSI);
	PHTR_DEBUG4("ORG=%s MDN=%s MNC=%s IMSI=%s", value.toStr(), MDN, MNC, IMSI);
	
}


/**
Phone 으로부터 받은 Request 메시지에서 각종 정보를 추출하여 멤버 변수에 저장.
*/
void Transaction::setTransactionInfo()
{
	HTTP::RequestHeader* h = request.getHeader();

	if (h==0)		return;

	//!!  MDN, MIN, IMSI 세팅.
	parsePhoneNumberInfo();
	
	FastString<256> foundElement;
	foundElement = h->getElement( "Connection" );
	
	if (foundElement.incaseEqual("Close"))
	{
		//!!! opera browser 업로드시에  단말 요청 헤더에 Connection: close 가 포함된 경우.
		this->connCloseRequested = true;
		PAS_INFO1("Connection:Close Found [%s]", phoneNumber);
	}

	/// Channel Info
	HTTP::HeaderElement::value_t channelInfo = h->getElement( "HTTP_CHANNEL_INFO" );
	channelInfo.trim();
	if (channelInfo.size() < 4)	
	{
		STRCPY( this->chInfo, channelInfo, LEN_CH_INFO );
	}
	else if (channelInfo.size() > 0)
	{
		int nPos = channelInfo.split( &foundElement, ":" );
		if (nPos >= 0) {
			channelInfo.split( &foundElement, ":", nPos+1 );
			STRCPY( this->chInfo, foundElement, LEN_CH_INFO );
		}
		else
		{
			STRCPY( this->chInfo, channelInfo, LEN_CH_INFO );
		}
	}

	if (this->chInfo[0] == '\0')
		strcpy(chInfo, "N/A");

	/// User Agent
	HTTP::HeaderElement::value_t userAgent = h->getElement( "USER-AGENT" );

	if (userAgent.size()==0)
		userAgent =  h->wildSearchElement( "USER-AGENT" );
		
	userAgent.trim();

	if (userAgent.size() > 0) 
	{
		int nPos = userAgent.split( &foundElement, "(" );			// Mozilla/1.22
		HTTP::HeaderElement::value_t extractAgent = userAgent.substr( nPos );

		STRNCPY(this->userAgentLine, extractAgent, LEN_USER_AGENT);
		
		nPos = userAgent.split( &foundElement, ";", nPos+1 );	// (compatible
		foundElement.trim();
		if (strncasecmp(foundElement, "compatible", 5) != 0)
		{
			nPos = userAgent.split( &foundElement, ";", nPos+1 );	// (compatible
			foundElement.trim();
		}
		
		nPos = userAgent.split( &foundElement, ";", nPos+1 );	// KUN/1.2.3
		foundElement.trim();
		STRNCPY( this->browser, foundElement, LEN_BROWSER );
		
		nPos = userAgent.split( &foundElement, ";", nPos+1 );	// KTF-X6000
		foundElement.trim();
		STRNCPY( this->msModel, foundElement, LEN_MS_MODEL );
		
	}

	if (this->browser[0] == '\0')
		strcpy(browser, "Unknown");
		
	if (this->msModel[0] == '\0')
		strcpy(msModel, "Unknown");
	
	// URL
	STRNCPY(this->orgUrl, h->getOrgUrl().toStr(), MAX_URL_LEN);
	STRNCPY(this->realUrl, h->getUrl().toStr(), MAX_URL_LEN);
	if (this->realUrl[0] == '\0') // 2006.12.16
		STRNCPY(this->realUrl, this->orgUrl, MAX_URL_LEN)	
	
	PAS_TRACE2("OrgURL[%s] RealURL[%s]", this->orgUrl, this->realUrl);

	// 요청  URL 에 CPname 이 있는 경우
	parseCpName_New(this->realUrl); //2007.1.25

	/*
	char *cpname_pos = strstr(this->realUrl, "cpname=");

	if (cpname_pos)
	{
		PAS_DEBUG1("CPNAME: %s", cpname_pos);
		HTTP::HeaderElement::value_t cpdata ((const char*)(cpname_pos + 1));
		parseCpName(cpdata, "&", this->cpName, this->svcCode);
	}
	*/

	// SSL RESUL 메시지인 경우. 2006-12-13
	if (h->getMethod() == HTTP::RM_RESULT)
	{
		HTTP::HeaderElement::value_t cpdata = h->getElement( "CPdata" );
		if( cpdata.size() > 0 )
		{
			parseCpName_New(cpdata.toStr()); //2007.1.25
			//parseCpName(cpdata, ";", this->cpName, this->svcCode);
		}
	}
			
	// INIT-PATH : 브라우저의 맨 첫 request 에만 헤더에 포함된다.
	foundElement = h->getElement( "KTF-INIT-PATH");
	if (foundElement.size() > 0)
		STRNCPY(ktfInitPath, foundElement.toStr(), LEN_KTF_INIT_PATH)  // STRNCPY() 뒤에 세미 콜론 쓰면 안된다.
	else
		strcpy(ktfInitPath, "N/A");
	

	// Request Count
	foundElement = h->getElement( "COUNTER");
	if (foundElement.size() > 0)
		STRNCPY(counterStr, foundElement, LEN_COUNTER)
	else
		strcpy(counterStr, "N/A");
		
	this->counter = strtol(foundElement.toStr(), 0, 10);
	
	// CKEY
	// 두번째 값으로 기존 PAS에는  thread ID 를 사용하였으나, 여기에서는 다른 것을 사용해야 한다.
	// rand 를 이용하자.  Unique 값을 만들기 위해.  
	int randomNumber = rand() & 0xFFFF;
	if( this->counter == 0 )
	{
		snprintf(cKey, sizeof(cKey), "KEYN%04X%04X%04XCNT%05X", sockfd, randomNumber, phoneReqBytes, 0xFFFFF);
	}

	else
	{
		snprintf(cKey, sizeof(cKey), "KEYN%04X%04X%04XCNT%05X", sockfd, randomNumber, phoneReqBytes, this->counter);
	}
}

/**
CP connectin info.
hotnumber 검사 후에 호출해야 한다. ( hotnumber 적용이든 아니든 상관없다.)
*/
void Transaction::setCpConnInfo_first()
{
	const HTTP::RequestHeader* h = request.getHeader();
	//char NameOrIp[LEN_HOST_NAME];

	// host	
	STRNCPY(cpHostName, h->getHost(), LEN_HOST_NAME); // host name (domain)

	// ip
	if (Util::isIpAddr(cpHostName)) 
	{
		STRNCPY(this->cpIpAddr, cpHostName, LEN_IPADDR); // IP addr
	}

	// port
	this->cpPort = h->getPort();

	PAS_TRACE3("setCpConnInfo_first(): Host[%s] IP[%s] Port[%d]", cpHostName, cpIpAddr, cpPort);
	PHTR_DEBUG3("setCpConnInfo_first(): Host[%s] IP[%s] Port[%d]", cpHostName, cpIpAddr, cpPort);
}

/**
ACL 조회 결과,  결과값을 DNS  처럼 이용하는 경우 처리.
*/
void Transaction::setCpConnInfo_apply_ACL(const char *NameOrIp, int port)
{
	if (NameOrIp != NULL && port != 0) {
	// ACL 에서 DNS lookup 한 경우
		if (Util::isIpAddr(NameOrIp)) 
		{
			STRNCPY(this->cpIpAddr, NameOrIp, LEN_IPADDR); // IP addr
		}
		else
		{
			STRNCPY(this->cpHostName, NameOrIp, LEN_HOST_NAME); // IP addr
			this->cpIpAddr[0] = '\0';
		}
		
		this->cpPort = port;

		PAS_DEBUG3("Apply ACL Host[%s] IP[%s] Port[%d]", cpHostName, cpIpAddr, cpPort);
		PHTR_DEBUG3("Apply ACL Host[%s] IP[%s] Port[%d]", cpHostName, cpIpAddr, cpPort);
	}
}

/**
CP connectin info.
ACL 검사 후에 호출해야 한다. ( ACL DNS 적용이든 아니든 상관없다.)
CP 의 호스트 명 , IP address 정보를 세팅.
*/
void Transaction::setCpConnInfo_second()
{
	if ( ! Util::isIpAddr(this->cpIpAddr))
	{
		PAS_TRACE2("Before DNS:%s, %s", this->cpHostName, this->cpIpAddr);
		PHTR_DEBUG2("Before DNS:%s, %s", this->cpHostName, this->cpIpAddr);

		//bool result = Util2::getHostByName_threadsafe(this->cpHostName, this->cpIpAddr, LEN_IPADDR);
		bool result = DNS::Manager::instance()->getHostByName(this->cpHostName, this->cpIpAddr, LEN_IPADDR);
		
		if (result == false) // DNS 실패한 경우, 그냥 호스트 이름으로.
		{
			/*
			strcpy(this->cpIpAddr, "127.0.0.1");
			this->cpPort = 8000;
			*/

			// 2007.1.27
			strcpy(this->cpIpAddr, "0.0.0.0");
			
			PAS_NOTICE3("DNS failed for %s. Setting CP as %s:%d", this->cpHostName, this->cpIpAddr, this->cpPort);		
			PHTR_NOTICE3("DNS failed for %s. Setting CP as %s:%d", this->cpHostName, this->cpIpAddr, this->cpPort);		
		}

		else 
		{
			PAS_TRACE2("After DNS:%s, %s", this->cpHostName, this->cpIpAddr);		
			PHTR_DEBUG2("After DNS:%s, %s", this->cpHostName, this->cpIpAddr);
		}
	}

	PAS_TRACE3("setCpConnInfo_second(): Host=%s, IP=%s, Port=%d", cpHostName, cpIpAddr, cpPort);
	PHTR_DEBUG3("setCpConnInfo_second(): Host=%s, IP=%s, Port=%d", cpHostName, cpIpAddr, cpPort);
}

/**
URL 의 경우에는 delimiter == &
Http header 의 경우에는  delimiter == ;
http://www.magicn.com:80/R2/images/weather/wea004007_240.sis?cpname=kweather&svccod
e=kweatherweather800&bintype=sis
*/
void Transaction::parseCpName(HTTP::HeaderElement::value_t &cpdata, const char *delimiter, char *_cpName, char *_svcCode)
{
	// CPname 을 구하기 위한 파싱
	FastString<128> outerItem;
	FastString<128> inItem;

	if (cpdata.size() <= 0)
		return;
		
	PAS_DEBUG1("parseCpName(): %s", cpdata.toStr());
	
	int pos1 = cpdata.split( &outerItem, delimiter );
	int pos2= outerItem.split(&inItem, "=");
	PAS_DEBUG2("parseCpName(): [%s]  [%s]", outerItem.toStr(), inItem.toStr());
	outerItem.split(&inItem, "=", pos2+1);
	inItem.trim();
	STRCPY(_cpName, inItem, LEN_CP_NAME);

	cpdata.split( &outerItem, delimiter, pos1+1 );
	pos2= outerItem.split(&inItem, "=");
	PAS_DEBUG2("parseCpName(): [%s]  [%s]", outerItem.toStr(), inItem.toStr());
	
	outerItem.split(&inItem, "=", pos2+1);
	inItem.trim();
	STRCPY(_svcCode, inItem, LEN_SVC_CODE);
	
}

void Transaction::parseCpName_New(const char *fullSourceStr)
{
	const char *pos1 = Util2::strcasestr(fullSourceStr, "cpname=");

	if (pos1)
	{
		Util2::getCpnameSvcCode(pos1 + 7, this->cpName, LEN_CP_NAME);
	}

	const char *pos2 = Util2::strcasestr(fullSourceStr, "svccode=");

	if (pos2)
	{
		Util2::getCpnameSvcCode(pos2 + 8, this->svcCode, LEN_SVC_CODE);
	}
}

void Transaction::logStrangeNumber(const char *orgPhonenumber)
{
	MyLog* mylog = new MyLog();
	if (mylog == NULL)
		return;

	filename_t filename = Config::instance()->getLogPrefix();
	filename += "anonymous";
	mylog->open(".", filename);
	mylog->logprint(LVL_INFO, "HTTP PhoneNumber=%s MNC=%s MINnumber=%s\n", 
				orgPhonenumber, MNC, MINnumber);

	mylog->hexdump(LVL_INFO, request.getRawHeader()->rd_ptr(), request.getHeadLeng(), "PHONE REQ MESG ");
			
	delete mylog;
}

void Transaction::setLog( MyLog *log )
{
	tracelog = log;
}

HTTP::Request * Transaction::getRequest()
{
	return &request;
}

const HTTP::Request * Transaction::getRequest() const
{
	return &request;
}

HTTP::Response * Transaction::getResponse()
{
	return &response;
}

const HTTP::Response * Transaction::getResponse() const
{
	return &response;
}

void Transaction::setErrorRespCode( int code )
{
	errorRespCode = code;
}

int Transaction::getErrorRespCode()
{
	return errorRespCode;
}

bool Transaction::isDone()
{
	return jobDone;
}

void Transaction::setDone()
{
	jobDone = true;
}

bool Transaction::isSSL()
{
	return _isSSL;
}

bool Transaction::streaming() const
{
	return _streaming;
}

void Transaction::streaming( bool stream )
{
	if(_streaming != stream)
	{
		if(stream)
			PAS_DEBUG("Set streaming is on.");
		else
			PAS_DEBUG("Set streaming is off.");

		_streaming = stream;
	}
}

bool Transaction::operator<( const Transaction& right )
{
	return seqNum < right.seqNum;
}

bool Transaction::operator==( const Transaction& right )
{
	return seqNum == right.seqNum;;
}

bool Transaction::isNeedHandleSpeedupTag() const
{
	if( Config::instance()->process.serverID.incaseFind("ME") < 0 )
		return false;

	if( !Config::instance()->speedupTag.enable )
		return false;

	if( response.getHeader()->getElement("content-type").incaseFind("text/html") < 0 )
		return false;

	if( 299 < response.getHeader()->getStatusCode() )
		return false;

	if(response.getBodyLeng() <= 0 )
		return false;

	if( !isSpeedupVender() )
		return false;

	if( streaming() )
		return false;

	return true;
}

SpeedUpTagStatus Transaction::getSpeedupStatus() const
{
	if( !isNeedHandleSpeedupTag() )
		return SUTS_NEED_NOT;

	if( Config::instance()->speedupTag.maxSize < response.getBodyLeng() )
		return SUTS_BIG_CONTENT;

	if( isLucentPool() )
	{
		if( isSpeedupRemovePhone() )
			return SUTS_POOL_REMOVE_PHONE;
		else
			return SUTS_POOL_INSERT_PHONE;
	}
	else
	{
		if( isSpeedupRemoveVender() )
			return SUTS_NON_POOL_REMOVE_VENDER;
		else
			return SUTS_NON_POOL_INSERT_VENDER;
	}

	// no reached
}

bool Transaction::isNeedInsertSpeedupTag() const
{
	SpeedUpTagStatus status = getSpeedupStatus();
	return (status == SUTS_POOL_INSERT_PHONE || status == SUTS_NON_POOL_INSERT_VENDER);
}

bool Transaction::isNeedRemoveSpeedupTag() const
{
	SpeedUpTagStatus status = getSpeedupStatus();
	return (status == SUTS_POOL_REMOVE_PHONE || status == SUTS_NON_POOL_REMOVE_VENDER);
}

bool Transaction::isSpeedupRemovePhone() const
{
	if( strcasecmp(msModel, "KTF-X3000") == 0 )
		return true;

	if( strcasecmp(msModel, "KTF-X3300") == 0 )
		return true;

	return false;
}

bool Transaction::isLucentPool() const
{
	intIP_t start = Util::ipaddr2int("10.220.0.0");
	intIP_t end = Util::ipaddr2int("10.221.255.255");

	intIP_t target = Util::ipaddr2int(phoneIpAddr);

	return (start <= target && target <= end);
}

bool Transaction::isSpeedupVender() const
{
	static const char* vender[] = {"SPH", "LG-", "CX-", "CYBER", "KTF-", "MOTOROLA", "V6", "V7", "PD", "PG"};

	int arraySize = sizeof(vender)/sizeof(vender[0]);
	for(int i = 0; i < arraySize; ++i)
		if(strncasecmp(msModel, vender[i], strlen(vender[i])) == 0)
			return true;

	return false;
}

bool Transaction::isSpeedupRemoveVender() const
{
	static const char* vender[] = {"SPH", "KTF-", "PD", "PG-"};

	int arraySize = sizeof(vender)/sizeof(vender[0]);
	for(int i = 0; i < arraySize; ++i)
		if(strncasecmp(msModel, vender[i], strlen(vender[i])) == 0)
			return true;

	return false;
}

