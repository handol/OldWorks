#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#if defined(WIN32) || defined(WINDOWS) || defined(_WIN32) || defined(__WIN32__)
	#ifndef WIN32
	#define WIN32
	#endif 

	#include <windows.h>
	#include <direct.h>
	#include <io.h>
	#include <fcntl.h>
	#include <time.h>
	#include <sys/types.h>
	#include <sys/stat.h>


	#define	strcasecmp	_stricmp
	#define	strncasecmp	_strnicmp

#else
	#include <unistd.h>
	#include <fcntl.h>
	#include <sys/types.h>
	#include <sys/stat.h>
#endif

#include "FileUtil.h"


int FileUtil::check_file_exist(char *fname)
{
#ifndef WIN32
	struct stat buf;
	if (stat(fname, &buf) < 0) return 0;
	else return 1;
#else
	return 0;
#endif
}

// if this is NOT a directory, but a file, delete the file and make this directory
// if this does NOT exist, make this new directory
// return 0 if good
// return -1 if fail
int FileUtil::check_this_directory(char *fname)
{
	#ifndef WIN32
	struct stat buf;
	if (stat(fname, &buf) < 0 || ! S_ISDIR(buf.st_mode))
		return (mkdir(fname, 0755) );
	#else
		mkdir(fname);
	#endif	
	return 0;
}


/**

_fstat64 <sys/stat.h> and <sys/types.h> -- Windows
http://msdn.microsoft.com/library/default.asp?url=/library/en-us/vclib/html/_crt__chsize.asp
*/
int FileUtil::get_file_size(char *fname)
{
#ifndef WIN32
	struct stat buf;	
	if (stat(fname, &buf) < 0) return 0;
#else
	struct _stat buf;	
	if (_stat(fname, &buf) < 0) return 0;
#endif
	return buf.st_size;

}

int FileUtil::load_file_2_new_mem(char *fname, char **mem_ptr)
{
	char *new_mem;
	int	fsize=0;
	
	fsize = get_file_size(fname);
	if (fsize <=0 ) return -1;

	new_mem = new char[fsize+1];
	*mem_ptr = new_mem;
	return load_file_2_given_mem(new_mem, fname, fsize );
}



int FileUtil::load_file_2_given_mem(char *mem, char *fname, int max_size)
{
	FILE *fp;
	char buf[1024];
	int	n=0;

	mem[0] = 0;
	if ( (fp=fopen(fname, "r")) == NULL) return -1;
	while (n < max_size && fgets(buf, 1024, fp) != NULL) {		
		strcpy(mem+n, buf);
		n += strlen(buf);
	}	
	mem[n] = 0;
	fclose(fp);
	
	return n;
}



int FileUtil::save_file_2_from_mem(char *mem, char *fname)
{
	FILE *fp;
	int	n_write=0;

	if ( (fp=fopen(fname, "w")) == NULL) return -1;
	n_write = fputs(mem, fp);
	fclose(fp);
	
	return n_write;
}

int FileUtil::append_file_2_from_mem(char *mem, char *fname)
{
	FILE *fp;
	int	n_write=0;

	if ( (fp=fopen(fname, "a")) == NULL) return -1;
	n_write = fputs(mem, fp);
	fclose(fp);
	
	return n_write;
}

int	FileUtil::file_read_lines(char *fname)
{
	int cnt=0;
	FILE *fp;
	char buf[1024];

	if ( (fp=fopen(fname, "r")) == NULL) {
		printf("Read Failed: %s\n", fname); 
		return -1;
	}
	while (fgets(buf, 1024, fp) != NULL) {
		#if 0
		ch = StrUtil::first_nonspace(buf);		
		if (ch==0 || ch=='#') continue;
		#endif 
		cnt++;
	}
	fclose(fp);
	return 0;
}

int	FileUtil::countLines(char *fname)
{
	int	fd;
	char	buf[256];
	int	readbytes;
	int	n;
	int	linecount=0;
	fd = open(fname, 0);
	if (fd <= 0) return 0;
	while(1) {
		readbytes = read(fd, buf, 256);
		if (readbytes <= 0) break;
		for(n=0; n<readbytes; n++)
			if (buf[n]=='\n') linecount++;
	}
	close(fd);
	return linecount + 1;	
}

