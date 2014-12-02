#include "simpleStr.h"

void	simpleStr::add(char *str, int _len=0) {
	if (_len==0) _len = strlen(str);
	if (len + _len > SIMPLE_STR_MAX) return ;
	strcpy(ptr, str);
	len += _len;
	ptr += _len;
}

void	simpleStr::add(int ch) {
	if (len + 1 > SIMPLE_STR_MAX) return ;
	*ptr++ = ch;
	*ptr = 0;
	len += 1;
}

