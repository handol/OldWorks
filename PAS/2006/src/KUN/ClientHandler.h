#ifndef __CLIENT_EVENT_HANDLER_H__
#define __CLIENT_EVENT_HANDLER_H__

#include <ace/Reactor.h>
#include <ace/Message_Block.h>

#include "Common.h"
#include "ReactorPool.h"
#include "UserInfo.h"
#include "Session.h"
#include "TransactionInfo.h"
#include "PasEventHandler.h"
#include "HttpRequestHeader.h"
#include "HttpRequest.h"
#include "WorkInfo.h"
#include "CpEventHandler.h"
#include "AuthHandler.h"
#include "CpList.h"
#include "SantaResponse.h"
#include "SantaEventHandler.h"
#include "UserInfoMng.h"
#include "PasDataLog.h"
#include "ResponseBuilder.h"
#include "HttpKTFSSLResult.h"
#include "FileDump.h"
#include "GuideCode.h"
#include "GuideReadResponse.h"
#include "CorrelationKey.h"

/**
@brief PASGW와 단말 간의 통신을 제어

 * 단말기와 직접적으로 통신하고, CpHandler를 통해 CP WEB과 간접적으로 통신하고,
 * AuthAgent를 통해 PasAuth와 간접적으로 통신하고, SantaHandler를 통해 Santa와 간접적으로 통신한다.
 *
 * 단말기로 부터 요청을 받아 PasAuth와 Santa를 통해 인증을 하고, CpHandler을 통해 CP WEB으로 데이터를 요청하여
 * CP WEB으로 부터 수신한 데이터를 단말기로 Relay 한다.
 *
 * 이 객체의 생명주기는 단말기가 PASGW로 접속을 하게 되면 PasAcceptEventHandler의 handle_input에서 생성을하고,
 * 단말기가 접속을 종료하면 제거를 한다.
 * 단말기가 능동적으로 접속 종료를 하지 않고, 장시간 idle 상태로 있으면 ClientHandler의 자체적인 Timeout 이벤트에 의해
 * 단말기와의 소켓을 강제 종료한다.
 *
 * 객체 제거를 시도할 때 CpHandler, AuthEventHandler, SantaHandler과 연결되어 있는지 확인하고 이중 연결된 것이
 * 한개 이상 존재한다면 연결이 모두 종료되길 기다린 후, 모든 연결이 종료됐을 때 ClientHandler 가 스스로 WorkQueue에 자신을 제거하도록 하는 작업을 등록한다.
 * 만약, 연결된 외부 객체(CpHandler, AuthEventHandler, SantaHandler)가 없다면 즉시 ClientHandler 객체를 제거한다.
 */

#define	FREE_FRONT	(1)
#define	FREE_BACK	(2)

