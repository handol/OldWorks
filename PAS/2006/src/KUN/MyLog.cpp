/**

PAS2006
@author handol@gmail.com 한대희 011-430-0258
@date 2004.11.30
*/

#include "MyLog.h"
#include "Util2.h"
#include "basicDef.h"

#include  <stdio.h>
#include  <time.h>
#include  <ctype.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pthread.h>
#include "PasLog.h"

#include "Mutex.h"

MyLog::MyLog()
{
	logPath[0] = '\0';
	dirName[0] = '\0';
	moduleName[0] = '\0';

	old_day = 0;
	priority = LVL_DEBUG;
	timeprint = 1;
	thridprint = 1;
	withYear = false;
}


MyLog::~MyLog()
{
	this->close();
}


/**
if this is NOT a directory, but a file, delete the file and make this directory
if this does NOT exist, make this new directory
@return 0 if good
@return -1 if fail
*/
int MyLog::mkdir_ifnot(char *fname)
{
	struct stat buf;
	if (stat(fname, &buf) < 0 || ! S_ISDIR(buf.st_mode))
		return (mkdir(fname, 0755) );
	return 0;
}


/**
homedir이 주어지면 homedir/log 아래에 module_name.mmdd로 로그 화일을 만들고,
homedir없으면 현재 dir에 만든다.
*/
int MyLog::open(const char *homedir, const char *module_name)
{
	if (module_name == NULL)
	{
		logPath[0] = 0;
		return -1;
	}

	if (homedir != NULL)
	{
		STRNCPY(dirName, homedir, sizeof(dirName)-1);
	}

	if (module_name != NULL)
	{
		STRNCPY(moduleName, module_name, sizeof(moduleName)-1);
	}

	logopen();

	return 0;
}


int MyLog::openWithYear(const char *homedir, const char *module_name)
{
	
	withYear = true;
	return this->open(homedir, module_name);
}

int MyLog::overwrite()
{
	return logfd.open(getLogPath(), 1);
}

char *MyLog::getLogPath()
{
	time_t curTime = time(NULL);
	struct tm datetm;
	localtime_r(&curTime, &datetm);

	if (withYear)
		snprintf(logPath,  sizeof(logPath)-1, "%s/%s.%04d%02d%02d.log", 
			dirName, moduleName, datetm.tm_year + 1900, datetm.tm_mon + 1, datetm.tm_mday);

	else
		snprintf(logPath, sizeof(logPath)-1, "%s/%s.%02d%02d.log", 
			dirName, moduleName, datetm.tm_mon + 1, datetm.tm_mday);
			
	return logPath;
}


int MyLog::close()
{
	return 0;
}


// 일자가 바뀌었거나 화일 open이 안된 경우 새로 화일을 연다.
int MyLog::logopen()
{
	int res = 0;
	time_t curTime = time(NULL);
	struct tm datetm;
   	localtime_r(&curTime, &datetm);
	if (old_day != datetm.tm_mday)
	{
		res = logfd.open(getLogPath());
		old_day = datetm.tm_mday;
	}
	return res;
}



/**
/**
-- printf() format --
http://www.dgp.toronto.edu/~ajr/209/notes/printf.html

%ld long
%u unsigned decimal number
%lu unsigned long decimal number

*/
void MyLog::logprint(int pri, const char *fmt, ...)
{
	if (pri < priority) return;
	int res = logopen();
	if (res < 0)
	{
		PAS_ERROR1("MyLog::logprint %s File open error", getLogPath());
		return;
	}

	time_t curTime = time(NULL);
	struct tm datetm;
	localtime_r(&curTime, &datetm);

	if (timeprint)
	{
		int ret = logfd.print("%02d:%02d:%02d ", datetm.tm_hour, datetm.tm_min, datetm.tm_sec);
		if( ret < 0 )
			PAS_ERROR1("MyLog::logprint %s File write error", getLogPath());
	}

	if (pri >= LVL_WARN)
	{
		int warn = logfd.print("[WARN] ");
		if( warn < 0 )
			PAS_ERROR1("MyLog::logprint %s WARN write error", getLogPath());
	}

	char buff[1024];
	va_list args;
	va_start(args, fmt);
	int Write = vsnprintf(buff, sizeof(buff), fmt, args);	
	if( Write > 0 )
		buff[Write] = '\0';		
	logfd.print("%s", buff);
	va_end(args);
}


