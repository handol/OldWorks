#ifndef __FAST_STRING_HPP__
#define __FAST_STRING_HPP__

#include <assert.h>
#include <string.h>
#include <stdarg.h>
#include <algorithm>

#include "basicDef.h"

#ifndef IS_WHITE_SPACE
#define	IS_WHITE_SPACE(X)	((X)==' ' || (X)=='\t' || (X)=='\n' || (X)=='\r')
#endif

/// 고정사이즈 문자열 핸들링 클래스
/**
 * 내부적으로 동적 메모리 할당을 하는 가변사이즈 문자열 핸들링 클래스와는 달리
 * 고정사이즈 버퍼를 사용하므로서 동적할당의 시간을 제거한 문자열 핸들링 클래스
 *
 * 버퍼 오버플로우가 발생하지 않도록 내부적으로 사이즈 체크를 한다.
 **/
template<size_t MaxSize>
class FastString
{
public:
	FastString()
	{
		clear();

		ASSERT(buf[MaxSize-1] == '\0');
		ASSERT(strlen(buf) == bufLen);
	}

	FastString(const char* str, size_t size)
	{
		clear();
		this->insert(0, str, size);

		ASSERT(buf[MaxSize-1] == '\0');
		ASSERT(strlen(buf) == bufLen);
	}

	FastString(const char* str)
	{
		clear();
		this->insert(0, str);

		ASSERT(buf[MaxSize-1] == '\0');
		ASSERT(strlen(buf) == bufLen);
	}

	template<typename T1>
	FastString(const T1& value)
	{
		clear();
		this->assign(value);

		ASSERT(buf[MaxSize-1] == '\0');
		ASSERT(strlen(buf) == bufLen);
	}

	~FastString()
	{
//		ASSERT(buf[MaxSize-1] == '\0');
//		ASSERT(strlen(buf) == bufLen);
	}

	char firstCh()
	{
		if (bufLen > 0)
			return buf[0];
		else
			return '\0';
	}

	/*
	/// 특정 위치의 문자 반환
	const char& operator [] (size_t index) const
	{
		ASSERT(index < bufLen);

		return buf[index];
	}
	*/

	/// 특정 위치의 문자 반환
	char& operator [] (size_t index)
	{
		ASSERT(index < bufLen);

		return buf[index];
	}

	/// 대소문자를 구별하는 문자열 비교
	/**
	 * @seealso casecmp
	 **/
	bool operator == (const char* rhs) const
	{
		return strcmp(buf, rhs) == 0;
	}

	/// 대소문자를 구별하는 문자열 비교
	bool operator != (const char* rhs) const
	{
		return strcmp(buf, rhs) != 0;
	}

	/// 문자열 크기 비교
	bool operator < (const char* rhs) const
	{
		return strcmp(buf, rhs) < 0;
	}

	/// 문자열 더하기
	template<typename T1>
	FastString<MaxSize>& operator += (const T1& value)
	{
		this->append(value);
		return *this;
	}

	/// 문자열 더하기
	template<typename T1>
	FastString<MaxSize> operator + (const T1& rhs) const
	{
		FastString<MaxSize> newString(*this);
		newString.append(rhs);
		return newString;
	}

	/// 문자열 셋팅
	template<typename T1>
	FastString<MaxSize> operator = (const T1& rhs)
	{
		this->assign(rhs);
		return *this;
	}


	/// 문자열 리턴 (null terminated string)
	operator const char* () const
	{
		return buf;
	}

	/// 문자열 셋팅
	template<typename T1>
	int assign(const T1& value)
	{
		clear();
		this->append(value);
		return bufLen;
	}

	/// 문자열 리턴 (null terminated string)
	const char* toStr() const
	{
		return buf;
	}

	/// 문자열을 int로 변환
	int toInt() const
	{
		return atoi(buf);
	}

	/// 문자열을 float로 변환
	float toFloat() const
	{
		return atof(buf);
	}

	/// 문자열을 double로 변환
	double toDouble() const
	{
		return atof(buf);
	}

