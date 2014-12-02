
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#ifdef WIN32
#include "windowsDef.h"
#endif

#include "URLParse.h"


#define PRN	printf

char *gPROT_name[7] ={
	"unkown",
	"http",
	"https",
	"mail",
	"ftp",
	"file",
	"news"
};

char gOnlyRoot[3] = "/";


char *gNull = "Null";

// return TRUE if string has '?', '&', '"'
int URLParse::has_CGIchar(char *str)
{
	while (*str) {
		//if (*str=='?' || *str=='&' || *str=='"') return 1;
		if (*str=='?' || *str=='&') return 1;
		str++;
	}
	return 0;
}

int URLParse::has_CGIchar(char *str, int len)
{
	int	n=0;
	while (*str && n++<len) {
		//if (*str=='?' || *str=='&' || *str=='"') return 1;
		if (*str=='?' || *str=='&') return 1;
		str++;
	}
	return 0;
}

/**
@brief path�κ��� DIR�� �����ϰ� ȭ�ϸ� �ش��ϴ� �κи� fname�� �����Ѵ�.
@return n : fname�� ���� ���.
@return 0 : path�� only DIR�� ���. fname == NULL string
*/

int URLParse::get_fname(char *fname, char *path)
{
	char *ptr=NULL;
	int	len;

	fname[0] = 0;
	ptr = get_fname(path);
	if (ptr) {
		len = ptr - path;
		STRNCPY(fname,  ptr, URLPARSE_FILELEN);
		return len;
	}
	else 
		return 0;
}


/**
@brief path�κ��� DIR�� �����ϰ� ȭ�ϸ� �ش��ϴ� �κи� fname�� �����Ѵ�.
@return 1 : fname�� ���� ���.
@return 0 : path�� only DIR�� ���. fname == NULL string
*/

char *URLParse::get_fname(char *path)
{
	char *dir_ptr=NULL;

	dir_ptr = strrchr(path, '/');
	
	if (dir_ptr) {
		if (dir_ptr[1] != 0) 
			return dir_ptr + 1;
		else	
			return NULL;
	}
	else 
		return path;

}

/**
ȭ���̸����� ȭ�� Ȯ���ڸ� ���Ѵ�. 

HTML/text Ÿ�԰� binary Ÿ���� ����Ÿ�� �����غ����� Ȯ���ڸ� ���Ѵ�.
HTTP header�� �м��ϴ� ���� �� ��Ȯ�ϴ�.
*/
int URLParse::get_fileext()
{
	char *ptr_ext=0;
	char *ptr;

	if (strstr(url.file, ".pdf") || strstr(url.file, ".zip")
		|| strstr(url.file, ".swf") || strstr(url.file, ".mp3")
		|| strstr(url.file, ".doc") || strstr(url.file, ".ppt")
		|| strstr(url.file, ".ps")) {
			url.file_ext = EXT_UNDEF;
			return EXT_UNDEF;
	}

	ptr =  strchr(url.file, '?');
	if (ptr && ptr - url.file < 30) { // CGI���  25�� ���� �ְ��� 
		url.file_head_len = (ptr - url.file);
		url.file_ext = EXT_CGI;
		return EXT_CGI;
	}

	//file_head_len:   /hello/aaa.shtml  --> aaa�� file_head_len, 3
	//�ִ� 10byte�� 
	ptr=strchr(url.file, '.');
	if (ptr && ptr - url.file < 10) url.file_head_len = (ptr - url.file);
	else url.file_head_len = 10;

	if (ptr) 
		ptr_ext = ptr+1;	
	else 
		ptr_ext = strrchr(url.file, '.');
	
	// text, txt�� �ƴϸ� �� HTML�� ��������...
	if (ptr_ext) {
		if (strcasecmp(ptr_ext+1, "txt")==0)
			url.file_ext = EXT_TXT;
		else if (strcasecmp(ptr_ext+1, "text")==0)
			url.file_ext = EXT_TXT;
		/*
		else if (strncasecmp(ptr_ext+1, "htm", 3)==0)
			url.file_ext = EXT_HTML;
		
		else if (strncasecmp(ptr_ext+1, "shtm", 4)==0)
			url.file_ext = EXT_HTML;
		*/
		
		/*
		// jsp, ��� �ʹ�  ���� 
		else if (strcmp(ptr_ext+1, "cfm")==0)
			url.file_ext = EXT_HTML;
		else if (strcmp(ptr_ext+1, "asp")==0)
			url.file_ext = EXT_HTML;
		else if (strcmp(ptr_ext+1, "jsp")==0)
			url.file_ext = EXT_HTML;
		else if (strcmp(ptr_ext+1, "pl")==0)
			url.file_ext = EXT_HTML;
		*/	
		else
			//url.file_ext = EXT_UNDEF;
			url.file_ext = EXT_HTML;
		
	}
	else
		//url.file_ext = EXT_UNDEF;
		url.file_ext = EXT_HTML;

	return url.file_ext;
}


