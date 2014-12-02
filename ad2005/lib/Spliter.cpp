/* 
2004.11.2  handol@gmail.com �Ѵ���
�̵���ȭ�� iman messenger �������̸� �̾��ִ� imanGW ������ �����ϴ� �ҽ�.
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
/* �ѱ� ó�� �ÿ� isspace () ����ϸ� ������ �Ǵ� ������ ����. */
#define	ISSPACE(X)	((X)==' ' || (X)=='\t' || (X)=='\n' || (X)=='\r')
#endif

//#define ISSPACE(X)	isspace((X))

int Spliter::getNext(char *outbuf, int maxlen, int separator)
{
// �ϴ� ��:  str�� split�� �� element�� �ϳ��� ����.
// outbuf : �־���  �޸�.  �� �Լ����� ������� buf�� ����.
// maxlen: buf�� �ִ�ũ��
// return: ������� ����. 0 �� �ɼ��� �ִ�.
// return -1: next�� ���� ���. �� ó���� ���.
	char *found=0;
	int outlen=0;

	if (_is_end  || _i >= _orglen)
	{
		_is_end = 1;
		if (outbuf)	outbuf[0] = 0;
		return -1;
	}

	if (separator==' ') {
	/*  space�� ��� : python�� split()�� ����. ����� ���ڿ��� return	*/
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
	/* ������ separator �������� ���ڿ� return */
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
separator ���ڰ� ���� ���� ��� ó��.
@return �߰ߵ� separator ����
@return -1 if finished

// �ϴ� ��:  str�� split�� �� element�� �ϳ��� ����.
// outbuf : �־���  �޸�.  �� �Լ����� ������� buf�� ����.
// maxlen: buf�� �ִ�ũ��
// return -1: next�� ���� ���. �� ó���� ���.
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
iman message �Ľ��� ����.
�Ʒ��� ���� ��� '%261%26'�� skip�ϱ� ����.

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
	/* %261%26 �߿� ���� %�� ���� �����̴�.
	���������� 6����Ʈ ����.
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
	int	outlen; // ���� ���� ����
	int	copylen;  // ���� ���� ������ ����
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
