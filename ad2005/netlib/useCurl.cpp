#include <string.h>

#include <curl/curl.h>
#include <curl/types.h>
#include <curl/easy.h>

#include "useCurl.h"

#include <string.h>

#ifdef WIN32
#define	strcasecmp	_stricmp
#define	strncasecmp	_strnicmp
#endif

#ifndef WIN32
#include <strings.h>
#endif


/**
구현 참조:
 curl 매뉴얼
 curlpp (cpp wrapper) 의 curl.cpp, easy.cpp -- 각종 curl option 설정 예제

 --- CURL 의 기본 header -----
 GET / HTTP/1.1
User-Agent: libcurl-agent/1.0
Host: anydic.com
Pragma: no-cache
Accept: *\/\*
*/

/**
CURLcode curl_global_init(long flags ); 

flag:
CURL_GLOBAL_ALL Initialize everything possible. This sets all known bits. 
CURL_GLOBAL_SSL Initialize SSL 
CURL_GLOBAL_WIN32 Initialize the Win32 socket libraries. 
CURL_GLOBAL_NOTHING Initialise nothing extra. This sets no bit. 

RETURN VALUE
If this function returns non-zero, something went wrong and 
you cannot use the other curl functions

*/

/*
HTTP header만 받아오기.

--- easy.c
CURL *curl_easy_init(void)
{
  CURLcode res;
  struct SessionHandle *data;
  ..
  return data;
}

--- url.c
CURLcode Curl_open(struct SessionHandle **curl)
{
CURLcode res = CURLE_OK;
  struct SessionHandle *data;
  data = (struct SessionHandle *)calloc(1, sizeof(struct SessionHandle));
...
	data->set.httpreq = HTTPREQ_GET;
...
  *curl = data;
   return CURLE_OK;
}
*/

int	useCurl::prepare()
{
	CURLcode global_init;
	global_init =  curl_global_init(CURL_GLOBAL_ALL);
	return (int) global_init;
}

void useCurl::finish() 
{
	curl_global_cleanup();
}


useCurl::useCurl() 
{
	handle = curl_easy_init();
	databuf = NULL;
	datalen = 0;
	locationUrl = 0;
	contentsType = 0;
	req_headers = NULL;
}

useCurl::~useCurl()
{
	curl_easy_cleanup(handle);
	if (databuf) delete [] databuf;
	if (req_headers) 
		curl_slist_free_all(req_headers); /* free the req header list */
}

