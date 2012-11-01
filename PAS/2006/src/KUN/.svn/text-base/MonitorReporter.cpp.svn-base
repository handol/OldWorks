// -*- C++ -*-
#include "MonitorReporter.h"
#include "Config.h"
#include "Util2.h"
#include "PasLog.h"
#include "DebugLog.h"

#include <ace/SOCK_Dgram.h>

MonitorReporter *MonitorReporter::oneInstance = NULL;

int	MonitorReporter::pasmonCount = 0;
int	MonitorReporter::myPort = 0;
char	MonitorReporter::myHost[256] = {0};

MonitorReporter *MonitorReporter::instance(ACE_Thread_Manager* threadManager)
{
	
	if (oneInstance == NULL) 
		oneInstance = new MonitorReporter(threadManager);
		
	return oneInstance;

}


MonitorReporter::MonitorReporter(ACE_Thread_Manager* threadManager)
: ACE_Task<PAS_SYNCH>(threadManager)
{
	runFlag = true;
}

MonitorReporter::~MonitorReporter(void)
{
}



/**
idle 이 20분 이상된 사용자는 제거한다.
*/
int	MonitorReporter::svc(void)
{
	Config *conf = Config::instance();	
	int pasmon_period = conf->process.pasmonPeriod;

	PAS_NOTICE("MonitorReporter::svc start");	
	PAS_NOTICE1("MonitorReporter:: pasmon_period = %d secs", pasmon_period);

	gethostname(myHost, sizeof(myHost)-1);
	myPort = conf->network.listenPort;
	
	while(runFlag)
	{		
		// 주의 !! -- pasmon_period 가 5의 배수여야 한다. 2007.1.17
		time_t now = Util2::sleepToSync(5);
		
		if (now % pasmon_period == 0)
		{
			usleep(100000); // 너무 정확해도 KUNLOG 와 sync 가 안 맞다. 그래서 0.1 정도 자자. (2007.1.15)
			openMainLogFile();
			sendPasMon();
		}
	}

	PAS_NOTICE("MonitorReporter::svc stop");	
	return 0;
}

void	MonitorReporter::increasePasMon(void)
{
	pasmonCount++;
}



/**
pasmon 로그

12/27/06 13:57:40 pasgw1        9090    664
12/27/06 13:58:00 pasgw1        9090    739
12/27/06 13:58:21 pasgw1        9090    618
*/
int	MonitorReporter::sendPasMon(void)
{
	int	tmp_pasmonCount = pasmonCount;
	pasmonCount = 0;

	// 메시지 내용 설정
	PasMonMesgType msg;

	memset(&msg, 0x00, sizeof(msg));
	strcpy(msg.host, myHost);
	msg.port = myPort;
	msg.count = tmp_pasmonCount;

	// 메시지 UDP 전송.
	Config *conf = Config::instance();
	
	ACE_INET_Addr remote_addr( conf->process.pasmonPort,  conf->process.pasmonAddr.c_str());
	ACE_INET_Addr local_addr;
	ACE_Time_Value timeout(5);
	ACE_SOCK_Dgram aceDgram(local_addr);

	ACE_Time_Value before = ACE_OS::gettimeofday();

	int nSend = aceDgram.send( (void *) &msg, sizeof(msg), remote_addr, 0, &timeout );
	
	ACE_Time_Value after = ACE_OS::gettimeofday();
	ACE_Time_Value diff = after - before;
	// UDP send 에 1초 이상 소요된 경우 로그 출력.
	if (diff.sec() >= 1) 
	{
		PAS_NOTICE2("UDP DELAY PasMon :  %d.%d sec", diff.sec(), diff.usec());
	}

	PAS_INFO1("PasMon - %d", tmp_pasmonCount);
	 
	if( nSend < 0 )
	{
		PAS_NOTICE2("PasMon UDP[%d] send() ERROR : %s", aceDgram.get_handle(), ACE_OS::strerror(ACE_OS::last_error()));
		// 못 보낸 경우 counter 를 원복 .
		pasmonCount += tmp_pasmonCount;
	}
	aceDgram.close();
       
	return 0;
}

