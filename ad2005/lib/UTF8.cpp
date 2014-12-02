#include "stdafx.h"
#include "UTF8.h"

int MultibyteToUTF8(const char* src,int nbyte,char** dest)
{
	unsigned short* unicode=NULL;
	int strlen = UNICODE_encode(src,nbyte,&unicode);
	int ret = UTF8_encode(unicode,strlen,dest);
	delete[] unicode;
	return ret;
}

int UNICODE_encode(const char* src,int bytelen,unsigned short** unicode)
{
	if(src == NULL || bytelen == 0 || IsBadReadPtr(src,bytelen) == TRUE) 
	{
		*unicode = NULL;
		return 0;	
	}
	int nLen = MultiByteToWideChar(CP_ACP,0,src,bytelen,NULL,NULL);
	
	if(nLen == 0) 
	{
		*unicode = NULL;
		return 0;
	}

	*unicode = new unsigned short[nLen+1];
	if(*unicode == NULL || IsBadWritePtr(*unicode,(nLen+1)*sizeof(unsigned short)) == TRUE) 
	{
		*unicode = NULL;
		return 0;
	}

	ZeroMemory(*unicode,nLen*sizeof(unsigned short)+2);
	MultiByteToWideChar(CP_ACP,0,src,bytelen,*unicode,nLen);
	return nLen;
}

int UTF8_encode(const unsigned short* src,int srclen,char** multibyte)
{
	if(src == NULL || srclen == 0 || IsBadReadPtr(src,srclen*sizeof(unsigned short)) == TRUE) 
	{
		*multibyte = NULL;
		return 0;	
	}

	int nLen = WideCharToMultiByte(CP_UTF8,0,src,srclen,*multibyte,0,NULL,NULL);
	
	if(nLen == 0) 
	{
		*multibyte = NULL;
		return 0;
	}

	*multibyte = new char[nLen+1];

	if(*multibyte == NULL || IsBadWritePtr(*multibyte,nLen+1) == TRUE) 
	{
		*multibyte = NULL;
		return 0;
	}

	ZeroMemory(*multibyte,nLen+1);
	WideCharToMultiByte(CP_UTF8,0,src,srclen,*multibyte,nLen,NULL,NULL);
	return nLen;
}


int UTF8ToMultibyte(const char* src,int nbyte,char** dest)
{
	unsigned short* unicode=NULL;
	int strlen = UTF8_decode(src,nbyte,&unicode);
	int ret = UNICODE_decode(unicode,strlen,dest);
	delete[] unicode;
	return ret;
}


int UNICODE_decode(const unsigned short* src,int srclen,char** multibyte)
{
	if(src == NULL || srclen == 0 || IsBadReadPtr(src,srclen*sizeof(unsigned short)) == TRUE) 
	{
		*multibyte = NULL;
		return 0;	
	}
	
	int nLen = WideCharToMultiByte(CP_ACP,0,src,srclen,*multibyte,0,NULL,NULL);

	if(nLen == 0) 
	{
		*multibyte = NULL;
		return 0;
	}

	*multibyte = new char[nLen+1];

	if(*multibyte == NULL || IsBadWritePtr(*multibyte,nLen+1) == TRUE) 
	{
		*multibyte = NULL;
		return 0;
	}

	ZeroMemory(*multibyte,nLen+1);
	WideCharToMultiByte(CP_ACP,0,src,srclen,*multibyte,nLen,NULL,NULL);
	return nLen;
}


int UTF8_decode(const char* src,int bytelen,unsigned short** unicode)
{
	
	if(src == NULL || bytelen == 0 || IsBadReadPtr(src,bytelen) == TRUE) 
	{
		*unicode = NULL;
		return 0;	
	}

	int nLen = MultiByteToWideChar(CP_UTF8,0,src,bytelen,NULL,NULL);

	if(nLen == 0) 
	{
		*unicode = NULL;
		return 0;
	}

	*unicode = new unsigned short[nLen+1];

	if(*unicode == NULL || IsBadWritePtr(*unicode,(nLen+1)*sizeof(unsigned short)) == TRUE) 
	{
		*unicode = NULL;
		return 0;
	}

	ZeroMemory(*unicode,nLen*sizeof(unsigned short)+2);
	MultiByteToWideChar(CP_UTF8,0,src,bytelen,*unicode,nLen);
	return nLen;
}