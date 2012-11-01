#ifndef __SISOR_QUEUE_H__
#define __SISOR_QUEUE_H__

#include <ace/Synch.h>
#include <ace/Singleton.h>
#include <ace/Time_Value.h>

#include "Config.h"
#include "SisorProxy.h"
#include "MutexQueue.hpp"

typedef std::vector<SisorProxy*> VSisorProxy;

class SisorQueue
{
public:
	SisorQueue();
	static SisorQueue* instance();

	SisorProxy* get();

	void put(SisorProxy* pSisor);

private:
	VSisorProxy _pool;
	PasMutex _lock;

	ACE_Time_Value _lastConnFailTime; ///< 가장 최근 Connect 실패 시간
};

#endif