void	useCurl::defaultOpt()
{
	/* no progress meter please */
	curl_easy_setopt(handle, CURLOPT_NOPROGRESS, 1);

	/* shut up completely */
	curl_easy_setopt(handle, (CURLoption)CURLOPT_MUTE, 1);

	curl_easy_setopt(handle, CURLOPT_DNS_CACHE_TIMEOUT, 5);
	curl_easy_setopt(handle, CURLOPT_DNS_USE_GLOBAL_CACHE, 5);

	curl_easy_setopt(handle, CURLOPT_HTTPAUTH, CURLAUTH_ANY);
	
	curl_easy_setopt(handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");

	curl_easy_setopt(handle, CURLOPT_TIMEOUT, 6);

	/* response header 부분도 결과 메시지에 포함되게 한다. */
	curl_easy_setopt(handle, CURLOPT_HEADER, 1);

}

/**
-- curl_easy_getinfo() 는 Curl_getinfo() 를 호출 : easy.c
-- Curl_getinfo()  는 getinfo.c 에 정의
*/
void	useCurl::getInfo()
{
	curl_easy_getinfo(handle, CURLINFO_CONTENT_TYPE, &contentsType);
	curl_easy_getinfo(handle, CURLINFO_RESPONSE_CODE, &httpCode);
	curl_easy_getinfo(handle, CURLINFO_HEADER_SIZE, &headSize);
	if (contentsType==0) contentsType = "Content-Type Unknown";
}

void useCurl::printInfo()
{
	printf("contentsType = %s\n", contentsType);
	printf("httpCode = %d\n", httpCode);
	printf("headSize = %d\n", (int)headSize);
	printf("maxdatalen = %d\n", maxdatalen);
	printf("datalen = %d\n", datalen);
	printf("data= %s\n", databuf);
}

/**
user_passwd -- "myname:thesecret"
*/
void useCurl::setPasswd(char *user_passwd)
{
	 curl_easy_setopt(handle, CURLOPT_USERPWD, user_passwd);
}

void useCurl::setVerbose()
{
	
	curl_easy_setopt(handle, CURLOPT_VERBOSE , TRUE);
}

/**
User-Agent 값을 바꾼다. 

Default == User-Agent: libcurl-agent/1.0
*/
void useCurl::setUserAgent(char *agentname)
{
	char user_agent[128];
	
	sprintf(user_agent, "User-Agent: %s", agentname);
	req_headers = curl_slist_append(req_headers, user_agent);
	/* pass our list of custom made headers */
	curl_easy_setopt(handle, CURLOPT_HTTPHEADER, req_headers);
}


size_t useCurl::write_data(void *ptr, size_t size, size_t nmemb,
                              void *ourpointer)
{
/* do what you want with the data */
	useCurl	*obj = (useCurl *) ourpointer;
	int	realsize = size * nmemb;
	if (obj->datalen + realsize < obj->maxdatalen) {
		memcpy(obj->databuf + obj->datalen, ptr, realsize);
		obj->datalen += realsize;
		obj->databuf[obj->datalen] =0;
	}
	return realsize;
}

/**
curl_easy_perform() 을 수행하여 결과 메시지를 메모리에 저장한다.
return : HTML data 크기.
*/
int	useCurl::httpGet(char *urladdr, int _maxdatalen)
{
	datalen = 0;
	/* memory 할당  */	
	maxdatalen = _maxdatalen;
	if (databuf != NULL) delete [] databuf;
	
	databuf = new char[maxdatalen];		
	if (databuf == NULL) return -1;
	
	defaultOpt();
	curl_easy_setopt(handle, CURLOPT_URL , urladdr);

	/* send all data to this function  */
	curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, useCurl::write_data );

	/* we pass our 'object' to the callback function */
	curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void *)this);

	curl_easy_perform(handle);
	getInfo();
	
	return datalen;
}

/**
redirect 되는 경우, header 에서  location  값을 구하여 return.
*/
char *useCurl::location()
{
	char *line = databuf;

	if (databuf==0) return 0;
	if (locationUrl) return locationUrl;
	
	while(1) {
		if (line[0]=='\n' || line[0]=='\r') break;
		
		if (strncasecmp(line, "LOCATION", 8)==0) {
			char *mark = strchr(line, ':');
			if (mark==0) return 0;
			mark++;
			if (mark[0]==' ') mark++;

			locationUrl = mark;
			
			for(char *ptr = locationUrl; *ptr; ptr++)
				if (*ptr=='\n' || *ptr=='\r') {
					*ptr = 0;
					break;
				}
				
			return locationUrl;			
		}
		/* else */
		line = strchr(line+1, '\n');
		if (line) line++;	
		else break;
	}
	
	return 0;
}

/**
결과 메시지에서 header부분과 body부분을 분리하기 위한 것.

getInfo() 에서 headSize 를 구하기 때문에 필요없다.
*/
char *useCurl::find_httpbody(char *httpresp_buf)
{
	char *ptr = httpresp_buf;
	
	while (1) {
		while (*ptr!='\n' && *ptr) ptr++;
		if (*ptr==0) return 0;
		if (ptr[1]=='\n') return (ptr+2);
		if (ptr[1]=='\r' && ptr[2]=='\n') return (ptr+3);
		ptr++;
	}
	return 0;
}


