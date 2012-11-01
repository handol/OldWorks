#include "KtfInfo.h"
#include <string.h>
#include <strings.h>


/**
브라우져 타입 체크.  setTransactionInfo() 이후에 호출해야 한다.
*/
KtfBrowserType	KtfInfo::checkBrowserType(const char *browser)
{
	if (strstr(browser, "MSMB")!=NULL || strstr(browser, "PIM")!=NULL ) {
		return BROWSER_TYPE_ME;
	}
	// color phone : MSMB12C

	if ( strstr(browser, "KUN")!=NULL) {
		return BROWSER_TYPE_KUN;
	}

	return BROWSER_TYPE_OTHER;
}

bool KtfInfo::isNewBrowser(const char *browser)
{
	/// 캐싱 하면 안되는 브라우저 판별
	/// 캐싱 플랙이 1이 되면, Auth 서버에서 해당 단말기에 대해서 캐싱을 하게되고
	/// 다음 인증부터 캐싱 데이터를 참조하여 인증 절차를 건너뛴다.
	/// 단말기의 IP가 변경된 경우에는 다시 인증을 거쳐야한다.
	if( strstr(browser, "MSMB101") || strstr(browser, "MSMB11") ||
		strstr(browser, "MSMB12") || strstr(browser, "MSMB13") ||
		strstr(browser, "MSMB14") || strstr(browser, "PIM3.0") ||
		strstr(browser, "KUN") )
		return true;

	return false;
}


/**
가상번호 판단
CPhoneTran::GetNumberType()
{
	if (016-1xx)
		VIRTUAL_NUMBER_016_2G
	if (016-90xx, 016-91xx)
		VIRTUAL_NUMBER_016_1X
	
	if (018-1xx, 018-9xx)
		VIRTUAL_NUMBER_018_1ST
		
	otherwise
		REAL_NUMBER_XXX
}
*/
bool KtfInfo::isVirtualNumber(const char *_phoneNumber)
{
	// 핸드폰 번호로 접속한 망 정보 알아내기
	// HTTP_PHONE_NUMBER: 820162010022
	char * phoneNumber = (char*)_phoneNumber;

	// '82' 가 앞에 있든 없든 처리 가능해야 한다.
	if (phoneNumber[0]=='8' || phoneNumber[1]=='2')
		phoneNumber += 2;

	// '82'가 앞에 있는 경우 '016' 에서 '0' 은 생략될 수 있다.
	if (phoneNumber[0]=='0')
		phoneNumber++;

	if( !strncmp("16", phoneNumber, 2))
	{
		// 국 번호 첫째 자리가 1 혹은 9 로 시작하면 SANTA 조회 해야함
		// 1xx 국번은 2G망, 90x 91x 국번은 1x망
		if( phoneNumber[2] == '1')
		{	
			return true;
		}
		else if (phoneNumber[2] == '9' )
		{
			if( phoneNumber[3] == '0' || phoneNumber[3] == '1' )
				return true;
		}
	}
	else if(!strncmp("18", phoneNumber, 2) )
	{
		// 국 번호 첫째 자리가 1 혹은 9 로 시작하면 SANTA 조회 해야함
		// 1xx 국번은 2G망, 90x 91x 국번은 1x망
		if( phoneNumber[2] == '1')
		{	
			return true;
		}
		else if (phoneNumber[2] == '9' )
		{
			return true;
		}
	}

	return false;
}

