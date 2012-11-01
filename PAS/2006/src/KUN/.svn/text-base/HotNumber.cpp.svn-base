/** @file  HotNumber.cpp
  * @author 현근창

     @brief VoiceN, WINC, HotNumber, 통합검색 등의 특수 URL 변환.
     
      변환시에 주의할 점은 다음과 같다.
      VoiceN, WINC, HotNumber 등의 숫자와 관련한 서비스에는 숫자만 추출하여 변환된 URL에 포함시킨다.  숫자 문자열 뒤부분의'/'등의 문자는 제외하여야 한다.
     통합검색(한글URL) 서비스를 위해 URL을 변환할 때는 맨 끝의'/'문자를 포함하여야 한다.

*/

#include "HotNumber.h"
#include "Config.h"
#include "CGI.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/**
WINC, Hotnumber 등의 번호값을 구하여 dest 에 저장한다.
*/
int	HotNumber::getHotDigits(const char *src, char *dest, int destMax)
{
	int	i=0;
	while(*src && i < destMax)
	{
		if (*src == '/') break;
		if (*src == 0x0B)
			*dest = '#';
		else
			*dest = *src;

		src++;
		dest++;
		i++;
	}
	
	*dest = 0;
	return i;
}

/**
@brief  **, ## 등이 들어간 특수 URL의 원본값을 받아, 그와 관련된  magicn 사이트의 주소로 변환한다.
@param url
@return 1:true(핫넘버) 0:false(일반url)

변환시에 주의할 점은 다음과 같다.
   VoiceN, WINC, HotNumber 등의 숫자와 관련한 서비스에는 숫자만 추출하여 변환된 URL에 포함시킨다.  숫자 문자열 뒤부분의'/'등의 문자는 제외하여야 한다.
   통합검색(한글URL) 서비스를 위해 URL을 변환할 때는 맨 끝의'/'문자를 포함하여야 한다.
*/

int HotNumber::convert(const char *url, int urllength)
{
	const char *pPos = NULL;
	char oddsharp[3] = { '0x0b', '0x0b', 0 };
	int RetVal = 0;
	
	if( urllength == 0 )
		urllength = strlen(url);

	// 최소한의 데이터를 포함하는지 체크
	if( urllength < 8 )
		return RetVal;

	// 저장해야 할 버퍼보다 데이터가 크면 에러
	if( urllength > MAXLEN_MAGICN_URL )
		return RetVal;

	pPos = &url[7];

	if( pPos[0] == '*' )
	{
		pPos++;

		char	digits[32];

		getHotDigits(pPos, digits, sizeof(digits)-1);		

		// *** 별 3개 핫넘버
		if( strncmp(digits, "**", 2) == 0 )
		{
			snprintf( magicnUrl, sizeof(magicnUrl)-1, "http://hot.k-merce.com/nkey.asp?num=%s", digits );
		}

		// ** 별 2개 핫넘버
		else
		if( strncmp(digits, "*", 1) == 0 )
		{
			snprintf( magicnUrl, sizeof(magicnUrl)-1, "http://hot.k-merce.com/nkey.asp?num=%s", digits );
		}

		// *## 샾 두개 핫넘버 혹은 *[0x0b][0x0b] 핫넘버...단말기 종류에 따라서 다르게 올 수 있다.
		else
		if( !strncmp(digits, "##", 2) || !strncmp(digits, oddsharp, 2) )
		{
			snprintf( magicnUrl, sizeof(magicnUrl)-1, "http://hot.k-merce.com/nkey.asp?num=%s", digits );
		}

		// *# 샾 한개 핫넘버 혹은 *[0x0b] 핫넘버...단말기 종류에 따라서 다르게 올 수 있다.
		else
		if( digits[0] == '#')
		{
			snprintf( magicnUrl, sizeof(magicnUrl)-1, "http://hot.k-merce.com/nkey.asp?num=%s", digits );
		}

		// * 별 한개 핫넘버
		else
		{
			// *0 에 대한 핫넘버 처리
			if( digits[0] == '0' )
			{
				snprintf( magicnUrl, sizeof(magicnUrl)-1, "http://nkey.magicn.com/nkeymain.asp?mobile=%s", digits );

			}
			// *1 ~ 9 에 대한 WINC 핫넘버 처리
			// @todo '#' 기호는 %23 으로 변환.
			else if( digits[0] >= '1' && digits[0] <= '9' )
			{
				char escapedUrl[256];
				CGI::cgiUnescapeChars(escapedUrl, sizeof(escapedUrl)-1, digits, strlen(digits));
				
					snprintf( magicnUrl, sizeof(magicnUrl)-1, "http://ktf.winc.or.kr/hot.asp?winc=%s", digits );
			}

			// 그 외의 경우(예:*aa, *가 등)에는 일반 url 로 처리
			// 단말기에서 핫넘버를 입력 후 다시 지우고 매직엔키를 누르면 PAS에 http://* 과 같이 들어온다.
			// 이 때 기존 PAS 에서는 WINC URL 을 넘겨준다.
			else
			{
				snprintf( magicnUrl, sizeof(magicnUrl)-1, "http://ktf.winc.or.kr" );
			}
		}

		RetVal = 1;
	}

	else	if( pPos[0] == '!' )
	{
		// KTF Search URL - OLD: "http://ktfsearch.magicn.com/search.asp?brtype=go&keyword=%s"
		// KTF Search URL - OLD: "http://ktfsearch.magicn.com/MagicN/KUN/TypeB/list_result_category.asp?search_category=%%C5%%EB%%C7%%D5&search_keyword=%s"

		// keyword 예 )   http://!010%C0%CD/
		
		pPos++;

		char escapedUrl[256];
		//CGI::cgiUnescapeChars(escapedUrl, sizeof(escapedUrl)-1, pPos, strlen(pPos));
		STRNCPY(escapedUrl, pPos, sizeof(escapedUrl)-1);
		
		int urlleng =  strlen(escapedUrl);
		
		if (urlleng > 1 && escapedUrl[urlleng-1] != '/')
		{
			escapedUrl[urlleng] = '/';
			escapedUrl[urlleng+1] = '\0';
		}
		Config *conf = Config::instance();
	
		snprintf( magicnUrl, sizeof(magicnUrl)-1, "%s%s", conf->hotnumber.ktfSearchUrl.toStr(), escapedUrl );

		RetVal = 1;
	}

	else
		return RetVal;

	

	return RetVal;
}

