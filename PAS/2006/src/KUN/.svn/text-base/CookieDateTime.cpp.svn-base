#include "CookieDateTime.h"

CookieDateTime::CookieDateTime(void)
{
}

CookieDateTime::~CookieDateTime(void)
{
}

time_t CookieDateTime::toSecond()
{
	struct tm tmpTM;
	tmpTM.tm_year = year - 1900;
	tmpTM.tm_mon = month -1;
	tmpTM.tm_mday = day;
	tmpTM.tm_hour = hour;
	tmpTM.tm_min = minute;
	tmpTM.tm_sec = second;

	return mktime(&tmpTM);
}
