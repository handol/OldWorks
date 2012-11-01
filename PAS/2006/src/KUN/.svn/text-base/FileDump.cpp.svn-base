#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>

#include "Common.h"
#include "StrUtil.h"
#include "FileDump.h"

int check_this_directory(char *fname)
{
	struct stat buf;
	if (stat(fname, &buf) < 0 || ! S_ISDIR(buf.st_mode))
		return (mkdir(fname, 0755) );
	return 0;
}

FileDump::FileDump()
{
	fd = NULL;
	beforePtr = NULL;
	beforeLen = 0;
	seqNum = 0;
}

FileDump::~FileDump()
{
	if (fd)
		fclose(fd);
}

void FileDump::init(const char *desc, int kind)
{
	get_nowtime();
		
	check_this_directory((char*)"dump");

	#if 0
	char filename[128];
	STRNCPY(filename, fname, 48);
	/* windows 에서 화일 이름에 사용할 수 업는 문자 : \ / : * ? " < > |  */
	/* & 문자 추가 */
	StrUtil::replaceChars(filename, (char*)"\\/:*?\"<>|&.!",  '_');
	#endif
	if( fd != NULL )
		fclose(fd);
		
	char realname[256];
	sprintf(realname, "%s/%s-S-%d-%03d-%s", "dump", nowtime, kind, seqNum, desc);

	

	fd = fopen(realname, "w");
}

void	FileDump::write(const char *data, int leng)
{
	if (fd == NULL)
		return;

	fwrite(data, leng, 1, fd);
	fflush(fd);
	seqNum++;
}

void	FileDump::before(char *data, int orglen)
{
	beforePtr = data;
	beforeLen = orglen;
}

void	FileDump::after(int afterlen)
{
	int consumed = beforeLen - afterlen;
	write(beforePtr, consumed);
	
}

void FileDump::get_nowtime()
{
	time_t		t_val;
	struct tm	*t;
	
	time(&t_val);
	t = localtime(&t_val);
	sprintf(nowtime, "%02d%02d-%02d%02d-%02d", 
		t->tm_mon+1,
		t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
}
