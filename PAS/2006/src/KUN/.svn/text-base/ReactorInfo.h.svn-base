#ifndef __REACTOR_INFO_H__
#define __REACTOR_INFO_H__

class ReactorInfo
{
public:
	ReactorInfo()
	{
		pReactor = NULL;
		thrID = 0;
		numClients = 0;
		busy = false;
	}
	
	ACE_Reactor *pReactor;
	int	thrID;
	unsigned int numClients; // 현재 처리 중인 갯수. 
	volatile bool busy;
};


class ReactorBusyGuard
{
public:
	ReactorBusyGuard(ReactorInfo* pRInfo) : pReactorInfo(pRInfo)
	{
		pReactorInfo->busy = true;
	}

	~ReactorBusyGuard()
	{
		pReactorInfo->busy = false;
	}

private:
	ReactorInfo* pReactorInfo;
};

#endif

