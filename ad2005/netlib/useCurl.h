#ifndef USECURL_H
#define USECURL_H

#ifndef TRUE
	#define	TRUE 1
#endif

/*
http://www.seoconsultants.com/w3c/status-codes/

HTTP Status Codes that the Web Server can return - Definitions
200 OK 
301 Moved Permanently 
302 Found 
304 Not Modified 
307 Temporary Redirect 
400 Bad Request 
401 Unauthorized 
403 Forbidden 
404 Not Found 
410 Gone 
500 Internal Server Error 
501 Not Implemented 

---------
301, 302, 307 은 redirect 되는 것이다.
header 에 Location: 값이 지정되어 있다.

--------- http res header --------
HTTP/1.1 301 Moved Permanently
Cache-Control: private
Content-Length: 0
Content-Type: text/html
Location: http://www.seoconsultants.com/
Server: Microsoft-IIS/6.0
MicrosoftOfficeWebServer: 5.0_Pub


HTTP/1.1 302 Found
Cache-Control: private
Content-Length: 0
Content-Type: text/html
Location: http://www.seoconsultants.com/
Server: Microsoft-IIS/6.0
MicrosoftOfficeWebServer: 5.0_Pub


HTTP/1.1 307 Temporary Redirect
Cache-Control: private
Content-Length: 0
Content-Type: text/html
Location: http://www.seoconsultants.com/
Server: Microsoft-IIS/6.0
MicrosoftOfficeWebServer: 5.0_Pub

*/

#include <curl/curl.h>

#define	MYCURL_MAX_DATASIZE	(1024*20)
class useCurl {

/** static */
public:	
		
	static int prepare();
	static void finish();
	
private:
	static size_t write_data(void *ptr, size_t size, size_t nmemb,
                              void *ourpointer);
	
/*-- static END ---*/

public:
	useCurl();
	~useCurl();
	void setPasswd(char *user_passwd);
	void setVerbose();
	void setUserAgent(char *agentname);

	char *find_httpbody(char *httpresp_buf);
	int	httpGet(char *urladdr, int	_maxdatasize=MYCURL_MAX_DATASIZE);
	int	httpResult() {
		return httpCode;
	}
	int	isRedirect() {
		return (httpCode==301 || httpCode==302 || httpCode==307);
	}

	char *location() ; // redirect 되는 경우, header 에서  location  값을 구하여 return;
	char *httpType() {
		return contentsType;
	}
	void printInfo();
	char *header() {
		if (databuf) {
			databuf[headSize-1] = 0;
			return databuf;
		}
		else	return NULL;
	}
	char *body() {
		if (databuf) return databuf + headSize;
		else	return NULL;
	}
	int	bodysize() {
		return datalen - headSize;
	}
		
private:
	void	defaultOpt();
	void	getInfo();

	CURL *handle;

	/** http info */
	char *contentsType;
	char *locationUrl;
	int	headSize;
	int	httpCode;
	//int	httpVersion;
	int		maxdatalen;
	char	*databuf;
	int		datalen;

	//
	struct curl_slist *req_headers;

	
};

#endif
