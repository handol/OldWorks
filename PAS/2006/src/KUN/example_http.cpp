#include "HttpTypes.h"
#include "HttpUrlParser.h"
#include "HttpRequestHeader.h"
#include "HttpResponseHeader.h"

#include <ace/SOCK_Connector.h>
#include <ace/SOCK_Stream.h>
#include <ace/INET_Addr.h>
#include <ace/Message_Block.h>

int main(const int argc, const char* argv[])
{
	// 사용법
	if(argc < 2)
	{
		printf("usage : %s URL\n", argv[0]);
		return 0;
	}

	// URL 파싱
	HTTP::url_t url(argv[1]);
	HTTP::UrlParser urlParser;
	
	urlParser.parse(url);

	HTTP::host_t host = urlParser.getHost();
	if(host.isEmpty())
	{
		printf("URL에 호스트 정보가 없습니다.");
		return 0;
	}

	int port = urlParser.getPort();

	// 웹 서버 접속 정보 셋팅
	ACE_SOCK_Connector connector;
	ACE_SOCK_Stream stream;
	ACE_INET_Addr addr(port, host);

	// 웹 서버에 접속
	connector.connect(stream, addr);

	// 요청 http 헤더 셋팅
	HTTP::RequestHeader reqHeader;

	HTTP::host_t hostAndPort;
	if(port == 80)
		hostAndPort = host;
	else
		hostAndPort.sprintf("%s:%d", host.toStr(), port);

	reqHeader.setHost(hostAndPort);
	reqHeader.setUrl(urlParser.getPath());
	reqHeader.setMethod(HTTP::RM_GET);
	reqHeader.setVersion("HTTP/1.1");
	
	// 요청 http 헤더 생성
	HTTP::header_t reqRawHeader;
	reqHeader.build(&reqRawHeader);

	// rawHeader 출력
	printf("[Request Header]\n%s\n", reqRawHeader.toStr());

	// 요청 송신
	stream.send(reqRawHeader.toStr(), reqRawHeader.size());

	HTTP::ResponseHeader resHeader;
	HTTP::header_t resRawHeader;

	// 데이터 수신
	ACE_Message_Block recvBuffer(102400);
	while(true)
	{
		int recvSize = stream.recv(recvBuffer.wr_ptr(), recvBuffer.space());
		
		// 에러
		if(recvSize < 0)
		{
			printf("수신 에러\n");
			return -1;
		}

		// 접속 종료
		if(recvSize == 0)
		{
			printf("접속 종료\n");
			return -1;
		}

		// move write pointer
		ACE_ASSERT(recvSize <= recvBuffer.space());
		recvBuffer.wr_ptr(recvSize);

		// 헤더 찾기
		int ret = resHeader.getHeader(&resRawHeader, recvBuffer.rd_ptr(), recvBuffer.length());
		if(ret == 0)
		{
			// move read pointer
			recvBuffer.rd_ptr(resRawHeader.size());
			break;	
		}
	}

	// 응답 헤더 파싱
	int ret = resHeader.parse(resRawHeader, resRawHeader.size());
	if(ret < 0)
	{
		printf("파싱 실패\n");
		return -1;
	}

	// 응답 코드 
	HTTP::ResponseStatus resStatus = resHeader.getStatus();
	if(resStatus != HTTP::RS_OK)
	{
		printf("잘 못된 요청\n");
		return -1;
	}

	// 응답 body 사이즈 확인
	size_t contentLength = resHeader.getContentLength();
	if(contentLength == 0)
	{
		printf("수신 내용 없음\n");
		return -1;
	}

	// body 수신
	ACE_Message_Block resRawBody(contentLength+1);

	// 수신 버퍼에 남아 있는 데이터가 있나?
	if(recvBuffer.length() > 0)
	{
		resRawBody.copy(recvBuffer.rd_ptr(), recvBuffer.length());
	}

	// body 데이터 수신
	while(resRawBody.length() < contentLength)
	{
		int recvSize = stream.recv(resRawBody.wr_ptr(), resRawBody.space());
		
		// 에러
		if(recvSize < 0)
		{
			printf("수신 에러\n");
			return -1;
		}

		// 접속 종료
		if(recvSize == 0)
		{
			printf("접속 종료\n");
			return -1;
		}
		
		// move write pointer
		ACE_ASSERT(recvSize <= resRawBody.space());
		resRawBody.wr_ptr(recvSize);
	}

	// rawBody 마지막에 NULL 추가 (printf를 사용하기 위해)
	ACE_ASSERT(1 <= resRawBody.space());
	char* writePtr = resRawBody.wr_ptr();
	*writePtr = '\0';
	resRawBody.wr_ptr(1);

	// body 출력
	printf("[Response Data]\n%s\n", resRawBody.rd_ptr());

	return 0;
}