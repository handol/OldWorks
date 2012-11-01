#include <ace/SOCK_Stream.h> 
#include <ace/Reactor.h>

#include "PasAcceptor.h"
#include "Common.h"
#include "ClientHandler.h"
#include "PasLog.h"
#include "Config.h"
#include "SystemStatistic.h"
#include "ReactorPool.h"
#include "DebugLog.h"
#include "KunHandler.h"
#include "MeHandler.h"

#define PRINT_CLIENT_ADDR


unsigned	int	PasAcceptor::numAccepts = 0;

PasAcceptor::PasAcceptor(ACE_Reactor* pReactor, int _numofQ)
: ACE_Event_Handler(pReactor), numOfQueues(_numofQ)
{
	pSysStat = SystemStatistic::instance();
}

PasAcceptor::~PasAcceptor(void)
{
}

int PasAcceptor::handle_input(ACE_HANDLE fd)
{
	if (fd == ACE_INVALID_HANDLE)
	{
		PAS_ERROR1("PasAcceptor::handle_input, Invalid ACE_HANDLE fd=%d", fd);
		return 0;
	}

	#ifdef CHECK_MAX_CLIENTS
	// 
	if (Config::instance()->process.checkMaxClients)
	{
		if (ClientHandler::numClientHandlers > (unsigned)Config::instance()->process.maxClients )
		{
			usleep(100000);
			PAS_ERROR2("PasAcceptor:: Too many concurent clients (%d). Limit=(%d) by config settting.",
				ClientHandler::numClientHandlers, Config::instance()->process.maxClients);
			return 0;
		}
	}
	#endif
	
	ACE_SOCK_STREAM sock;
	acceptor.accept(sock);
	//sock.enable(ACE_NONBLOCK);

	// client용 이벤트 핸들러 생성
	int	newsockfd = sock.get_handle();

	if (newsockfd <= 0)
	{
		PAS_ERROR2( "PasAcceptor::handle_input, accept ERROR %s. new sock fd = %d", ACE_OS::strerror(ACE_OS::last_error()), newsockfd);
		sock.close();
		sleep(1);
		return 0;
	}

	// increase statistic
	numAccepts++;
	if (2 * 1000 * 1000 * 1000 < numAccepts)
	{
		PAS_INFO("Connection counter reset to zero.");
		numAccepts = 0;
	}

	/// 단말기의 IP 주소를 구하기. -- ClientHandler 에서 수행하므로 여기서는 하지 말자.
	ACE_INET_Addr peer;
	sock.get_remote_addr( peer );
	PAS_INFO4( "Phone connected. Addr[%s:%d] Sock[%d] ConnectionCount[%d] ", 
		peer.get_host_addr(), peer.get_port_number(), newsockfd, numAccepts);
	
	
	ClientHandler* pEH = createHandler();
	pEH->init(newsockfd);
	
	// reactor에 새로운 client 등록
	// pClientEH 는 소켓이 종료될때 스스로 자신을 메모리에서 삭제한다.
	pEH->addEvent(READ_MASK);
	
	pSysStat->clientConnectionAccept(1);

	
	return 0;
}

int PasAcceptor::open(int listenPort)
{
	ACE_INET_Addr addr(listenPort);
	
	if(acceptor.open(addr, 1, PF_INET, 100) < 0)
	{
		PAS_ERROR("PasAcceptor::open >> Port Listen 실패");
		return -1;
	}

	PAS_NOTICE2("Listen Start, Port[%d] FD[%d]", listenPort, acceptor.get_handle());

	ACE_Time_Value interval(60, 0);
	timeTickID = reactor()->schedule_timer(this, NULL, interval, interval);
	
	return 0;
}

int PasAcceptor::handle_timeout(const ACE_Time_Value& /* current_time */, const void* /* act */)
{
	pSysStat->writeLog();
	return 0;
}

ACE_HANDLE PasAcceptor::get_handle(void) const
{
	return acceptor.get_handle();
}

void PasAcceptor::set_handle(ACE_HANDLE fd)
{
	acceptor.set_handle(fd);
}

void PasAcceptor::stop()
{
	acceptor.close();
	reactor()->cancel_timer(timeTickID);
	ACE_Reactor_Mask mask = ACE_Event_Handler::ALL_EVENTS_MASK | ACE_Event_Handler::DONT_CALL;
	reactor()->remove_handler(this, mask);
}

ClientHandler* PasAcceptor::createHandler() const
{
	if(Config::instance()->process.serverID.incaseFind("KUN") >= 0)
	{
		return new KunHandler(ReactorPool::instance()->workerReactor());
	}

	return new MeHandler(ReactorPool::instance()->workerReactor());
}

