#ifndef __COOKIEITEM_H__
#define __COOKIEITEM_H__

#include "Common.h"

class CookieItem
{
public:
	CookieItem(void);
	~CookieItem();

public:
	SmallString domain;
	MediumString path;
	bool secure;
	time_t expires;
	SmallString name;
	MediumString value;
};

typedef std::vector<CookieItem> VCookieItem;

#endif // __COOKIEITEM_H__
