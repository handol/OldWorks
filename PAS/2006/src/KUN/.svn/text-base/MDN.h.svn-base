#ifndef __MDN_H__
#define __MDN_H__

#include <sys/types.h>

#include "Common.h"

enum MDNType
{
	MDNType_ODD,
	MDNType_EVEN
};

class MDN
{
public:
	MDN();
	MDN(const MDN& value);
	MDN(const int64_t value);
	MDN(const char* value);

	bool operator == (const MDN& rhs) const;
	bool operator != (const MDN& rhs) const;
	bool operator < (const MDN& rhs) const;
	bool operator > (const MDN& rhs) const;
	bool operator <= (const MDN& rhs) const;
	bool operator >= (const MDN& rhs) const;

	MDN& operator = (const MDN& rhs);
	MDN& operator = (const int64_t rhs);
	MDN& operator = (const char* rhs);

	operator int64_t() const
	{
		return toInt64();
	}

	operator TinyString() const
	{
		return toString();
	}

	TinyString toString() const;
	int64_t toInt64() const;
	
	bool valid() const;
	MDNType getType();
	

private:
	int64_t mdn;
};

#endif






