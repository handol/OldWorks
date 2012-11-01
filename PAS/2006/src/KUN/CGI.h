#ifndef CGI_H
#define CGI_H

typedef enum {
	cgiEscapeRest,
	cgiEscapeFirst,
	cgiEscapeSecond
} cgiEscapeState;

typedef enum {
	cgiUnescapeSuccess,
	cgiUnescapeMemory
} cgiUnescapeResultType;

class CGI
{
public:
	static void cgiSetupConstants() ;
	static cgiUnescapeResultType cgiUnescapeChars(char *s, int max_dest, const char *cp, int len);

private:
	static int cgiHexValue[256];
};

#endif
