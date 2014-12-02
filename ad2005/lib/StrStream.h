#ifndef STRSTREAM_H
#define	STRSTREAM_H

#include <stdio.h>
#include <stdarg.h>
#include <string.h>


class StrStream {
	public:

	StrStream() {
		bufPtr = 0;
		buff = 0;
		bufLen = 0;
		maxLen = 0;
	}
	
	void init (int maxbytes) {		
		maxLen = maxbytes - 1 ;
		bufPtr = buff = new char[maxbytes];
		buff[0] = 0;
		
	}
	~StrStream() {
		if (buff) delete [] buff;
	}
	void add(char *fmt, ...);
	void rawadd(char *string, int	 len =0);
	char *str() {
		return buff;
	}
	int	len() {
		return bufLen;
	}

	void	print() {
		if (buff) printf("%s", buff);
	}
	
	private:

	va_list args;

	char    *buff;
	char    *bufPtr;
	int bufLen;
	int	maxLen;
};
#endif
