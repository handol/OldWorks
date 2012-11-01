#include "HttpBaseHeader.h"
#include "PasLog.h"
#include "Util.h"

using namespace HTTP;

BaseHeader::BaseHeader(void)
{
	elements.reserve(30);
}

BaseHeader::~BaseHeader(void)
{
	clear();
}

/**
HTTP header와 body의 구분 위치를 찾아, header의 길이를 return.
header와 body의 구분문자열: "\r\n\r\n", "\n\n", "\r\n\n" 등 모두 처리할 수 있도록 한다. 
header의 길에에는 구분 문자열 길이까지 포함한다.
@return -1 if not found
*/
int	BaseHeader::searchHeaderEnd(const char* src, size_t srcSize)
{
	if (srcSize == 0) 
		return 0;

	const char* srcPtr = src;
	size_t	n=0;
	while (n < srcSize) 
	{
		if (srcPtr[0]=='\n')
		{
			if (n+1 < srcSize && srcPtr[1]=='\n') 
			{
				return (int)n + 2;
			}
			else if (n+2 < srcSize && srcPtr[1]=='\r'  && srcPtr[2]=='\n') 
			{
				return (int)n + 3;
			}
		}
		srcPtr++;
		n++;
	}
	return 0;
}

/**
날것의 버퍼 데이타에서 http header 부분까지만 잘라서 FastString (header_t) 에 복사한다.
*/
int BaseHeader::getHeader(header_t* pDest, const char* srcBuf, size_t srcBufSize)
{
	ASSERT(pDest != NULL);
	ASSERT(srcBuf != NULL);
	ASSERT(srcBufSize > 0);

	pDest->clear();

	const size_t headerSize = BaseHeader::searchHeaderEnd(srcBuf, srcBufSize);
	if (headerSize == 0) 
		return -1;

	// 헤더 중간에 NULL 이 들어있는지 확인한다. (NULL이 있으면 안된다.)
	if(memchr(srcBuf, 0x00, headerSize) != NULL)
	{
		PAS_NOTICE("NULL char in request header.");
		PAS_NOTICE_DUMP("Request header", srcBuf, headerSize);			
		return -1;
	}

	pDest->append(srcBuf, headerSize);
	
	return 0;
}

/**
한 라인의 헤더를 파싱하여 key/value 를 구한다.
예외 상황)  key가 없는 경우.  ":" 이 없는 경우. 
2006-11-29
*/
int	BaseHeader::parseElement(const line_t& line)
{
	// 헤더에 ":" 이 없는 경우.
	if(line.find(':') < 0)
		return -1;

	key_t key;
	int pos = line.split(&key, ':');
	key.trim();

	// key 가 없는 경우
	if(key.isEmpty())
		return -1;

	// extract value
	if(pos+1 < static_cast<int>(line.size()))
	{				
		value_t value;
		line.substr(&value, pos+1);
		value.trim();
		addElement(key, value);
	}

	// value is empty
	else
	{
		value_t value;
		addElement(key, value);
	}

	return 0;
}

/**
특정 헤더는 맨 앞에 넣고 싶다.
*/
int BaseHeader::addElementAtFront(const key_t& key, const value_t& value)
{
	// add
	elements.insert(elements.begin(), HeaderElement(key, value));

	//PAS_TRACE2("addElementAtFront(): key[%s] value[%s]", key.toStr(), value.toStr());

	return 0;
}

/**

기존에 addElement() 는 새로운 Element 만 추가하는 것이었다.
이제는 search 없이 무조건 insert. 
같은 Element 가 중복될 수 있다.   --> Set-Cookie 등
*/
int BaseHeader::addElement(const key_t& key, const value_t& value)
{
	// add
	
	if (key.incaseEqual("Host"))
		addElementIfNot(key, value);	
	else
	
		elements.push_back(HeaderElement(key, value));

	//PAS_TRACE2("addElement(): key[%s] value[%s]", key.toStr(), value.toStr());

	return 0;
}

/**

기존에 addElement() 의 이름을 addElementIfNot 으로 변경.
*/
int BaseHeader::addElementIfNot(const key_t& key, const value_t& value)
{
	HeaderElement* pElem = getElementPtr(key);

	// key exist
	if(pElem != NULL)
	{
		PAS_TRACE1("key[%s] is exist", (const char*)key);
		return -1;
	}

	// add
	elements.push_back(HeaderElement(key, value));

	//PAS_TRACE2("put header elem key[%s] value[%s]", key.toStr(), value.toStr());

	return 0;
}


int BaseHeader::replaceElement(const key_t& key, const value_t& value)
{
	HeaderElement* pElem = getElementPtr(key);

	// key exist
	if(pElem != NULL)
	{
		//PAS_TRACE2("replaceElement(): Replace key[%s] value[%s]", key.toStr(), value.toStr());
		// replace
		pElem->value = value;
	}

	// not exist
	else
	{
		//PAS_TRACE2("replaceElement(): Add key[%s] value[%s]", key.toStr(), value.toStr());
		// add
		elements.push_back(HeaderElement(key, value));
	}

	return 0;
}

