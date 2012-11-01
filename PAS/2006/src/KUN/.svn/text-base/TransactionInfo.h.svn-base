#ifndef TRANSACTIONINFO_H
#define	TRANSACTIONINFO_H



#include <list>

#include "Common.h"

#include "HttpRequest.h"
#include "HttpResponse.h"

#include "MyLog.h"
#include "ActiveObjectChecker.h"
#include "CorrelationKey.h"



#define RESCODE_OK 200

#define RESCODE_URL_INVALID 400
#define RESCODE_SANTA 421
#define RESCODE_AUTH 422

#define RESCODE_CP_CONN 431
#define RESCODE_CP_TIMEOUT 408

#define RESCODE_STATFILTER 299
#define RESCODE_MULTIPROXY 399
#define RESCODE_WRONG_PROXY 488
#define RESCODE_WRONG_BROWSER 489


#define	HEADERNAME_LEN 32

enum SpeedUpTagStatus
{
	SUTS_NEED_NOT = 0,
	SUTS_BIG_CONTENT = 3,
	SUTS_POOL_REMOVE_PHONE = 11,
	SUTS_POOL_INSERT_PHONE = 12,
	SUTS_NON_POOL_REMOVE_VENDER = 13,
	SUTS_NON_POOL_INSERT_VENDER = 14
};



/// Transaction 정보 관리
/**
@author DMS

하나의 Transaction에 대한 정보를 모두 포함하고 있는 클래스
Transaction이라 함은, 요청/응답 의 한쌍을 Transaction이라 본다.

한번의 Transaction을 처리하기까지 많은 정보가 필요하며
그것들의 참조/관리를 용이하게 하기 위해서 만든 클래스이다.

아래는 Transaction에서 참조/관리 하는 정보들이다.

[공통 정보]
Sequence Number
Request
Response
Thread ID (이 transaction 을 처리하는 thread 의 번호)

[단말에서 오는 정보]
USER-AGENT
PHONE_NUMBER
MNC
MDN
MIN
IMSI
ChInfo
BaseID
MsModel
OrgURL
RealURL
COUNTER
CKEY
KTF-INIT-PATH

[CP 응답 정보]
CP Name
SVC Code
HashKey
BillInfo

[메시지 송/수신 정보]
Phone IP
Phone Port
CP Host Name
CP IP
CP Port
Transaction 시작 시간
Transaction 응답 시간
Phone Request Bytes
Phone Response Bytes
Phone Response Code

Phone Start Time
Phone End Tiem
Phone Response Time

CP에 접속한 시간
CP Request Bytes
CP Response Bytes
CP Response Code

CP Start Time
CP End Tiem
CP Response Time

SSL 시작 시간
SSL 응답 시간
SSL Request Bytes
SSL Response Bytes
*/

class Transaction : public ActiveObjectChecker
{
public:
	//-------------------------------------------------------
	// 멤버 함수
	//-------------------------------------------------------
	Transaction();
	Transaction(const Transaction &old);
	virtual ~Transaction();

	bool operator < (const Transaction& right);
	bool operator ==  (const Transaction& right);

	void clear();
	void clearSizeAndTime();
	
	void setLog(MyLog *log);

	/// set id : Transaction id (seqNum)을 설정하면서  Request/Response 메시지의 seqNum 도 같이 설정한다.
	void id(int _id);

	/// get id
	int	id();

	HTTP::Request *getRequest();
	const HTTP::Request *getRequest() const;
	HTTP::Response *getResponse();
	const HTTP::Response *getResponse() const;
	
	void setErrorRespCode(int code);
	int	getErrorRespCode();
	void recvPhoneReq();
	void sendPhoneResp();
	void connectCp();
	void sendCpReq();
	void recvCpResp();
	void setCpTime();

	static void parseCpName(HTTP::HeaderElement::value_t &cpdata, const char *delimiter, char *_cpName, char *_svcCode);
	
