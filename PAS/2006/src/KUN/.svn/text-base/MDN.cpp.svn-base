#include <stdlib.h>

#include "Common.h"
#include "MDN.h"

MDN::MDN()
{
	mdn = 0LL;
}

MDN::MDN(const MDN& value)
{
	*this = value;
}

MDN::MDN(int64_t value)
{
	*this = value;
}

MDN::MDN(const char* value)
{
	*this = value;
}

bool MDN::operator == (const MDN& rhs) const
{
	return mdn == rhs.mdn;
}

bool MDN::operator != (const MDN& rhs) const
{
	return mdn != rhs.mdn;
}

bool MDN::operator < (const MDN& rhs) const
{
	return mdn < rhs.mdn;
}

bool MDN::operator > (const MDN& rhs) const
{
	return mdn > rhs.mdn;
}

bool MDN::operator <= (const MDN& rhs) const
{
	return mdn <= rhs.mdn;
}

bool MDN::operator >= (const MDN& rhs) const
{
	return mdn >= rhs.mdn;
}

MDN& MDN::operator = (const MDN& rhs)
{
	mdn = rhs.mdn;

	return *this;
}

MDN& MDN::operator = (const int64_t rhs)
{
	mdn = rhs;

	return *this;
}

MDN& MDN::operator = (const char* rhs)
{
	mdn = atoll(rhs);
	return *this;
}

TinyString MDN::toString() const
{
	TinyString str;
	str.sprintf("%011lld", mdn);
	return str;
}

int64_t MDN::toInt64() const
{
	return mdn;
}

bool MDN::valid() const
{
	if(0 < mdn && mdn <= 99999999999LL)
		return true;
	else
		return false;
}

MDNType MDN::getType()
{
	if(mdn % 2 == 0)
		return MDNType_EVEN;
	else
		return MDNType_ODD;
}
