#include <iostream.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "PasLog.h"
#include "CookieDateTime.h"
#include "CookieDB.h"

#define DATEPIVOTYEAR   30						  // 2-digit years less than this are 20xx,  greater-or-equal are 19xx


void CookieDB::set( const cookie_t& cookie )
{
	CookieLine line;
	line.set(cookie);

	CookieItem item;

	item.domain = line.get("domain");
	line.remove("domain");

	item.path = line.get("path");
	line.remove("path");

	MediumString expires = line.get("expires");
	line.remove("expires");
	if(expires.isEmpty())
		item.expires = 0;
	else
		item.expires = parseCookieDate(expires);

	item.secure = line.exist("secure");
	line.remove("secure");

	for(size_t i = 0; i < line.size(); ++i)
	{
		item.name = line[i].first;
		item.value = line[i].second;

		add(item);
	}

	printLog();
}

cookie_t CookieDB::get( const host_t& domain, const path_t& path )
{
	CookieLine result;

	VCookieItem::iterator it = _items.begin();
	VCookieItem::iterator itE = _items.end();

	PAS_DEBUG2("Get cookie : domain[%s] path[%s]", domain.c_str(), path.c_str());

	for( ; it != itE; ++it)
	{
		// 도메인은 대소문자를 구분하지 않고, path는 구분한다.
		// PAS의 쿠키는 UserInfo 가 살아 있는 동안만 유효하므로 expires 값은 무시하고 항상 쿠키를 적용한다.
		// 이것은 일반 웹브라우져가 재시작하지 않는 한 expires된 쿠키가 유효한 것과 동일한 처리이다.
		if(it->domain.incaseFind(domain) >= 0 &&  it->path.find(path) >= 0)
		{
			result.add(it->name, it->value);
		}
		else
		{
			PAS_TRACE2("Cookie no match : domain[%s] path[%s]",
				it->domain.c_str(), it->path.c_str());
		}
	}

	return result.toCookie();
}

time_t CookieDB::parseCookieDate( const char* date )
{
	CookieDateTime dt;

	if (isalpha(*date))
	{
		// Skip "Dow, "
		do
		{
			++date;
		} while (isalpha(*date));

		if (*date == ',') ++date;

		if (*date == '\040') ++date;

		if ( skipNum(&date, &dt.day)
			&& skipMon(&date, &dt.month)
			&& skipNum(&date, &dt.year)
			&& skipNum(&date, &dt.hour)
			&& skipNum(&date, &dt.minute)
			&& skipNum(&date, &dt.second) )
		{
			// Now pszDate should begin with " GMT" ...  should I verify this???
			// Success : calculate date in seconds from  day,mon,year,hour,min,sec

			if (dt.year < DATEPIVOTYEAR)
				dt.year += 2000;
			else if (dt.year <= 99)
				dt.year += 1900;

			return dt.toSecond();
		}
	}

	ThrowException(ECODE_INVALID_PARAMETER);
}

bool CookieDB::skipMon( const char* *ppsz, short *pnumResult )
{
	char c;
	int nn;
	bool bRet = 0;
	const char* psz = *ppsz;

	//SKIPWHITE(psz);
	while (isspace(*(psz))) ++(psz);

	nn = match3Letter("janfebmaraprmayjunjulaugsepoctnovdec", psz);

	if (nn >= 0)
	{
		bRet = 1;
		*pnumResult = nn;
		psz += 3;
		if ( (c = *psz) == '-' || c == ':' || c == ',' || c == '/') ++psz;
	}

	*ppsz = psz;
	return bRet;
}

bool CookieDB::skipNum( const char* *ppsz, short *pnumResult )
{
	int nNum = 0;
	bool bIsNum = 0;
	const char* psz = *ppsz;

	//SKIPWHITE(psz);
	while (isspace(*(psz))) ++(psz);

	if (isdigit(*psz))
	{
		char c;
		bIsNum = 1;

		while (isdigit( (c = *psz) ))
		{
			nNum = nNum * 10 + c - '0';
			++psz;
		}

		if (c == '-' || c == ':' || c == ',' || c == '/') ++psz;

		*pnumResult = nNum;
	}

	*ppsz = psz;
	return bIsNum;
}

int CookieDB::match3Letter( const char* haystack, const char* niddle )
{
	char c;
	char c0 = tolower(*niddle);				  // assume it's a letter
	int nn = 0;

	for (;;)
	{
		c = *haystack;
		if (c == '\0')
		{
			nn = -1;
			break;
		}
		if (c == c0
			&& haystack[1] == tolower(niddle[1])
			&& haystack[2] == tolower(niddle[2]))
		{
			break;
		}
		++nn;
		haystack += 3;
	}

	return nn;
}

void CookieDB::add( const CookieItem& item )
{
	VCookieItem::iterator it = _items.begin();
	VCookieItem::iterator itE = _items.end();

	for( ; it != itE; ++it)
	{
		// 도메인은 대소문자를 구분하지 않고, path는 구분한다.
		if(it->domain.incaseEqual(item.domain) && it->path == item.path && it->name.incaseEqual(item.name))
		{
			// update
			*it = item;
			return;
		}
	}

	// add
	_items.push_back(item);
}

void CookieDB::printLog()
{
	VCookieItem::iterator it = _items.begin();
	VCookieItem::iterator itE = _items.end();

	for( ; it != itE; ++it)
	{
		PAS_DEBUG5("Stored Cookie : domain[%s] path[%s] name[%s] value[%s] expires[%d]",
			it->domain.c_str(), it->path.c_str(), it->name.c_str(), it->value.c_str(), it->expires);
	}
}
