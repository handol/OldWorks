#ifndef __FAST_STRING_HPP__
#define __FAST_STRING_HPP__

#include <assert.h>
#include <string.h>
#include <stdarg.h>
#include <vector>
#include <algorithm>

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
		#ifndef NDEBUG
			initForDebug();
		#endif

		clear();
	}

	FastString(const char* str, const size_t size)
	{
		#ifndef NDEBUG
			initForDebug();
		#endif

		assign(str, size);
	}

	FastString(const char* str)
	{
		#ifndef NDEBUG
			initForDebug();
		#endif

		assign(str);
	}

	explicit FastString(const char value)
	{
		#ifndef NDEBUG
			initForDebug();
		#endif

		assign(value);
	}

	explicit FastString(const int value)
	{
		#ifndef NDEBUG
			initForDebug();
		#endif

		assign(value);
	}

	explicit FastString(const unsigned int value)
	{
		#ifndef NDEBUG
			initForDebug();
		#endif

		assign(value);
	}

	explicit FastString(const int64_t value)
	{
		#ifndef NDEBUG
			initForDebug();
		#endif

		assign(value);
	}

	explicit FastString(const float value)
	{
		#ifndef NDEBUG
			initForDebug();
		#endif

		assign(value);
	}

	explicit FastString(const double value)
	{
		#ifndef NDEBUG
			initForDebug();
		#endif

		assign(value);
	}

	// FastString(const FastString<SrcMaxSize>& str)가 같은 역할을 할 수 있는 범용적인
	// 함수이지만 SunCC의 버그로 인해 이 함수를 추가해야 한다.
	FastString(const FastString<MaxSize>& str)
	{
		#ifndef NDEBUG
			initForDebug();
		#endif

		assign(str);
	}

	template<size_t SrcMaxSize>
	FastString(const FastString<SrcMaxSize>& str)
	{
		#ifndef NDEBUG
			initForDebug();
		#endif

		assign(str);
	}

	~FastString()
	{
		assert(valid());
	}

	char firstCh()
	{
		return buf[0];
	}

	/// 특정 위치의 문자 반환
	const char& operator [] (int index) const
	{
		assert(0 <= index);
		assert(index < static_cast<int>(bufLen));

		return buf[index];
	}

	/// 특정 위치의 문자 반환
	char& operator [] (int index)
	{
		assert(0 <= index);
		assert(index < static_cast<int>(bufLen));

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
	FastString<MaxSize>& operator = (const T1& rhs)
	{
		assign(rhs);
		return *this;
	}

	// FastString<MaxSize>& operator = (const T1& rhs)가 같은 역할을 할 수 있는 범용적인
	// 함수이지만 SunCC의 버그로 인해 이 함수를 추가해야 한다.
	FastString<MaxSize>& operator = (const FastString<MaxSize>& rhs)
	{
		assign(rhs);
		return *this;
	}

	/// 문자열 리턴 (null terminated string)
	operator const char* () const
	{
		return buf;
	}

	/// 문자열 셋팅
	int assign(const char* str)
	{
		clear();
		append(str);
		return bufLen;
	}

	/// 문자열 셋팅
	int assign(const char* str, int size)
	{
		clear();
		append(str, size);
		return bufLen;
	}

	/// 문자열 셋팅
	int assign(const char value)
	{
		clear();
		append(value);
		return bufLen;
	}

	/// 문자열 셋팅
	int assign(const int value)
	{
		clear();
		append(value);
		return bufLen;
	}

	/// 문자열 셋팅
	int assign(const unsigned int value)
	{
		clear();
		append(value);
		return bufLen;
	}

	/// 문자열 셋팅
	int assign(const int64_t value)
	{
		clear();
		append(value);
		return bufLen;
	}

	/// 문자열 셋팅
	int assign(const float value)
	{
		clear();
		append(value);
		return bufLen;
	}

	/// 문자열 셋팅
	int assign(const double value)
	{
		clear();
		append(value);
		return bufLen;
	}

	/// 문자열 셋팅
	template<size_t SrcMaxSize>
	int assign(const FastString<SrcMaxSize>& str)
	{
		clear();
		append(str);
		return bufLen;
	}

	/// 문자열 리턴 (null terminated string)
	const char* toStr() const
	{
		return buf;
	}

	/// 문자열 리턴 (null terminated string)
	const char* c_str() const
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
			// moveSize + nullChar
			memmove(buf, &buf[n], (bufLen-n)+1);

			bufLen -= n;
		}

		assert(valid());
		
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

		assert(valid());
		
		return bufLen;
	}

	/// 문자열 최대 사이즈
	static inline size_t maxSize()
	{
		return MaxSize;
	}

	/// 문자열 길이
	inline size_t size() const
	{
		return bufLen;
	}

	/// 남은 사이즈
	inline size_t freeSize() const
	{
		return MaxSize - bufLen;
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
		if(bufLen < index)
			index = bufLen;

		if(MaxSize <= index)
			return bufLen;

		int rightStringLen = bufLen - index;

		// rightStringLen + nullChar
		copy(index+1, index, rightStringLen+1);

		buf[index] = ch;

		if(bufLen < MaxSize)
			++bufLen;

		assert(valid());

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
		assert(size <= strlen(str));

		if(str == NULL || size == 0)
			return bufLen;

		if(bufLen < index)
			index = bufLen;

		if(MaxSize <= index)
			return bufLen;

		int rightStringLen = bufLen - index;
		size_t moveDestIndex = index+size;

		if(moveDestIndex < MaxSize)
		{
			// rightStringLen + nullChar
			copy(moveDestIndex, index, rightStringLen+1);
		}

		int copiedSize = copy(index, str, size);

		bufLen = std::min(bufLen+copiedSize, MaxSize);

		assert(valid());

		return bufLen;
	}

	int insert(size_t index, const char* str)
	{
		return insert(index, str, strlen(str));
	}

	/// 문자열 마지막에 문자 붙이기
	/**
	 * @return 문자열 길이
	 *
	 * @seealso insert
	 **/
	int append(const char ch)
	{
		return insert(bufLen, ch);
	}

	/// 문자열 마지막에 문자열 붙이기
	/**
	  * @return 문자열 길이
	 *
	 * @seealso insert
	 **/
	int append(const char* str, size_t size)
	{
		return insert(bufLen, str, size);
	}

	/// 문자열 마지막에 문자열 붙이기
	/**
	* @return 문자열 길이
	*
	* @seealso insert
	**/
	int append(const char* str)
	{
		return insert(bufLen, str);
	}

	/// 문자열 마지막에 변수 붙이기
	/**
	 * @return 문자열 길이
	 **/
	int append(const int val)
	{
		char str[32];
		snprintf(str, sizeof(str), "%d", val);
		return append(str);
	}

	/// 문자열 마지막에 변수 붙이기
	/**
	 * @return 문자열 길이
	 **/
	int append(const unsigned int val)
	{
		char str[32];
		snprintf(str, sizeof(str), "%u", val);
		return append(str);
	}

	/// 문자열 마지막에 변수 붙이기
	/**
	* @return 문자열 길이
	**/
	int append(const int64_t val)
	{
		char str[32];
		snprintf(str, sizeof(str), "%lld", val);
		return append(str);
	}

	/// 문자열 마지막에 변수 붙이기
	/**
	 * @return 문자열 길이
	 **/
	int append(const float val)
	{
		char str[32];
		snprintf(str, sizeof(str), "%f", val);
		return append(str);
	}

	/// 문자열 마지막에 변수 붙이기
	/**
	 * @return 문자열 길이
	 **/
	int append(const double val)
	{
		char str[32];
		snprintf(str, sizeof(str), "%lf", val);
		return append(str);
	}

	/// 문자열 마지막에 FastString 붙이기
	/**
	 * @return 문자열 길이
	 **/
	template<size_t SrcMaxSize>
	int append(const FastString<SrcMaxSize>& value)
	{
		return append(value.c_str(), value.size());
	}

	/// 문자열 삭제
	/**
	 * @param index 삭제할 문자열의 위치 (zero based index)
	 * @param size 삭제할 문자열 길이
	 * @return 문자열 길이
	 **/
	int erase(size_t index, size_t size = 0xffffffff)
	{
		if(bufLen <= index)
			return bufLen;

		size_t rightStringLen = bufLen - index;
		if(size >= rightStringLen)
		{
			buf[index] = '\0';
			bufLen = index;
		}
		else
		{
			// + nullChar
			copy(index, index+size, rightStringLen+1);
			bufLen -= size;
		}

		assert(valid());
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
		int newBufLen = vsnprintf(buf, MaxSize+1, fmt, args);
		va_end(args);

		if(newBufLen < 0)
		{
			bufLen = 0;
			buf[0] = '\0';
		}
		else
		{
			bufLen = std::min(MaxSize, static_cast<size_t>(newBufLen));
		}

		assert(valid());

		return bufLen;
	}

	int append_sprintf(const char *fmt, ...)
	{
		char newString[MaxSize+1];

		va_list args;
		va_start(args, fmt);
		vsnprintf(newString, MaxSize, fmt, args);
		va_end(args);

		return append(newString);;
	}

	/// 문자열 추출
	/**
	 * @param index 추출한 문자열 위치(zero based index)
	 * @param size 추출할 문자열 길이
	 * @return 추출된 문자열 (문자열을 담고 있는 FastString과 동일한 사이즈의 FastString을 리턴)
	 **/
	FastString<MaxSize> substr(size_t index, size_t size = 0xffffffffU) const
	{
		FastString<MaxSize> str;
		if(index >= bufLen)
			return str;

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
	int substr(FastString<SrcMaxSize>* pDestStr, size_t index, size_t size = 0xffffffffU) const
	{
		pDestStr->clear();
		if(index >= bufLen)
			return -1;

		size_t substrSize = std::min(bufLen - index, size);
		pDestStr->append(&buf[index], substrSize);
		return pDestStr->size();
	}

	template <size_t SrcMaxSize>
	int substr(FastString<SrcMaxSize>& destStr, size_t index, size_t size = 0xffffffffU) const
	{
		return substr(&destStr, index, size);
	}

	/// 문자 찾기
	/**
	 * @param ch 찾을 문자
	 * @return 문자 시작 위치(zero based index), 없으면 -1
	 **/
	int find(const char ch, size_t startPos = 0) const
	{
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
		if(bufLen <= startPos)
			return -1;

		const char* pPos = strstr(&buf[startPos], str);

		if(pPos == NULL)
			return -1;

		int findPos = (int)(pPos - buf);

		return findPos;
	}

	/// 대소문자를 구분하지 않는 문자열 찾기
	/**
	* @param str 찾을 문자열 문자
	* @return 문자열 시작 위치(zero based index), 없으면 -1
	**/
	int incaseFind(const char* str, size_t startPos = 0) const
	{
		if(bufLen <= startPos)
			return -1;

		const char* pPos = strcasestr(&buf[startPos], str);

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
		pDestStr->clear();

		if(bufLen <= startPos)
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
			assert(pos > (int)startPos);
			substr(pDestStr, startPos, pos-startPos);
		}

		return pos;
	}

	template <size_t SrcMaxSize>
	int split(FastString<SrcMaxSize>& destStr, const char delimiter, size_t startPos = 0) const
	{
		return split(&destStr, delimiter, startPos);
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
		FastString<MaxSize> str;

		if(startPos >= bufLen)
			return str;

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
		pDestStr->clear();

		if(startPos >= bufLen)
			return -1;

		int pos = find(delimiter, startPos);
		if(pos < 0)
		{
			substr(pDestStr, startPos);
			return bufLen;
		}
		else if(pos == static_cast<int>(startPos))
		{
			pDestStr->clear();
		}
		else
		{
			assert(pos > (int)startPos);
			substr(pDestStr, startPos, pos-startPos);
		}

		return pos;
	}

	template <size_t SrcMaxSize>
	int split(FastString<SrcMaxSize>& destStr, const char* delimiter, size_t startPos = 0) const
	{
		return split(&destStr, delimiter, startPos);
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
		pDestStr->clear();

		if(startPos >= bufLen)
			return -1;

		int pos = split(pDestStr, '\n', startPos);
		if(pos < 0)
			return -1;

		size_t len = pDestStr->size();
		if(len > 0)
		{
			if((*pDestStr)[len-1] == '\r')
			{
				pDestStr->erase(len-1, 1);
			}
		}

		return pos;
	}

	template <size_t SrcMaxSize>
	int getLine(FastString<SrcMaxSize>& destStr, size_t startPos = 0) const
	{
		return getLine(&destStr, startPos);
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
		FastString<MaxSize> fastStr;
		getLine(&fastStr, startPos);
		return fastStr;
	}

	/// 문자열을 모두 삭제
	int clear()
	{
		bufLen = 0;
		buf[0] = '\0';
		
		assert(valid());
		return 0;
	}

	/// 문자열 치환
	void replace(const char* niddle, const char* changeTo)
	{
		int pos = find(niddle);
		if(pos < 0)
			return;

		FastString<MaxSize> destStr;

		int begin = 0;
		int end = pos;
		int lenNiddle = strlen(niddle);

		while(end >= 0)
		{
			destStr.append(&buf[begin], end-begin);
			destStr.append(changeTo);
			begin = end+lenNiddle;
			end = find(niddle, begin);
		}

		// 뒤에 남은 부분 붙여 넣기
		if(begin < static_cast<int>(size()))
		{
			destStr.append(&buf[begin], size()-begin);
		}

		assign(destStr);
	}

	/// 문자열을 standard output으로 출력한다.
	inline int print() const
	{
		return printf("%s", buf);
	}

	/// 문자열과 NewLine을 standard output으로 출력한다.
	inline int println() const
	{
		return printf("%s\n", buf);
	}

	inline bool isEmpty() const
	{
		return bufLen == 0;
	}

	inline const char* front() const
	{
		return buf;
	}

	inline char* front()
	{
		return buf;
	}

