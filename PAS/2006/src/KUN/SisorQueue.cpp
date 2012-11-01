#include "Exception.h"
#include "SisorQueue.h"
#include "Mutex.h"

SisorQueue::SisorQueue() : _lastConnFailTime(0, 0)
{

}

SisorQueue* SisorQueue::instance()
{
	return ACE_Singleton<SisorQueue, ACE_SYNCH_MUTEX>::instance();
}

void SisorQueue::put( SisorProxy* pSisor )
{
	WriteMutex<PasMutex> writeLock(_lock);

	if(_pool.size() < Config::instance()->sisor.connectionPoolSize)
		_pool.push_back(pSisor);
	else
		delete pSisor;
}

SisorProxy* SisorQueue::get()
{
	WriteMutex<PasMutex> writeLock(_lock);

	SisorProxy* pSisor = NULL;

	if(!_pool.empty())
	{
		pSisor = _pool.back();
		_pool.pop_back();
		return pSisor;
	}
		

	// 접속하려는 SISOR가 존재하지 않을 경우 connection fail 이 발생하기 까지 
	// 수초 정도 블럭킹 되는 현상이 있다. 이 현상으로 인해 장애시 KUN의 TPS에
	// 많은 저하가 발생하므로, 이를 방지하기 위해 최근에 connection fail 이 
	// 발생했다면 connection 을 시도하지 않고 곧 바로 Exception 을 던진다.
	ACE_Time_Value curTime = ACE_OS::gettimeofday();
	if(curTime - _lastConnFailTime <= ACE_Time_Value(Config::instance()->sisor.connectionRetryIntervalSec))
		ThrowException(ECODE_CONNECT_FAIL);

	// queue 가 비었다면, 새로운 proxy 를 생성해서 리턴한다.
	try
	{
		pSisor = new SisorProxy;
		pSisor->connect(Config::instance()->sisor.host, Config::instance()->sisor.port);
		return pSisor;
	}
	catch (Exception e)
	{
		delete pSisor;

		_lastConnFailTime = ACE_OS::gettimeofday();
		ThrowException(ECODE_CONNECT_FAIL);
	}
}