	void setCpAddr();
	void setCpConnInfo_first();
	void setCpConnInfo_apply_ACL(const char *NameOrIp, int port);
	void setCpConnInfo_second();
	void parsePhoneNumberInfo();
	void setTransactionInfo();
	void setSantaResult(const char *santaMDN, const char *santaIMSI);
	void setSantaResult_New(const char *santaMDN, const char *santaIMSI);
	void setPhoneNumber(const char *tmp_phoneNumber);
	void setAnonymous();
	
	bool isDone();
	void setDone();
	
	void beginSSL();
	void endSSL();
	
	bool isSSL();

	void onSslDataRecv(int recvBytes);
	void onSslDataSend(int sendBytes);

	void parseCpName_New(const char *fullSourceStr);
	int	validatePhoneNumber(const char *http_phoneNumber, char *good_phoneNumber);
	void logStrangeNumber(const char *orgPhonenumber);
	
	/// 스트리밍 사용 여부 확인
	/**
	 * @return 스트리밍 사용 여부
	 * @exception no throw
	 *
	 * @date 2007/03/26
	 * @author Sehoon Yang
	 *
	 * 스트리밍이란 CP로 부터 받은 응답 내용을 PAS에 저장하지 않고, 수신 즉시 핸드폰으로 송신하는 기능이다.
	 * 동영상이나 대용량 파일의 전송 최적화를 하기 위함이다.
	 **/
	bool streaming() const;

	/// 스트리밍 사용 여부 설정
	/**
	 * @param stream 스트리밍 사용 여부
	 * @exception no throw
	 *
	 * @date 2007/03/26
	 * @author Sehoon Yang
	 **/
	void streaming(bool stream);

	/// 스피트업 테그를 삽입해야 하는가?
	/**
	 * CP로 부터 응답 받은 데이터에 스피드업 테그를 삽입해야 한다면 true 를 리턴하고,
	 * 아니라면 false 를 리턴한다.
	 *
	 * 삽입 판단 기준은 Phone 의 IP가 Lucent 대역일 경우 SPEEDUP_REMOVE_PHONE 이 아닐경우 추가하며,
	 * Lucent 대역이 아닐 경우 SPEEDUP_REMOVE_VENDOR 이 아닐 경우 추가한다.
	 *
	 * @return true 이면 삽입해야하고, false 이면 삽입하지 않는다.
	 *
	 * @exception No throw 
	 *
	 * @date 2007/06/01 
	 * @author SeHoon Yang
	 **/
	bool isNeedInsertSpeedupTag() const;

	/// 스피트업 테그를 제거해야 하는가?
	/**
	* CP로 부터 응답 받은 데이터에 스피드업 테그를 삽입해야 한다면 true 를 리턴하고,
	* 아니라면 false 를 리턴한다.
	*
	* 삽입 판단 기준은 Phone 의 IP가 Lucent 대역일 경우 SPEEDUP_REMOVE_PHONE 이면 제거하고,
	* Lucent 대역이 아닐 경우 SPEEDUP_REMOVE_VENDOR 이면 제거한다.
	*
	* @return true 이면 제거해야하고, false 이면 제거하지 않는다.
	*
	* @exception No throw 
	*
	* @date 2007/06/01 
	* @author SeHoon Yang
	**/
	bool isNeedRemoveSpeedupTag() const;

	SpeedUpTagStatus getSpeedupStatus() const;

	

	//-------------------------------------------------------
	// 멤버 변수
	//-------------------------------------------------------

	int	threadId; // 이 transaction 을 처리하는 thread 의 번호.
	bool is3G;

	bool connCloseRequested;  // 단말 요청 헤더에 Connection: close 가 포함된 경우.

	bool hotNumberConverted;
	int	sockfd;
	int	errorRespCode;

