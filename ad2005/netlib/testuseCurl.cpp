#include <stdio.h>
#include "useCurl.h"

int testCurl()
{
	useCurl url;
	char *bodyptr;
	url.setVerbose();
	url.httpGet("http://anydic.com");
	url.printInfo();
	bodyptr = url.body();
	if (bodyptr) printf("** %c[%d] %c[%d]\n", bodyptr[0], bodyptr[0], bodyptr[1], bodyptr[1]);
	url.httpGet("http://www.cnn.com");
	url.printInfo();
	bodyptr = url.body();
	if (bodyptr) printf("** %c[%d] %c[%d]\n", bodyptr[0], bodyptr[0], bodyptr[1], bodyptr[1]);
	
	return 0;
}

int main(int argc, char argv[])
{
	useCurl::prepare();

	testCurl();
	
	useCurl::finish();
}