int BaseHeader::delElement(const key_t& key)
{
	HeaderElements::iterator it = elements.begin();
	HeaderElements::iterator itE = elements.end();

	for( ; it != itE; ++it)
	{
		if(it->key.incaseEqual(key))
		{
			elements.erase(it);
			return 0;
		}
	}

	return -1;
}

value_t BaseHeader::getElement(const key_t& key) const
{
	const HeaderElement* pElem = getElementPtr(key);
	if(pElem == NULL)
		return value_t();

	return pElem->value;
}

/**
key에 대한 value가  리턴(FastString)되는 것이 아니라, 주어진 buffer에 복사해준다.
@param value_buf  값을 복사할 버퍼.
@param buf_len NULL 문자를 포함한 버퍼의 길이여야 한다.
@return 1 if key found, 0 if not found.
*/
int BaseHeader::getElement(const key_t& key, char *value_buf, int buf_len) const
{
	const HeaderElement* pElem = getElementPtr(key);
	if(pElem == NULL) {
		value_buf[0] = '\0';
		return 0;
	}
	else {
		strncpy(value_buf, pElem->value.toStr(), buf_len);
		value_buf[buf_len] = '\0';
		return 1;
	}
}


const HeaderElement* BaseHeader::getElementPtr(const key_t& key) const
{
	HeaderElements::const_iterator it = elements.begin();
	HeaderElements::const_iterator itE = elements.end();

	for( ; it != itE; ++it)
	{
		if(it->key.incaseEqual(key))
			return &(*it);
	}

	return NULL;
}

HeaderElement* BaseHeader::getElementPtr(const key_t& key)
{
	HeaderElements::iterator it = elements.begin();
	HeaderElements::iterator itE = elements.end();

	for( ; it != itE; ++it)
	{
		if(it->key.incaseEqual(key))
			return &(*it);
	}

	return NULL;
}


/**
getElement 는 이름이 일치하는 것을 찾는다 ( 대소문자 무시)
wildSearchElement 는 주어진 key 를 포함하는 문자열이면 true.
*/
value_t BaseHeader::wildSearchElement(const key_t& key)
{
	HeaderElements::iterator it = elements.begin();
	HeaderElements::iterator itE = elements.end();

	for( ; it != itE; ++it)
	{
		if(strstr(it->key.toStr(), key.toStr()) != NULL)
			return it->value;
	}

	return value_t();
}

/**
헤더 목록 중에 wildkey 문자열을 포함하는 key 의 value 를 return 하며, 발견된 key의 값을 matchedkey 에 저장한다.
2006.12.8
*/
value_t BaseHeader::wildSearchElement_getkey(const key_t& wildkey, char *matchedkey, int matchkeylen)
{
	HeaderElements::iterator it = elements.begin();
	HeaderElements::iterator itE = elements.end();

	for( ; it != itE; ++it)
	{
		if(strstr(it->key.toStr(), wildkey.toStr()) != NULL)
		{
			strncpy(matchedkey, it->key.toStr(), matchkeylen);
			matchedkey[matchkeylen] = 0;
			return it->value;
		}
	}

	
	return value_t();
}

void BaseHeader::clear()
{
	if (!elements.empty())
		elements.clear();
}

Keys BaseHeader::getKeys() const
{
	Keys keys;
	keys.reserve(elements.size());

	HeaderElements::const_iterator it = elements.begin();
	HeaderElements::const_iterator itE = elements.end();

	for( ; it != itE; ++it)
	{
		keys.push_back(it->key);
	}

	return keys;
}

int BaseHeader::buildAllLines(header_t* pDestHeader) const
{
	ASSERT(pDestHeader != NULL);

	HeaderElements::const_iterator it = elements.begin();
	HeaderElements::const_iterator itE = elements.end();

	line_t line;
	bool host_found = false;
	for( ; it != itE; ++it)
	{
		// elements 에 host 정보가 복수개 있을 경우, 중복 생성을 막기 위해
		if (it->key.incaseEqual("Host"))
		{
			if(host_found)
				continue;
			
			host_found = true;
		}

		// 공백 하나를 넣어 주자. KTF. CP 들이 보통 공백을 넣어준다.
		// 공백 없이 하자. (2006.10.11 양동기 차장님 의견. HASH_KEY element 때문에)
		line.sprintf("%s: %s", it->key.toStr(), it->value.toStr());

		if(line.freeSize() == 0)
			PAS_INFO2("Header element is too big. KEY[%s] VALUE[%s]", it->key.toStr(), it->value.toStr());
		
		// 용량 초과?
		// +2 for carriage return and new line
		if(pDestHeader->freeSize() < line.size() + 2)
		{
			PAS_NOTICE("Not enough space for write http header.");
			continue;
		}
		
		pDestHeader->append(line);
		pDestHeader->append("\r\n");
	}

	return 0;
}
