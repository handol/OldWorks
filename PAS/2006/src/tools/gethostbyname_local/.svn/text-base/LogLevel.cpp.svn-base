#include "Common.h"
#include "LogLevel.h"

LogLevel::LogLevel()
{
	level = LLT_VERYLOW;
}

LogLevel::~LogLevel()
{

}

LogLevel* LogLevel::Instance()
{
	return ACE_Singleton<LogLevel, PAS_SYNCH_MUTEX>::instance();
}

int LogLevel::setLevel(LogLevelType level_)
{
	switch(level_)
	{
	case LLT_HIGH:
		level = level_;
		ACE_LOG_MSG->priority_mask(LM_TRACE | LM_DEBUG | LM_INFO | LM_NOTICE | LM_ALERT | LM_WARNING | LM_ERROR, ACE_Log_Msg::PROCESS);
		PAS_NOTICE("Set LogLevel = HIGH");
		break;

	case LLT_MEDIUM:
		level = level_;
		ACE_LOG_MSG->priority_mask(LM_DEBUG | LM_INFO | LM_NOTICE | LM_ALERT | LM_WARNING | LM_ERROR, ACE_Log_Msg::PROCESS);
		PAS_NOTICE("Set LogLevel = MEDIUM");
		break;

	case LLT_LOW:
		level = level_;
		ACE_LOG_MSG->priority_mask(LM_INFO | LM_NOTICE | LM_ALERT | LM_WARNING | LM_ERROR, ACE_Log_Msg::PROCESS);
		PAS_NOTICE("Set LogLevel = LOW");
		break;

	case LLT_VERYLOW:
		level = level_;
		ACE_LOG_MSG->priority_mask(LM_NOTICE | LM_ALERT | LM_WARNING | LM_ERROR, ACE_Log_Msg::PROCESS);
		PAS_NOTICE("Set LogLevel = VERYLOW");
		break;

	default:
		PAS_ERROR("LogLevel::setLevel >> 알 수 없는 log 레벨");
		return -1;
	}

	return 0;
}

LogLevelType LogLevel::getLevel()
{
	return level;
}
