#ifndef __DNS_CACHE_H__
#define __DNS_CACHE_H__

#include <map>

#include <ace/Thread_Mutex.h>

#include "Common.h"
#include "DNSCacheData.h"

namespace DNS
{
	typedef std::map<host_t, CacheData> DNSMap;

	class Cache
	{
	// operator
	public:
		bool get(const host_t& host, CacheData& destData);
		void set(const CacheData& data);
		int size();

	// attribute
	private:
		ACE_RW_Thread_Mutex lock;
		DNSMap data;
	};
};

#endif





