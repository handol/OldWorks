#ifndef __CP_EVENT_HANDLER_H__
#define __CP_EVENT_HANDLER_H__

#include "PasEventHandler.h"
#include <ace/Reactor.h>
#include <ace/Message_Block.h>

#include "TransactionInfo.h"
#include "HttpResponseHeader.h"
#include "HttpResponse.h"
#include "HttpRequest.h"
#include "FileDump.h"


enum CPState
{
	CPS_NONE,
	CPS_CONNECTING,
	CPS_WAIT_RESPONSE_HEADER,		///< CP WEB으로 부터 Http 응답 Header를 수신해야 하는 상태
	CPS_WAIT_RESPONSE_BODY,			///< CP WEB으로 부터 Http 응답 Body를 수신해야 하는 상태 (Content-Length)
	CPS_WAIT_RESPONSE_CHUNK_SIZE,	///< CP WEB으로 부터 Http 응답 Chunked Data 의 사이즈를 수신해야 하는 상태 (Chunked Data)
	CPS_WAIT_RESPONSE_CHUNK_DATA,	///< CP WEB으로 부터 Http 응답 Chunked Data 의 Body를 수신해야 하는 상태 (Chunked Data)
	CPS_RECEIVED_RESPONSE,			///< CP WEB으로 부터 Http 응답 Header/Body 가 모두 수신 됐을 때
	CPS_SSL							///< SSL 데이터 릴레이
};

class CpResponseData
{
public:

	CpResponseData(const char* data, const int dataSize, const Transaction* pTr) 
		: _data(data), _dataSize(dataSize), _pTransaction(pTr)
	{

	}

	const char* getData()
	{
		return _data;
	}

	const int getDataSize()
	{
		return _dataSize;
	}

	const Transaction* getTransaction()
	{
		return _pTransaction;
	}

private:
	const char* _data;
	const int _dataSize;
	const Transaction* _pTransaction;
};

/// CP WEB에 데이터를 요청하거나 응답을 받는 클래스
/**
 * ClientHandler 로 부터 요청 데이터를 넘겨 받아 CP WEB에게 직접 데이터를 요청하고,
 * CP WEB으로 부터 응답 수신이 완료되면 ClientHandler 에게 완료된 데이터를 넘겨주는 역할을 한다.
 *
 * Http 파이프라이닝이 가능하며, 복수개의 요청 내용을 기억하기 위해 requestedQueue가 존재한다.
 *
 * CpHandler는 ClientHandler가 생성하며, CP WEB과 소켓 접속이 종료되면 스스로를 제거한다.
 **/
class CpHandler :
	public PasHandler
{
// 타입
public:
	


// 멤버 함수
public:
	CpHandler(ACE_Reactor* pReactor, PasHandler* pRequester);
	virtual ~CpHandler(void);

	static unsigned int sentDirectCnt;
	static unsigned int sentByQueCnt;
	
	void init();
	bool isConnecting();

	/// Timer 이벤트가 발생 했을 때
	virtual int handle_timeout(const ACE_Time_Value &current_time, const void *act=0);

	virtual void onReceived();
	virtual void onCloseByPeer();
	
	int setHost(host_t host);
	int setPort(const int port);

	host_t getHost();
	int getPort();

	/// CP WEB에게 데이터를 요청한다.
	/**
	 * Memory new/delete 은 ClientHandler에서 처리.
	 **/
	int start(Transaction *pTransaction);
	int sendToCp(Transaction *pTransaction);
	
	bool isRemovable();

	int connectSSL(const host_t &host, int port);
	int sendSSLData(char* buf, size_t bufSize);

	void reset(const host_t& host, const int port);

	int getReceiveBodySize();
	virtual void close();

protected:
	void changeState(CPState _state);
	
	/// recv 중에 connection close 를 감지한 경우 호출됨
	virtual void onRecvFail();

	/// send 중에 connection close 를 감지한 경우 호출됨
	virtual void onSendFail();

	void onConnect();

	virtual	char* setMyInfo();
	
private:
	time_t lastCpRecvSec;
	int finishSSL(int error_code);
	
	/// 수신된 Body 데이터가 모두 수신 됐는지 확인
	/**
	 * CP WEB의 응답혁신이 Chunked Data 일때는 사용하면 안되고,
	 * Content-Length 정보가 있는 경우만 사용해야 한다.
	 **/
	bool isCompletedReceiveResponseBody();

	/// CP WEB으로 부터 Header가 수신 됐을 때
	void onReceivedResponseHeader();
	
	/// CP WEB으로 부터 Body가 수신 됐을 때
	void onReceivedResponseBody();

	/// CP WEB으로 부터 Chunk Data의 Size 정보가 담긴 데이터가 수신됐을 때
	void onReceivedResponseChunkSize();

	/// CP WEB으로 부터 Chunk Data의 Body 데이터가 수신됐을 때
	void onReceivedResponseChunkData();

	/// content-length 없고 Chunked 도 아닌 경우를 처리하기 위해
	void onOddResponseBody();

	/// content-length 가 있는 경우
	void onNonOddResponseBody();
	
	/// CP WEB으로 부터 SSL 데이터 수신
	void onReceivedSSLData();

	/// CP로 부터 Header/Body가 모두 수신이 완료 됐을 때
	int onCompletedReceiveResponse();

	/// Chunked 이면 Contents-Length 가 있는 경우. Contents-Length 보다 더 많이 온 경우 등의 예외 처리.
	int handleOddLengthResponse();
	
	/// Chunk 결과의 헤더 재정리.
	int	reformatHeader(HTTP::Response* resultHttpResponse);

	// CP timeout 등의 예외 상황시에 모두 finish().
	int	finishAll(int errorCode);

	/// 하나의 transaction 이 완료되었다고 판단될 때 호출
	int	finish(int errorCode=RESCODE_OK);

	void onReceivedPartOfBody(const char* srcBuf, const int srcSize);

// 멤버 변수
private:
	bool isSSL;
	bool closeFuncExecuted;
	CPState state; ///< 상태
	
	HTTP::Response* currHttpResponse; ///< CP WEB으로 부터 수신한 응답 내용을 담는 임시 보관소
	PasHandler* pRequester; ///< 작업을 요청한 ClientHandler 객체

	host_t _host; ///< 접속한 CP WEB의 host
	int _port; ///< 접속한 CP WEB의 port

	// ClientHandler 로부터 Transaction 을 넘겨받아 Q 에 관리.
	TransactionQueue requestedQueue; ///< CP WEB 서버로 요청을 송신한 내역이 담긴 큐

	FileDump *filedump;

	int _receivedBodySize;
};

#endif
