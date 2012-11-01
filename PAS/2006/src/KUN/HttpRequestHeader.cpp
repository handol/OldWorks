#include "Common.h"
#include "HttpRequestHeader.h"
#include "HttpUrlParser.h"
#include <ace/Message_Block.h>
#include "FastString.hpp"
#include "PasLog.h"

using namespace std;
using namespace HTTP;

RequestHeader::RequestHeader(void)
{
	clear();
}

RequestHeader::~RequestHeader(void)
{
}

int RequestHeader::parse(const char* srcBuf, size_t srcBufSize)
{
	PAS_TRACE("RequestHeader::parse");
	ASSERT(srcBuf != NULL);
	ASSERT(srcBufSize > 0);

	clear();

	// 헤더 찾기
	header_t header;
	if(getHeader(&header, srcBuf, srcBufSize) < 0)
	{
		PAS_TRACE("Not Found Http Header");
		return -1;
	}

	//PAS_TRACE1("RequestHeader::parse >> Header\n%s", header.toStr());

	// 헤더 파싱
	bool isStartLine = true;
	int lineEndPos = -1;
	line_t line;
	while((lineEndPos = header.getLine(&line, lineEndPos+1)) >=0)
	{
		line.trim();

		if(line.size() == 0)
			continue;

		if(isStartLine)
		{
			isStartLine = false;
			int result = parseStartLine(line);
			if(result < 0)
			{
				PAS_NOTICE("RequestHeader::parse >> StartLine 파싱 실패");
				PAS_NOTICE_DUMP("RequestHeader", srcBuf, srcBufSize);
				PAS_NOTICE_DUMP("Error Line", line.toStr(), line.size());
				return -1;
			}
		}
		else
		{
			int result = parseElement(line);
			if(result < 0)
			{
				PAS_DEBUG("RequestHeader::parse >> Header Element 파싱 실패");
				PAS_DEBUG_DUMP("RequestHeader", srcBuf, srcBufSize);
				PAS_DEBUG_DUMP("Error Line", line.toStr(), line.size());
			}
		}
	}

	// host 정보가 있다면 host, port 빼오기
	parseHostInfo();

	return 0;
}

void RequestHeader::parseHostInfo()
{
	value_t hostInfo = getElement("host");
	if(!hostInfo.isEmpty())
	{
		UrlParser urlParser;
		if(urlParser.parse(hostInfo) < 0)
		{
			PAS_NOTICE("RequestHeader::parseHostInfo >> Host 정보 파싱 실패");
			PAS_NOTICE_DUMP("hostInfo", hostInfo.toStr(), hostInfo.size());
		}
		else
		{
			host = urlParser.getHost();
			port = urlParser.getPort();

			PAS_TRACE2("RequestHeader::parseHostInfo >> Set Host[%s] and Port[%d]", host.toStr(), port);
		}
	}
}