	char userAgentLine[LEN_USER_AGENT+1];
	char phoneNumber[LEN_PHONE_NUM+1];
	char MNC[LEN_MNC+1];
	char MDN[LEN_MDN+1];
	char MINnumber[LEN_MDN+1];
	char IMSI[LEN_IMSI+1];
	char chInfo[LEN_CH_INFO+1];
	char baseId[LEN_BASE_ID+1];
	char msModel[LEN_MS_MODEL+1];
	char browser[LEN_BROWSER+1];
	char orgUrl[MAX_URL_LEN+1];
	char realUrl[MAX_URL_LEN+1];
	char counterStr[LEN_COUNTER+1];
	int	counter;
	char cKey[LEN_CKEY+1];
	char ktfInitPath[LEN_KTF_INIT_PATH+1];

	// CP response 메시지에서 얻는 정보
	char cpName[LEN_CP_NAME+1];
	char svcCode[LEN_SVC_CODE+1];

	char hashKey[LEN_HASH_KEY+1];
	char billInfo[LEN_BILL_INFO+1];

	// 메시지 송수신 관련 정보
	char phoneIpAddr[LEN_IPADDR+1];		// 단말기의 IP 주소 문자열(123.123.123.123)
	unsigned int nIPAddr;				// 단말기의 IP 주소 unsigned int 형
	int	phonePort;
	char cpHostName[LEN_HOST_NAME+1];
	char cpIpAddr[LEN_IPADDR+1];
	int	cpPort;

	unsigned int trStartSec; // sec from epoch. time() 으로 구하는 값.
	int	phoneReqBytes;
	int	phoneRespBytes;
	int	phoneRespCode;

	time_t phoneStartSec;
	time_t phoneEndSec;		
	int	phoneRespSec;

	time_t callstarttime; // OLD PAS 소스의 user 정보의 이름 그대로. CP connect 시각.
	
	int	cpReqBytes;
	int	cpRespBytes;
	int	cpRespCode;

	// santa time
	ACE_Time_Value santaStartTime;
	ACE_Time_Value santaEndTime;

	// guide time
	ACE_Time_Value guideStartTime;
	ACE_Time_Value guideEndTime;

	// cp time
	ACE_Time_Value cpStartTime;
	ACE_Time_Value cpEndTime;
	ACE_Time_Value cpRespTime;
	double cpRespMicrosec;

	// total(phone) time
	ACE_Time_Value phoneStartTime;
	ACE_Time_Value phoneEndTime;
	
	time_t cpStartSec;
	time_t cpEndSec;
	int	cpRespSec;
	
	int	sslReqBytes;
	int	sslRespBytes;

	time_t sslStartTm;
	time_t sslEndTm;

	char headerNameForPhoneNum[HEADERNAME_LEN+1]; // 2006.12.8

	CorrelationKey correlationKey;

private:

	//-------------------------------------------------------
	// 멤버 함수
	//-------------------------------------------------------

	/// 스피트업 테그 처리 여부
	/**
	* 스피트업 테그를 삽입하거나 삭제를 해야 한다면 true 를 리턴하고, 아니면 false 를 리턴한다.
	*
	* 판단 기준은, 현재 구동중인 PAS가 ME처리용 PAS인지 여부와 Config 의 설정값과 CP로 부터 응답 받은
	* 데이터 형식과 사이즈, 응답 코드, 그리고 Phone 제조 벤더를 확인해서 결정된다.
	*
	* @return true 이면 처리하고, false 이면 처리하지 않는다.
	*
	* @exception No throw 
	*
	* @date 2007/06/01 
	* @author SeHoon Yang
	**/
	bool isNeedHandleSpeedupTag() const;

	bool isSpeedupRemovePhone() const;
	bool isLucentPool() const;
	bool isSpeedupVender() const;
	bool isSpeedupRemoveVender() const;
	

	//-------------------------------------------------------
	// 멤버 변수
	//-------------------------------------------------------
	bool jobDone;
	int seqNum;
	bool _isSSL;
	bool _streaming; ///< 스트리밍 사용 여부

	HTTP::Request request;
	HTTP::Response response;
	bool aclApplied;
	bool hotNumberApplied;

	MyLog *tracelog;
};



//typedef MutexQueue<Transaction*> TransactionQueue;
typedef NullmutexQueue<Transaction*> TransactionQueue;

#endif
