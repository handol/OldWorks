#include "Exception.h"
#include "PasLog.h"

#include "AsciiSerializer.h"

#ifndef __sparcv9

void AsciiSerializer::push(Archive& ar, const time_t& value) const
{
	TinyString str(static_cast<int>(value));
	ar.push(str.toStr(), str.size());
}

void AsciiSerializer::pop(Archive& ar, time_t& value)
{
	char buf[1024];
	int readSize;

	readSize = ar.popFirstOf(buf, sizeof(buf), _spliter);
	
	if(readSize <= 0)
		ThrowException(ECODE_NO_DATA);

	if(readSize == sizeof(buf))
		ThrowException(ECODE_NOT_ENOUGH_SPACE);

	buf[readSize] = '\0';
	value = atoi(buf);
}

#endif

void AsciiSerializer::push(Archive& ar, const int& value) const
{
	TinyString str(value);
	ar.push(str.toStr(), str.size());
}

void AsciiSerializer::push(Archive& ar, const int64_t& value) const
{
	TinyString str(value);
	ar.push(str.toStr(), str.size());
}

void AsciiSerializer::push(Archive& ar, const bool& value) const
{
	if(value)
		ar.push('1');
	else
		ar.push('0');
}

void AsciiSerializer::pop(Archive& ar, int& value)
{
	char buf[1024];
	int readSize;

	readSize = ar.popFirstOf(buf, sizeof(buf), _spliter);
	
	if(readSize <= 0)
		ThrowException(ECODE_NO_DATA);

	if(readSize == sizeof(buf))
		ThrowException(ECODE_NOT_ENOUGH_SPACE);

	buf[readSize] = '\0';
	value = atoi(buf);
}

void AsciiSerializer::pop(Archive& ar, int64_t& value)
{
	char buf[1024];
	int readSize;

	readSize = ar.popFirstOf(buf, sizeof(buf), _spliter);
	
	if(readSize <= 0)
		ThrowException(ECODE_NO_DATA);

	if(readSize == sizeof(buf))
		ThrowException(ECODE_NOT_ENOUGH_SPACE);

	buf[readSize] = '\0';
	value = atoll(buf);
}

void AsciiSerializer::pop(Archive& ar, bool& value)
{
	char buf[1024];
	int readSize;

	readSize = ar.popFirstOf(buf, sizeof(buf), _spliter);
	
	if(readSize <= 0)
		ThrowException(ECODE_NO_DATA);

	if(readSize == sizeof(buf))
		ThrowException(ECODE_NOT_ENOUGH_SPACE);

	buf[readSize] = '\0';
	
	if(strcmp(buf, "0") == 0)
		value = false;
	else if(strcmp(buf, "1") == 0)
		value = true;
	else
		ThrowException(ECODE_WRONG_DATA);
}

void AsciiSerializer::pushSpliter(Archive& ar) const
{
	ar.push(_spliter);
}
