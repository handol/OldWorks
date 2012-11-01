/**

PAS2006
@author handol@gmail.com 한대희 011-430-0258
@date 2004.11.30
*/
#include "MyLog.h"

#include  <stdio.h>
#include  <time.h>
#include  <ctype.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pthread.h>


MyLog::MyLog()
{
	logPath[0] = '\0';
	dirName[0] = '\0';
	moduleName[0] = '\0';
	logfd = 0;
	old_day = 0;
	datetm = 0;
	priority = LVL_DEBUG;
	timeprint = 1;
	thridprint = 1;
	tostd = 0;
}

MyLog::~MyLog()
{
	 if (logfd) fflush(logfd);
}

void	MyLog::set_stderr(void) {
// http://developer.novell.com/ndk/doc/clib/index.html?page=/ndk/doc/clib/sngl_enu/data/sdk983.html
	tostd = 1;
	logfd = fdopen( fileno(stderr), "w");
}

void	MyLog::set_stdout(void) {
	tostd = 1;
	logfd = fdopen( fileno(stdout), "w");
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
int MyLog::open(char *homedir, char *module_name)
{
	if (module_name == NULL)
	{
		logPath[0] = 0;
		return -1;
	}

	if (homedir != NULL)
	{
		strncpy(dirName, homedir, sizeof(dirName)-1);
		dirName[sizeof(dirName)-1] = 0;
	} 

	if (module_name != NULL)
	{
		strncpy(moduleName, module_name, sizeof(moduleName)-1);
		moduleName[sizeof(moduleName)-1] = 0;
	}

	logopen();
	
	return 0;
}

char *MyLog::getLogPath()
{
	sprintf(logPath, "%s/%s", dirName, moduleName);
	//printf("MyLog:: %s\n", logPath);
	return logPath;
}

int MyLog::close()
{
    if (logfd && !tostd)  fclose(logfd);
    return 0;
}



/**
화일을 지우고 새로 쓰기 (overwrite)
*/
void	MyLog::overwrite()
{
	time_t  t_val;

	time(&t_val);
	datetm = localtime(&t_val);
	if (logfd == 0 || old_day != datetm->tm_mday)
	{
		old_day = datetm->tm_mday;

		if (logfd && !tostd) fclose(logfd);        
		logfd = fopen(getLogPath(), "w");
	}
}


// 일자가 바뀌었거나 화일 open이 안된 경우 새로 화일을 연다.
void	MyLog::logopen()
{
	time_t  t_val;

	if (tostd) return;
	time(&t_val);
	datetm = localtime(&t_val);
	if (logfd == 0 || old_day != datetm->tm_mday)
	{
		old_day = datetm->tm_mday;
		if (logfd  && !tostd) fclose(logfd);
		
		logfd = fopen(getLogPath(), "w+");
	}
}

/**
-- printf() format --
http://www.dgp.toronto.edu/~ajr/209/notes/printf.html

%ld long 
%u unsigned decimal number 
%lu unsigned long decimal number 


*/
void MyLog::logprint(int pri, const char *fmt, ...)
{
	va_list args;
	//char buf[1024*10]={0};

    if (pri < priority) return;
    logopen();
    if (logfd == 0)     {
		//printf("%s", buf);
		//fflush(stdout);
		return;
	}

    va_start(args, fmt);
    vfprintf(logfd, fmt, args);
    va_end(args);

	if (! tostd) fflush(logfd);
}


void MyLog::_goodhex(byte *ptr, int size, int maxsize)
{
    int i;
	int	half, full;
	full = MIN(maxsize, size);
	half = MIN(maxsize >> 1, size);
	
    for(i=0; i<half; i++)
        fprintf(logfd, "%02X ", ptr[i]);
    fprintf(logfd, "  ");
		
	for(    ;i<full;i++)
		fprintf(logfd, "%02X ", ptr[i]);

	for(    ;i<maxsize;i++)
		fprintf(logfd, "   ");
	
    for(i=0; i<full; i++)
        if (isprint(ptr[i])) fprintf(logfd, "%c", ptr[i]);
    else fprintf(logfd, ".");
    fprintf(logfd, "\n");

}


void MyLog::hexdump(int pri, char *src, int size, const char *mesg /* NULL */)
{
    int i;
	if (pri < priority) return;
	if (logfd == 0) return;
	if (mesg)
		logprint(pri, "%s HEXDUMP %d bytes\n", mesg, size);
	else
		logprint(pri, "HEXDUMP %d bytes\n", size);

	for(i=0; i<size; i+= 16)
		_goodhex((byte*)src+i, size-i, 16);
		
	fprintf(logfd, "\n");
	if (! tostd) fflush(logfd);
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
