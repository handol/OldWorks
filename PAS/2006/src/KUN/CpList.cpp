#include "CpList.h"
#include <algorithm>
#include "CpEventHandler.h"
#include "PasLog.h"
#include "Mutex.h"

int CpList::add(CpHandler* pCP)
{
	ACE_ASSERT(pCP);

	cpList.push_back(pCP);

	return 0;
}

int CpList::addIfNot(CpHandler* pCP)
{
	ACE_ASSERT(pCP);

	if(isExist(pCP))
		return -1;

	//실제 사용시 get() 으로 중복 여부를 검사하고 add 하므로  여기서 다시 검사하지 않아도 된다.
	if(isExist(pCP->getHost(), pCP->getPort()))
		return -1;
	
	cpList.push_back(pCP);

	return 0;
}


CpHandler* CpList::get(const host_t host, const int port)
{
	CpPtrs::iterator it = cpList.begin();
	CpPtrs::iterator itE = cpList.end();

	for( ; it != itE; ++it)
	{
		if((*it)->getHost() == host && (*it)->getPort() == port)
			return *it;
	}

	return NULL;
}

int CpList::del(CpHandler* pCP)
{
	CpPtrs::iterator it;
	it = std::find(cpList.begin(), cpList.end(), pCP);

	if(it == cpList.end())
		return -1;

	cpList.erase(it);
	return 0;
}

int CpList::deleteAll()
{
	CpPtrs::const_iterator it = cpList.begin();
	CpPtrs::const_iterator itE = cpList.end();

	//for( ; it != itE; ++it)
	//{
	//	if (! (*it)->isRemovable()) return -1;
	//}

	for( ; it != itE; ++it)
	{
		delete *it;
	}

	cpList.clear();
	return 0;
}

bool CpList::isExist(const host_t host, const int port)
{
	CpPtrs::const_iterator it = cpList.begin();
	CpPtrs::const_iterator itE = cpList.end();

	for( ; it != itE; ++it)
	{
		if((*it)->getHost() == host && (*it)->getPort() == port)
			return true;
	}

	return false;
}

bool CpList::isExist(const CpHandler* pCP)
{
	CpPtrs::const_iterator it;
	it = std::find(cpList.begin(), cpList.end(), pCP);

	return (it != cpList.end());
}

int CpList::size()
{
	return cpList.size();
}

void CpList::requestCloseToAll()
{
	PAS_TRACE("CpList::requestCloseAll");

	CpPtrs::iterator it = cpList.begin();
	CpPtrs::iterator itE = cpList.end();

	for( ; it != itE; ++it)
	{
		(*it)->requestClose();
	}
}
