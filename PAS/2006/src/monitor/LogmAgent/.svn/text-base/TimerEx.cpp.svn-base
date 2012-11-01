//////////////////////////////////////////////////////////////////////
//
// TimerEx.cpp: implementation of the CTimerEx class.
//
//////////////////////////////////////////////////////////////////////
#include <iostream.h>
#include <unistd.h>
#include "TimerEx.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CTimerEx::CTimerEx()
{
	m_uTime = 0;
	m_TargetTime = 0;
}

CTimerEx::~CTimerEx()
{

}

void CTimerEx::cron_sync_min()
{
	if ( m_uTime >= 10 )
	{
		m_uTime = 1;
	}

	int min_gap;
    	struct tm  *tm;

    	m_TargetTime = time((time_t*)0);
    	tm = localtime(&m_TargetTime);

	tm->tm_min = tm->tm_min%10;

	if(tm->tm_min >= m_uTime) min_gap = 10 - tm->tm_min - 1 + m_uTime;
	else min_gap = m_uTime - tm->tm_min - 1;

	m_TargetTime += (min_gap * 60) + (60 - tm->tm_sec);

	return;
}

void CTimerEx::cron_sync_sec()
{
	if ( m_uTime >= 60 )
	{
		m_uTime = 1;
	}

	int sec_gap;
    	struct tm  *tm;

    	m_TargetTime = time((time_t*)0);
    	tm = localtime(&m_TargetTime);

	tm->tm_sec = tm->tm_sec % 60;

	if ( tm->tm_sec >= m_uTime ) sec_gap = m_uTime + (60 - tm->tm_sec);
	else sec_gap = m_uTime - tm->tm_sec;

	m_TargetTime += sec_gap; 

	return;

}

void CTimerEx::cron_sleep()
{
	int    seconds_to_wait;
	seconds_to_wait = (int) (m_TargetTime - time((time_t*)0));

#ifdef DEBUG
	cout << "seconds_to_wait:" << seconds_to_wait << endl;
#endif

	while (seconds_to_wait > 0) {
		seconds_to_wait = (int) sleep((unsigned int) seconds_to_wait);
	}

	return;
} 

int CTimerEx::SetTimer(unsigned short uTime)
{
	m_uTime = uTime;
	return 1;
}
