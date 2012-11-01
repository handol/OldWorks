#include <string.h>

#include <ace/Thread_Mutex.h>
#include <ace/Thread_Manager.h>

#include "Common.h"
#include "DNSQuerier.h"
#include "DNSManager.h"
#include "Mutex.h"
#include "Config.h"
#include "DNSCache.h"
#include "Util.h"

using namespace DNS;

Manager::Manager()
{
	numOfQueriers = 0;
	MaxNumOfQueriers = Config::instance()->dns.maxNumOfQueriers;    
	QueryTimeoutSec = Config::instance()->dns.queryTimeoutSec;
	CacheEnable = Config::instance()->dns.cacheEnable;
	CacheTimeoutSec = Config::instance()->dns.cacheTimeoutSec;
}

Manager::~Manager()
{
}

Manager* Manager::instance(void)                  
{                                                             
	return ACE_Singleton<Manager, ACE_SYNCH_MUTEX>::instance();
}                      		

void Manager::removeAllQuerier()
{
	PAS_NOTICE("DNS Manager >> Stop All Querier");

	while(size() > 0)
	{
		bool result = removeQuerier();
		if(result == false)
			sleep(1);
	}
}

bool Manager::getHostByName(const char* domainName, char* dotstrIP, size_t destlen)
{
	if(CacheEnable)
		return getHostByNameCache(domainName, dotstrIP, destlen);
	else
		return getHostByNameDirect(domainName, dotstrIP, destlen);
}

bool Manager::getHostByNameDirect(const char* domainName, char* dotstrIP, size_t destlen)
{
	PAS_TRACE1("DNS Manager >> Quering %s", domainName);

	bool queryResult = queryToDNS(domainName, dotstrIP, destlen);

	// success
	if(queryResult)
	{
		PAS_TRACE("DNS Manager >> DNS Query success");
		return true;
	}

	// fail
	else
	{
		PAS_TRACE("DNS Manager >> DNS Query fail");
		return false;
	}
}

bool Manager::getHostByNameCache(const char* domainName, char* dotstrIP, size_t destlen)
{
	PAS_TRACE1("DNS Manager >> Querying %s", domainName);

	CacheData cacheData;
	const host_t host(domainName);

	// Look up in cache
	bool foundCache = cache.get(host, cacheData);

	// found in cache
	if(foundCache)
	{
		ACE_ASSERT(cacheData.host == domainName);

		time_t diff = time(NULL) - cacheData.updateTime;
		bool isExpired = (diff > CacheTimeoutSec);

		// if not expired then using cached data
		if(isExpired == false)
		{
			Util::int2ipaddr(cacheData.ip, dotstrIP, destlen);

			PAS_TRACE2("DNS Manager >> Found in cache, %s => %s", domainName, dotstrIP);

			return true;
		}

		PAS_TRACE1("DNS Manager >> Cache expired, %s", domainName);
	}
	else
	{
		PAS_TRACE1("DNS Manager >> Not found in cache, %s", domainName);
	}

	// cache expired or not found in cache
	bool queryResult = queryToDNS(domainName, dotstrIP, destlen);

	// success
	if(queryResult)
	{
		PAS_TRACE("DNS Manager >> DNS Query success");

		// update cache
		if(foundCache)
		{
			host_t host(domainName);
			cacheData.set(host, Util::ipaddr2int(dotstrIP));
			cache.set(cacheData);
		}
		
		// add to cache
		else
		{
			// Cache의 과중한 메모리 증가를 막기위해서, Cache 데이터가 10만개 이하면 추가하고, 아니면 추가하지 않는다.
			if(cache.size() < 100000)
			{
				host_t host(domainName);
				cacheData.set(host, Util::ipaddr2int(dotstrIP));
				cache.set(cacheData);
			}
		}

		return true;
	}

	// fail
	else
	{
		PAS_INFO("DNS Manager >> DNS Query fail");

		// DNS Query 가 fail 일때, 만료된 Cache 데이터가 있다면 cache 된 데이터를 사용한다
		if(foundCache)
		{
			Util::int2ipaddr(cacheData.ip, dotstrIP, destlen);

			PAS_INFO2("DNS Manager >> Use expired cache data, %s => %s", domainName, dotstrIP);

			return true;
		}

		return false;
	}

}

