#include "ResponseBuilder.h"



using namespace std;

int ResponseBuilder::ProxyChange(HTTP::Response*pResponse, const host_t& host, const int port)
{
	ACE_ASSERT(!host.isEmpty());
	ACE_ASSERT(0 <= port && port <= 0xffff);

	/*
	HTTP/1.1 399 Use Proxy
	KTF_HTTP_KEY:[Hash Key];RQSIZE=[Request Size];RSSIZE=[Result Size]
	[Hash Special Header Name][Hash Key Check]
	Server: KTF-PAS/2.0
	Connection: Keep-Alive
	Location: [Destination Host Addr]:[Destination Host Port]
	Content-Type: text/html
	Cache-control: private
	Pragma: no-cache
	Content-Length: [Message size]
	\r\n
	[Message]\r
	*/

	host_t hostAndPortOfDestProxy;
	hostAndPortOfDestProxy.sprintf("%s:%d", host.toStr(), port);

	HTTP::ResponseHeader header;
	header.setVersion("HTTP/1.1");
	header.setStatusCode(399);
	header.setStatusString("Use Proxy");
	header.addElement("Server", "KTF-PAS/3.0"); // version up~ ^_^
	header.addElement("Connection", "Keep-Alive");
	header.addElement("Location", hostAndPortOfDestProxy);
	header.addElement("Content-Type", "text/html");
	header.addElement("Cache-control", "private");
	header.addElement("Pragma", "no-cache");

	const char* html = "<html></html>\r\n";

	HTTP::value_t contentLength;
	contentLength.sprintf( "%d", strlen(html) );
	header.addElement("Content-Length", contentLength);

	pResponse->setHeader(header);
	pResponse->setBody(html, strlen(html));

	return  0;
}


int ResponseBuilder::InvalidProxy_toME(HTTP::Response*pResponse)
{
	HTTP::ResponseHeader header;
	header.setVersion("HTTP/1.1");
	header.setStatusCode(488);
	header.setStatusString("Invalid Proxy");

	header.addElement("Content-Type", "text/html");
	header.addElement("Cache-control", "private");
	header.addElement("Pragma", "no-cache");

	const char* html = "<HTML><body>브라우저의 \"옵션설정->네트웍설정->프락시서버\" 메뉴에서 프락시서버는 m.magicn.com으로, 포트는 7090으로 변경하여 주세요.</body></HTML>\r\n";

	HTTP::value_t contentLength;
	contentLength.sprintf( "%d", strlen(html) );
	header.addElement("Content-Length", contentLength);

	pResponse->setHeader(header);
	pResponse->setBody(html, strlen(html));

	return  0;
}


int ResponseBuilder::InvalidProxy_toKUN(HTTP::Response*pResponse)
{
	HTTP::ResponseHeader header;
	header.setVersion("HTTP/1.1");
	header.setStatusCode(488);
	header.setStatusString("Invalid Proxy");

	header.addElement("Content-Type", "text/html");
	header.addElement("Cache-control", "private");
	header.addElement("Pragma", "no-cache");

	const char* html = "<HTML><body>브라우저의 \"옵션설정->프락시서버\" 메뉴에서 프락시서버는 ktfkunproxy.magicn.com으로, 포트는 9090으로 변경하여 주세요.</body></HTML>\r\n";

	HTTP::value_t contentLength;
	contentLength.sprintf( "%d", strlen(html) );
	header.addElement("Content-Length", contentLength);

	pResponse->setHeader(header);
	pResponse->setBody(html, strlen(html));

	return  0;
}


int ResponseBuilder::InvalidAgent(HTTP::Response*pResponse)
{
	HTTP::ResponseHeader header;
	header.setVersion("HTTP/1.1");
	header.setStatusCode(489);
	header.setStatusString("Invalid Agent");

	header.addElement("Content-Type", "text/html");
	header.addElement("Cache-control", "private");
	header.addElement("Pragma", "no-cache");

	const char* html = "<HTML><body>접속하신 브라우저는 지원하지 않는 브라우저 입니다.</body></HTML>\r\n";

	HTTP::value_t contentLength;
	contentLength.sprintf( "%d", strlen(html) );
	header.addElement("Content-Length", contentLength);

	pResponse->setHeader(header);
	pResponse->setBody(html, strlen(html));

	return  0;
}


/**
@brief Client 에게 Auth 인증 실패 메시지 송신
*/
int ResponseBuilder::AuthFailed(HTTP::Response*pResponse)
{
	const char* html = "<html><body></body></html>\r\n";
	TimeOutError( pResponse, html, strlen(html), 408 );

	return  0;
}

/**
@brief Client 에게 Santa 인증 실패 메시지 송신
*/
int ResponseBuilder::SantaFailed(HTTP::Response*pResponse)
{
	const char* html = "<html><body>단말기의 전화번호를 매핑 DB로 부터 얻어오지 못하였습니다.<br>고객센터로 문의 하시기 바랍니다.</body></html>\r\n";
	TimeOutError( pResponse, html, strlen(html), 408 );

	return 0;
}

