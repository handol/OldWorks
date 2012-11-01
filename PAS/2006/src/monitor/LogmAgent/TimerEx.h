//////////////////////////////////////////////////////////////////////
//
// TimerEx.h: interface for the CTimerWrap class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(TIMEREX_H__B5E785C9_45D6_4ED6_928C_228E26AD8C36__INCLUDED_)
#define TIMEREX_H__B5E785C9_45D6_4ED6_928C_228E26AD8C36__INCLUDED_

#include <time.h>

enum TIMETYPE 
{ T_HOUR = 0, T_MIN, T_SEC };

class CTimerEx  
{
private:
	time_t m_TargetTime;
	unsigned short m_uTime;

public:
	int SetTimer(unsigned short uTime);
	void cron_sync_min();
	void cron_sync_sec();
	void cron_sleep();
	CTimerEx();
	virtual ~CTimerEx();

};

#endif // !defined(TIMEREX_H__B5E785C9_45D6_4ED6_928C_228E26AD8C36__INCLUDED_)
