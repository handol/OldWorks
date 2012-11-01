#ifndef __H_MY_FILE__
#define __H_MY_FILE__

#pragma once

#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "Mutex.h"

class MyFile
{
public :
	MyFile();
	virtual ~MyFile();

	int open(const char *fileName, int flagOverWrite=0);
	int openForRead(const char *fileName);
	int print(const char *fmt, ...);
	int	readLine(char *buff, int buffsize);
	bool empty();
	int close();
	
protected:
	
	int _readstream();
	int _flushlinestream(char *_buff, int _buffsize, int flushsize);
	
	PasMutex pasLock;
	char buff[8192];
	char filename[256];
	int nFD;
	bool bEmpty;
	char *readPointer; // buff 에서 다음 read할 포인터
	int	unReadBytes; // buff 에서 read 안되고 남아 있는 바이트 수
};

#endif

