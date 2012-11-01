#include "DNSCache.h"
#include "Mutex.h"

using namespace DNS;

bool Cache::get(const host_t& host, CacheData& destData)
{
	ReadMutex<ACE_RW_Thread_Mutex> readGuard(lock);

	DNSMap::iterator it = data.find(host);

	// found
	if(it != data.end())
	{
		ACE_ASSERT(host == it->first);

		destData = it->second;
		return true;
	}

	// not found
	return false;
}

void Cache::set(const CacheData& node)
{
	WriteMutex<ACE_RW_Thread_Mutex> writeGuard(lock);
	data[node.host] = node;
}

int Cache::size()
{
	ReadMutex<ACE_RW_Thread_Mutex> readGuard(lock);
	return static_cast<int>(data.size());
}