	/// 대소문자 구별이 없는 문자열 비교
	/**
	 * @return 같으면 true, 다르면 false
	 **/
	bool incaseEqual (const char* rhs) const
	{
		return strcasecmp(buf, rhs) == 0;
	}

	/// 문자열의 좌우 공백 제거
	int trim()
	{
		trimRight();
		trimLeft();
		return bufLen;
	}

	/// 문자열의 왼쪽 공백 제거
	int trimLeft()
	{
		if(bufLen == 0)
			return 0;

		char* str = buf;
		size_t n;
		for(n=0; n<bufLen; ++str, ++n)
		{
			if(!IS_WHITE_SPACE(*str))
				break;
		}

		if(n > 0)
		{
			// (bufLen-n) + [null char size = 1]
			memmove(buf, &buf[n], (bufLen-n)+1);

			bufLen -= n;
		}

		ASSERT(buf[MaxSize-1] == '\0');
		ASSERT(strlen(buf) == bufLen);
		return bufLen;
	}

	/// 문자열 오른쪽 공백 제거
	int trimRight()
	{
		if(bufLen == 0)
			return 0;

		char* str = &buf[bufLen-1];
		int n;
		for(n=bufLen-1; n>=0; --str, --n)
		{
			if(!IS_WHITE_SPACE(*str))
				break;
		}

		n+= 1;
		buf[n] = '\0';
		bufLen = n;

		ASSERT(buf[MaxSize-1] == '\0');
		ASSERT(strlen(buf) == bufLen);
		return bufLen;
	}

	/// 문자열 최대 사이즈
	/**
	 * 버퍼의 마지막에 null이 추가되므로, 클래스 생성할때의 MaxSize 보다 1 작다.
	 **/
	static size_t maxSize()
	{
		return MaxSize-1;
	}

	/// 문자열 길이
	size_t size() const
	{
		return bufLen;
	}

	/// 남은 사이즈
	size_t freeSize() const
	{
		return maxSize() - size();
	}

	/// 문자 삽입
	/**
	 * 버퍼가 가득 찼을 경우 문자열의 뒷부분을 버리고 문자를 삽입한다.
	 *
	 * @param index 삽입할 위치(zero based index)
	 * @param ch 삽입할 문자
	 * @return 문자열 길이
	 **/
	int insert(size_t index, char ch)
	{
		ASSERT(index < MaxSize-1);

		int rightStringLen = size - index;

		if(index < MaxSize-2)
		{
			// rightStringLen + [null char size = 1]
			copy(index+1, index, rightStringLen+1);
		}

		buf[index] = ch;
		++bufLen;

		ASSERT(buf[MaxSize-1] == '\0');
		ASSERT(strlen(buf) == bufLen);
		return bufLen;
	}

	/// 문자열 삽입
	/**
	 * 삽입할 문자열은 null terminated string 이면 null을 만나기 전까지의 문자열을 삽입한다.
	 * 삽입할 문자열이 null terminated string이 아니라면 size 값을 통해 길이를 제한 할 수 있다.
	 * size 를 통해 삽입할 문자열의 길이를 제한 할 수 있다.
	 * 버퍼가 가득 찼을 경우 문자열의 뒷부분을 버리고 문자열을 삽입한다.
	 *
	 * @param index 삽입할 위치(zero based index)
	 * @param str 삽입할 문자열
	 * @param size 삽입할 문자열의 사이즈
	 * @return 문자열 길이
	 **/
	int insert(size_t index, const char* str, size_t size)
	{
		ASSERT(index <= bufLen);
		ASSERT(strlen(buf) == bufLen);

		if(size == 0)
			return bufLen;

		int rightStringLen = bufLen - index;
		size_t moveDestIndex = index+size;

		if(moveDestIndex < MaxSize-1)
		{
			// rightStringLen + [null char size = 1]
			copy(moveDestIndex, index, rightStringLen+1);
		}

		int copiedSize = copy(index, str, size);

		bufLen = std::min(bufLen+copiedSize, MaxSize-1);

		ASSERT(buf[MaxSize-1] == '\0');
		ASSERT(buf[bufLen] == '\0');
		ASSERT(strlen(buf) == bufLen);
		return bufLen;
	}

