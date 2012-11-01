#ifndef __RUNTIME_LOGGER_H__
#define __RUNTIME_LOGGER_H__

#include <ace/Time_Value.h>

#include "PasLog.h"

class RuntimeLogger
{
public:
	RuntimeLogger(const char* function, const delaySec) : _start(ACE_OS::gettimeofday()), _function(function), _delaySec(delaySec)
	{
	
	}

	~RuntimeLogger(void)
	{
		ACE_Time_Value diff = ACE_OS::gettimeofday() - _start;
		if(diff.sec() >= _delaySec)
		{
			PAS_INFO3("RuntimeLogger >> Func[%s] time[%d.%06d]", _function, diff.sec(), diff.usec());
		}
	}

private:
	ACE_Time_Value _start;
	const char* _function;
	const int _delaySec;
};

#endif
