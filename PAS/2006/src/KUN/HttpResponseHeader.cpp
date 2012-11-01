#include "HttpResponseHeader.h"
#include "PasLog.h"

using namespace HTTP;

ResponseHeader::ResponseHeader(void)
{
	clear();
}

ResponseHeader::~ResponseHeader(void)
{
}

/**
날것의 문자열에서 header element를 추출하여 저장한다. 
즉,  문자열을 파싱하여  ( header name, value ) pair 구조체 값을 설정해 놓는다.

parse()  <--> build()
*/
int ResponseHeader::parse(const char* srcBuf, size_t srcBufSize)
{
	PAS_TRACE0("ResponseHeader::parse");
	ASSERT(srcBuf != NULL);
	ASSERT(srcBufSize > 0);

	clear();

	header_t header;
	if(getHeader(&header, srcBuf, srcBufSize) < 0)
	{
		PAS_DEBUG("Not Found Http Header");
		return -1;
	}

	//PAS_DEBUG1("BaseHeader::parse >> Header\n%s", header.toStr());

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
				PAS_INFO("StartLine 파싱 실패");
				PAS_INFO_DUMP("Http Header", srcBuf, srcBufSize);
				PAS_INFO_DUMP("Fail Line", line.toStr(), line.size());
				return -1;
			}
		}
		else
		{
			int result = parseElement(line);
			if(result < 0)
			{
				PAS_INFO("Header Element 파싱 실패");
				PAS_INFO_DUMP("Http Header", srcBuf, srcBufSize);
				PAS_INFO_DUMP("Error Line", line.toStr(), line.size());
			}
		}
	}

	return 0;
}

int	ResponseHeader::parseStartLine(const line_t& line)
{
	PAS_TRACE("ResponseHeader::parseStartLine");

	// http version
	int pos = line.split(&version, ' ');
	if(pos < 0)
	{
		PAS_INFO("Not Found Http Version");
		PAS_INFO1("HttpLine[%s]", line.toStr());
		return -1;
	}

	version.trim();
	if(version.isEmpty())
	{
		PAS_INFO("Not Found Http Version");
		PAS_INFO1("HttpLine[%s]", line.toStr());
		return -1;
	}

	// 중간 공백 skip
	pos = line.findNotOf(" \t", pos+1);
	if(pos < 0)
	{
		PAS_INFO("Not Found Http Status Code");
		PAS_INFO1("HttpLine[%s]", line.toStr());
		return -1;
	}

	// status code
	FastString<12> statusCodeStr;
	pos = line.split(&statusCodeStr, ' ', pos);
	if(pos < 0)
	{
		PAS_INFO("Not Found Http Status Code");
		PAS_INFO1("HttpLine[%s]", line.toStr());
		return -1;
	}

	statusCodeStr.trim();
	if(statusCodeStr.isEmpty())
	{
		PAS_INFO("Not Found Http Status Code");
		PAS_INFO1("HttpLine[%s]", line.toStr());
		return -1;
	}

	// status
	statusCode = statusCodeStr.toInt();
	const bool validCodeRange = (100 <= statusCode && statusCode <= 599);
	if(validCodeRange == false)
	{
		PAS_INFO1("Invalid status code[%d]", statusCode);
		PAS_INFO1("HttpLine[%s]", line.toStr());
		return -1;
	}

	// 중간 공백 skip
	pos = line.findNotOf(" \t", pos+1);
	if(pos < 0)
	{
		PAS_INFO("Not Found Status String");
		statusStr.append("Status String Empty");
		//  Status String 이 없어도 용서한다. -- @KTF
		//return -1;
	}
	else
	{
		// status string
		line.substr(&statusStr, pos);
	}

	statusStr.trim();
	if(statusStr.isEmpty())
	{
		PAS_INFO("Not Found Status String");
		//  Status String 이 없어도 용서한다. -- @KTF
		//return -1;
	}

	// @KTF 
	//  Status String 이 없어도 용서하고, 내가 직접 채운다.
	if(statusStr.isEmpty())
	{
		statusStr.append("Status String Empty");
	}
		
	return 0;
}

void ResponseHeader::clear()
{
	version.clear();
	statusCode = 0;
	statusStr.clear();
	
	BaseHeader::clear();
}

version_t ResponseHeader::getVersion() const
{
	return version;
}

ResponseStatus ResponseHeader::getStatus() const
{
	ResponseStatus status;
	
	switch(statusCode / 100)
	{
	case 1:
		status = RS_INFORMATION;
		break;

	case 2:
		status = RS_SUCCESS;
		break;

	case 3:
		status = RS_REDIRECTION;
		break;

	case 4:
		status = RS_CLIENT_ERROR;
		break;

	case 5:
		status = RS_SERVER_ERROR;
		break;

	default:
		PAS_WARNING1("Unknown Status Code[%d]", statusCode);
		status = RS_UNKNOWN;
		break;
	}

	return status;
}

int ResponseHeader::getStatusCode() const
{
	return statusCode;
}

status_t ResponseHeader::getStatusString() const
{
	return statusStr;
}

size_t ResponseHeader::getContentLength() const
{
	value_t v = getElement("Content-Length");
	if(!v.isEmpty())
		return (size_t)v.toInt();
	
	return 0;
}

bool ResponseHeader::hasChunked() const
{
	return getElement("Transfer-Encoding").incaseEqual("chunked");
}

int ResponseHeader::buildStartLine(line_t* pLine) const
{
	ASSERT(pLine != NULL);
	pLine->sprintf("%s %d %s\r\n", getVersion().toStr(), getStatusCode(), getStatusString().toStr());
	return 0;
}

/**
parse()  <--> build()
 header element 들 값에서 날것의 문자열을 만들어 낸다.
( header name, value ) pair 구조체 값에서 http header  문자열을 만든다.
*/
int ResponseHeader::build(header_t* pDestHeader) const
{
	ASSERT(pDestHeader != NULL);
	pDestHeader->clear();
	
	line_t line;
	ASSERT(pDestHeader->maxSize() >= line.maxSize());
	
	// start line
	buildStartLine(&line);
	pDestHeader->append(line);

	// header elements
	Keys keys = getKeys();
	Keys::iterator it = keys.begin();
	Keys::iterator itE = keys.end();

	/*
	for( ; it != itE; ++it)
	{
		// 공백 하나를 넣어 주자. KTF. CP 들이 보통 공백을 넣어준다.
		line.sprintf("%s: %s\r\n", it->toStr(), getElement(*it).toStr());
		
		// 용량 초과?
		if(line.size() > pDestHeader->freeSize())
			return -1;
		
		pDestHeader->append(line);
	}
	*/

	// handol
	buildAllLines(pDestHeader);
	

	// 공백 라인 추가
	if(pDestHeader->freeSize() < 2)
		return -1;

	pDestHeader->append("\r\n");

	return 0;
}

void ResponseHeader::setVersion(const char* str)
{
	version = str;
}

void ResponseHeader::setStatusString(const char* str)
{
	statusStr = str;
}

void ResponseHeader::setContentLength(const int len)
{
	int replaceResult = replaceElement("Content-Length", len);
	if(replaceResult < 0)
		PAS_ERROR("ResponseHeader::setContentLength >> replace fail");
}

void ResponseHeader::setStatusCode(const int code)
{
	statusCode = code;
}