	int insert(size_t index, const char* str)
	{
		ASSERT(index <= bufLen);
		insert(index, str, strlen(str));
		return bufLen;
	}

	/// 문자열 마지막에 문자 붙이기
	/**
	 * @return 문자열 길이
	 *
	 * @seealso insert
	 **/
	int append(const char ch)
	{
		ASSERT(strlen(buf) == bufLen);
		insert(bufLen, ch);
		return bufLen;
	}

	/// 문자열 마지막에 문자열 붙이기
	/**
	 * @return 0
	 *
	 * @seealso insert
	 **/
	int append(const char* str, size_t size)
	{
		ASSERT(strlen(buf) == bufLen);
		insert(bufLen, str, size);
		return bufLen;
	}

	int append(const char* str)
	{
		ASSERT(strlen(buf) == bufLen);
		insert(bufLen, str);
		return bufLen;
	}

	/// 문자열 마지막에 변수 붙이기
	/**
	 * @return 문자열 길이
	 **/
	int append(const int val)
	{
		ASSERT(strlen(buf) == bufLen);
		char str[32];
		snprintf(str, sizeof(str), "%d", val);
		append(str);
		return bufLen;
	}

	/// 문자열 마지막에 변수 붙이기
	/**
	 * @return 문자열 길이
	 **/
	int append(const unsigned int val)
	{
		ASSERT(strlen(buf) == bufLen);
		char str[32];
		snprintf(str, sizeof(str), "%u", val);
		append(str);
		return bufLen;
	}

	/// 문자열 마지막에 변수 붙이기
	/**
	 * @return 문자열 길이
	 **/
	int append(const float val)
	{
		ASSERT(strlen(buf) == bufLen);
		char str[32];
		snprintf(str, sizeof(str), "%f", val);
		append(str);
		return bufLen;
	}

	/// 문자열 마지막에 변수 붙이기
	/**
	 * @return 문자열 길이
	 **/
	int append(const double val)
	{
		ASSERT(strlen(buf) == bufLen);
		char str[32];
		snprintf(str, sizeof(str), "%lf", val);
		append(str);
		return bufLen;
	}

	/// 문자열 마지막에 FastString 붙이기
	/**
	 * @return 문자열 길이
	 **/
	template<size_t SrcMaxSize>
	int append(const FastString<SrcMaxSize>& value)
	{
		ASSERT(strlen(buf) == bufLen);
		this->append(value.toStr());
		return bufLen;
	}

	/// 문자열 삭제
	/**
	 * @param index 삭제할 문자열의 위치 (zero based index)
	 * @param size 삭제할 문자열 길이
	 * @return 문자열 길이
	 **/
	int erase(size_t index, size_t size = 0xffffffff)
	{
		ASSERT(strlen(buf) == bufLen);
		ASSERT(index < MaxSize-1);

		size_t rightStringLen = bufLen - index;
		if(size >= rightStringLen)
		{
			buf[index] = '\0';
			bufLen = index;
		}
		else
		{
			// size + [null char size = 1]
			copy(index, index+size, size+1);
			bufLen -= size;
		}

		ASSERT(buf[MaxSize-1] == '\0');
		ASSERT(strlen(buf) == bufLen);
		return bufLen;
	}

	/// 형식있는 문자열 출력
	/**
	 * C 표준 함수인 sprintf 와 동일하다.
	 * @return 문자열 길이
	 **/
	int sprintf(const char *fmt, ...)
	{
		va_list args;
		va_start(args, fmt);
		bufLen = vsnprintf(buf, MaxSize, fmt, args);
		va_end(args);

		if(bufLen > MaxSize-1)
			bufLen = MaxSize-1;

		ASSERT(buf[MaxSize-1] == '\0');
		ASSERT(strlen(buf) == bufLen);
		return bufLen;
	}

	/// 문자열 추출
	/**
	 * @param index 추출한 문자열 위치(zero based index)
	 * @param size 추출할 문자열 길이
	 * @return 추출된 문자열 (문자열을 담고 있는 FastString과 동일한 사이즈의 FastString을 리턴)
	 **/
	FastString<MaxSize> substr(size_t index, size_t size = 0xffffffff) const
	{
		ASSERT(strlen(buf) == bufLen);
		ASSERT(index < MaxSize-1);

		FastString<MaxSize> str;
		size_t substrSize = std::min(bufLen - index, size);
		substr(&str, index, substrSize);

		return str;
	}