// URL�� host�κ���  �̱�, ���� site���� �к� 
// ĳ���� ���� 
// char *host�� host �κи� �ִ� string���� ����. Null�� ������ string
int URLParse::get_country(char *host)
{
	char *ptr=0, *ptr2=0;
	int len=0;
	
	ptr = strrchr(host, '.');
	if (ptr==0) return 0;
	len = strlen(ptr) -1;
	ptr2 = strchr(ptr, ':');
	if (ptr2) len -= strlen(ptr2);
	if (len>=3) return NATION_GLOBAL;

	if (strncmp(ptr+1, "uk", 2)==0) return NATION_CA_UK;
	if (strncmp(ptr+1, "ca", 2)==0) return NATION_CA_UK;
	if (strncmp(ptr+1, "us", 2)==0) return NATION_US; // USA States
	if (strncmp(ptr+1, "au", 2)==0) return NATION_AU_NZ; // Austrailia
	if (strncmp(ptr+1, "nz", 2)==0) return NATION_AU_NZ; // New Zealand
	return 0;
}


void URLParse::print()
{
	PRN("URL= %s\n", url.urlstr);
	PRN("P=%s %s H=%s Port=%d\n", gPROT_name[url.prot], url.protocol, (url.host)?url.host:gNull, url.port);
	PRN("Path=%s F=%s is_index=%d\n", (url.path)?url.path:gNull, (url.file)?url.file:gNull, url.is_index);
	PRN("file_ext=%d, url_type=%d, country=%d\n", url.file_ext, url.url_type, url.country);
	PRN("Length: host=%d path=%d dir=%d file=%d\n\n", url.host_len, url.path_len, url.dir_len, url.file_len);
}

// return 1: if good URL
// return 0: if NOT good URL :: '#'�� �����ϰų� HTTP�� �ƴ� ��� 
//2003.3.21 :: �߿� ��ȭ --> mem alloc ������� ����..  
//      path, host�� ���� len�� �����ϰ�, org���� point�ϵ��� �� 
// parse��� url.file==NUL�̸� URL�� DIR�� ������ ����
// url�� host ���̶� path���� "/" ������.

int URLParse::parse(char *org_urlstr)
{
	char *cursor; // move forward as parsing proceed
	char *ptr;
	char *found;
	int	len=0, n;

	if (org_urlstr) {		
		STRNCPY(url.urlstr, org_urlstr, URLPARSE_URLLEN);
	}

	// default
	url.prot = http;
	url.port = 80;
	
	if (strchr(url.urlstr, '#')!=0)
		return 0;
	
	cursor = url.urlstr;
	
	
	// protocol part
	// http://
	// �׿ܴ� ���� 	mailto:// javascript:// ftp://
	found  = strstr(cursor, "://");
	// protocol �̸��� �ִٸ� 
	if (found) {
		// http �� �ƴ� ���� ���� �����ϱ��, ���⼭�� HTTP�� ����. 
		len = found - cursor;
		STRNCPY(url.protocol, cursor, len);
		cursor = found+3;
		
		if (strcasecmp(url.protocol, "http")==0) 
			url.prot = http;
		else
			url.prot = 0; // uknown protocol

		
		ptr = cursor;
		while(*ptr && *ptr!='/' && *ptr!=':')
			ptr++;

		// host part
		len = (ptr - cursor);
		n = MIN(len, URLPARSE_HOSTLEN);
		STRNCPY(url.host, cursor, n);
		url.host_len = n;
				
		if (*ptr==':') { // port
			url.port = (uint2) strtol(ptr+1, 0, 10);
			ptr++;
			while(isdigit(*ptr)) ptr++;			
		}
		
		if (*ptr=='/') { // dir
			STRNCPY(url.path, ptr, URLPARSE_PATHLEN);
			url.file = get_fname(url.path);
		}
		else { // just host only
			strcpy(url.path, "/");
		}
		
	}
	else {	
	// http�� �������� �ʰ� path�� �����ϴ� ��� 
		STRNCPY(url.path, cursor, URLPARSE_PATHLEN);
		url.file = get_fname(url.path);
		
	}
	
	url.path_len = strlen(url.path);
	if (url.file==NULL)
		url.dir_len = url.path_len;
	else
		url.dir_len = url.file - url.path;
	url.file_len = url.path_len - url.dir_len;
			
	url.country = get_country(url.host);
	
	if (url.file==0) {
		// 2003.3.22 path�� DIR�̸� '/'�� ���� 
		if (url.path_len>0 && url.path[url.path_len-1]!='/') {
			url.path[url.path_len] = '/';
			url.path_len++;
		}	
		
	}
	
	
	if (url.file==0 || strncmp(url.file, "index.", 6)==0) {
		if (url.file) url.file_head_len = 5;
		// else 0: default
		
		url.is_index = TRUE;
		url.file_ext = EXT_HTML;
		
	}

	/* 2005.7.11 @dahee */
	sprintf(url.urlstr, "http://%s%s", url.host, url.path);
	
	// ȭ�� Ȯ���� �˻� 
	if (url.file_ext==0) get_fileext();
	
	if (url.file) url.url_type = FILE_URL;
	else if (url.path_len <= 1) url.url_type = HOST_URL;
	else url.url_type = DIR_URL;
	
	return 1;
}


