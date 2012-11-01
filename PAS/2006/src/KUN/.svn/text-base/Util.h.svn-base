#ifndef __UTIL_H__
#define __UTIL_H__

/**
@brief 각종 유틸리

BillInfo Key 생성 및 빈번히 사용되는 간단한 함수들 클래스
맴버 함수는 모두 static 으로 되어 있으므로 인스턴스 생성 없이 사용 가능하다.

주요 기능은 아래와 같다.
BillInfo Key 생성
문자열 검색
특정 번호로 시작하는 MDN 찾기
폰번호를 int 형으로 변환
int 형 ip를 char 형으로 변환
char 형 ip를 int 형으로 변환
기타
*/

#include "Common.h"

class Util
{
public:
	

/// Bill Info Key 생성
/**
 * 함수 내부에서 PasMutex를 사용한다.
 * 함수를 실행 할때 마다 1씩 증가하는 sequnce 넘버를 10자리 16진수 문자열로 생성한다.
 *
 * @param destBuf key를 기록할 버퍼, 저장공간은 11바이트 이상(BillInfoKey 10bytes + null 1byte)
 * @return 항상 0
 **/
static	int CreateBillInfoKey(char* destBuf);

/// 문자열 검색하기
/** 
 * @note src는 null-terminated string 이 아니여도 무방하다.
 *
 * @param src 검색 대상 문자열
 * @param srcSize 검색 대상 문자열 사이즈
 * @param niddle 찾을 문자열(null-terminated string)
 * @return 문자열을 찾았다면 해당 문자열의 포인터를 리턴하고, 못 찾았다면 NULL을 리턴한다.
 **/
static	const char* SearchString(const char* src, size_t srcSize, const char* niddle);

static	char *normalizeMDN(const char *MDN);

static	bool mdnStartswith(const char *MDN, const char *mdnprefix);

static	intMDN_t phonenum2int(const char *phonenumstr);

static	intIP_t ipaddr2int(const char *ipaddrstr);

static	void int2ipaddr(intIP_t intip, char *dest, int destlen);

static	bool isIpAddr(const char *str);
};

#endif