int	RequestHeader::parseStartLine(const line_t& line)
{
	PAS_TRACE1("RequestHeader::parseStartLine() - [%s]", line.toStr());

	// method
	int pos = line.split(&methodStr, ' ');
	if(pos < 0)
	{
		PAS_NOTICE("RequestHeader::parseStartLine >> Not Found Http Method");
		PAS_NOTICE_DUMP("Line", line.toStr(), line.size());
		return -1;
	}

	methodStr.trim();
	if(methodStr.isEmpty())
	{
		PAS_NOTICE("RequestHeader::parseStartLine >> Not Found Http Method");
		PAS_NOTICE_DUMP("Line", line.toStr(), line.size());
		return -1;
	}

	if(methodStr.incaseEqual("GET"))
		method = RM_GET;
	else if(methodStr.incaseEqual("PUT"))
		method = RM_PUT;
	else if(methodStr.incaseEqual("POST"))
		method = RM_POST;
	// KTF SSL - begin
	else if(methodStr.incaseEqual("CONNECT"))
		method = RM_CONNECT;
	// KTF SSL -end
	else if(methodStr.incaseEqual("RESULT"))
		method = RM_RESULT;
	else if(methodStr.incaseEqual("DELETE"))
		method = RM_DELETE;
	else if(methodStr.incaseEqual("HEAD"))
		method = RM_HEAD;
	else if(methodStr.incaseEqual("TRACE"))
		method = RM_TRACE;
	else if(methodStr.incaseEqual("OPTIONS"))
		method = RM_OPTIONS;
	else
	{
		method = RM_NONE;
		PAS_NOTICE("RequestHeader::parseStartLine >> Unknown Http Method");
		PAS_NOTICE_DUMP("Line", line.toStr(), line.size());
		return -1;
	}

	/** KTF */
	if (method == RM_RESULT)
	{
		PAS_TRACE1("Http Request Header {method[%s] }",	methodStr.toStr());
		return 0;
	}
	
	// 중간 공백 skip
	pos = line.findNotOf(" \t", pos+1);
	if(pos < 0)
	{
		PAS_NOTICE("RequestHeader::parseStartLine >> Not Found Http URL");
		PAS_NOTICE_DUMP("Line", line.toStr(), line.size());
		return -1;
	}

	// url
	url_t url;
	pos = line.split(&url, ' ', pos);
	if(pos < 0)
	{
		PAS_NOTICE("RequestHeader::parseStartLine >> Not Found Http URL");
		PAS_NOTICE_DUMP("Line", line.toStr(), line.size());
		return -1;
	}

	url.trim();
	if(url.isEmpty())
	{
		PAS_NOTICE("RequestHeader::parseStartLine >> Not Found Http URL");
		PAS_NOTICE_DUMP("URL", url.toStr(), url.size());
		return -1;
	}

	orgUrl = url; // 파싱 할때 원본 요청 URL을 보관
	setUrl(url);

	// 중간 공백 skip
	pos = line.findNotOf(" \t", pos+1);
	if(pos < 0)
	{
		PAS_NOTICE("RequestHeader::parseStartLine >> Not Found Http Version");
		PAS_NOTICE_DUMP("Line", line.toStr(), line.size());
		return -1;
	}

	// http version
	pos = line.split(&version, ' ', pos);
	if(pos < 0)
	{
		PAS_NOTICE("RequestHeader::parseStartLine >> Not Found Http Version");
		PAS_NOTICE_DUMP("Line", line.toStr(), line.size());
		return -1;
	}

	version.trim();
	if(version.isEmpty())
	{
		PAS_NOTICE("RequestHeader::parseStartLine >> Not Found Http Version");
		PAS_NOTICE_DUMP("Line", line.toStr(), line.size());
		return -1;
	}


	PAS_TRACE3("Http Request Header {method[%s] url[%s] version[%s]}",
		methodStr.toStr(), url.toStr(), version.toStr());

	return 0;
}

url_t RequestHeader::getOrgUrl() const
{
	return orgUrl;
}

void RequestHeader::getOrgUrl(char *urlBuff, int size)  const
{
	strncpy(urlBuff, orgUrl.toStr(), size);
	urlBuff[size] = '\0';
}

void RequestHeader::clear()
{
	method = RM_NONE;
	methodStr.clear();
	host.clear();
	port = 80;
	path.clear();
	version.clear();
	orgUrl.clear();

	BaseHeader::clear();
}

RequestMethod RequestHeader::getMethod() const
{
	return method;
}

 method_t RequestHeader::getMethodStr() const
{
	return methodStr;
}

url_t RequestHeader::getUrl() const
{
	url_t url;
	if(port == 80)
		url.sprintf("http://%s%s", host.toStr(), path.toStr());
	else
		url.sprintf("http://%s:%d%s", host.toStr(), port, path.toStr());

	return url;
}

void RequestHeader::getUrl(char *urlBuff, int size)  const
{
	if (port==80)
		snprintf(urlBuff, size, "http://%s%s",host.toStr(), path.toStr());
	else
		snprintf(urlBuff, size, "http://%s:%d%s", host.toStr(), port, path.toStr());

	urlBuff[size] = '\0';
}