int	HotNumber::test()
{
		HotNumber hotnum;
		typedef struct _hotnumTestCase {
			const char *orgUrl; /** 원본(hotnumber) url */
			const char *magicnUrl; /** 변환된 url. magicN 용 url */
			const char *testCaseName; /* 어떤 테스트 케이스인가. 이름.*/
		} hotnumTestCase;

		hotnumTestCase testcases[] = {
			{ "http://*01030101557", "http://nkey.magicn.com/nkeymain.asp?mobile=01030101557", "voiceN" },
			{ "http://*1004/", "http://ktf.winc.or.kr/hot.asp?winc=1004", "WINC" },
			{ "http://**1004/", "http://hot.k-merce.com/nkey.asp?num=*1004", "HotNumber *"},
			{ "http://***1004/", "http://hot.k-merce.com/nkey.asp?num=**1004", "HotNumber **"},
			{ "http://*#1004/", "http://hot.k-merce.com/nkey.asp?num=#1004", "HotNumber #"},
			{ "http://*##1004/", "http://hot.k-merce.com/nkey.asp?num=##1004", "HotNumber ##"},
			{ "http://!010%30%40%09/", "http://ktfsearch.magicn.com/search.asp?brtype=go&keyword=010%30%40%09", "Hangul 1"},
			// *[0x0b][0x0b] 에 대한 테스트 코드 추가
			// *0 일 때의 테스트 코드 추가
			// *aa 등 일반 url 에 대한 코드 추가
			// * 혹은 ! 이 아닐 때의 테스트 코드 추가
			{ 0, 0, 0 }
		};

		/*
		hotnum.convert("http://*01030101557");
		if (strcmp(hotnum.getConverted(), "http://nkey.magicn.com/nkeymain.asp?mobile=01030101405") != 0)
		{
			printf("HotNumber::test - voiceN convert failed !\n");
		}
		*/

		for (int casenumber=0; casenumber < sizeof(testcases)/sizeof(hotnumTestCase); casenumber++)
		{
			if (testcases[casenumber].orgUrl == 0) break;

			hotnum.convert(testcases[casenumber].orgUrl);

			if (strcmp(hotnum.getConverted(), testcases[casenumber].magicnUrl) == 0)
			{
				printf("SUCC:");
			}
			else
			{
				printf("FAIL:");
			}
			printf(" - %s - %s --> %s\n", testcases[casenumber].testCaseName,
				testcases[casenumber].orgUrl, hotnum.getConverted());
		}

		return 0;
}

#ifdef TEST_MAIN
int main(int argc, char *argv[])
{
	HotNumber hn;
	hn.test();

	return 0;
}
#endif
