
#include "Common.h"
#include "PasLog.h"
#include "SysInfo.h"

#include "UserInfoMng.h"
#include "Mutex.h"
#include "MemoryPoolManager.h"
#include "ReactorPool.h"
#include "Config.h"
#include "ClientHandler.h"
#include "PasAcceptor.h"
#include "ReactorPool.h"

using namespace std;

SysInfo *SysInfo::oneInstance = NULL;

SysInfo *SysInfo::instance(ACE_Thread_Manager* threadManager)
{
	if (oneInstance == NULL) 
		oneInstance = new SysInfo(threadManager);
		
	return oneInstance;

}


SysInfo::SysInfo(ACE_Thread_Manager* threadManager)
: ACE_Task<PAS_SYNCH>(threadManager)
{
	last_mtime = 0;
	sysinfolog = new MyLog();
	overwritelog= new MyLog();
}

SysInfo::~SysInfo(void)
{
	if (sysinfolog) 
		delete sysinfolog;
	if (overwritelog) 
		delete overwritelog;
		
}


void	SysInfo::printSysInfo()
{
	static unsigned int lastAccepts = 0;
	static unsigned int lastRequests = 0;
	static unsigned int lastResponses = 0;

	sysinfolog->logprint(LVL_INFO, "\n");
	sysinfolog->logprint(LVL_INFO, "Threads: %d\n",  ReactorPool::instance()->getNumWorkers());
	sysinfolog->logprint(LVL_INFO, "Users: %d\n",  UserInfoMng::instance()->getCount());
	sysinfolog->logprint(LVL_INFO, "Accepts: %d / %d\n", 
		PasAcceptor::numAccepts - lastAccepts, 
		PasAcceptor::numAccepts);

	sysinfolog->logprint(LVL_INFO, "Clients: %d\n", ClientHandler::numClientHandlers);
	sysinfolog->logprint(LVL_INFO, "Requests: %d / %d / %d\n", 
		ClientHandler::numTransactionAlloc,
		ClientHandler::numRequests - lastRequests,
		ClientHandler::numRequests
		);
	sysinfolog->logprint(LVL_INFO, "Responses: %d / %d\n", 
		ClientHandler::numResponses - lastResponses,
		ClientHandler::numResponses);

	// worker 별  client 수 출력 
	ReactorPool::instance()->print(sysinfolog);

	MessageBlockManager* pMBManager = MessageBlockManager::instance();

	vint blockSizeList = pMBManager->getBlockSizes();
	vint::iterator it = blockSizeList.begin();
	vint::iterator itE = blockSizeList.end();
	for( ; it != itE; ++it)
	{
		int blockSize = *(it);
		int maxBlocks = pMBManager->getMaxFreeBlockCount (blockSize);
		int freeBlocks = pMBManager->getFreeBlockCount (blockSize);
		int allocBlocks = pMBManager->getAllocBlockCount (blockSize);
		if (freeBlocks || allocBlocks)
		sysinfolog->logprint(LVL_INFO, "[%4dK], Max %4d, Free %4d, Use %4d,  Total %4d M\n",
			blockSize >> 10, maxBlocks, freeBlocks, allocBlocks,  ((blockSize >> 10) * allocBlocks) >> 10);
	}

	lastAccepts =  PasAcceptor::numAccepts;
	lastRequests = ClientHandler::numRequests;
	lastResponses = ClientHandler::numResponses;
		
}

// 매번 overwrite
void	SysInfo::overWriteLog()
{
	static unsigned int lastAccepts = 0;
	static unsigned int lastRequests = 0;
	static unsigned int lastResponses = 0;

	overwritelog->overwrite();
	overwritelog->logprint(LVL_INFO, "Threads: %d\n",  ReactorPool::instance()->getNumWorkers());
	overwritelog->logprint(LVL_INFO, "Users: %d\n",  UserInfoMng::instance()->getCount());
	overwritelog->logprint(LVL_INFO, "Accepts: %d / %d\n", 
		PasAcceptor::numAccepts - lastAccepts, 
		PasAcceptor::numAccepts);

	overwritelog->logprint(LVL_INFO, "Clients: %d\n", ClientHandler::numClientHandlers);
	overwritelog->logprint(LVL_INFO, "Requests: %d / %d / %d\n", 
		ClientHandler::numTransactionAlloc,
		ClientHandler::numRequests - lastRequests,
		ClientHandler::numRequests);

	overwritelog->logprint(LVL_INFO, "Responses: %d / %d\n", 
		ClientHandler::numResponses - lastResponses,
		ClientHandler::numResponses);

	// worker 별  client 수 출력 
	ReactorPool::instance()->print(overwritelog);

	MessageBlockManager* pMBManager = MessageBlockManager::instance();

	vint blockSizeList = pMBManager->getBlockSizes();
	vint::iterator it = blockSizeList.begin();
	vint::iterator itE = blockSizeList.end();
	for( ; it != itE; ++it)
	{
		int blockSize = *(it);
		int maxBlocks = pMBManager->getMaxFreeBlockCount (blockSize);
		int freeBlocks = pMBManager->getFreeBlockCount (blockSize);
		int allocBlocks = pMBManager->getAllocBlockCount (blockSize);
		
		overwritelog->logprint(LVL_INFO, "[%4dK], Max %4d, Free %4d, Use %4d,  Total %4d M\n",
			blockSize >> 10, maxBlocks, freeBlocks, allocBlocks,  ((blockSize >> 10) * allocBlocks) >> 10);
	}

	overwritelog->logprint(LVL_INFO, "SentToPhone: %d / %d\n", ClientHandler::sentDirectCnt, ClientHandler::sentByQueCnt);
	overwritelog->logprint(LVL_INFO, "SentToCP: %d / %d\n", CpHandler::sentDirectCnt, CpHandler::sentByQueCnt);
	
	lastAccepts =  PasAcceptor::numAccepts;
	lastRequests = ClientHandler::numRequests;
	lastResponses = ClientHandler::numResponses;
}


/**
thread 의 main 함수이다.
현재 시간의 시분초 값 중에 분값을 확인하여 3분, 8분인 경우에 stat.cfg 화일을 다시 로드한다.
*/
int	SysInfo::svc(void)
{
	int period_sec = Config::instance()->process.sysinfointerval;
	ACE_Message_Block *mesg = 0;
	int lastsec = 0;

	char filename[64];
	PAS_NOTICE("SysInfo::svc start");
	PAS_INFO1("SysInfo:: msq Q = %X", msg_queue ());

	snprintf(filename, 60, "sysinfo-%d", Config::instance()->network.listenPort);
	sysinfolog->open((char*)".", filename);

	snprintf(filename, 60, "sysinfo-current-%d", Config::instance()->network.listenPort);
	overwritelog->open((char*)".", filename);

	
	// 주기적으로 화일 변경을 검사하여 데이타 리로딩.
	while(1) {
		int	now = time(NULL);

		overWriteLog();
		
		if (now - lastsec >= period_sec)
		{
			printSysInfo();
			lastsec = now;
		}

		// wait for 1 second
		ACE_Time_Value waitsec(ACE_OS::gettimeofday());
		waitsec += ACE_Time_Value(1,0);

		if (getq(mesg, &waitsec) >= 0)
		{
			if (mesg->length()==0) // 빈 메시지 수신 --> 종료를 의미.
			{
				break;
			}
		}
	}

	PAS_NOTICE("SysInfo::svc stop");
	return 0;
}

void SysInfo::stop()
{
	runFlag = false;
}



#ifdef TEST_MAIN

int main()
{
	
	
}
#endif
