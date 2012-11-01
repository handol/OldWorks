#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <assert.h>

#include <stdlib.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#include "Util.h"
#include "ace/Thread_Mutex.h"
#include "ace/Guard_T.h"
#include "Mutex.h"
#include "Common.h"

int Util::CreateBillInfoKey(char* destBuf)
{
	ASSERT(destBuf != NULL);

	static PasMutex lock;
	WriteMutex<PasMutex> writeMutex(lock);

	const int HASH_SP_MAX_COUNT = 10000000;
	const int MAX_RAND_NUMBER = 0xffff;
	static int hashCheckCount = HASH_SP_MAX_COUNT;
	static int randomNumber = 0;

	if(hashCheckCount >= HASH_SP_MAX_COUNT) 
	{
		hashCheckCount=0;
		unsigned rtime = (unsigned)time(NULL);
		rand_r(&rtime);
		randomNumber = rand() % (MAX_RAND_NUMBER+1);		
	}

	++hashCheckCount;
	snprintf(destBuf, MaxBillInfoKeySize, "%04X%06X", randomNumber, hashCheckCount);
	destBuf[MaxBillInfoKeySize] = 0;
	return 0;
}

const char* Util::SearchString(const char* src, size_t srcSize, const char* niddle)
{
	const size_t niddleLen = strlen(niddle);
	const char* niddlePtr = niddle;
	const char* srcPtr = src;
	const char* endPtr = src + srcSize;

	while(srcPtr != endPtr)
	{
		if(*srcPtr == *niddlePtr)
		{
			++niddlePtr;
			++srcPtr;

			if(*niddlePtr == '\0')
			{
				return srcPtr - niddleLen;
			}
		}
		else
		{
			if(niddle == niddlePtr)
				++srcPtr;
			else
				niddlePtr = niddle;
		}
	}

	return '\0';
}

/**
한국 국가번호 82 부분은 제외한다. 016, 010 의 맨앞 '0'도 제외한다.
MDN 에서 "82", "820" prefix 를 제외한 값을 return
*/
char	*Util::normalizeMDN(const char *MDN)
{
	char *pos = (char*)MDN;
	if (MDN[0]=='8' && MDN[1]=='2') 
	{
		pos += 2;
	}

	if (pos[0]=='0')
		pos++;
	return pos;
}

/**
주어지  MDN 이 prefix 로 시작하는지 검사.
*/
bool	Util::mdnStartswith(const char *MDN, const char *mdnprefix)
{
	const char *source = normalizeMDN(MDN);
	const char *prefix = normalizeMDN(mdnprefix);
	if (strncmp(source, prefix, strlen(prefix))==0)
		return true;
	else
		return false;
}

/**
전화번호를 INT 형으로 변환한다.
한국 국가번호 82 부분은 제외한다.
016, 010 의 맨앞 '0'도 제외한다.
*/
intMDN_t Util::phonenum2int(const char *phonenumstr)
{
	const char *pos = phonenumstr;
	intMDN_t result = 0;
	if (phonenumstr[0]=='8' && phonenumstr[1]=='2') 
	{
		pos += 2;
	}

	if (pos[0]=='0')
		pos++;

	result = (intMDN_t)strtol(pos, 0, 10);
	return result;
}

/**
문자열 형식의 ip 주소를 INT 형으로 변환한다.
예) 123.100.20.0 --> int
*/
intIP_t Util::ipaddr2int(const char *ipaddrstr)
{
	
	int a,b,c,d; 
	sscanf(ipaddrstr, "%d.%d.%d.%d", &a, &b, &c, &d ); 
	//result = inet_addr(ipaddrstr);
	intIP_t result = a << 24 | b << 16 | c << 8 | d;
	return result;
}

void Util::int2ipaddr(intIP_t intip, char *dest, int destlen)
{
	snprintf(dest, destlen, "%d.%d.%d.%d", 
		intip >> 24,
		(intip >> 16) & 0xFF,
		(intip >> 8) & 0xFF,
		intip & 0xFF);
}


/**
127.100.20.75 등의 IP 주소인지 검사.
*/
bool	Util::isIpAddr(const char *str)
{
	if (str[0]=='\0')
		return false;
		
	while(*str)
	{
		if (*str != '.' &&  ! ISDIGIT(*str))
			return false;
		str++;
	}
	return true;
}
