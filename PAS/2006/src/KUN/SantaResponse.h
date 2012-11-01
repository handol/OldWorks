#ifndef __SANTA_RESPONSE_H__
#define __SANTA_RESPONSE_H__

#include "SantaTypes.h"

namespace SANTA
{
	class Response
	{
	// 타입선언
	public:
		
	// 멤버함수
	public:
		int parse(const char* str, const int size);
		MDN_t getMDN();
		IMSI_t getIMSI();
		ResponseCode_t getResponseCode();
		ErrorString_t getErrorString();
		bool isOK();

	private:
		void clear();
		int parseItem(const line_t& item);

	// 멥버변수
	private:
		MDN_t mdn;
		IMSI_t imsi;
		ResponseCode_t responseCode;
		ErrorString_t errorString;
	};
}

#endif
