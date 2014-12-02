/* 
2004.11.2  handol@gmail.com 한대희
이동전화와 iman messenger 서버사이를 이어주는 imanGW 서버를 구현하는 소스.
*/
/* string  split iterator.
	splits a given string with a given separator, and produce each element per iteration.
*/

#include "Spliter.h"
#include <string.h>
#include <ctype.h>

#ifndef MIN
#define MIN(A, B) ((A)<(B))?(A):(B)
#endif

#ifndef ISSPACE
/* 한글 처리 시에 isspace () 사용하면 문제가 되는 것으로 보임. */
#define	ISSPACE(X)	((X)==' ' || (X)=='\t' || (X)=='\n' || (X)=='\r')
#endif

//#define ISSPACE(X)	isspace((X))

int Spliter::getNext(char *outbuf, int maxlen, int separator)
{
// 하는 일:  str을 split한 각 element를 하나씩 생산.
// outbuf : 주어진  메모리.  이 함수에서 결과물을 buf에 저장.
// maxlen: buf의 최대크기
// return: 결과물의 길이. 0 이 될수도 있다.
// return -1: next가 없는 경우. 다 처리한 경우.
	char *found=0;
	int outlen=0;

	if (_is_end  || _i >= _orglen)
	{
		_is_end = 1;
		if (outbuf)	outbuf[0] = 0;
		return -1;
	}

	if (separator==' ') {
	/*  space인 경우 : python의 split()와 유사. 비공백 문자열을 return	*/
		skipSpace();
		if (_ptr==0) {
			if (outbuf)	outbuf[0] = 0;
			_is_end = 1;
			return -1;			
		}

		while(! ISSPACE(*_ptr) && *_ptr && outlen < maxlen) {
			*outbuf++ = *_ptr++;
			_i++;
			outlen++;
		}
		*outbuf = 0;
		return outlen;
		
	}

	else {
	/* 지정된 separator 전까지의 문자열 return */
		//skipSpace(); //-- 2004.12.22

		found = strchr(_ptr, separator);
		if (found)
		{
			outlen = (unsigned int)found - (unsigned int)_ptr;
		}
		else
		{
			outlen = _orglen - _i;
		}

		if (outbuf) {
			memcpy(outbuf, _ptr, MIN(outlen, maxlen) );
			outbuf[outlen] = 0;
		}

		_i += outlen + 1;
		_ptr += outlen + 1;
		return outlen;
	}
	
}

/**
separator 문자가 여러 개인 경우 처리.
@return 발견된 separator 문자
@return -1 if finished

// 하는 일:  str을 split한 각 element를 하나씩 생산.
// outbuf : 주어진  메모리.  이 함수에서 결과물을 buf에 저장.
// maxlen: buf의 최대크기
// return -1: next가 없는 경우. 다 처리한 경우.
*/
int Spliter::getNext(char *outbuf, int maxlen, char *separators)
{

	char *found=0;
	int outlen=0;
	char	found_sep=0;
	if (_is_end  || _i >= _orglen)
	{
		_is_end = 1;
		if (outbuf)	outbuf[0] = 0;
		return -1;
	}

	found = strpbrk(_ptr, separators);
	if (found)
	{
		found_sep = *found;
		outlen = (unsigned int)found - (unsigned int)_ptr;
	}
	else
	{
		found_sep = 0;
		outlen = _orglen - _i;
	}

	if (outbuf) {
		memcpy(outbuf, _ptr, MIN(outlen, maxlen) );
		outbuf[outlen] = 0;
	}

	_i += outlen + 1;
	_ptr += outlen + 1;

	return found_sep;
}

int Spliter::skipSpace()
{
	while( ISSPACE(*_ptr) ) {
	//while(*_ptr==' ' || *_ptr=='\t') {
		_ptr++;
		_i++;
	}
	return _i;
}

/**
iman message 파싱을 위해.
아래와 같은 경우 '%261%26'을 skip하기 위해.

noteField = buddyId|memo|mobileNo|telNo%261%26easysong@hitel.net|
*/
int Spliter::skipIMANspecial()
{
	int	i=0;
	/*
	while(isdigit(*_ptr) || *_ptr=='%') {
		_ptr++;
		i++;
	}
	*/
	/* %261%26 중에 앞의 %를 만난 상태이다.
	고정적으로 6바이트 전진.
	*/
	if (_i + 6 <=  _orglen) {
		_i += 6;
		_ptr += 6;
		i = 6;
	}
	else {
		_ptr += (_orglen - _i);
		_i = _orglen;		
	}
	return i;
}

int Spliter::getLeftover(char *outbuf, int maxlen)
{
	int	outlen; // 현재 남은 길이
	int	copylen;  // 실제 복사 가능한 길이
	outlen = _orglen - _i;
	copylen = MIN(outlen, maxlen);
	memcpy(outbuf, _ptr, copylen);
	outbuf[copylen] = 0;
	return copylen;
}


int	Spliter::trim(char *dest, char *src)
{
	int	n=0;
	#if 0
	char tmp[128];

	if (src==dest) {
		strncpy(tmp, src, 127);
		tmp[127] = 0;
		src = tmp;
	}
	#endif

	while(*src) {
		if (! ISSPACE(*src) ) {
			*dest++ = *src;
			n++;
		}
		src++;
	}
	*dest = 0;
	return 0;
}
