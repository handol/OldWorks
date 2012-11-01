/**
@file SysInfo.h

@brief 서버 내부 정보 기록

주기적으로 서버 내부 정보를 sysinfo-9090.MMDD.log 화일에 저장한다.
그리고 1초마다  서버 내부 정보를 sysinfo-current-9090.MMDD.log 화일에 저장한다.
주기값 (초단위)은  컨피그 화일에 지정한다.

서버 내부 정보에는 thread 개수 , user  개수, accpet  개수, 

@author 
@date 2006.09.12
*/
#ifndef SYSINFO_H
#define SYSINFO_H

#include "SysInfo.h"
#include "MyLog.h"

class SysInfo:
	public ACE_Task<PAS_SYNCH>
{
public:
	static SysInfo *instance(ACE_Thread_Manager* threadManager=0);
	
	
	virtual ~SysInfo(void);

	virtual int svc(void);

	void stop();

	void	printSysInfo();

	void	overWriteLog();
		
private:
	static SysInfo *oneInstance;
	SysInfo(ACE_Thread_Manager* threadManager=0);

	bool runFlag;
	int	last_mtime;
	MyLog *sysinfolog;
	MyLog *overwritelog;
	
};
#endif
