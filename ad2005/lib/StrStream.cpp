#include "StrStream.h"

/*
_vsnprintf(), vsnprintf_s() ���� --> http://msdn2.microsoft.com/en-us/library/ms235384(vs.80).aspx
.Net �̶� ���� C++ �̶� �ణ �Լ� �̸� �ٸ�.
*/

void StrStream::add(char *fmt, ...)
{
	int	len=0;
	if (bufPtr==0) return;
	va_start(args, fmt);
	_vsnprintf(bufPtr, maxLen-bufLen-1, fmt, args);
	va_end(args);
	len = strlen(bufPtr);
	bufPtr += len;
	bufLen += len;
	*bufPtr = 0;
}

void StrStream::rawadd(char *string, int len)
{
	if (bufPtr==0) return;
	if (len==0) len = strlen(string);

	if (len < maxLen - bufLen) {
		strncpy(bufPtr, string, len);
		bufPtr += len;
		bufLen += len;
		*bufPtr = 0;
	}
 }