#include "Common.h"
#include "HttpUrlParser.h"
#include "PasLog.h"

using namespace HTTP;

UrlParser::UrlParser(void)
{
}

UrlParser::~UrlParser(void)
{
}

int UrlParser::parse(const char* url)
{
	ACE_ASSERT(url != NULL);
	ACE_ASSERT(strlen(url) > 0);

	if(strlen(url) <= 0)
	{
		PAS_NOTICE("URL is empty");
		return -1;
	}

	clear();

	url_t urlStr(url);
	urlStr.trim();

	// parsing host
	int pos = urlStr.incaseFind("http://");
	// if exist http scheme then skip scheme
	if(pos == 0)
		pos += 7;
	
	// if not exist http scheme
	else
	{
		pos = urlStr.incaseFind("ftp://");
		
		// if exist ftp scheme then skip scheme
		if(pos == 0)	
			pos += 6;
		else
			pos = 0;
	}
	
	int pos2 = urlStr.find("/", pos);
	
	// only host
	if(pos2 < 0)
	{
		PAS_TRACE("UrlParser::parse >> only host");
		urlStr.substr(&host, pos);
		path = "/";
	}

	// only path
	else if(pos2 == 0)
	{
		PAS_TRACE("UrlParser::parse >> only path");
		path = urlStr;
	}

	// host + path
	else
	{
		PAS_TRACE("UrlParser::parse >> host + path");
		urlStr.substr(&host, pos, pos2 - pos);
		urlStr.substr(&path, pos2);
	}
	
	// parsing port
	pos = host.find(":");
	
	// not exist port 
	if(pos < 0)
	{
		// set default port
		port = 80;
	}

	// exist port
	else
	{
		port = atoi(host.substr(pos+1));
		host.erase(pos);
	}

	return 0;
}

host_t UrlParser::getHost()
{
	return host;
}

int UrlParser::getPort()
{
	return port;
}

path_t UrlParser::getPath()
{
	return path;
}

void UrlParser::clear()
{
	host.clear();
	port = 0;
	path.clear();
}


void UrlParser::test()
{
	typedef struct _TestCase {
		/*-- 입력 데이타 */
		const char *orgUrl; /** 원본 Url  */

		/*-- 결과 테이타 : 확인 --*/
		const char *host;
		const int port;
		const char *path;
	} TestCase;

	TestCase testcases[] = {
		{ "211.174.49.200", "211.174.49.200",  80, "/"},
		{ "http://211.174.49.200", "211.174.49.200",  80, "/"},
		{ "http://211.174.49.200/", "211.174.49.200",  80, "/"},
		{ "http://211.174.49.200/hello.html", "211.174.49.200",  80, "/hello.html"},

		// port number
		{ "http://211.174.49.200:4000", "211.174.49.200",  4000, "/"},
		{ "http://211.174.49.200:5000/hello.html", "211.174.49.200", 5000, "/hello.html"},

		{ NULL, NULL, 0, NULL}
	};

	UrlParser url;

	TestCase *testcase = 0;
	int casenumber=0;

	/*-- --*/
	for (testcase = testcases ,casenumber=0;
			casenumber < static_cast<int>(sizeof(testcases)/sizeof(TestCase));
			casenumber++, testcase++)
	{
		if (testcase->orgUrl ==NULL ) break;
		url.parse(testcase->orgUrl);
		int isFail = strcmp( url.getHost().toStr(), testcase->host) != 0
			|| url.getPort() !=  testcase->port
			|| strcmp(url.getPath().toStr(), testcase->path) !=0;

		if (isFail)
			printf("FAIL:");
		else
			printf("SUCC:");

		printf("%s --> %s  %d  %s\n", testcase->orgUrl,
			url.getHost().toStr(), url.getPort(), url.getPath().toStr() );
	}

}

#ifdef TEST_MAIN

int main()
{
	UrlParser url;

	url.test();
}

#endif

HTTP::UrlParser::UrlParser( const char* url )
{
	parse(url);
}

