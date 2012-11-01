#include "HashKey.h"
#include "Config.h"

MyLog *HashKey::hashkeylog = NULL;

void HashKey::prepare()
{
	if (Config::instance()->process.HashKeyLog == true)
	{
		filename_t filename = Config::instance()->getLogPrefix();
		filename += "hashkey";
		hashkeylog = new MyLog();
		hashkeylog->openWithYear("./", filename);
	}
}

void HashKey::writeLog(char *mdn, char *phoneIp, const char *hashkey, const char*url, int reqsize, int respsize)
{
	#ifdef BENCH_TEST
	return;
	#endif
	if (hashkeylog)
		hashkeylog->logprint(LVL_INFO, "%s %15s %s %5d %6d %s\n",
			mdn, phoneIp, hashkey, reqsize, respsize, url);
}

int HashKey::getKtfHashKey(char *pDest, const char *pSrcString)
{
	ASSERT(pSrcString != NULL);
	ASSERT(pDest != NULL);

	const int OCTAL_VALUE = 8;

	char modString[OCTAL_VALUE] = {0};
	int iValue = strlen(pSrcString) / OCTAL_VALUE;
	int iMod = strlen(pSrcString) % OCTAL_VALUE;

	unsigned int uiRetValue[OCTAL_VALUE] = {0};

	for(int n = 0; n < iMod; n++) 
	{
		modString[n] = pSrcString[iValue*OCTAL_VALUE + n];
	}

	for(int i = 0; i < iValue; i++) 
	{
		for(int j = 0; j < OCTAL_VALUE; j++) 
		{
			uiRetValue[j] += pSrcString[i*OCTAL_VALUE+j];
		}
	}

	if(iMod) 
	{
		for(int k = 0; k < OCTAL_VALUE; k++) 
		{
			uiRetValue[k] += modString[k];
		}
	}

	for(int m = 0; m < OCTAL_VALUE; m++) 
	{
		uiRetValue[m] = uiRetValue[m] % 0x100;
	}

	snprintf(pDest, MaxHashKeySize, "%02X%02X%02X%02X%02X%02X%02X%02X",
		uiRetValue[0], uiRetValue[1], uiRetValue[2], uiRetValue[3],
		uiRetValue[4], uiRetValue[5], uiRetValue[6], uiRetValue[7]);
	pDest[MaxHashKeySize] = 0;
	
	return 0;
}


