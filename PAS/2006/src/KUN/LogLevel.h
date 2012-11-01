#ifndef __LOG_LEVEL_H__
#define __LOG_LEVEL_H__

/**
@brief Log 수준 설정
*/

#include "PasLog.h"

enum LogLevelType
{
	LLT_HIGH,
	LLT_MEDIUM,
	LLT_LOW,
	LLT_VERYLOW
};


class LogLevel
{
// 멤버함수
public:
	LogLevel();
	~LogLevel();

	static LogLevel* instance();
	int setLevel(LogLevelType level_);
	LogLevelType getLevel();

private:
	LogLevelType level;

// 멤버변수
public:
private:
};

#endif
