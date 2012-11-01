#ifndef __MUTEX_QUEUE_H__
#define __MUTEX_QUEUE_H__

#include <ace/Assert.h>
#include <vector>

#include "Common.h"
#include "Mutex.h"

template<typename T1>
class MutexQueue
{
// ¸â¹ö ÇÔ¼ö
public:
	void push(T1& val)
	{
		WriteMutex<PasMutex> writeLock(lock);
		_queue.push_back(val);
	}

	T1& front()
	{
		ReadMutex<PasMutex> readLock(lock);
		ACE_ASSERT(!_queue.empty());
		return _queue.front();
	}

	T1& back()
	{
		ReadMutex<PasMutex> readLock(lock);
		ACE_ASSERT(!_queue.empty());
		return _queue.back();
	}

	int pop_front(T1& val)
	{
		WriteMutex<PasMutex> writeLock(lock);
		
		if(_queue.empty())
			return -1;

		val = _queue.front();
		_queue.erase(_queue.begin());
	}

	int pop_back(T1& val)
	{
		WriteMutex<PasMutex> writeLock(lock);

		if(_queue.empty())
			return -1;
		
		val = _queue.back();
		_queue.pop_back();

		return 0;
	}
	
	bool empty()
	{
		ReadMutex<PasMutex> readLock(lock);
		return _queue.empty();
	}

	size_t size()
	{
		ReadMutex<PasMutex> readLock(lock);
		return _queue.size();
	}

private:

// ¸â¹ö º¯¼ö
private:
	PasMutex lock;
	typedef std::vector<T1> queue_type;
	queue_type _queue;
};

#endif
