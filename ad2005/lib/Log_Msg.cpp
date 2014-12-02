/**
Minic ACE files.

KTF-DMS-INISOFT mobile agent 2004
@author handol@gmail.com �Ѵ��� 011-430-0258
@date 2004.11.30
*/
#include "Log_Msg.h"
#include "FileUtil.h"
#include    <stdio.h>
#include    <time.h>
#include    <ctype.h>


Log_Msg * Log_Msg::_instance = NULL;
char Log_Msg::fName[256] = {0};
FILE * Log_Msg::logfd = 0;
int Log_Msg::old_day = 0;
struct tm * Log_Msg::t = 0;

int Log_Msg::priority = 0;
int Log_Msg::timeprint = 1;
int Log_Msg::thridprint = 1;
int Log_Msg::tostd = 0;

Log_Msg * Log_Msg::getInstance()
{
    if (_instance==0)
    {
        _instance = new Log_Msg();

    }
    return _instance;
}

void	Log_Msg::set_stderr(void) {
// http://developer.novell.com/ndk/doc/clib/index.html?page=/ndk/doc/clib/sngl_enu/data/sdk983.html
	tostd = 1;
	logfd = fdopen( fileno(stderr), "w");
}

void	Log_Msg::set_stdout(void) {
	tostd = 1;
	logfd = fdopen( fileno(stdout), "w");
}

/**
homedir�� �־����� homedir/log �Ʒ��� module_name.mmdd�� �α� ȭ���� �����,
homedir������ ���� dir�� �����.
*/
int Log_Msg::open(char *homedir, char *module_name)
{
    if (module_name == 0)
    {
        fName[0] = 0;
        return -1;
    }

    if (homedir==NULL)
    {
        sprintf(fName, "%s", module_name);
    }   
    else
    {
    	sprintf(fName, "%s/log", homedir);
    	FileUtil::check_this_directory(fName); // log directory�� ����.
        sprintf(fName, "%s/log/%s", homedir, module_name);
    }
	
    return 0;
}


int Log_Msg::close()
{
    if (logfd && !tostd)  fclose(logfd);
    return 0;
}



/**
ȭ���� ����� ���� ���� (overwrite)
*/
void	Log_Msg::overwrite()
{
    char logf_path[256];
    time_t  t_val;

    time(&t_val);
    t = localtime(&t_val);
    if (logfd == 0 || old_day != t->tm_mday)
    {
        old_day = t->tm_mday;

        if (logfd && !tostd) fclose(logfd);        
        sprintf(logf_path, "%s.%02d%02d", fName, t->tm_mon + 1, t->tm_mday);
        logfd = fopen(logf_path, "w");
    }
}


// ���ڰ� �ٲ���ų� ȭ�� open�� �ȵ� ��� ���� ȭ���� ����.
void	Log_Msg::logopen()
{
    char logf_path[256];
    time_t  t_val;

	if (tostd) return;
    time(&t_val);
    t = localtime(&t_val);
    if (logfd == 0 || old_day != t->tm_mday)
    {
        old_day = t->tm_mday;

        if (logfd  && !tostd) fclose(logfd);        
        sprintf(logf_path, "%s.%02d%02d", fName, t->tm_mon + 1, t->tm_mday);
        logfd = fopen(logf_path, "a");
    }
}

/**
-- printf() format --
http://www.dgp.toronto.edu/~ajr/209/notes/printf.html

%ld long 
%u unsigned decimal number 
%lu unsigned long decimal number 


*/
void Log_Msg::logprint(int pri, char *fmt, ...)
{
	va_list args;
	char buf[1024*10]={0};

    if (pri < priority) return;
    logopen();
    if (logfd == 0)     return;

    va_start(args, fmt);
    vsprintf(buf, fmt, args);
    va_end(args);

    //if (thridprint) fprintf(logfd, "[%lu] ", pthread_self());
    if (timeprint) fprintf(logfd, "%02d:%02d:%02d ", t->tm_hour, t->tm_min, t->tm_sec);
    if (pri >= LM_WARNING) fprintf(logfd, "[WARN] ");
    fprintf(logfd, "%s", buf);
    if (! tostd) fflush(logfd);
}


void Log_Msg::_goodhex(byte *ptr, int size, int maxsize)
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
;
	
    for(i=0; i<full; i++)
        if (isprint(ptr[i])) fprintf(logfd, "%c", ptr[i]);
    else fprintf(logfd, ".");
    fprintf(logfd, "\n");

}


void Log_Msg::hexdump(int pri, char *src, int size)
{
    int i;
    if (pri < priority) return;
    logprint(pri, "HEXDUMP %d bytes\n", size);
    for(i=0; i<size; i+= 16)
        _goodhex((byte*)src+i, size-i, 16);
	fprintf(logfd, "\n");
    if (! tostd) fflush(logfd);
}