//---------------------- PATH NAME
int URLParse::trim_path(char *new_path, char *path)
{
	int	n;
	char *up_dir, *curr_dir, *ptr;
	up_dir = strstr(path, "..");
	curr_dir = strstr(path, "./");
	if (up_dir==NULL && curr_dir==NULL) {
		strcpy(new_path, path);
		return 0;
	}
	
	for(ptr=path, n=0; *ptr; ) {
		while(*ptr && *ptr!='.') {
			if (n && *(new_path-1) == '/' && *ptr == '/')
				ptr++;
			else {
				*new_path++ = *ptr++;
				n++;
			}
		}
		
		if (*ptr==0) break;
		// *ptr==0 �ƴ϶�� *ptr=='.' �ΰ��̴�. 
		
		ptr++;		
		if (*ptr=='.') { // ../

			if (n < 2) continue;
			if (*(new_path-1)=='/') {
				new_path -= 2;
				n -= 2;
			}
			while (n && *new_path!='/') {
				n--;
				new_path--;
			}
			ptr++;
		}
		else if (*ptr=='/') { // ./
			//���� �������� ó�� 
		}
		else { // �׿� :  hello.html
			*new_path++ = '.';
			*new_path++ = *ptr++;
		}
	}
	*new_path = 0;
	return 0;
}

/**
	������ URL�� ���������� link �� merge�Ͽ� ���ο� URL�� construct.
	me �� parent ���� ��� �޴´�.
	@param parent : ���� HTML�������� URL
	@param child_link : ���� HTML ���������� <A> link�� url
	
*/ 

int URLParse::merge(URLst *parent)
{
	char path_merge[URLPARSE_PATHLEN*2+2];
	URLst	*me; /* myself. myself == child to the current URL page */

	me = &(this->url);

	if (me->host[0]==0 && parent->host[0]) {
		STRNCPY(me->host, parent->host, URLPARSE_HOSTLEN);
		me->port = parent->port;
	}
	
	if (me->path[0] != '/') {
		char *ptr;
		// parent->file != 0 �� ��� ó�� �ʿ� 		
		
		ptr = path_merge;
		strncpy(ptr, parent->path, parent->dir_len); 
		ptr += parent->dir_len;
		if (*(ptr-1)!='/') *ptr++ = '/';		

		strncpy(ptr, me->path, me->path_len); 
		ptr += me->path_len;
		*ptr = 0;
		
		trim_path(me->path, path_merge);		
	}

	if (me->port == 80) 
		sprintf(me->urlstr, "http://%s%s", me->host, me->path);
	else
		sprintf(me->urlstr, "http://%s:%d%s", me->host, me->port, me->path);
		
	return 0;
}

int	URLParse::isSubDir(URLParse &parent)
{
	char *givendir = this->url.path;
	char *basedir = parent.url.path;
	int	baselen = parent.url.dir_len;
	
	if (baselen==0) return 0;
	if (basedir[baselen-1]=='/') baselen --;
	
	if (strncmp(givendir, basedir, baselen)==0 && 
			(givendir[baselen]==0 || givendir[baselen]=='/') )
		return 1;
	return 0;
}

#ifdef TEST
void test_url_parse()
{
	URLParse A("http://www.anydic.com");
	A.print();

	URLParse B("http://www.anydic.com/aaa/bbb");
	B.print();

	URLParse C("http://www.anydic.com/aaa/bbb/yyy.cgi?aa=xx&bb=yy");
	C.print();

	URLParse D("aaa.html");
	D.print();

	D.inherit(A);
	D.print();

	URLParse E("/aaa.html");
	E.inherit(C);
	E.print();

	URLParse F("../../aaa.html");
	F.inherit(C);
	F.print();
	
}

int main(int argc, char *argv[])
{
	test_url_parse();
	return 0;
}
#endif


