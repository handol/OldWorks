#include "CGI.h"
#include "Spliter.h"

#include <stdio.h>
#include <stdlib.h>

void	CGI::init()
{
	numVals = 0;
	_method[0] = 0;
	
	varvals = new VarVal [MAX_VARVALS];
	if (varvals)
	memset(varvals, 0, sizeof(VarVal) * MAX_VARVALS);
}

int	CGI::parse(char *httpreq)
{
	char tmp[12];
	char path[1024];
	char	varname[CGI_VARNAME_LEN+1];
	char	varval[CGI_VARVAL_LEN+1];
	
	/* HTTP header parsing */
	Spliter header(httpreq, 0, ' ');
	header.getNext(tmp, sizeof (tmp) - 1); // method: GET
	header.getNext(path, sizeof (path) - 1);	// url, path: /search?query=hello
	//header.getNext(tmp, 23);	// http version: HTTP/1.1

	/* path parsing */
	Spliter pathsplit(path, 0, 0);
	pathsplit.getNext(tmp, 1, '/');
	pathsplit.getNext(_method, CGI_METHOD_LEN, '?');

	while(! pathsplit.isend() ) {
		pathsplit.getNext(varname, CGI_VARNAME_LEN, '=');
		pathsplit.getNext(varval, CGI_VARVAL_LEN, '&');
		add(varname, varval);
	}
	return 0;	
}

int	CGI::add(char *name, char *val)
{
	if (numVals >= MAX_VARVALS) return -1;

	strcpy(varvals[numVals].name, name);
	unescape(varvals[numVals].strval, val);
	numVals++;
	return 0;
}

/**
Get string value of the variable with a given name.
*/
char *CGI::getVal(char *name)
{
	for(int i=0; i<numVals; i++) {
		if (strcmp(varvals[i].name, name)==0)
			return	varvals[i].strval;
	}
	return "";
}

/**
Get integer value of the variable with a given name.
*/
int	CGI::getIntVal(char *name)
{
	return strtol( getVal(name), 0, 10 );
}

void	CGI::print()
{
	for(int i=0; i<numVals; i++)
	printf("%s = %s\n", varvals[i].name, varvals[i].strval);
}

void	CGI::print(StrStream &stream)
{
	for(int i=0; i<numVals; i++)
	stream.add("%s = %s\n", varvals[i].name, varvals[i].strval);
}

/**
%, + 기호 변환 
*/
void	CGI::unescape(char *dest, char *src)
{
	char num[3];	
	int	ch;
	num[2] = 0;
	
	for(; *src; src++) {
		if (*src=='%') {
			src++;
			num[0] = *src;
			src++;
			num[1] = *src;
						
			ch = strtol(num, 0, 16);	
			*dest++ = ch;
		}
		else if (*src=='+') {
			*dest++ = ' ';
		}
		else {
			*dest++ = *src;
		}
	}
	*dest = 0;
}

/**
특수문자이거나 한글 문자이면 escape (%XX) 필요.
*/
void	CGI::escape(char *dest, char *src)
{
	char *found;
	for(; *src; src++) {
		found = strchr("&?=$:/", *src);
		if (found || (*src & 0x80) ) {
			*dest++ = '%';
			sprintf(dest, "%02X", *src);
			dest += 2;
		}
		else if (*src==' ') {
			*dest++ = '+';
		}
		else {
			*dest++ = *src;
		}
	}
	*dest = 0;
}