#ifndef __MUTEX_H__
#define __MUTEX_H__

/**
@brief Read/Write Mutex 관리

초기 모델(WorkerThread Pool)에서 ACE Mutex 를 사용할 때
acquire/release 의 실수를 피하기 위하여 만든 템플릿 클래스

현재 모델(Reactor Pool)에서는
MemoryPool, UserInfoMng, PhoneTraceMng, StatFilterMng 등에서만 사용된다.
*/


#include "Common.h"
#include "PasLog.h"

template<typename ACE_MUTEX>
class ReadMutex
{
public:
	ReadMutex(ACE_MUTEX& lock) : lock_(lock)
	{
		//PAS_TRACE0("lock.acquire_read try...");
		lock_.acquire_read();
		//PAS_TRACE0("lock.acquire_read acquired");
	}

	~ReadMutex()
	{
		lock_.release();
		//PAS_TRACE0("lock.release_read");
	}

protected:
	ACE_MUTEX& lock_;
};

template<typename ACE_MUTEX>
class WriteMutex
{
public:
	WriteMutex(ACE_MUTEX& lock) : lock_(lock)
	{
		//PAS_TRACE0("lock.acquire_write try...");
		lock_.acquire_write();
		//PAS_TRACE0("lock.acquire_write acquired");
	}

	~WriteMutex()
	{
		lock_.release();
		//PAS_DEBUG0("lock.release_write");
	}

protected:
	ACE_MUTEX& lock_;
};

#endif
