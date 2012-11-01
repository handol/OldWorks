#include "CookieLine.h"

typedef CookieLine::Pair Pair;
typedef CookieLine::Pairs Pairs;

CookieLine::CookieLine(void)
{
}

CookieLine::~CookieLine(void)
{
}

Pair& CookieLine::operator[] (int index)
{
	ACE_ASSERT(0 <= index);
	ACE_ASSERT(index < size());
	return _pairs[index];
}

const Pair& CookieLine::operator[] (int index) const
{
	ACE_ASSERT(0 <= index);
	ACE_ASSERT(index < size());
	return _pairs[index];
}

void CookieLine::set( const cookie_t& cookie )
{
	cookies_t cookies = explode(cookie, ';');

	trim(cookies);
	makePairs(cookies);
}

MediumString CookieLine::get( const SmallString& name ) const
{
	Pairs::const_iterator it = find(name);
	if(it == _pairs.end())
		return MediumString();

	return it->second;
}

void CookieLine::remove( const SmallString& name )
{
	Pairs::iterator it = find(name);
	if(it == _pairs.end())
		return;

	_pairs.erase(it);
}

size_t CookieLine::size() const
{
	return _pairs.size();
}

Pairs::const_iterator CookieLine::find( const SmallString& name ) const
{
	Pairs::const_iterator it = _pairs.begin();
	Pairs::const_iterator itE = _pairs.end();

	for( ; it != itE; ++it)
	{
		if(it->first.incaseEqual(name))
			return it;
	}

	return itE;
}

Pairs::iterator CookieLine::find( const SmallString& name )
{
	Pairs::iterator it = _pairs.begin();
	Pairs::iterator itE = _pairs.end();

	for( ; it != itE; ++it)
	{
		if(it->first.incaseEqual(name))
			return it;
	}

	return itE;
}

void CookieLine::trim( cookies_t& cookies )
{
	cookies_t::iterator it = cookies.begin();
	cookies_t::iterator itE = cookies.end();

	for( ; it != itE; ++it)
	{
		it->trim();
	}
}

void CookieLine::makePairs( cookies_t &cookies )
{
	_pairs.clear();
	_pairs.reserve(cookies.size());

	cookies_t::iterator it = cookies.begin();
	cookies_t::iterator itE = cookies.end();

	for( ; it != itE; ++it)
	{
		SmallString name;
		MediumString value;

		int pos = it->split(name, "=");
		value = it->substr(pos+1);

		name.trim();
		value.trim();

		Pair pair;
		pair.first = name;
		pair.second = value;

		_pairs.push_back(pair);
	}
}

bool CookieLine::exist( const SmallString& name ) const
{
	return (find(name) != _pairs.end());
}

void CookieLine::add( const SmallString& name, const MediumString& value )
{
	if(exist(name))
		return;

	Pair pair;
	pair.first = name;
	pair.second = value;
	_pairs.push_back(pair);
}

cookie_t CookieLine::toCookie() const
{
	cookie_t cookie;

	Pairs::const_iterator it = _pairs.begin();
	Pairs::const_iterator itE = _pairs.end();

	for( ; it != itE; ++it)
	{
		cookie += it->first;
		cookie += "=";
		cookie += it->second;
		cookie += "; ";
	}

	// remove last "; "
	if(!cookie.isEmpty())
		cookie.erase(cookie.size()-2);

	return cookie;
}
