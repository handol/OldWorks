#ifndef __DNS_MANAGER_H__
#define __DNS_MANAGER_H__

#include <queue>

#include <ace/Thread_Mutex.h>

#include "DNSQuerier.h"
#include "DNSCache.h"

namespace DNS
{
	typedef std::queue<Querier*> QuerierQueue;

	class Manager
	{
	// operator
	public:
		Manager();
		~Manager();

		bool getHostByName(const char* domainName, char* dotstrIP, size_t destlen);
		bool getHostByNameDirect(const char* domainName, char* dotstrIP, size_t destlen);
		bool getHostByNameCache(const char* domainName, char* dotstrIP, size_t destlen);
		static Manager* instance(void);
		void removeAllQuerier();
		
	private:
		Querier* createQuerier();
		bool removeQuerier();
		Querier* getIdleQuerier();
		void putQuerier(Querier* pQuerier);
		int size();
		int getNumOfTimeout();
		bool queryToDNS(const char* domainName, char* dotstrIP, size_t destlen);


	// attribute
	private:
		ACE_Thread_Mutex querierLock; ///< queriers 를 핸들링할 때 발생하는 sync 조율용
		QuerierQueue queriers;
		int numOfQueriers;
		int MaxNumOfQueriers;
		int QueryTimeoutSec;
		bool CacheEnable;
		int CacheTimeoutSec;
		Cache cache;
	};
};


#endif