	/// 문자열 추출
	/**
	 * @param pDestStr 추출한 문자를 담을 FastString
	 * @param index 추출한 문자열 위치(zero based index)
	 * @param size 추출할 문자열 길이
	 * @return 추출된 문자열 길이
	 **/
	template <size_t SrcMaxSize>
	int substr(FastString<SrcMaxSize>* pDestStr, size_t index, size_t size = 0xffffffff) const
	{
		ASSERT(strlen(buf) == bufLen);
		ASSERT(pDestStr != NULL);
		ASSERT(index < MaxSize-1);

		pDestStr->clear();
		size_t substrSize = std::min(bufLen - index, size);
		pDestStr->append(&buf[index], substrSize);
		return pDestStr->size();
	}

	/// 문자 찾기
	/**
	 * @param ch 찾을 문자
	 * @return 문자 시작 위치(zero based index), 없으면 -1
	 **/
	int find(const char ch, size_t startPos = 0) const
	{
		ASSERT(strlen(buf) == bufLen);
		if(startPos >= bufLen)
			return -1;

		char* pPos = (char*)strchr(&buf[startPos], ch);

		if(pPos == NULL)
			return -1;

		int findPos = (int)(pPos - buf);
		return findPos;
	}

	/// 문자열 찾기
	/**
	 * @param str 찾을 문자열 문자
	 * @return 문자열 시작 위치(zero based index), 없으면 -1
	 **/
	int find(const char* str, size_t startPos = 0) const
	{
		ASSERT(strlen(buf) == bufLen);
		if(startPos >= bufLen)
			return -1;

		const char* pPos = strstr(&buf[startPos], str);

		if(pPos == NULL)
			return -1;

		int findPos = (int)(pPos - buf);

		return findPos;
	}

	/// 토큰 문자가 아닌 곳 찾기
	/**
	 * @param ch 토큰 문자
	 * @return 문자 시작 위치(zero based index), 없으면 -1
	 **/
	int findNotOf(const char ch, size_t startPos = 0) const
	{
		ASSERT(strlen(buf) == bufLen);
		if(startPos >= bufLen)
			return -1;

		char* pCh = &buf[startPos];
		while(*pCh != '\0')
		{
			if(*pCh != ch)
				break;

			++pCh;
		}

		if(*pCh == '\0')
			return -1;

		int findPos = (int)(pCh - buf);
		return findPos;
	}

	/// 토큰 문자열이 아닌 곳 찾기
	/**
	 * 토큰 문자열이 포함하는 문자가 아닌 문자가 처음 나타나는 곳
	 *
	 * @param str 토큰 문자열 문자
	 * @return 문자열 시작 위치(zero based index), 없으면 -1
	 **/
	int findNotOf(const char* str, size_t startPos = 0) const
	{
		ASSERT(strlen(buf) == bufLen);
		if(startPos >= bufLen)
			return -1;

		const char* pCh = &buf[startPos];
		while(*pCh != '\0')
		{
			if(strchr(str, *pCh) == NULL)
				break;

			++pCh;
		}

		if(*pCh == '\0')
			return -1;

		int findPos = (int)(pCh - buf);
		return findPos;
	}

	/// 토큰을 기준으로 한 문자열 추출
	/**
	 * startPos 부터 startPos 이후에 발견되는 첫 토큰의 바로 앞까지의 문자열을 추출한다.
	 * startPos 이후에 토큰이 존해하지 않는다면, startPos 부터 마지막까지의 문자열을 추출하고, 문자열 마지막에 있는 null 문자의 index를 리턴한다.
	 *
	 * @param pDestStr 추출한 문자를 담을 FastString
	 * @param delimiter 토큰(문자)
	 * @param startPos 추출 시작 위치 (zero based index)
	 * @return 토큰 위치 (zero based index) 또는 null 문자의 index
	 **/
	template <size_t SrcMaxSize>
	int split(FastString<SrcMaxSize>* pDestStr, const char delimiter, size_t startPos = 0) const
	{
		ASSERT(strlen(buf) == bufLen);
		pDestStr->clear();

		if(startPos >= bufLen)
			return -1;

		int pos = find(delimiter, startPos);
		if(pos < 0)
		{
			substr(pDestStr, startPos);
			return bufLen;
		}
		else if(pos == (int)startPos)
		{
			pDestStr->clear();
		}
		else
		{
			ASSERT(pos > (int)startPos);
			substr(pDestStr, startPos, pos-startPos);
		}

		return pos;
	}

