#ifndef __HTTP_HEADER_BUILDER_H__
#define __HTTP_HEADER_BUILDER_H__

#include "HttpTypes.h"
#include "HttpBaseHeader.h"

namespace HTTP
{
	class ResponseHeader : public BaseHeader
	{
	// 메소드 선언
	public:
		ResponseHeader(void);
		~ResponseHeader(void);

		void setVersion(const char* str);
		void setStatusString(const char* str);
		void setContentLength(const int len);
		void setStatusCode(const int code);

		version_t getVersion() const;
		ResponseStatus getStatus() const;
		status_t getStatusString() const;
		
		/// content-length 필드의 값을 리턴한다.
		/**
		* 헤더에 content-length 필드가 존재하지 않는다면, 0을 리턴한다.
		*/
		size_t getContentLength() const;
		int getStatusCode() const;
		bool hasChunked() const;

		/// 헤더 파싱
		int parse(const char* srcBuf, size_t srcBufSize);

		/// 헤더 생성
		int build(header_t* pDestHeader) const;

		/// 파싱 결과 clear
		void clear();

	private:
		int parseStartLine(const line_t& line);
		int buildStartLine(line_t* pLine) const;


	// 멤버 변수 선언
	private:
		// response info
		version_t version;
		int statusCode;
		status_t statusStr;
	};
};

#endif
