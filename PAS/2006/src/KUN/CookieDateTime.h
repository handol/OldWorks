#ifndef __COOKIEDATETIME_H__
#define __COOKIEDATETIME_H__

#include <time.h>

class CookieDateTime
{
public:
	CookieDateTime(void);
	~CookieDateTime(void);

	time_t toSecond();

public:
	short second;
	short minute;
	short hour;
	short day;
	short month;
	short year;
	short weekDay;
};

#endif // __COOKIEDATETIME_H__