	/// 토큰을 기준으로 한 문자열 추출
	/**
	 * startPos 부터 startPos 이후에 발견되는 첫 토큰의 바로 앞까지의 문자열을 추출한다.
	 * startPos 이후에 토큰이 존해하지 않는다면, startPos 부터 마지막까지의 문자열을 추출하고, 문자열 마지막에 있는 null 문자의 index를 리턴한다.
	 *
	 * @param delimiter 토큰(문자)
	 * @param startPos 추출 시작 위치 (zero based index)
	 * @return 추출된 문자열 (문자열을 담고 있는 FastString과 동일한 사이즈의 FastString을 리턴)
	 **/
	FastString<MaxSize> split(const char delimiter, size_t startPos = 0) const
	{
		ASSERT(strlen(buf) == bufLen);
		if(startPos >= bufLen)
			return -1;

		FastString<MaxSize> str;
		split(&str, delimiter, startPos);
		return str;
	}

	/// 토큰을 기준으로 한 문자열 추출
	/**
	 * startPos 부터 startPos 이후에 발견되는 첫 토큰의 바로 앞까지의 문자열을 추출한다.
	 * startPos 이후에 토큰이 존해하지 않는다면, startPos 부터 마지막까지의 문자열을 추출하고, 문자열 마지막에 있는 null 문자의 index를 리턴한다.
	 *
	 * @param pDestStr 추출한 문자를 담을 FastString
	 * @param delimiter 토큰(문자열 - null terminated string)
	 * @param startPos 추출 시작 위치 (zero based index)
	 * @return 토큰 위치 (zero based index) 또는 null 문자의 index
	 **/
	template <size_t SrcMaxSize>
	int split(FastString<SrcMaxSize>* pDestStr, const char* delimiter, size_t startPos = 0) const
	{
		ASSERT(strlen(buf) == bufLen);
		pDestStr->clear();

		if(startPos >= bufLen)
			return -1;

		int pos = find(delimiter, startPos);
		if(pos < 0)
		{
			substr(pDestStr, startPos);
			return bufLen;
		}
		else if(pos == startPos)
		{
			pDestStr->clear();
		}
		else
		{
			ASSERT(pos > startPos);
			substr(pDestStr, startPos, pos-startPos);
		}

		return pos;
	}

	/// 토큰을 기준으로 한 문자열 추출
	/**
	 * startPos 부터 startPos 이후에 발견되는 첫 토큰의 바로 앞까지의 문자열을 추출한다.
	 * startPos 이후에 토큰이 존해하지 않는다면, startPos 부터 마지막까지의 문자열을 추출하고, 문자열 마지막에 있는 null 문자의 index를 리턴한다.
	 *
	 * @param delimiter 토큰(문자열 - null terminated string)
	 * @param startPos 추출 시작 위치 (zero based index)
	 * @return 추출된 문자열 (문자열을 담고 있는 FastString과 동일한 사이즈의 FastString을 리턴)
	 **/
	FastString<MaxSize> split(const char* delimiter, size_t startPos = 0) const
	{
		ASSERT(strlen(buf) == bufLen);
		FastString<MaxSize> fastStr;
		split(&fastStr, delimiter, startPos);
		return fastStr;
	}

