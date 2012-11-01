#ifndef __NULL_MUTEX_QUEUE_H__
#define __NULL_MUTEX_QUEUE_H__

#include <ace/Assert.h>
#include <vector>

#include "Common.h"
#include "Mutex.h"

template<typename T1>
class NullmutexQueue
{
// ¸â¹ö ÇÔ¼ö
public:
	void push(T1& val)
	{
		_queue.push_back(val);
	}

	T1& front()
	{
		ACE_ASSERT(!_queue.empty());
		return _queue.front();
	}

	T1& back()
	{
		ACE_ASSERT(!_queue.empty());
		return _queue.back();
	}

	int pop_front(T1& val)
	{
		if(_queue.empty())
			return -1;

		val = _queue.front();
		_queue.erase(_queue.begin());
		
		return 0;
	}

	int pop_back(T1& val)
	{
		if(_queue.empty())
			return -1;

		val = _queue.back();
		_queue.pop_back();
		
		return 0;
	}
	
	bool empty()
	{
		return _queue.empty();
	}

	size_t size()
	{
		return _queue.size();
	}

private:

// ¸â¹ö º¯¼ö
private:
	typedef std::vector<T1> queue_type;
	queue_type _queue;
};

#endif
