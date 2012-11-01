#ifndef __HTTP_KTF_SSL_RESULT_H__
#define __HTTP_KTF_SSL_RESULT_H__

/**
@brief SSL 관련 http 메시지 파싱
*/

#include <string>
#include <vector>
#include <map>
#include <ace/Message_Block.h>
#include "FastString.hpp"
#include "HttpTypes.h"
#include "HttpBaseHeader.h"

namespace HTTP
{
	class KTFSSLResult : public BaseHeader
	{
	// 메소드 선언
	public:
		KTFSSLResult(void);
		virtual ~KTFSSLResult(void);

		/// 헤더 파싱
		int parse(const char* srcBuf, size_t srcBufSize);

		/// 파싱 결과 clear
		virtual void clear();

	private:
		int parseStartLine(const line_t& line);
		
	// 멤버변수 선언
	private:
	};
};

#endif
