#include <strings.h>
#include <stdlib.h>
#include "SisorProtocol.h"

SIP::CHK_RESULT SIP::checkStartToken(const ACE_Message_Block& src)
{
	// data length check
	if(src.length() < 4)
		return CR_NOT_ENOUGH;

	// existence check of start token
	if(strncmp(src.rd_ptr(), "SOX ", 4) == 0)
		return CR_OK;

	return CR_BAD;
}

SIP::CHK_RESULT SIP::checkLength(const ACE_Message_Block& src)
{
	// data length check
	CHK_RESULT startTokenChkRes = checkStartToken(src);
	if(startTokenChkRes != CR_OK)
		return startTokenChkRes;

	if(src.length() < 8)
		return CR_NOT_ENOUGH;

	// length format check
	const char* strLength = src.rd_ptr() + 4;
	
	// 숫자가 아니면 에러
	if(strLength[0] != ' ' && (strLength[0] < '0' || '9' < strLength[0]))
		return CR_BAD;

	if(strLength[1] != ' ' && (strLength[1] < '0' || '9' < strLength[1]))
		return CR_BAD;

	if(strLength[2] != ' ' && (strLength[2] < '0' || '9' < strLength[2]))
		return CR_BAD;

	if(strLength[3] < '0' || '9' < strLength[3])
		return CR_BAD;

	return CR_OK;
}

int SIP::getBodyLength(const ACE_Message_Block& src)
{
	// data length check
	if(checkLength(src) != CR_OK)
		return -1;

	// extract length from src
	char str[5];
	memcpy(str, (src.rd_ptr()+4), 4);
	str[4] = '\0';
	
	// convert length string to integer
	int len = atoi(str);
	if(len <= 0) 
		return -1;

	return len;
}

int SIP::getTotalLength(const ACE_Message_Block& src)
{
	int bodyLen = getBodyLength(src);
	if(bodyLen < 0)
		return -1;

	return 4 + 4 + bodyLen + 4;
}

SIP::CHK_RESULT SIP::checkBody(const ACE_Message_Block& src)
{
	// data length check
	CHK_RESULT lenChkRes = checkLength(src);
	if(lenChkRes != CR_OK)
		return lenChkRes;

	int bodyLen = getBodyLength(src);
	
	if(static_cast<int>(src.length()) < (4 + 4 + bodyLen))
		return CR_NOT_ENOUGH;

	return CR_OK;
}

SIP::CHK_RESULT SIP::checkEndToken(const ACE_Message_Block& src)
{
	// data length check
	CHK_RESULT lenChkRes = checkBody(src);
	if(lenChkRes != CR_OK)
		return lenChkRes;

	int totalLen = getTotalLength(src);
	if(totalLen < 0)
		return CR_BAD;

	if(static_cast<int>(src.length()) < totalLen)
		return CR_NOT_ENOUGH;

	// existence check of start token
	const char* strEnd = src.rd_ptr() + totalLen - 4;
	
	if(strncmp(strEnd, " EOX", 4) == 0)
		return CR_OK;

	return CR_BAD;
}

int SIP::getBody(ACE_Message_Block& dest, const ACE_Message_Block& src)
{
	// data length check
	int bodyLen = getBodyLength(src);
	if(bodyLen < 0)
		return -1;

	int totalLen = getTotalLength(src);
	if(totalLen < 0)
		return -1;

	if(static_cast<int>(src.length()) < totalLen)
		return -1;

	// body copy from src to dest
	if(static_cast<int>(dest.size()) < bodyLen)
		dest.size(bodyLen);

	dest.reset();
	int ret = dest.copy(src.rd_ptr()+8, bodyLen);
	if(ret < 0) 
		return -1;

	return 0;
}


int SIP::encode(ACE_Message_Block& dest, const ACE_Message_Block& src)
{
	int destBufSize = src.length() + 4 + 4 + 4;

	dest.reset();
	if(static_cast<int>(dest.size()) < destBufSize)
		dest.size(destBufSize);

	dest.copy("SOX ", 4);

	sprintf(dest.wr_ptr(), "%4d", src.length());
	dest.wr_ptr(4);

	dest.copy(src.rd_ptr(), src.length());
	dest.copy(" EOX", 4);

	return 0;
}


int SIP::encode(char* dest, int destSize, const char* src, int srcSize)
{
	if(src == NULL || srcSize <= 0 || dest == NULL || destSize <= 0)
		return -1;

	int resultSize = srcSize + 4 + 4 + 4;

	if(destSize < resultSize)
		return -1;

	memcpy(dest, "SOX ", 4);
	sprintf(&dest[4], "%4d", srcSize);
	memcpy(&dest[8], src, srcSize);
	memcpy(&dest[8+srcSize], " EOX", 4);

	return resultSize;
}