/**
@brief CP SSL conn 연결 성공
*/
int ResponseBuilder::CpSSLConnSuccessed(HTTP::Response* pResponse)
{
	/*
	HTTP/1.1 200 Connection established\r\n
	Via: 1.0 Freetel_PAS\r\n
	\r\n
	*/

	HTTP::ResponseHeader header;
	header.setVersion("HTTP/1.1");
	header.setStatusCode(200);
	header.setStatusString("Connection established");
	header.addElement("Via", "KTF-PAS/3.0");

	pResponse->clear();
	pResponse->setHeader(header);

	return 0;
}

/**
@brief CP SSL conn 연결 성공
*/
int ResponseBuilder::CpSSLConnFailed(HTTP::Response* pResponse)
{
	/*
	HTTP/1.1 200 Connection established\r\n
	Via: 1.0 Freetel_PAS\r\n
	\r\n
	*/

	HTTP::ResponseHeader header;
	header.setVersion("HTTP/1.1");
	header.setStatusCode(408);
	header.setStatusString("Connection failed");
	header.addElement("Via", "KTF-PAS/3.0");

	pResponse->clear();
	pResponse->setHeader(header);

	return 0;
}


/**
@brief CP conn 연결 실패
*/
int ResponseBuilder::CpConnFailed(HTTP::Response*pResponse)
{
	const char* html = "<html><body> CP 서버에 접속이 지연되고 있습니다.</body></html>\r\n";
	TimeOutError( pResponse, html, strlen(html), 408 );
	return 0;
}

/**
@brief CP timeout
*/
int ResponseBuilder::CpTimeout(HTTP::Response*pResponse)
{
	const char* html = "<html><body> CP 응답이 지연되고 있습니다.</body></html>\r\n";
	TimeOutError( pResponse, html, strlen(html), 408 );
	return 0;
}

/**
@brief CP timeout
*/
int ResponseBuilder::DnsFail(HTTP::Response*pResponse)
{
	const char* html = "<html><body> 조회가 지연되고 있습니다. 다시 시도해 주세요. </body></html>\r\n";
	TimeOutError( pResponse, html, strlen(html), 408 );
	return 0;
}

/**
공지처리 되는 경우.
"HTTP/1.1  299  OK " 로 응답해야 한다.
*/
int ResponseBuilder::StatFilterBlocked(HTTP::Response*pResponse, char *body, int bodylen)
{
	body[bodylen] = '\r';
	body[bodylen+1] = '\0';
	bodylen++;
	
	HTTP::ResponseHeader header;
	header.setVersion("HTTP/1.1");
	header.setStatusCode(299);
	header.setStatusString("OK");

	header.addElement("Content-Type", "text/html");
	header.addElement("Cache-control", "private");
	header.addElement("Pragma", "no-cache");

	HTTP::value_t contentLength;
    contentLength.sprintf( "%d", bodylen );
    header.addElement("Content-Length", contentLength);
	pResponse->setHeader(header);

	pResponse->setBody(body, bodylen);
	
	return 0;
}

int ResponseBuilder::TimeOutError(HTTP::Response*pResponse, const char *body, int bodylen, int resCode)
{
	if (bodylen==0)
		bodylen = strlen(body);

	HTTP::ResponseHeader header;
	header.setVersion("HTTP/1.1");
	header.setStatusCode(resCode);
	header.setStatusString("Request TimeOut");
	
	header.addElement("Content-Type", "text/html");
	header.addElement("Cache-control", "private");
	header.addElement("Pragma", "no-cache");

	HTTP::value_t contentLength;
    contentLength.sprintf( "%d", bodylen );
    header.addElement("Content-Length", contentLength);
	pResponse->setHeader(header);

	pResponse->setBody(body, bodylen);
	
	return 0;
}

int ResponseBuilder::Redirect(HTTP::Response*pResponse, const url_t& url)
{
	HTTP::ResponseHeader header;
	header.setVersion("HTTP/1.1");
	header.setStatusCode(302);
	header.addElement("Server", "KTF-PAS/3.0"); // version up~ ^_^
	header.addElement("Connection", "Keep-Alive");
	header.addElement("Location", url);
	header.addElement("BILL_INFO", "KTF_BILL_INFO_PAGE");
	header.setStatusString("Object Moved");

	pResponse->setHeader(header);
	
	return 0;
}

int ResponseBuilder::Forbidden(HTTP::Response*pResponse)
{
	HTTP::ResponseHeader header;
	header.setVersion("HTTP/1.1");
	header.setStatusCode(HTTP_SCODE_FORBIDDEN);
	header.setStatusString("Forbidden");

	header.addElement("Content-Type", "text/html");
	header.addElement("Cache-control", "private");
	header.addElement("Pragma", "no-cache");

	const char* html = "<HTML><body>접속하신 페이지는 접근 할 수 없는 페이지입니다.</body></HTML>\r\n";

	HTTP::value_t contentLength;
	contentLength.sprintf( "%d", strlen(html) );
	header.addElement("Content-Length", contentLength);

	pResponse->setHeader(header);
	pResponse->setBody(html, strlen(html));

	return 0;
}