void MyLog::_goodhex(byte *ptr, int size, int maxsize)
{
	int i;
	int half, full;
	full = MIN(maxsize, size);
	half = MIN(maxsize >> 1, size);

	for(i=0; i<half; i++)
		logfd.print("%02X ", ptr[i]);
	logfd.print("  ");

	for(    ;i<full;i++)
		logfd.print("%02X ", ptr[i]);

	for(    ;i<maxsize;i++)
		logfd.print("   ");

	for(i=0; i<full; i++)
		if (isprint(ptr[i]))
			logfd.print("%c", ptr[i]);
		else 
			logfd.print(".");
			
	logfd.print("\n\n");

}

void MyLog::_goodhex2(byte *ptr, int size, int maxsize)
{
	typedef FastString<1024> hexDumpBuffType;
	hexDumpBuffType buff;
	
	int i;
	int half, full;
	full = MIN(maxsize, size);
	half = MIN(maxsize >> 1, size);

	for(i=0; i<half; i++)
		buff.append_sprintf("%02X ", ptr[i]);
	buff.append_sprintf("  ");

	for(    ;i<full;i++)
		buff.append_sprintf("%02X ", ptr[i]);

	for(    ;i<maxsize;i++)
		buff.append_sprintf("   ");

	for(i=0; i<full; i++)
		if (isprint(ptr[i]))
			buff.append_sprintf("%c", ptr[i]);
		else 
			buff.append_sprintf(".");
			
	buff.append_sprintf("\n");
	logfd.print("%s", buff.toStr());
}

void MyLog::hexdump(int pri, char *src, int size, const char *mesg /* NULL */)
{
	int i;
    	#define	DUMP_STEP	(32)
	if (pri < priority) return;
	if (mesg)
		logprint(pri, "%s HEXDUMP %d bytes\n", mesg, size);
	else
		logprint(pri, "HEXDUMP %d bytes\n", size);

	for(i=0; i<size; i+= DUMP_STEP)
		_goodhex2((byte*)src+i, size-i, DUMP_STEP);

}


void MyLog::printHttpMesg(const char *mesg, char *src, int size, int flagPrintBody)
{

	int headerSize = 0;

	if (
		strncasecmp(src, "HTTP", 4)==0
		|| strncasecmp(src, "GET", 3)==0
		|| strncasecmp(src, "POST", 4)==0
		|| strncasecmp(src, "PUT", 3)==0
		|| strncasecmp(src, "CONNECT", 7)==0
		|| strncasecmp(src, "RESULT", 6)==0
		|| strncasecmp(src+1, "GET", 3)==0
		|| strncasecmp(src+1, "POST", 4)==0
		|| strncasecmp(src+1, "PUT", 3)==0
		|| strncasecmp(src+1, "CONNECT", 7)==0
		|| strncasecmp(src+1, "RESULT", 6)==0
		)
	{
		headerSize = Util2::getHeaderSize(src, size);
	}

	int bodySize = size - headerSize;

	if (headerSize > 0)
	{
		logprint(LVL_INFO, "=== %s : Len[%d] Header[%d] Body[%d]\n", mesg, size, headerSize, bodySize);
	}
	else
	{
		logprint(LVL_INFO, "=== %s : All Body, Len[%d]\n", mesg, size);
	}

	if (headerSize > 0)
	{
		char remember = src[headerSize];
		src[headerSize] = '\0';
		logprint(LVL_INFO, src, headerSize);
		src[headerSize] = remember;
	}

	if (flagPrintBody && bodySize > 0)
		hexdump(LVL_INFO, src+headerSize, MIN(bodySize, 256));

}


#ifdef TEST_MAIN

main()
{
	MyLog log;

	log.open("./", "test2.trace");
	log.logprint(1, "hihihihiiih !!\n");
	log.logprint(0, "hihihihiiih !!\n");

	MyLog *mylog = new MyLog();
	mylog->open("./", "test.trace");
	mylog->set_priority(0);
	mylog->logprint(1, "hello !!!!\n");
	mylog->logprint(0, "hello !!!!\n");

}
#endif
