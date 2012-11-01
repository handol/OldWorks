#include "CGI.h"

int CGI::cgiHexValue[256];
	
void CGI::cgiSetupConstants()
{
	int i;
	for (i=0; (i < 256); i++) {
		cgiHexValue[i] = 0;
	}
	cgiHexValue[static_cast<int>('0')] = 0;	
	cgiHexValue[static_cast<int>('1')] = 1;	
	cgiHexValue[static_cast<int>('2')] = 2;	
	cgiHexValue[static_cast<int>('3')] = 3;	
	cgiHexValue[static_cast<int>('4')] = 4;	
	cgiHexValue[static_cast<int>('5')] = 5;	
	cgiHexValue[static_cast<int>('6')] = 6;	
	cgiHexValue[static_cast<int>('7')] = 7;	
	cgiHexValue[static_cast<int>('8')] = 8;	
	cgiHexValue[static_cast<int>('9')] = 9;
	cgiHexValue[static_cast<int>('A')] = 10;
	cgiHexValue[static_cast<int>('B')] = 11;
	cgiHexValue[static_cast<int>('C')] = 12;
	cgiHexValue[static_cast<int>('D')] = 13;
	cgiHexValue[static_cast<int>('E')] = 14;
	cgiHexValue[static_cast<int>('F')] = 15;
	cgiHexValue[static_cast<int>('a')] = 10;
	cgiHexValue[static_cast<int>('b')] = 11;
	cgiHexValue[static_cast<int>('c')] = 12;
	cgiHexValue[static_cast<int>('d')] = 13;
	cgiHexValue[static_cast<int>('e')] = 14;
	cgiHexValue[static_cast<int>('f')] = 15;
}


/**
cp : 소스 
s: 데스트
*/
cgiUnescapeResultType CGI::cgiUnescapeChars(char *s, int max_dest, const char *cp, int len)
{

	cgiEscapeState escapeState = cgiEscapeRest;
	int escapedValue = 0;
	int srcPos = 0;
	int dstPos = 0;
	
	while (srcPos < len && dstPos < max_dest) {
		int ch = cp[srcPos];
		switch (escapeState) {
			case cgiEscapeRest:
			if (ch == '%') {
				escapeState = cgiEscapeFirst;
			} else if (ch == '+') {
				//if (dstPos && s[dstPos]!=' ') // dahee(2002.9.13): trim spaces
				s[dstPos++] = ' ';
			} else {
				s[dstPos++] = ch;	
			}
			break;
			case cgiEscapeFirst:
			escapedValue = cgiHexValue[ch] << 4;	
			escapeState = cgiEscapeSecond;
			break;
			case cgiEscapeSecond:
			escapedValue += cgiHexValue[ch];
			s[dstPos++] = escapedValue;
			escapeState = cgiEscapeRest;
			break;
		}
		srcPos++;
	}
	/*
	// trimming
	if (dstPos && s[dstPos-1]==' ')
		s[dstPos-1]=0;
	*/
	s[dstPos] = '\0';
	return cgiUnescapeSuccess;
}		
	

