#include <stdio.h>

#include "Exception.h"

const char* ExceptionStrings[] = 
{
	"Begin",

	"Unknown",
	"Error",
	"Success",
	"Invalid parameter",
	"Initialize fail",
	"Query fail",

	"Connect fail",
	"Not connected",
	"Disconnected",
	"Recv fail",
	"Send fail",

	"Wrong data",
	"Not enough space",
	"No data",
	"No more data",
	"Timeout",
	"No exist",
	"Already exist",

	"Open fail",
	"Not opened",
	"Read fail",
	"Write fail",
	"Close fail",
	"Seek fail",

	"End"
};

const char* Exception::toString()
{
	return c_str();
}

const char* Exception::c_str()
{
	if(ECODE_BEGIN < _code && _code < ECODE_END)
	{
		int codeNum = static_cast<int>(_code);
		snprintf(_errorString, sizeof(_errorString), "Exception[%s:%d:%s]", 
			_file, _line, ExceptionStrings[codeNum]);
	}
	else
	{
		snprintf(_errorString, sizeof(_errorString), "Exception[%s:%d:%s]", 
			_file, _line, "Incorrect ecode");
	}

	return _errorString;
}

Exception::Exception( const char* file, const int line, ExceptionCode exceptionCode ) : _code(exceptionCode), _file(file), _line(line)
{

}

ExceptionCode Exception::get() const
{
	return _code;
}