class ClientHandler :
	public PasHandler
{
	//-------------------------------------------------------
	// 멤버 타입
	//-------------------------------------------------------
protected:
	enum AclResult
	{
		AR_HOLD_PROXY,			///< PROXY 변경 미필요
		AR_CHANGE_PROXY,		///< PROXY 변경 필요
		ACL_DNS_APPLIED
	};

private:
	enum ClientState
	{
		CS_WAIT_REQUEST_HEADER,	///< Client로 부터 웹페이지 요청헤더를 기다리는 상태
		CS_WAIT_REQUEST_BODY,	///< Client로 부터 요청헤더 수신 후 요청바디를 기다리는 상태
		CS_RECEIVED_REQUEST,	///< Client로 부터 용청헤더/바디를 모두 수신 완료한 상태
		CS_CLOSING,				///< ClientHandler 종료 시도 상태
		CS_SSL_CONNECTING,		/// SSL 서버에 연결 중이면 수신 대기 
		CS_SANTA_WAITING,		/// SANTA 응답을 기다리는 중.
		CS_SSL
	};

public:
	//-------------------------------------------------------
	// 멤버 함수
	//-------------------------------------------------------
	ClientHandler(ReactorInfo* rInfo);
	virtual ~ClientHandler(void);
	void init(int newsockfd);

	//-------------------------------------------------------
	// 멤버 변수
	//-------------------------------------------------------
	static unsigned int numTransactionAlloc;
	static unsigned int numRequests;
	static unsigned int numClientHandlers;
	static unsigned int numResponses;

	static unsigned int sentDirectCnt;
	static unsigned int sentByQueCnt;

protected:
	//-------------------------------------------------------
	// 멤버 함수
	//-------------------------------------------------------

	/// ACL 정보를 조회하여 라우팅을 한다.
	/**
	* ACL 라우팅 정책에 의해 요청해야 할 CP의 Host/Port 값을 변환한다.
	* Client 요청을 현재 Proxy에서 처리해야 할 경우는 AR_HOLD_PROXY를 리턴하고,
	* 다른 Proxy에서 처리해야 할 경우 AR_CHANGE_PROXY를 리턴한다.
	*
	* @note AR_CHANGE_PROXY를 리턴 받았다면, proxyHost와 proxyPort 에 담겨진 값으로 Client가 재접속하도록 지시한다.
	*       AR_HOLD_PROXY를 리턴 받았다면, proxyHost/proxyPort는 사용하지 않아야 한다.
	*
	* @return 현재 Proxy서버에서 요청을 처리해야 할 경우 AR_HOLD_PROXY를 리턴하고,
	*         다른 Proxy에서 요청을 처리해야 할 경우 AR_CHANGE_PROXY를 리턴한다.
	**/
	AclResult applyACL(HTTP::Request* pRequest, host_t& proxyHost, int& proxyPort);
	
	void afterCpTransaction(Transaction *tr);

	//-------------------------------------------------------
	// 멤버 변수
	//-------------------------------------------------------
	Session	session;
	UserInfo* userInfo;


private:
	//-------------------------------------------------------
	// 멤버 함수
	//-------------------------------------------------------

	virtual void preStartTransaction(Transaction* tr) {}
	virtual void preAfterCpTransaction(Transaction* tr) {}

	/// CP에게 웹페이지를 요청하기 직전의 이벤트
	virtual void preRequestToCP(Transaction *tr) {}
	virtual int browserTypeCheck( Transaction * tr ) = 0;

	/// send 중에 connection close 를 감지한 경우 호출됨
	virtual void onCloseByPeer();
	virtual void onRecvFail();
	virtual void onSendFail();

	/// 송신 데이터가 모두 송신되었을 경우
	virtual void onSendQueueEmpty();

	/// timer가 만료됐을 때 호출 되는 함수
	virtual int handle_timeout(const ACE_Time_Value &current_time, const void *act=0);

	/// timer가 만료됐을 때 호출 되는 함수
	virtual int handle_exception(ACE_HANDLE fd);

	/// Client와 연결된 소켓으로 부터 데이터가 수신됐을 때
	virtual void onReceived();

	/// CP나 Auth나 Santa로 부터 이벤트가 발생했을 때 호출하는 함수
	virtual void onCommand(CommandID cid, PasHandler* pEH, void* arg1 = NULL, void* arg2 = NULL);

	virtual void close();

	void startSession();
	void finishSession();

	int	consumeRecvBuffer();

	bool isHttpHeaderCandidate(char *buff, int size);
		
	/// 요청 내용의 바디가 모두 수신 됐는지 확인
	/**
	 * 요청 헤더의 content-length와 실제 수신된 바디의 사이즈를 비교한다.
	 *
	 * @return 모두 수신됐다면 true, 아니라면 false
	 **/
	bool isCompletedReceiveRequestBody();

	/// Client로 부터 요청 내용의 헤더가 수신 될때
	int onReceivedRequestHeader();

	/// Client로 부터 요청 내용의 바디가 수신 될때
	int onReceivedRequestBody();

	/// SSL 릴레이 데이터 수신;
	int onReceivedSSLData(Transaction *tr);

	/// Client로 부터 요청 내용이 수신 됐을 때
	int onCompletedReceiveRequest();

	/// CP로 요청한 웹페이지가 응답이 완료 됐을 때
	void onCompletedCPResponse(CpHandler* pCP,  Transaction *resTransaction);

	/// CP WEB으로 접속이 성공했을 때
	void onCompletedCPConnection(CpHandler* pCP, int isSucc);

	/// CP로 부터 Header를 수신했을 때
	/**
	* @exception no throw
	*
	* @date 2007/03/26
	* @author Sehoon Yang
	**/
	void onReceivedHeaderFromCP(CpHandler* pCP, CpResponseData* pData);

	/// CP로 부터 body 데이터의 일부를 수신했을 때
	/**
	 * @exception no throw
	 *
	 * @date 2007/03/26
	 * @author Sehoon Yang
	 **/
	void onReceivedPartialBodyFromCP(CpHandler* pCP, CpResponseData* pData);
	

	/// CP로 네트워크 접속을 시도한다.
	/**
	 * 요청된 CP의 host/port로 이미 접속된 CpEH가 CpList에 존재한다면, 해당 CpEH를 리턴한다.
	 * CpList에 존재하지 않는다면, CpEH를 Heap 에 생성하고 소켓 접속 요청을 하고, CpList에 생성한 CpEH를 등록한 후 CpEH를 리턴한다.
	 * CpHandler 가 리턴됐다고 해서 접속이 완료됐음을 의미하는 것은 아니다.
	 *
	 * @param host 접속 시도 할 host
	 * @param port 접속 시도 할 port
	 * @return CpHandler
	 **/
	CpHandler* pickCpHandler(const host_t& host, int port);


	/// PasAuth로 네트워크 접속을 시도한다.
	/**
	 * Heap 에 AuthEventHandler를 생성한 후 멤버변수인 pAuthEventHandler에 Heap주소를 기록하고, 소켓 접속을 요청한다.
	 * 0 이 리턴됐다고 해서 접속이 완료됐음을 의미하는 것은 아니다.
	 *
	 * @param host 접속 시도 할 host
	 * @param port 접속 시도 할 port
	 * @return 접속 시도가 정상적으로 수행됐다면 0, 실패했다면 -1
	 **/
	int requestAuth(Transaction *tr, const host_t& host, int port);

	/// Santa로 네트워크 접속을 시도한다.
	/**
	 * Heap 에 SantaHandler를 생성한 후 멤버변수인 pSantaHandler에 Heap주소를 기록하고, 소켓 접속을 요청한다.
	 * 0 이 리턴됐다고 해서 접속이 완료됐음을 의미하는 것은 아니다.
	 *
	 * @param host 접속 시도 할 host
	 * @param port 접속 시도 할 port
	 * @return 접속 시도가 정상적으로 수행됐다면 0, 실패했다면 -1
	 **/

	/// 응답헤더에 hashkey를 추가한다.
	int addHashKey(HTTP::ResponseHeader* pDestHeader, const url_t& requestUrl, const size_t requestSize, int responseBodyLeng);

	/// 응답헤더에 bill info 를 추가한다.
	int addBillInfo(HTTP::ResponseHeader* pDestHeader);
	int addGuidePageBillInfo(HTTP::ResponseHeader* pDestHeader);

	/// 응답헤더에 Proxy Info 를 추가한다.
	void addProxyInfo(HTTP::RequestHeader* pDestHeader);

	/// CP에게 웹페이지를 요청한다.
	/**
	 * 멤버변수인 pHttpRequest를 이용해 CP에게 웹페이지 요청을 한다.
	 **/
	void requestToCP(Transaction *tr);

	/// Hot Number 정보를 조회하여 라우팅을 한다.
	bool applyHotNumber(Transaction *tr);

	/// CP로 요청한 웹페이지가 응답이 완료 됐을 때
	int onCommandCPCompleted(CpHandler* pCP,  Transaction *resTransaction);

	/// CP 접속이 완료 됐을 때
	int onCommandCPConnected(CpHandler* pCP, int isSucc);

	/// CP의 소켓 접속이 종료 됐을 때
	int onCommandCPClosed(CpHandler* pCP);

	/// CP WEB으로부터 SSL DATA를 수신했을 때
	int onCommandCPReceivedSSLData(CpHandler* pCP, const char* buf, size_t bufSize);

	/// Santa로 요청한 사항이 응답 완료 됐을 때
	int onCommandSantaCompleted(SantaHandler* pSanta, const SANTA::Response* pResponse);

	/// Santa의 소켓 접속이 종료 됐을 때
	int onCommandSantaClosed(SantaHandler* pSanta);

	/// Santa Timeout
	int onCommandSantaTimeOut(SantaHandler* pSanta);

	int checkSanta(Transaction *tr);
	
	/// Client(단말기)가 서비스를 정상적으로 이용할 수 있는지 확인
	/**
	 * 데이터를 요청한 Client가 정상적으로 서비스를 이용해도 되는 사용자인지 아닌지를 PasAuth와 Santa를 통해 인증을 한다.
	 *
	 * @return 서비스를 이용해도 되는 경우 0을 리턴하고, 그렇지 않은 경우 -1을 리턴한다.
	 */
	int checkAuth(Transaction *tr);

	// AuthAgent 에 보낼 request 를 작성한다.
	void makeRequestAuth(AUTH::RequestBody &body, const int seq, const char* pMin, const unsigned int ip, const int port, const int startConn, const int newBrowser, int g3GCode, const char* pMdn, const char* pMsModel);

	bool checkStatFilter(Transaction *tr, char *notimesg );

	int	getUserInfo(Transaction *tr);
	int	getUserInfo_SSL();

	int allocTransaction();

	void freeTransaction(int flag_pos, Transaction *tr_to_free);

	void freeTransactionFromQ();

	void midTransaction(Transaction *tr);

	int additionalInfoToReqHeader( Transaction * tr );

	virtual int procACL(Transaction* tr) = 0;

	HTTP::ResponseHeader additionalInfoToResHeader(const HTTP::ResponseHeader& header, const char* phoneNumber, const url_t& orignalRequestURL, 
		const int requestDataSize, const int responseBodySize, const host_t& requestHost, const CorrelationKey& correlationKey);
	
	int startTransaction(Transaction *startedTransaction);

	int finishTransaction(Transaction *finishedTransaction);

	int	sendRespToPhoneHeadbody(HTTP::Response* resToPhone);
	int sendRespToPhone(HTTP::Response* resToPhone);

	virtual	char* setMyInfo();

	// CP 에게 SSL 접속 요청
	void requestSSLConnection(Transaction *tr, const host_t& host, const int port);
	
	// Client 에게 SSL 접속이 성공했음을 알림
	void responseSSLConnectionSuccess();

	// Client 에게 SSL 접속이 실패했음을 알림
	void responseSSLConnectionFail();

	// SSL 트랜잭션 정보 기록
	void storeSSLTransaction(const HTTP::KTFSSLResult& sslResult);

	void logHttpReq(const char *mesg, Transaction *tr, bool printBody = false);
	void logHttpResp(const char *mesg, Transaction *tr, bool printBody = false);

	void startSSL(Transaction *tr);
	void finishSSL(Transaction *tr, bool resultRecved = false);

	void changeState(ClientState _state);	

	void printRecentTransaction();
	
	/// Guide 페이지 표시 처리를 한다.
	/**
	 * 접속 후 처음 요청이 아닐 경우 sisor 와 통신 후, 안내페이지 표시 여부를 결정한다.
	 * 안내페이지를 표시해야 할 경우에는, 단말기에 안내페이지로 이동하도록 redirect 지시 내용을
	 * 송신하고 -1을 리턴하며, 안내페이지를 표시 할 필요 없을 경우 아무것도 하지 않고 0을 리턴한다.
	 * 
	 * redirect 를 지시했을 경우는 파라미터로 넘어온 tr을 내부적으로 마무리하므로, 외부에서 tr 을
	 * 마무리 해서는 안된다.
	 * 
	 * @param tr 기록할 트랜잭션 정보
	 * @return 0 : 안내페이지를 표시할 필요가 없이 계속 진행해야 할 때 0을 리턴한다.
	 * @return 1 : 안내페이지로 redirect를 했거나, 문제가 발생해서 작업을 중단해야 할 때 1을 리턴한다.
	 *
	 * @date 2007/02/23
	 * @author Sehoon Yang
	 **/
	int procGuide(Transaction* tr);

	/// 안내페이지 표시 처리 (모든 서비스용)
	/**
	 * 안내페이지를 표시해야 할지 말아야 할지를 판단하여
	 * 표시할 필요가 있으면, 단말사용자를 안내페이지로 redirect 시킨다.
	 * 
	 * @return 안내페이지로 redirect 됐다면 1, 아니면 0
	 *
	 * @date 2007/02/25
	 * @author Sehoon Yang
	 **/
	int procShowGuidePage(Transaction* tr);

	/// 안내페이지 표시 처리 (Fimm 서비스 전용)
	/**
	* Fimm을 사용하는 단말기중 옛날 단말기는, 여타의 단말기와 방식이 달라
	* 별도의 처리가 필요하다.
	*
	* 안내페이지를 표시해야 할지 말아야 할지를 판단하여 표시할 필요가 있다면,
	* 단말사용자를 안내페이지로 redirect 시킨다.
	* 안내페이지를 표시할 필요가 없다면, 요청 URL 에서 reqURL 필드값을 이용해서
	* 해당 URL로 redirect 시킨다.
	* 
	* @return 안내페이지나 포워딩 URL로 redirect 됐다면 1, 아니면 0
	*
	* @date 2007/02/25
	* @author Sehoon Yang
	**/
	int procFimmSpecificShowGuidePage(Transaction* tr);

	/// 요청 URL 의 뒤에 Query 로 주어진 reqURL 주소로 redirect 시킨다.
	/**
	 * 요청 URL 이 "http://ftpkunproxy.magicn.com:9090/?reqURL=http://www.magicn.com" 과 같이
	 * reqURL 이라는 값이 있을 때 해당 URL 로 redirect 시킨다.
	 * 
	 * @return redirect 됐으면 1, 아니면 0
	 *
	 * @date 2007/03/02
	 * @author Sehoon Yang
	 **/
	int redirectToReqURL( Transaction* tr );

	/// 안내페이지를 계속 Skip 하도록 설정 여부 처리
	/**
	 * 안내페이지를 앞으로 계속 Skip 하도록 사용자가 요청했는지를 판단하여
	 * 앞으로 계속 Skip 하도록 설정했다면, 이 사실을 sisor에 알리고, 단말 사용자는
	 * 해당 서비스의 시작 페이지로 redirect 시킨다.
	 * 
	 * @return Skip 하도록 설정하고 redirect 시켰다면 1, 아니면 0
	 *
	 * @date 2007/02/25
	 * @author Sehoon Yang
	 **/
	int procSetSkipGuide(Transaction* tr);

	/// 해당 URL이 Fimm 전용 안내페이지 표시 URL 인지 판단한다.
	/**
	 * Fimm 서비스를 이용하는 단말기 중, 특정(옛날) 단말기의 경우 안내페이지 
	 * 표시 여부판단이 기준이 일단 단말과 방식이 다르다.
	 *
	 * Fimm 전용 안내페이지 요청일 경우 다음과 같은 URL 이 수신된다.
	 * "http://ktfkunproxy.magicn.com:9090/?reqURL=fimm.co.kr"
	 * 
	 * @param url 판단 기준이 되는 URL
	 * @return Fimm 전용일 경우 GCODE_Fimm을 리턴하고, 알 수 없는 URL일 경우 GCODE_Unknown를 리턴한다.
	 *
	 * @date 2007/02/25
	 * @author Sehoon Yang
	 **/
	GuideCode getFimmSpecificGuideCode(const url_t& url);

	/// 요청 URL 의 뒤에 Query 로 주어진 reqURL 주소를 읽어 온다.
	/**
	* 요청 URL 이 "http://ktfkunproxy.magicn.com:9090/?reqURL=http://www.magicn.com" 과 같이
	* reqURL 이라는 값이 있을 때 reqURL 값을 읽어 온다
	* 
	* @return reqURL 있으면 그 값이 셋팅되고, 없으면 empty 이다.
	*
	* @date 2007/03/02
	* @author Sehoon Yang
	**/
	url_t getReqURL(const url_t& url);
	
	/// 안내 페이지로 redirect
	/**
	 * 입력받은 GuideCode 에 해당하는 안내페이지로 단말이 redirect 하도록 
	 * 하는 메시지를 송신한다.
	 * 
	 * @param tr Transaction
	 * @param gcode redirect 할 안내페이지
	 * @return redirect를 했을 경우 0, redirect를 실패 했을 경우 -1
	 *
	 * @date 2007/02/25
	 * @author Sehoon Yang
	 **/
	int redirectToGuidePage(Transaction *tr, const GuideCode gcode);
	
	/// Fimm 전용 안내 페이지로 redirect
	/**
	 * Fimm을 위한 안내 페이지가 별도로 있는 것이 아니고,
	 * Fimm 직접 접속 단말을 위해, redirect 주소 뒤에 "?FromPAS=Y" 라는 추가
	 * Query 를 덧붙인다.
	 * 
	 * @param tr Transaction
	 * @return redirect를 했을 경우 0, redirect를 실패 했을 경우 -1
	 *
	 * @date 2007/02/28
	 * @author Sehoon Yang
	 **/
	int redirectToFimmSpecificGuidePage(Transaction* tr);

	/// 서비스별 SkipGuide 셋팅 결과 페이지로 redirect
	/**
	* 입력받은 GuideCode 에 해당하는 SkipGuide 셋팅 결과 페이지로 단말을
	* redirect 시킨다.
	* 
	* @param tr Transaction
	* @param gcode 서비스코드
	* @return 항상 0
	*
	* @date 2007/02/25
	* @author Sehoon Yang
	**/
	int redirectToGuideResultPage(Transaction *tr, const GuideCode gcode);

	/// 서비스별 SkipGuide 셋팅 결과 페이지 URL 얻어오기
	/**
	 * @param gcode 서비스코드
	 * @param reqURL 사용자가 요청한 URL
	 * @return redirect 해야할 URL
	 *
	 * @date 2007/02/28
	 * @author Sehoon Yang
	 * 
	 * @see redirectToGuideResultPage
	 **/
	url_t getGuideResultURL( const GuideCode gcode, const url_t& reqURL );

	/// 안내페이지 표시 여부를 알아온다.
	/**
	 * Sisor 에게 질의를 해서, 안내페에지의 표시 여부를 알아온다.
	 *
	 * Exception : Sisor와 통신과정에 문제가 있을 경우 exception이 발생한다.
	 * 
	 * @date 2007/02/25
	 * @author Sehoon Yang
	 **/
	GuideReadResponse getSkipGuide(Transaction* tr, const MDN& mdn, const url_t& reqURL);

	/// 안내페이지을 계속 Skip 하도록 셋팅한다.
	/**
	 * 안내페이지를 앞으로 계속 Skip 하도록 Sisor에게 알린다.
	 *
	 * Exception : Sisor와 통신과정에 문제가 있을 경우 exception이 발생한다.
	 *
	 * @date 2007/02/25
	 * @author Sehoon Yang
	 **/
	bool setSkipGuide(Transaction* tr, const MDN& mdn, const GuideCode& gcode);

	/// 요청 URL을 통해 어떤 서비스에 대한 Skip Flag 셋팅 요청인지 판단한다.
	/**
	 * @param reqURL 요청 URL
	 * @return 특정 서비스에 대한 Skip 셋팅 요청일 경우 해당 서비스 코드를 반환한다.
	 * @return Skip 셋팅 요청이 아니거나, 알 수 없는 요청일 경우 GCODE_Unknown 을 반환한다.
	 *
	 * @date 2007/02/25
	 * @author Sehoon Yang
	 **/
	GuideCode getSkipGuideCodeFromURL(const url_t& reqURL);

	/// 요청한 페이지가 Forbidden 임을 단말사용자에게 송신한다.
	/**
	 * 요청한 내역이 Forbidden 임을 알리는 http response를 단말 사용자에게 송신한다.
	 * 
	 * @date 2007/02/26
	 * @author Sehoon Yang
	 **/
	void sendHTTPForbidden(Transaction* tr);

	/// 세션정보를 update 한다.
	/**
	 * 세션정보를 Sisor 에게 update 하도록, 세션정보를 Sisor로 송신한다.
	 * 세션정보는 변화량만을 Sisor에게 알리면, Sisor 가 기존데이터에 누적을 시킨다.
	 * 
	 * @date 2007/02/26
	 * @author Sehoon Yang
	 **/
	void updateSessionInfo();

	/// redirect 해야할 안내페이지 URL 을 얻어 온다.
	/**
	* 안내페이지로 redirect 할 때 URL 뒤에 사용자가 원래 가려고 했던 URL을
	* 추가해 줘야 한다. 
	* 즉, 안내페이지는 http://magicn.com/guide.asp 이고, 사용자가 요청한 페이지는
	* http://magicn.com/onlyadult.asp 라고 하면다면,
	* http://magicn.com/guide.asp?reqURL=http://magicn.com/onlyadult.asp 와 같이 
	* URL을 설정해야 한다.
	* 
	* @param gcode 서비스코드
	* @param reqURL 사용자가 요청한 URL
	* @return 안내페이지 URL
	*
	* @date 2007/02/28
	* @author Sehoon Yang
	**/
	url_t getGuideURL( const GuideCode gcode, const url_t& reqURL );

	/// URL 에 query 를 추가한다.
	/**
	 * URL := http://host/path?query
	 * 
	 * @param srcURL 원본 URL
	 * @param key 추가할 query 의 key
	 * @param value 추가할 key 의 value
	 * @return query 가 추가된 URL
	 *
	 * @date 2007/02/28
	 * @author Sehoon Yang
	 **/
	url_t addQueryToURL(const url_t& srcURL, const char* key, const char* value);
	
	bool procService(Transaction* tr);

	bool addCorrelationKey(HTTP::RequestHeader& destHeader, const CorrelationKey& key);
	bool addCorrelationKey(HTTP::ResponseHeader& destHeader, const CorrelationKey& key);

	/// 안내페이지 연동 로그 기록
	/**
	 * @exception No throw 
	 *
	 * @date 2007/06/05 
	 * @author SeHoon Yang
	 **/
	void writeGuideCommunicationLog(const ACE_Time_Value& duration, const GuideCode& gcode) const;

	/// 비정상 종료 로그 기록
	/**
	 * @exception No throw 
	 *
	 * @date 2007/06/05 
	 * @author SeHoon Yang
	 **/
	void writeAbnormalCloseLog();
	
	

	//-------------------------------------------------------
	// 멤버 변수
	//-------------------------------------------------------
	
	bool everRecvPacket;
	
	ReactorInfo* myReactor;
	
	// currTransaction 은 transactionQueue 에 담겨 있으므로 별도의 삭제가 필요 없다.
	Transaction* currTransaction;

	// currHttpRequest는 transaction에 담겨 있으므로 별도의 삭제가 필요 없다.
	HTTP::Request* currHttpRequest;	///< Client로 부터 수신한 Request 정보
	
	TransactionQueue transactionQueue;

	ClientState state;
	//CpList cpList;					///< ClientHandler:1 <---> N:CPEventHandler 로 연결된 CP 리스트
	//CpHandler _cp;
	CpHandler* _pCP;

	SantaHandler santaHandler;
	AuthHandler authHandler;

	PasDataLog* paslog;
	bool isTrace; // phoneTrace 로 설정된 전화번호이며  true 
	MyLog* httplog;
	FileDump* filedump;
	bool closeFuncExecuted;
	int _sentBodySizeByStream; // 스트리밍 방식으로 전송한 body 사이즈
};

#endif
