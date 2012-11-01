// -*- C++ -*-
#include "UserInfoMng.h"
#include "Mutex.h"
#include "Config.h"
#include "Util.h"

#include <list>
#include <map>

#include <sys/types.h>
 #include <sys/socket.h>
 #include <netinet/in.h>
 #include <arpa/inet.h>

UserInfoMng *UserInfoMng::oneInstance = NULL;

UserInfoMng *UserInfoMng::instance(ACE_Thread_Manager* threadManager)
{
	
	if (oneInstance == NULL) 
		oneInstance = new UserInfoMng(threadManager);
		
	return oneInstance;

}


UserInfoMng::UserInfoMng(ACE_Thread_Manager* threadManager)
: ACE_Task<PAS_SYNCH>(threadManager)
{
	runFlag = true;
	userCount = 0;
	maxIdleSec = 60 * 3;
}

UserInfoMng::~UserInfoMng(void)
{
	// @todo 각 목록 내부의 UserInfo 를 delete 해야 한다.
	
	MDNs.clear();
	IPs.clear();
}

UserInfo *UserInfoMng::getUser(char *MDN, char*ipAddr)
{
	UserInfo *user = search(MDN, ipAddr);
	if (user==NULL)
		user = add(MDN, ipAddr);
	return user;
}

UserInfo *UserInfoMng::searchByMdn(char *MDN, char*ipAddr)
{
	ReadMutex<PasMutex> readMutex(lockUserInfo);

	UserInfo user;
	user.set(MDN, ipAddr);
	
	map<intMDN_t,  UserInfo*>::const_iterator mdnpos;
	mdnpos = MDNs.find(user.getIntPhoneNumber());
	if (mdnpos != MDNs.end()) //
	{
		return mdnpos->second;
	}
	return NULL;
}

UserInfo *UserInfoMng::searchByAddr(char *MDN, char*ipAddr)
{
	ReadMutex<PasMutex> readMutex(lockUserInfo);
	
	UserInfo user;
	user.set(MDN, ipAddr);
	
	map<intMDN_t,  UserInfo*>::const_iterator ippos;
	ippos = IPs.find(user.getIntIpAddr());
	if (ippos != IPs.end()) // NOT found
	{
		return ippos->second;
	}

	return NULL;
}

UserInfo *UserInfoMng::search(char *MDN, char*ipAddr)
{
	ReadMutex<PasMutex> readMutex(lockUserInfo);
	
	UserInfo user;
	user.set(MDN, ipAddr);
	
	map<intMDN_t,  UserInfo*>::const_iterator mdnpos;
	mdnpos = MDNs.find(user.getIntPhoneNumber());
	if (mdnpos != MDNs.end()) // NOT found
	{
		return mdnpos->second;
	}

	map<intMDN_t,  UserInfo*>::const_iterator ippos;
	ippos = IPs.find(user.getIntIpAddr());
	if (ippos != IPs.end()) // NOT found
	{
		return ippos->second;
	}

	return NULL;
}

UserInfo *UserInfoMng::add(char *MDN, char*ipAddr)
{
	WriteMutex<PasMutex> writeMutex(lockUserInfo);

	PAS_TRACE3("UserInfoMng::add(): MDN=%s, IP=%s, Count=%d", MDN, ipAddr,  MDNs.size());
	
	UserInfo *user = new UserInfo();
	if (user==0) return NULL;
	
	user->set(MDN, ipAddr);
	MDNs[user->getIntPhoneNumber()] = user;
	IPs[user->getIntIpAddr()] = user;
	userCount = MDNs.size();
	
	PAS_TRACE3("UserInfoMng::add(): MDN=%d, IP=%X, Count=%d", user->getIntPhoneNumber(), user->getIntIpAddr(), userCount);
	return user;
}

/**
idle 이 20분 이상된 사용자는 제거한다.
*/
int	UserInfoMng::svc(void)
{
	PAS_NOTICE("UserInfoMng::svc start");

	Config *conf = Config::instance();

	maxIdleSec = conf->process.userIdleSec;
	PAS_NOTICE1("UserInfoMng:: User Idle Sec = %d secs", maxIdleSec);

	// 프로그램 종료시 sleep 때문에 장시간 blocking 되는 것을 방지하기 위해
	// sleepCount를 이용해서 처리한다.
	int sleepCount = 0;
	while(runFlag)
	{
		sleep(1);
		++sleepCount;
		if(sleepCount < 60)
			continue;

		removeIdleUsers();
		sleepCount = 0;
	}
	return 0;
}

int	UserInfoMng::removeIdleUsers(void)
{
	WriteMutex<PasMutex> writeMutex(lockUserInfo);

	map<intMDN_t, UserInfo*>::const_iterator mdnpos = MDNs.begin();

	vector<intMDN_t> idleMdnList;
	vector<intIP_t> idleIpList;

	time_t now = time(NULL);
	for( ; mdnpos != MDNs.end(); mdnpos++)
	{
		// 상용에서는 20분 
		UserInfo *userInfo = mdnpos->second;
		
		if (userInfo && userInfo->isIdle(now, maxIdleSec))
		{
			idleMdnList.push_back(mdnpos->first);
			idleIpList.push_back(mdnpos->second->getIntIpAddr());
		}
	}

	if (idleMdnList.size()==0)
		return 0;
		
	//PAS_INFO1("UserInfoMng:: found %d IDLE users", idleMdnList.size() );

	
	int	delCount = 0;
	for (unsigned int i=0; i<idleMdnList.size(); i++)
	{
		char stripaddr[32];
		Util::int2ipaddr(idleIpList[i], stripaddr, sizeof(stripaddr)-1);
		PAS_TRACE2("UserInfoMng:: Idle user{MDN[0%d] IP[%s]}", idleMdnList[i], stripaddr);

		map<intMDN_t, UserInfo*>::const_iterator mdnfound = MDNs.find(idleMdnList[i]);
		if(mdnfound != MDNs.end())
		{
			delete mdnfound->second;

			MDNs.erase(idleMdnList[i]);
			IPs.erase(idleIpList[i]);
			delCount++;
		}
	}

	PAS_INFO3("UserInfoMng:: %d idle, %d deleted, %d left", 
		idleMdnList.size(), delCount, MDNs.size());
		
	userCount = MDNs.size();
	return delCount;
}