	/// line 추출
	/**
	 * line 의 기분은 NewLine ("\n") 또는 CarigeReturn + NewLine ("\r\n") 이다.
	 * startPos 부터 가정 먼저 발견되는 "\n"(or "\r\n")까지의 문자열을 추출한다.
	 * 추출된 문자열에는 "\n" 이나 "\r\n"은 포함되어 있지 않다.
	 * startPos 이후에 "\n"(or "\r\n")이 없다면, 문자열 마지막에 있는 null 문자의 index 를 리턴한다.
	 *
	 * @param pDestStr 추출한 문자를 담을 FastString
	 * @param startPos 추출 시작 위치 (zero based index)
	 * @return "\n"의 위치 (zero based index) 또는 null 문자의 index
	 **/
	template <size_t SrcMaxSize>
	int getLine(FastString<SrcMaxSize>* pDestStr, size_t startPos = 0) const
	{
		ASSERT(strlen(buf) == bufLen);
		pDestStr->clear();

		if(startPos >= bufLen)
			return -1;

		int pos = split(pDestStr, '\n', startPos);
		if(pos < 0)
			return -1;

		size_t len = pDestStr->size();
		if(len > 0 && (*pDestStr)[len-1] == '\r')
		{
			pDestStr->erase(len-1, 1);
		}

		return pos;
	}

	/// line 추출
	/**
	 * line 의 기분은 NewLine ("\n") 또는 CarigeReturn + NewLine ("\r\n") 이다.
	 * startPos 부터 가정 먼저 발견되는 "\n"(or "\r\n")까지의 문자열을 추출한다.
	 * 추출된 문자열에는 "\n" 이나 "\r\n"은 포함되어 있지 않다.
	 *
	 * @param pDestStr 추출한 문자를 담을 FastString
	 * @param startPos 추출 시작 위치 (zero based index)
	 * @return 추출된 문자열 (문자열을 담고 있는 FastString과 동일한 사이즈의 FastString을 리턴)
	 **/
	FastString<MaxSize> getLine(size_t startPos = 0) const
	{
		ASSERT(strlen(buf) == bufLen);
		FastString<MaxSize> fastStr;
		getLine(&fastStr, startPos);
		return fastStr;
	}

	/// 문자열을 모두 삭제
	int clear()
	{
		#ifndef NDEBUG
			memset(buf, '#', sizeof(buf));
			buf[MaxSize-1] = '\0';
		#endif

		bufLen = 0;
		buf[0] = '\0';
		/* handol */
		buf[MaxSize-1] = '\0';
		buf[MaxSize-2] = '\0';
		buf[MaxSize-3] = '\0';
		buf[MaxSize-4] = '\0';

		ASSERT(buf[MaxSize-1] == '\0');
		ASSERT(strlen(buf) == bufLen);
		return 0;
	}

	/// 문자열을 standard output으로 출력한다.
	int print() const
	{
		return printf("%s", buf);
	}

	/// 문자열과 NewLine을 standard output으로 출력한다.
	int println() const
	{
		return printf("%s\n", buf);
	}

	bool isEmpty() const
	{
		return bufLen == 0;
	}

private:
	/// 복사
	int copy(size_t destIndex, const char* str, size_t copySize)
	{
		ASSERT(destIndex < MaxSize-1);
		ASSERT(str != NULL);
		ASSERT(copySize > 0);

		int overSize = (destIndex + copySize) - (MaxSize-1);

		if(overSize > 0)
		{
			ASSERT(copySize > (size_t)overSize);
			copySize -= overSize;
		}

		memmove(&buf[destIndex], str, copySize);

		ASSERT(buf[MaxSize-1] == '\0');
		return copySize;
	}

	/// 복사
	int copy(size_t destIndex, size_t srcIndex, size_t copySize)
	{
		ASSERT(destIndex < MaxSize-1);
		ASSERT(srcIndex < MaxSize-1);

		int overSize = (destIndex + copySize) - (MaxSize-1);
		if(overSize > 0)
		{
			ASSERT(copySize > (size_t)overSize);
			copySize -= overSize;
		}

		memmove(&buf[destIndex], &buf[srcIndex], copySize);

		ASSERT(buf[MaxSize-1] == '\0');
		return copySize;
	}

private:
	/* handol : 4 bytes more */
	char buf[MaxSize+4];	///< 문자열 버퍼 (null terminated string)
	size_t bufLen;		///< 문자열 길이
};

#endif

