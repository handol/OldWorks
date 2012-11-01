#ifndef ResponseBuilder_H
#define ResponseBuilder_H

/**
@brief 예외 상황 처리 메시지를 단말로 전송

Pasgw 가 Client(단말기)에 보낼 응답 페이지들을 미리 정의한 클래스
각 맴버함수들은 static 으로 선언되어 객체 생성없이 사용될 수 있다.

보통 Client <-> Pasgw <-> CP 의 순서로 요청/응답이 발생하지만
특별한 경우(예:CP 없음, 인증실패 등)는 Pasgw가 직접 응답을 해야한다.
응답 내용도 동일하므로 이렇게 미리 만들어두면 편하다.
*/


#include "HttpRequest.h"
#include "HttpResponse.h"

enum HttpStatusCode
{
	// Informational 1xx
	HTTP_SCODE_CONTINUE = 100,
	HTTP_SCODE_SWITCHING_PROTOCOLS = 101,
	
	// Successful 2xx
	HTTP_SCODE_OK = 200,
	HTTP_SCODE_CREATED = 201,
	HTTP_SCODE_ACCEPTED = 202,
	HTTP_SCODE_NON_AUTH_INFO = 203, // Non-Authoritative Information
	HTTP_SCODE_NO_CONTENT = 204,
	HTTP_SCODE_RESET_CONTENT = 205,
	HTTP_SCODE_PARTIAL_CONTENT = 206,
	
	// Redirection 3xx
	HTTP_SCODE_MULTI_CHOICES = 300, // Multiple Choices
	HTTP_SCODE_MOVED_PERMANENTLY = 301,
	HTTP_SCODE_FOUND = 302, 
	HTTP_SCODE_SEE_OTHER = 303,
	HTTP_SCODE_NOT_MODIFIED = 304,
	HTTP_SCODE_USE_PROXY = 305,
	HTTP_SCODE_UNUSED_306 = 306, // (Unused)
	HTTP_SCODE_TEMP_REDIRECT = 307, // Temporary Redirect
	
	// Client Error 4xx
	HTTP_SCODE_BAD_REQUEST = 400,
	HTTP_SCODE_UNAUTHORIZED = 401,
	HTTP_SCODE_PAYMENT_REQUIRED = 402,
	HTTP_SCODE_FORBIDDEN = 403, 
	HTTP_SCODE_NOT_FOUND = 404,
	HTTP_SCODE_METHOD_NOT_ALLOWED = 405,
	HTTP_SCODE_NOT_ACCEPTABLE = 406,
	HTTP_SCODE_PROXY_AUTH_REQUIRED = 407, // Proxy Authentication Required
	HTTP_SCODE_REQUEST_TIMEOUT = 408, 
	HTTP_SCODE_CONFILICT = 409,
	HTTP_SCODE_GONE = 410,
	HTTP_SCODE_LENGTH_REQUIRED = 411,
	HTTP_SCODE_PRECONDITION_FAILED = 412,
	HTTP_SCODE_REQUEST_ENTITY_TOO_LARGE = 413,
	HTTP_SCODE_REQUEST_URI_TOO_LONG = 414,
	HTTP_SCODE_UNSUPPORTED_MEDIA_TYPE = 415,
	HTTP_SCODE_REQUESTED_RANGE_NOT_SATISFIABLE = 416,
	HTTP_SCODE_EXPECTATION_FAILED = 417,
	
	// Server Error 5xx
	HTTP_SCODE_INTERNAL_SERVER_ERROR = 500,
	HTTP_SCODE_NOT_IMPLEMENTED = 501,
	HTTP_SCODE_BAD_GATEWAY = 502,
	HTTP_SCODE_SERVICE_UNAVAILABLE = 503,
	HTTP_SCODE_GATEWAY_TIMEOUT = 504,
	HTTP_SCODE_HTTP_VERSION_NOT_SUPPORTED = 505
};

class ResponseBuilder
{
public:

	/// Client에게 PROXY 변경을 알리는 응답페이지를 송신
	/**
	 * Multi Proxy 정책에 의해, 다른 Proxy 서버로 접속을 해야하는 Client에게
	 * 다른 Proxy로 접속해야 함을 알리고, 어떤 Proxy로 접속해야하는지 정보에 대한 정보를 포함한
	 * Http Response 를 전송한다.
	 *
	 * @note
	 * Client가 새로 접속해야할 Proxy 서버의 host/port는 AclRouter에게 질의하면 ACL_DENY_ACL을 리턴한다.
	 * ACL_DENY_ACL리턴할 때 함수 호출 인자로 넘긴 host와 port에 새로 접속해야하는 Proxy 서버의 주소를 기록해 주므로
	 * AclRouter로 부터 받은 Host/Port 를 Client에게 접속하도록 지시하면 된다.
	 *
	 * @see AclRouter::searchALL
	 *
	 * @param host Client가 새로 접속해야할 Proxy 서버의 host
	 * @param port Client가 새로 접속해야할 Proxy 서버의 port
	 **/
	static int ProxyChange(HTTP::Response*pResponse, const host_t& host, const int port);
	static int InvalidProxy_toME(HTTP::Response*pResponse);
	static int InvalidProxy_toKUN(HTTP::Response*pResponse);
	static int InvalidAgent(HTTP::Response*pResponse);	

	static int AuthFailed(HTTP::Response*pResponse);
	static int SantaFailed(HTTP::Response*pResponse);
	static int CpSSLConnSuccessed(HTTP::Response*pResponse);
	static int CpSSLConnFailed(HTTP::Response* pResponse);
	static int CpConnFailed(HTTP::Response*pResponse);
	static int CpTimeout(HTTP::Response*pResponse);
	static int DnsFail(HTTP::Response*pResponse);
	static int StatFilterBlocked(HTTP::Response*pResponse, char *body, int bodylen);
	static int TimeOutError(HTTP::Response*pResponse, const char *body, int bodylen, int resCode=408);
	static int Redirect(HTTP::Response*pResponse, const url_t& url);
	static int Forbidden(HTTP::Response*pResponse);

private:

};


#endif
