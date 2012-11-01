#include "HttpResponse.h"
#include "PasLog.h"
#include <ace/Message_Block.h>

#define	BODY_BUF_INIT_SIZE	(16*1024)

using namespace HTTP;

int	Response::HeaderBuffBytes = 4*1024;
Response::Response()
{
	contentLength = 0;
	chunkSize = 0;
	receivedChunkSize = 0;
	bodyBufferExpandCount = 0;

	pMBManager = MessageBlockManager::instance();
	ACE_ASSERT(pMBManager != NULL);
	
	pRawHeader = pMBManager->alloc(HeaderBuffBytes);
	ACE_ASSERT(pRawHeader != NULL);

	pRawBody = NULL;

}

Response::~Response()
{
	ACE_ASSERT(pRawHeader != NULL);
	pMBManager->free(pRawHeader);
	
	if (pRawBody != NULL)
		pMBManager->free(pRawBody);
}

int Response::getBodyBufferExpandCount() const
{
	return bodyBufferExpandCount;
}


int Response::setHeader(const ResponseHeader& header_)
{
	header_t rawHeader_;
	int buildResult = header_.build(&rawHeader_);
	if(buildResult < 0)
	{
		PAS_ERROR("Response::setHeader >> raw header 생성 실패");
		return -1;
	}

	return setHeader(rawHeader_);
}

int Response::setHeader(const header_t& headerIN)
{
	return setHeader(headerIN, headerIN.size());
}


/**
날 문자열을 파싱하여  header element를 추출하여 저장하고,
날 문자열을 내부 메시지 버퍼에 복사한다.
*/
int Response::setHeader(const char* buf, const size_t bufSize)
{
	ACE_ASSERT(buf);
	ACE_ASSERT(bufSize > 0);

	// 헤더 파싱
	int resultParse = header.parse(buf, bufSize);
	if(resultParse < 0)
	{
		PAS_DEBUG("헤더 파싱 실패");
		return -1;
	}

	// 헤더 복사 버퍼 공간 확보 (resize)
	if(pRawHeader->size() < bufSize)
	{
		pMBManager->free(pRawHeader);
		pRawHeader = pMBManager->alloc(bufSize);
		if(pRawHeader == NULL)
		{
			PAS_ERROR("Response::setHeader, ReAllocMessageBlock fail!!");
			return -1;
		}
	}
	
	// 헤더 복사
	pRawHeader->reset();
	pRawHeader->copy(buf, bufSize);

	// Content-Length
	contentLength = header.getContentLength();

	return 0;
}


const ACE_Message_Block* Response::getRawHeader() const
{
	return pRawHeader;
}

const ACE_Message_Block* Response::getRawBody() const
{
	return pRawBody;
}

const ResponseHeader* Response::getHeader() const
{
	return &header;
}

size_t Response::getContentLength() const
{
	return contentLength;
}

void Response::setChunkSize(const size_t size)
{
	chunkSize = size;
}

size_t Response::getChunkSize() const
{
	return chunkSize;
}

void Response::setRecevicedChunkSize(const size_t size)
{
	receivedChunkSize = size;
}

size_t Response::getReceivedChunkSize() const
{
	return receivedChunkSize;
}

bool Response::hasChunked() const
{
	return header.hasChunked();
}

int Response::getHeadLeng() const
{
	return pRawHeader->length();
}

int Response::getBodyLeng() const
{
	if( pRawBody == NULL )
		return 0;

	return pRawBody->length();
}

int Response::setBody(const char* buf, const size_t bufSize)
{
	ACE_ASSERT(buf);
	ACE_ASSERT(bufSize > 0);

	size_t newSize = std::max(bufSize, contentLength);
	newSize = std::max(newSize, (size_t)BODY_BUF_INIT_SIZE);

	if (pRawBody == NULL) {
		pRawBody = pMBManager->alloc(newSize);
		if(pRawBody == NULL)
		{
			PAS_ERROR("Response::setBody, AllocMessageBlock fail!!");
			return -1;
		}
	}

	ACE_ASSERT(pRawBody != NULL);
	// resize
	if(pRawBody->size() < bufSize)
	{
		pMBManager->free(pRawBody);
		pRawBody = pMBManager->alloc(newSize);
		if(pRawBody == NULL)
		{
			PAS_ERROR("Response::setBody, ReAllocMessageBlock fail!!");
			return -1;
		}
	}

	// copy
	pRawBody->reset();
	pRawBody->copy(buf, bufSize);

	// reset buf expand counter
	bodyBufferExpandCount = 0;

	refreshContentLength();

	return 0;
}


int Response::appendBody(const char* buf, const size_t bufSize)
{
	ACE_ASSERT(buf != NULL);
	ACE_ASSERT(bufSize > 0);

	if (pRawBody == NULL) 
	{
		// 아래를 안해 주면 대용량 처리할 때 문제가 된다.
		// 메모리 할당/해제 회수를 줄일려면 아래를 실행 해 주어야 한다.
		size_t newSize = std::max(bufSize, contentLength);

		// chunk 인 경우 빨리 버퍼를 충분히 할당하기 위해. (2007.1.3)
		// 빈번한 버퍼 크기 확장을 막기 위해, 초기 최소 사이즈를 제한
		newSize = std::max(newSize, (size_t)BODY_BUF_INIT_SIZE);

		pRawBody = pMBManager->alloc(newSize);
		if(pRawBody == NULL)
		{
			PAS_ERROR("Response::appendBody, AllocMessageBlock fail!!");
			return -1;
		}

		// reset buf expand counter
		bodyBufferExpandCount = 0;
	}

	ACE_ASSERT(pRawBody != NULL);
	
	// 버퍼 크기 확장
	if(pRawBody->space() < bufSize)
	{
		// 빈번한 버퍼 크기 확장을 막기 위해
		size_t newSize = std::max((pRawBody->size() + (bufSize * 2)), (pRawBody->size() * 2));	
		
		PAS_DEBUG2("Response::appendBody OldSize[%d] NewSize[%d]", pRawBody->size(), newSize);

		// 버퍼 사이즈 변경(resize)
		ACE_Message_Block* pTmpBlock = pRawBody;
		pRawBody = pMBManager->alloc(newSize);
		if(pRawBody == NULL)
		{
			PAS_ERROR("Response::appendBody, AllocMessageBlock fail!!");
			return -1;
		}

		// 기존 버퍼로 부터 데이터 복원
		int resultCopy = pRawBody->copy(pTmpBlock->rd_ptr(), pTmpBlock->length());
		if(resultCopy < 0)
		{
			PAS_ERROR("Response::appendBody >> 기존 데이터 복사 실패");
		}

		// 임시 버퍼 삭제
		pMBManager->free(pTmpBlock);

		bodyBufferExpandCount++;
	}

	// append
	int resultCopy = pRawBody->copy(buf, bufSize);
	if(resultCopy < 0)
	{
		PAS_ERROR("Response::appendBody >> 신규 데이터 복사 실패");
		return -1;
	}

	return 0;
}

void Response::refreshContentLength()
{
	ACE_ASSERT(pRawBody != NULL);
		
	int realLength = pRawBody->length();

	contentLength = realLength;
	if(realLength > 0)
	{
		header.replaceElement("Content-Length", realLength);
	}
	else
	{
		header.delElement("Content-Length");
	}
}

void Response::clear()
{
	header.clear();
	pRawHeader->reset();
	if (pRawBody != NULL)
		pRawBody->reset();
	contentLength = 0;
	chunkSize = 0;
	receivedChunkSize = 0;
}
