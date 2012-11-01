#ifndef __ASCII_SERIALIZER_H__
#define __ASCII_SERIALIZER_H__

#include "Serializer.h"
#include "Archive.h"

class AsciiSerializer : public Serializer
{
public:
	AsciiSerializer(const char spliter) : _spliter(spliter)
	{
	}

	virtual int serialize(Archive& ar) const = 0;
	virtual int unserialize(Archive& ar) = 0;

protected:
#ifndef __sparcv9
	void push(Archive& ar, const time_t& value) const;
	void pop(Archive& ar, time_t& value);
#endif

	void push(Archive& ar, const int& value) const;
	void push(Archive& ar, const int64_t& value) const;
	void push(Archive& ar, const bool& value) const;

	void pop(Archive& ar, int& value);
	void pop(Archive& ar, int64_t& value);
	void pop(Archive& ar, bool& value);

	void pushSpliter(Archive& ar) const;

protected:
	const char _spliter;
};

#endif //__ASCII_SERIALIZER_H__
