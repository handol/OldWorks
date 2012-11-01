#ifndef __EXCEPTION_H__
#define __EXCEPTION_H__

#define ThrowException(ecode) throw Exception(__FILE__, __LINE__, ecode)


enum ExceptionCode
{
	ECODE_BEGIN = 0,

	ECODE_UNKNOWN,
	ECODE_ERROR,
	ECODE_SUCCESS,
	ECODE_INVALID_PARAMETER,
	ECODE_INIT_FAIL,
	ECODE_QUERY_FAIL,
	
	ECODE_CONNECT_FAIL,
	ECODE_NOT_CONNECTED,
	ECODE_DISCONNECTED,
	ECODE_RECV_FAIL,
	ECODE_SEND_FAIL,

	ECODE_WRONG_DATA,
	ECODE_NOT_ENOUGH_SPACE,
	ECODE_NO_DATA,
	ECODE_NO_MORE_DATA,
	ECODE_TIMEOUT,
	ECODE_NO_EXIST,
	ECODE_ALREADY_EXIST,
	
	ECODE_OPEN_FAIL,
	ECODE_NOT_OPENED,
	ECODE_READ_FAIL,
	ECODE_WRITE_FAIL,
	ECODE_CLOSE_FAIL,
	ECODE_SEEK_FAIL,

	ECODE_END
};

class Exception
{
public:
	Exception(const char* file, const int line, ExceptionCode exceptionCode);

	ExceptionCode get() const;

	const char* toString();
	const char* c_str();

private:
	const ExceptionCode _code;
	const char* _file;
	const int _line;
	char _errorString[128];
};

#endif




