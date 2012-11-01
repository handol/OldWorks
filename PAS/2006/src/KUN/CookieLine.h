#ifndef __COOKIELINE_H__
#define __COOKIELINE_H__

#include "Common.h"

class CookieLine
{
public:
	typedef std::pair<SmallString, MediumString> Pair;
	typedef std::vector<Pair> Pairs;

public:
	CookieLine(void);
	~CookieLine(void);

	Pair& operator[] (int index);
	const Pair& operator[] (int index) const;
	void set(const cookie_t& cookie);
	MediumString get(const SmallString& name) const;
	void add(const SmallString& name, const MediumString& value);
	void remove(const SmallString& name);
	size_t size() const;
	bool exist(const SmallString& name) const;
	cookie_t toCookie() const;
	
private:
	Pairs::const_iterator find(const SmallString& name) const;
	Pairs::iterator find(const SmallString& name);
	void trim(cookies_t& cookies);
	void makePairs( cookies_t &cookies );

private:
	Pairs _pairs;
	
};

#endif // __COOKIELINE_H__
