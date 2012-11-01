#ifndef HASHKEY_H
#define HASHKEY_H

#include "Common.h"
#include "MyLog.h"

class HashKey
{
public:
	
/**
@brief Hash Key 생성

 * @param pDest key를 기록할 버퍼, 저장공간은 17바이트 이상(HashKey 16bytes + null 1byte)
 * @param pSrcString HashKey를 생성할 때 참고할 문자열 (null terminated string, 길이 제한 없음)
 * @return 정상 0, 에러 -1
*/
	static	void	prepare();
	static	void	writeLog(char *mdn, char *phoneIp, const char *hashkey, const char*url, int reqsize, int respsize);
	static	int	getKtfHashKey(char *pDest, const char *pSrcString);
	static	MyLog* hashkeylog;
};

#endif