private:
	/// 복사
	int copy(size_t destIndex, const char* str, size_t copySize)
	{
		int overSize = static_cast<int>(destIndex + copySize) - static_cast<int>(MaxSize);

		if(overSize > 0)
		{
			copySize -= overSize;
		}

		memmove(&buf[destIndex], str, copySize);
		
		return copySize;
	}

	/// 복사
	int copy(size_t destIndex, size_t srcIndex, size_t copySize)
	{
		int overSize = static_cast<int>(destIndex + copySize) - static_cast<int>(MaxSize);
		if(overSize > 0)
		{
			copySize -= overSize;
		}

		memmove(&buf[destIndex], &buf[srcIndex], copySize);

		return copySize;
	}

	bool valid()
	{
		#ifdef NDEBUG
			return strlen(buf) == bufLen;
		#else
			return (buf[MaxSize] == '\0' && strlen(buf) == bufLen);
		#endif
	}

	void initForDebug()
	{
		memset(buf, '#', sizeof(buf));
		buf[MaxSize] = '\0';
	}

	// 대소문자를 구분하지 않는 문자열 찾기
	const char * strcasestr (const char *haystack, const char *needle) const
	{
		const char *p, *startn = 0, *np = 0;

		for (p = haystack; *p; p++) {
			if (np) {
				if (toupper(*p) == toupper(*np)) {
					if (!*++np)
						return startn;
				} else
					np = 0;
			} else if (toupper(*p) == toupper(*needle)) {
				np = needle + 1;
				startn = p;
			}	
		}

		return 0;
	}


private:
	char buf[MaxSize+1];///< 문자열 버퍼 (null terminated string)
	size_t bufLen;		///< 문자열 길이
};

template <typename FASTSTRING>
std::vector<FASTSTRING> explode(const FASTSTRING& str, const char delimiter)
{
	std::vector<FASTSTRING> strings;

	FASTSTRING tmpStr;
	int pos = -1;
	while(true)
	{
		pos = str.split(&tmpStr, delimiter, pos+1);
		if(pos < 0)
			return strings;

		strings.push_back(tmpStr);
	}
}

#endif

