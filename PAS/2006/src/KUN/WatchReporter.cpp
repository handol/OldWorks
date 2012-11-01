// -*- C++ -*-
#include "WatchReporter.h"
#include "Config.h"
#include "Util2.h"
#include "PasLog.h"
#include "DebugLog.h"

#include <ace/SOCK_Dgram.h>

WatchReporter *WatchReporter::oneInstance = NULL;

int	WatchReporter::msgwatchsvrCount = 0;
int	WatchReporter::myPort = 0;
char	WatchReporter::myHost[256] = {0};

WatchReporter *WatchReporter::instance(ACE_Thread_Manager* threadManager)
{
	
	if (oneInstance == NULL) 
		oneInstance = new WatchReporter(threadManager);
		
	return oneInstance;

}


WatchReporter::WatchReporter(ACE_Thread_Manager* threadManager)
: ACE_Task<PAS_SYNCH>(threadManager)
{
	runFlag = true;
}

WatchReporter::~WatchReporter(void)
{
}



/**
idle 이 20분 이상된 사용자는 제거한다.
*/
int	WatchReporter::svc(void)
{
	Config *conf = Config::instance();	
	int msgwatch_period = conf->process.watchsvrPeriod;

	PAS_NOTICE("WatchReporter::svc start");	
	PAS_NOTICE1("WatchReporter:: msgwatch_period = %d secs", msgwatch_period);

	gethostname(myHost, sizeof(myHost)-1);
	myPort = conf->network.listenPort;
	
	while(runFlag)
	{
		// 주의 !! -- msgwatch_period 가 5의 배수여야 한다. 2007.1.17
		time_t now = Util2::sleepToSync(5);

		if (now % msgwatch_period == 0)
		{
			// debug 로그 화일 이름 (날짜 ) 갱신을 위해. 2007.1.16
			openMainLogFile();
			
			usleep(100000); // 너무 정확해도 KUNLOG 와 sync 가 안 맞다. 그래서 0.1 정도 자자. (2007.1.15)
			sendMsgWatchServer();
			
		}
			
	}
	PAS_NOTICE("WatchReporter::svc stop");
	return 0;
}

void	WatchReporter::IncreaseMsgWatchSvr(void)
{
	msgwatchsvrCount++;
}

/**

msgwatchsvr 로그
06/12/23 11:32:28 pasgw1:pasgw_9090(pasgw1):2413
06/12/23 11:33:29 pasgw1:pasgw_9090(pasgw1):2284

*/
int	WatchReporter::sendMsgWatchServer(void)
{
	int	tmp_msgwatchsvrCount = msgwatchsvrCount;
	msgwatchsvrCount = 0;

	WatchSvrMesgType            WatchMsg;
	
	STRNCPY( WatchMsg.ServerID, myHost, MAX_NAME_LEN-1);	
	snprintf(WatchMsg.ServiceID, MAX_NAME_LEN,  "pasgw_%d(%s)", myPort, myHost);
	WatchMsg.MessageCount= tmp_msgwatchsvrCount;

	char sSendMsg[256];
	memset( sSendMsg, 0x00, sizeof( sSendMsg ) );
	snprintf( sSendMsg, sizeof(sSendMsg)-1, "%s:%s:%04d", WatchMsg.ServerID
			, WatchMsg.ServiceID,WatchMsg.MessageCount );

	// 메시지 UDP 전송.
	Config *conf = Config::instance();
	
	ACE_INET_Addr remote_addr( conf->process.watchsvrPort,  conf->process.watchsvrAddr.c_str());
	ACE_INET_Addr local_addr;
	ACE_Time_Value timeout(5);
	ACE_SOCK_Dgram aceDgram(local_addr);

	ACE_Time_Value before = ACE_OS::gettimeofday();
	
        ssize_t nSend = aceDgram.send( (void *) sSendMsg, sizeof(sSendMsg), remote_addr, 0, &timeout );
        
        ACE_Time_Value after = ACE_OS::gettimeofday();
        ACE_Time_Value diff = after - before;
       	// UDP send 에 1초 이상 소요된 경우 로그 출력.
	if (diff.sec() >= 1) 
	{
		PAS_NOTICE2("UDP DELAY WatchSvr :  %d.%d sec", diff.sec(), diff.usec());
	}
	
	PAS_INFO1("MsgWatch - %d", tmp_msgwatchsvrCount);
        if( nSend < 0 )
        {
                PAS_NOTICE2("WatchSvr UDP[%d] send() ERROR : %s", aceDgram.get_handle(), ACE_OS::strerror(ACE_OS::last_error()));
                // 못 보낸 경우 counter 를 원복 .
                msgwatchsvrCount += tmp_msgwatchsvrCount;
        }
        aceDgram.close();
	return 0;
}

