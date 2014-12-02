#ifndef __FUNC_LOGGER_H__
#define __FUNC_LOGGER_H__

#include <ace/Time_Value.h>



class FuncLogger
{
public:
	FuncLogger(const char* function, const int delaySec=0) : /*_start(ACE_OS::gettimeofday()), */ _function(function), _delaySec(delaySec)
	{
		ACE_DEBUG ((LM_DEBUG, "%s ==>>\n", _function));
	}

	~FuncLogger(void)
	{
		ACE_DEBUG ((LM_DEBUG, "%s <<==\n", _function));
		/*
		ACE_Time_Value diff = ACE_OS::gettimeofday() - _start;
		if(diff.sec() >= _delaySec)
		{
			ACE_DEBUG ((LM_DEBUG, "FuncLogger >> Func[%s] time[%d.%06d]", _function, diff.sec(), diff.usec());
		}
		*/
	}

private:
	ACE_Time_Value _start;
	const char* _function;
	const int _delaySec;
};

#endif