host_t RequestHeader::getHost() const
{
	return host;
}

void RequestHeader::getHost(char *hostBuff, int size)  const
{
	strncpy(hostBuff, host.toStr(), size);
	hostBuff[size] = '\0';
}

int RequestHeader::getPort() const
{
	return port;
}

version_t RequestHeader::getVersion() const
{
	return version;
}

size_t RequestHeader::getContentLength() const
{
	value_t v = getElement("content-length");
	if(!v.isEmpty())
		return (size_t)v.toInt();

	return 0;
}

void RequestHeader::setUrl(const url_t& url)
{
	UrlParser urlParser;

	urlParser.parse(url);
	//host_t newhost  = urlParser.getHost();
	host = urlParser.getHost();
	port = urlParser.getPort();
	path = urlParser.getPath();

	// host 정보가 있는 경우에만 host 설정.
	if (host.size() > 0)	
		buildHostInfo(host, port); /* 2006. 09. 26  Cp 로 보내는 메시지 변경이 안 일어나는 버그 */
}


void RequestHeader::setHostPort(const host_t& host_, int port_)
{
	host = host_;
	port = port_;
	buildHostInfo(host, port);
}

void RequestHeader::setHost(const host_t& host_)
{
	host = host_;
	buildHostInfo(host, port);
}

void RequestHeader::setPort(int port_)
{
	port = port_;
	buildHostInfo(host, port);
}

void RequestHeader::setVersion(const version_t& versionIN)
{
	version = versionIN;
}

void RequestHeader::setMethod(RequestMethod methodIN)
{
	method = methodIN;
}

method_t RequestHeader::methodToString(RequestMethod methodIN) const
{
	method_t _methodStr;
	switch(methodIN)
	{
	case RM_GET:
		_methodStr = "GET";
		break;

	case RM_POST:
		_methodStr = "POST";
		break;

	case RM_DELETE:
		_methodStr = "DELETE";
		break;

	case RM_HEAD:
		_methodStr = "HEAD";
		break;

	case RM_OPTIONS:
		_methodStr = "OPTIONS";
		break;

	case RM_PUT:
		_methodStr = "PUT";
		break;

	case RM_TRACE:
		_methodStr = "TRACE";
		break;

	default:
		ACE_ASSERT(false);
		break;
	}

	return _methodStr;
}

int RequestHeader::buildStartLine(line_t* pLine) const
{
	ACE_ASSERT(pLine != NULL);
	ACE_ASSERT(!methodToString(method).isEmpty());
	ACE_ASSERT(!getUrl().isEmpty());
	ACE_ASSERT(!getVersion().isEmpty());

	pLine->sprintf("%s %s %s\r\n", methodToString(method).toStr(), getPath().toStr(), getVersion().toStr());
	return 0;
}

void RequestHeader::buildHostInfo(const host_t& host_, const int port_)
{
	host_t hostAndPort;
	if(port_ == 80)
		hostAndPort = host_;
	else
		hostAndPort.sprintf("%s:%d", host_.toStr(), port_);

	replaceElement("Host", hostAndPort);
}

int RequestHeader::build(header_t* pDestHeader) const
{
	ASSERT(pDestHeader != NULL);
	pDestHeader->clear();

	line_t line;
	ASSERT(pDestHeader->maxSize() >= line.maxSize());

	// start line
	buildStartLine(&line);
	pDestHeader->append(line);

	buildAllLines(pDestHeader);

	// 공백 라인 추가
	if(pDestHeader->freeSize() < 2)
	{
		PAS_NOTICE("Not enough space for write http header.");
		return -1;
	}

	pDestHeader->append("\r\n");

	return 0;
}

void RequestHeader::setPath(const path_t& pathIN)
{
	path = pathIN;
}

path_t RequestHeader::getPath() const
{
	return path;
}

HTTP::header_t HTTP::RequestHeader::build() const
{
	header_t rawHeader;
	build(&rawHeader);
	return rawHeader;
}