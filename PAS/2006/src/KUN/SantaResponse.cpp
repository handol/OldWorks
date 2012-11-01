#include "PasLog.h"
#include "SantaTypes.h"
#include "SantaResponse.h"

using namespace SANTA;

int Response::parse(const char* str, const int size)
{
	// source := "item&item&item&...."
	// item   := "leftPart=rightPart"

	ACE_ASSERT(str != NULL);
	ACE_ASSERT(size > 0);

	clear();

	line_t source(str, size);
	line_t item;
	int delimPos = 0;	
	while(true)
	{
		// item 추출
		delimPos = source.split(&item, '&', delimPos);
		if(delimPos < 0 )
			break;

		delimPos++;

		item.trim();
		if(item.isEmpty())
			continue;

		// item 파싱
		parseItem(item);
	}

	return 0;
}

int Response::parseItem(const line_t& item)
{
	// item := "leftPart=rightPart"

	// leftPart 추출
	line_t leftPart;
	int delimPos = item.split(&leftPart, '=');
	leftPart.trim();
	if(leftPart.isEmpty())
	{
		PAS_ERROR1("Response::parseItem >> leftPart가 없음, item[%s]", item.toStr());
		return -1;
	}
	
	// rightPart 추출
	line_t rightPart;
	if(delimPos+1 < static_cast<int>(item.size()))
	{
		item.substr(&rightPart, delimPos+1);
		rightPart.trim();
	}

	// rightPart가 없다면
	else
	{
		// rightPart는 그냥 공백
	}

	PAS_DEBUG2( "Response::parseItem >> Left Part[%s] Right Part[%s]", leftPart.toStr(), rightPart.toStr() );

	// RESPONSE
	if(leftPart.incaseEqual("RESPONSE"))
	{
		responseCode = rightPart;
	}

	// IMSI
	else if(leftPart.incaseEqual("IMSI"))
	{
		imsi = rightPart;
	}

	// MDN
	else if(leftPart.incaseEqual("MDN"))
	{
		mdn = rightPart;
	}

	// ERROR_STRING
	else if(leftPart.incaseEqual("ERROR_STRING"))
	{
		errorString = rightPart;
	}

	// Unknown
	else
	{
		PAS_ERROR1("Response::parseItem >> 파싱실패, item[%s]", item.toStr());
		return -1;
	}

	return 0;
}

MDN_t Response::getMDN()
{
	return mdn;
}

IMSI_t Response::getIMSI()
{
	return imsi;
}

ResponseCode_t Response::getResponseCode()
{
	return responseCode;
}

ErrorString_t Response::getErrorString()
{
	return errorString;
}

void Response::clear()
{
	mdn.clear();
	imsi.clear();
	responseCode.clear();
	errorString.clear();
}

/**
@brief
SANTA 에 요청한 결과가 제대로 왔는지 체크
한가지 주의할 점은, 인증의 성공/실패 의미가 아니라는 것
*/
bool Response::isOK()
{
	return responseCode.incaseEqual( "s" );
}
