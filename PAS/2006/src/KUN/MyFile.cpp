#include "MyFile.h"

MyFile::MyFile()
{
	nFD = -1;
	buff[0] = '\0';
	filename[0] = '\0';
	bEmpty = true;
	readPointer = NULL;
	unReadBytes = 0;
}

MyFile::~MyFile()
{
	close();
}

// 시스템 콜인 open 호출, flag 은 open 의 man 페이지 참조
int MyFile::open(const char *fileName, int flagOverWrite)
{
	WriteMutex<PasMutex> writeMutex(pasLock);

	// 파일명이 바뀌면 이전 파일을 닫고 새로 연다.
	if(nFD > 0) 
	{
		if(flagOverWrite == 0 &&  strcmp(fileName, filename) == 0 )
		{
			return 0;
		}

		else close();
	}

	int flag = 0;
	if (flagOverWrite) // overwrite mode
		flag = O_CREAT|O_TRUNC | O_RDWR;
	else // append mode
		flag = O_CREAT|O_APPEND|O_WRONLY;
		
	int permission = S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH;
	nFD = ::open( fileName, flag, permission );
	if( nFD == -1 )
		return -1;

	bEmpty = false;

	return 0;
}

// 시스템 콜인 open 호출, flag 은 open 의 man 페이지 참조
int MyFile::openForRead(const char *fileName)
{
	WriteMutex<PasMutex> writeMutex(pasLock);

	// 파일명이 바뀌면 이전 파일을 닫고 새로 연다.
	if(nFD > 0) 
	{
		close();
	}

	int flag = 0;
		flag = O_RDONLY;
		
	int permission = S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH;
	nFD = ::open( fileName, flag, permission );
	if( nFD == -1 )
		return -1;

	bEmpty = false;

	return 0;
}

int MyFile::close()
{
	if( nFD > 0 )
	{
		bEmpty = true;
		::close(nFD);
		nFD = -1;
	}

	return 0;
}

int MyFile::print(const char *fmt, ...)
{
	WriteMutex<PasMutex> writeMutex(pasLock);

	if(nFD < 0)
		return -1;

	va_list args;

	va_start(args, fmt);


/*
The vprintf(), vfprintf(), and vsprintf()  functions  return
the  number  of  characters transmitted (not including \0 in
the case of vsprintf()). The  vsnprintf()  function  returns
the  number  of characters formatted, that is, the number of
characters that would have been written to the buffer if  it
were large enough. Each function returns a negative value if
an output error was encountered.
int vsnprintf(char *s, size_tn, const char *format,  va_listap);*/

	int Write = vsnprintf(buff, sizeof(buff), fmt, args);
	if( Write > 0 )
		buff[Write] = '\0';

	int Writed = write( nFD, buff, Write );
	if( Writed != Write )
	{
		// 기록해야할 사이즈와 기록한 사이즈가 다르면 에러
		PAS_ERROR3( "MyFile::print Writed size [%d] less than [%d] [org] %s", Writed, Write, buff );
	}

	va_end(args);

	return Writed;
}

bool MyFile::empty()
{
	return bEmpty;
}

/**
화일에서 읽어서 내부 버퍼에 담는다.
*/
int MyFile::_readstream()
{
	if (unReadBytes > 0)
		return unReadBytes;
		
	int	readn = ::read(nFD, buff, sizeof(buff));
	if (readn < 0)
		return -1;

	unReadBytes = readn;
	readPointer = buff;
	//printf("_readstream = %d\n", readn);
	return readn;
}

/**
내부 버퍼에 담긴 내용을 flushsize 만큼 dest buff에 복사한다.
return -1 if EOF
*/
int MyFile::_flushlinestream(char *_buff, int _buffsize, int flushsize)
{
	int realflushsize = flushsize;
	if (realflushsize > _buffsize)
		realflushsize = _buffsize;
	if (realflushsize > unReadBytes)
		realflushsize = unReadBytes;
		
	strncpy(_buff, readPointer, realflushsize);
	_buff[realflushsize] = 0;

	//printf("_flushlinestream = %d\n", realflushsize);
	unReadBytes -= realflushsize;
	readPointer += realflushsize;
	return realflushsize;
}

int MyFile::readLine(char *_buff, int _buffsize)
{
	int	copiedBytes = 0;
	
	while(copiedBytes < _buffsize)
	{
		if (_readstream() < 0)
			break;

		int copySize=0;
		char *newline = strchr(readPointer, '\n');
		if (newline!=NULL)
		{
			copySize = newline - readPointer + 1;
		}
		else
		{
			copySize = unReadBytes;
		}
		
		int realCopySize = _flushlinestream(_buff+copiedBytes,  _buffsize-copiedBytes, copySize);
		copiedBytes += realCopySize;

		//printf("unReadBytes=%d readPointer=%X newline=%X copySize=%d realCopySize=%d copiedBytes=%d\n",
		//	unReadBytes, readPointer, newline, copySize, realCopySize, copiedBytes);
			
		if (newline!=NULL || realCopySize==0)
			break;
	}
	//printf("=====\n");
	if (copiedBytes==0)
		return -1;
	return copiedBytes;
	
}

