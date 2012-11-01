#ifndef __HTTP_BASE_HEADER_H__
#define __HTTP_BASE_HEADER_H__

#include "Common.h"
#include "HttpTypes.h"
#include "ActiveObjectChecker.h"

namespace HTTP
{
	class BaseHeader : public ActiveObjectChecker
	{
	// 메소드 선언
	public:
		BaseHeader(void);
		virtual ~BaseHeader(void);

		/// 헤더 추출
		/**
		 * dest에는 http header의 footer인 "\r\n\r\n"까지 포함한다.
		 *
		 * @param dest 헤더를 담을 버퍼
		 * @param src 데이터 버퍼
		 * @return 정상 0, src에 헤더가 없다면 -1
		 **/
		static int getHeader(header_t* pDest, const char* srcBuf, size_t srcBufSize);

		/// HTTP header와 body의 구분 위치를 찾아, header의 길이를 return.
		static int	searchHeaderEnd(const char* src, size_t srcSize);


		int addElementAtFront(const key_t& key, const value_t& value);
		int addElementAtFront(const key_t& key, const int& value)
		{
			return addElementAtFront(key, value_t(value));
		}
		
		/// element 추가
		int addElement(const key_t& key, const value_t& value);
		int addElement(const key_t& key, const int& value)
		{
			return addElement(key, value_t(value));
		}

		/**
		 * @return success : 0, key가 이미 존재하면 -1
		 **/
		 int addElementIfNot(const key_t& key, const value_t& value);
		 int addElementIfNot(const key_t& key, const int& value)
		 {
			 return addElementIfNot(key, value_t(value));
		 }

		/// element 수정
		/**
		 * key가 존재하면 수정하고, key가 없다면 추가
		 *
		 * @return 항상 0
		 **/
		int replaceElement(const key_t& key, const value_t& value);
		int replaceElement(const key_t& key, const int& value)
		{
			 return replaceElement(key, value_t(value));
		}

		/// element 삭제
		/**
		 * @return success : 0, fail : -1
		 **/
		int delElement(const key_t& key);

		/// get element
		/**
		 * @return key 에 해당하는 value, key가 존재하지 않는다면 0 size value 를 리턴
		 **/
		value_t getElement(const key_t& key) const;

		/// key에 대한 value가  리턴(FastString)되는 것이 아니라, 주어진 buffer에 복사해준다.
		int getElement(const key_t& key, char *value_buf, int buf_len) const;

		// wildSearchElement 는 주어진 key 를 포함하는 문자열이면 true.
		value_t wildSearchElement(const key_t& key);

		value_t wildSearchElement_getkey(const key_t& wildkey, char *matchedkey, int matchkeylen);
		
		/// 모든 element의 키
		Keys getKeys() const;

		/// clear
		virtual void clear();

		int buildAllLines(header_t* pDestHeader) const;

	protected:
		const HeaderElement* getElementPtr(const key_t& key) const;
		HeaderElement* getElementPtr(const key_t& key);
		int parseElement(const line_t& line);

	// 멤버 변수 선언
	protected:
		HeaderElements elements;
	};
};

#endif