bool Manager::queryToDNS(const char* domainName, char* dotstrIP, size_t destlen)
{
	Querier* pQuerier = getIdleQuerier();
	if(pQuerier == NULL)
	{
		if(size() < MaxNumOfQueriers)
		{
			pQuerier = createQuerier();
			PAS_NOTICE2("DNS Manager >> Number of Querier is %d, Number of Timeout is %d", 
				size(), getNumOfTimeout());
		}
		else
		{
			PAS_NOTICE2("DNS Manager >> DNS Querier is Full, Number of Querier is %d, Number of Timeout is %d", 
				size(), getNumOfTimeout());

			return false;
		}
	}

	ACE_ASSERT(pQuerier != NULL);

	ACE_Message_Block* pMB = MemoryPoolManager::instance()->alloc(MAX_HOST_LEN);
	ACE_ASSERT(pMB != NULL);
	ACE_ASSERT(pMB->length() == 0);
	pMB->copy(domainName, strlen(domainName)+1);

	pQuerier->putq(pMB);

	ACE_Time_Value timeout(ACE_OS::gettimeofday());
	timeout += ACE_Time_Value(QueryTimeoutSec, 0);
	pMB = pQuerier->getResponse(&timeout);
	
	// DNS Query Timeout
	if(pMB == NULL)
	{
		// 다른 쓰레드가 사용하지 않도록 TIMEOUT 상태로 큐에 삽입
		pQuerier->setTimeout();
		putQuerier(pQuerier);
		PAS_NOTICE2("DNS Manager >> DNS Timeout %d sec, %s", QueryTimeoutSec, domainName);
		return false;
	}

	// Get Response
	else
	{
		// 다른 쓰레드가 사용할 수 있도록 큐에 삽입
		putQuerier(pQuerier);
	}

	// DNS Query fail
	if(pMB->length() == 0)
	{
		MemoryPoolManager::instance()->free(pMB);				
		PAS_INFO1("DNS Manager >> DNS Query Fail, %s", domainName);
		return false;
	}

	// dest buf size is small
	if(destlen < pMB->length())
	{
		MemoryPoolManager::instance()->free(pMB);
		PAS_NOTICE1("DNS Manager >> Dest Buf size is too small, %s", domainName);
		return false;
	}

	strncpy(dotstrIP, pMB->rd_ptr(), pMB->length());

	PAS_DEBUG2("DNS Manager >> %s => %s", domainName, dotstrIP);
	
	MemoryPoolManager::instance()->free(pMB);
	return true;
}

Querier* Manager::createQuerier()
{
	PAS_NOTICE("DNS Manager >> Create New Querier");

	Querier* pQuerier = new Querier();
	pQuerier->activate();

	numOfQueriers++;

	return pQuerier;
}

bool Manager::removeQuerier()
{
	WriteMutex<ACE_Thread_Mutex> guard(querierLock);

	if(queriers.empty() == true)
		return false;

	int queueSize = queriers.size();
	int scanSize = 0;
	bool fullScan = false;
	while(fullScan == false)
	{
		Querier* pQuerier = queriers.front();
		queriers.pop();

		// idle 인 querier 를 찾아서 삭제한다.
		if(pQuerier->isIdle() == true)
		{
			ACE_Message_Block* pMB = MemoryPoolManager::instance()->alloc(1);
			ACE_ASSERT(pMB);
			ACE_ASSERT(pMB->length() == 0);
			
			// put empty MB for thread stop
			pQuerier->putq(pMB);
			ACE_Thread_Manager::instance()->wait_task(pQuerier);

			delete pQuerier;

			numOfQueriers--;

			return true;
		}

		queriers.push(pQuerier);
		
		++scanSize;
		fullScan = (scanSize >= queueSize);
	}

	return false;
}

Querier* Manager::getIdleQuerier()
{
	WriteMutex<ACE_Thread_Mutex> guard(querierLock);

	if(queriers.empty() == true)
		return false;

	int queueSize = queriers.size();
	int scanSize = 0;

	// 큐를 순회하면서 IDLE 인 것을 찾아서 리턴한다.
	bool fullScan = false;
	while(fullScan == false)
	{
		Querier* pQuerier = queriers.front();
		queriers.pop();
		
		if(pQuerier->isIdle() == true)
		{
			return pQuerier;
		}

		queriers.push(pQuerier);

		++scanSize;
		fullScan = (scanSize >= queueSize);
	}

	return NULL;
}

int Manager::getNumOfTimeout()
{
	ReadMutex<ACE_Thread_Mutex> guard(querierLock);

	if(queriers.empty() == true)
		return 0;

	int numOfTimeout = 0;
	int queueSize = queriers.size();
	int scanSize = 0;

	// 큐를 순회하면서 IDLE 인 것을 찾아서 리턴한다.
	bool fullScan = false;
	while(fullScan == false)
	{
		Querier* pQuerier = queriers.front();
		queriers.pop();
		
		if(pQuerier->isTimeout() == true)
		{
			numOfTimeout++;
		}

		queriers.push(pQuerier);

		++scanSize;
		fullScan = (scanSize >= queueSize);
	}

	return numOfTimeout;
}

void Manager::putQuerier(Querier* pQuerier)
{
	WriteMutex<ACE_Thread_Mutex> guard(querierLock);

	queriers.push(pQuerier);
}

int Manager::size()
{
	return numOfQueriers;
}
