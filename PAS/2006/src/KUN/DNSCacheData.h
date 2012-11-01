#ifndef __DNS_CACHE_DATA_H__
#define __DNS_CACHE_DATA_H__

#include <string>
#include <time.h>

#include <ace/Time_Value.h>

namespace DNS
{
	class CacheData
	{
	// operator
	public:
		CacheData()
		{
			updateTime = 0;
		}

		bool operator < (const CacheData& rhs) const
		{
			return host < rhs.host;
		}

		const CacheData& operator = (const CacheData& rhs)
		{
			host = rhs.host;
			ip = rhs.ip;
			updateTime = rhs.updateTime;

			return *this;
		}

		void set(const host_t host_, const intIP_t ip_)
		{
			host = host_;
			ip = ip_;
			updateTime = time(NULL);
		}

	// attribute
	public:
		host_t host;
		intIP_t ip;
		time_t updateTime;
	};
};


#endif





