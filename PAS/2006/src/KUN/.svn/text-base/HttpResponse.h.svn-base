#ifndef __HTTP_RESPONSE_H__
#define __HTTP_RESPONSE_H__

#include <ace/Message_Block.h>
#include "HttpTypes.h"
#include "HttpResponseHeader.h"
#include "MutexQueue.hpp"
#include "NullmutexQueue.hpp"
#include "MemoryPoolManager.h"
#include "Common.h"
#include "ActiveObjectChecker.h"

namespace HTTP
{
	class Response : public ActiveObjectChecker
	{
	public:
		//-------------------------------------------------------
		// 멤버 함수
		//-------------------------------------------------------

		Response();
		virtual ~Response();

		void setSeqNum(int _seq)
		{
			seqNum = _seq;
		}

		int getSeqNum()
		{
			return seqNum;
		}

		// ACE_Message_Block 참조를 줄이기 위해 head, body 참조 함수 추가 2006.10.15 -- handol
		int	getHeadLeng() const;
		int	getBodyLeng() const;
		int getBodyBufferExpandCount() const;
		
		// input function
		int setHeader(const ResponseHeader& header);
		int setHeader(const header_t& header);
		int setHeader(const char* buf, const size_t bufSize);
		int setBody(const char* buf, const size_t bufSize);
		int appendBody(const char* buf, const size_t bufSize);
		void setChunkSize(const size_t size);
		void setRecevicedChunkSize(const size_t size);

		// output function
		const ACE_Message_Block* getRawHeader() const;
		const ACE_Message_Block* getRawBody() const;
		const ResponseHeader* getHeader() const;
		size_t getContentLength() const;
		size_t getChunkSize() const;
		size_t getReceivedChunkSize() const;

		bool hasChunked() const;
		void refreshContentLength();

		void clear();

		//-------------------------------------------------------
		// 멤버 변수
		//-------------------------------------------------------
		static int HeaderBuffBytes;
	
	private:
		//-------------------------------------------------------
		// 멤버 변수
		//-------------------------------------------------------
		ResponseHeader header;
		ACE_Message_Block* pRawHeader;
		ACE_Message_Block* pRawBody;
		size_t contentLength;				///< 헤더에 담긴 content-length, rawBody 의 버퍼 사이즈와는 관련없음
		size_t chunkSize;					///< 수신해야할 chunkSize
		size_t receivedChunkSize;			///< 수신한 chunkSize
		MessageBlockManager* pMBManager;	///< MessageBlock 메모리 풀 매니저
		int	seqNum;

		int bodyBufferExpandCount;
		
		
	};
};

#endif
