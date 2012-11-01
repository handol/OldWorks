// -*- C++ -*-
#include "MyAcceptor.h"

//#include <ace/OS.h>
#include <ace/SOCK_Stream.h>

#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <sys/errno.h>
#include <sys/time.h>
#include <unistd.h>

#include "Common.h"
#include "ClientHandler.h"
#include "PasLog.h"
#include "Config.h"
#include "ReactorPool.h"

MyAcceptor *MyAcceptor::oneInstance = NULL;



MyAcceptor *MyAcceptor::instance(ACE_Thread_Manager* threadManager)
{
	
	if (oneInstance == NULL) 
		oneInstance = new MyAcceptor(threadManager);
		
	return oneInstance;

}


MyAcceptor::MyAcceptor(ACE_Thread_Manager* threadManager)
: ACE_Task<PAS_SYNCH>(threadManager)
{
	runFlag = true;

	mySockfd = 0;
	myPort = 0;
	myHost[0] = 0;

	numAccepts = 0;
	numL4Checks = 0;
}

MyAcceptor::~MyAcceptor(void)
{
}

int	MyAcceptor::startListen(int serverPort)
{
	//ACE_INET_Addr addr(serverPort);
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr	= htonl(INADDR_ANY);
	addr.sin_port = htons(serverPort);
	
	if ((mySockfd = ACE_OS::socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		PAS_ERROR1( "MyAcceptor:: socket() failed %s", ACE_OS::strerror(ACE_OS::last_error()));
		return -1;
	}

    	int opt;
	struct linger lng;

	opt=1;
	if(ACE_OS::setsockopt(mySockfd, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, 4)<0) {
		PAS_ERROR1( "MyAcceptor:: setsockopt() failed %s", ACE_OS::strerror(ACE_OS::last_error()));
		return -1;
	}
	
	lng.l_onoff = lng.l_linger = 0;
	if (ACE_OS::setsockopt(mySockfd, SOL_SOCKET, SO_LINGER, (char *) &lng, sizeof(lng)) < 0) {
		PAS_ERROR1( "MyAcceptor:: setsockopt() failed %s", ACE_OS::strerror(ACE_OS::last_error()));
		return -1;
	}
	
	if (ACE_OS::bind(mySockfd , (struct sockaddr *) &addr,  sizeof(addr)) < 0) {
		PAS_ERROR1( "MyAcceptor:: bind() failed %s", ACE_OS::strerror(ACE_OS::last_error()));
		return -1;
	}    

	if (listen(mySockfd, 256) < 0) {
		PAS_ERROR1( "MyAcceptor:: listen() failed %s", ACE_OS::strerror(ACE_OS::last_error()));
		return -1;
	}

	PAS_NOTICE2("MyAcceptor:: Listen Start, Port[%d] Sock[%d]", serverPort, mySockfd);
	
	return(mySockfd);
}


void	MyAcceptor::stop()
{
	runFlag = false;
}



/**
서버 소켓에 rad 이벤트가 발생하기를 기다린다. 즉, 새 접속을 기다린다.
*/
int	MyAcceptor::waitConnection(int usec)
{
	fd_set	rset;
	struct	timeval	tzero;
	int	is_read;
	
	FD_ZERO(&rset);
	FD_SET(mySockfd, &rset);

	tzero.tv_sec = 0;
	tzero.tv_usec = usec;
	is_read = select(mySockfd+1, &rset, NULL, NULL, &tzero);

	/* if (is_read > 0 && FD_ISSET(mySockfd, &rset)) */
	if (is_read > 0)
		return (1);
	else if (is_read < 0)
		return is_read;
	else
		return (0);
}

int MyAcceptor::acceptConnection()
{
	struct sockaddr_in from;
	int	fromlen = sizeof(from);
	int	newConnSock;
	newConnSock = accept(mySockfd, (struct sockaddr *)&from, (socklen_t *)&fromlen);

	return newConnSock;	
}

int MyAcceptor::handleConnection()
{
	int newConnSock = acceptConnection();
	
	if (newConnSock <= 0)
	{
		PAS_ERROR2( "MyAcceptor:: accept ERROR %s. new sock fd = %d", ACE_OS::strerror(ACE_OS::last_error()), newConnSock);
		usleep(100000);
		return -1;
	}

	/// 단말기의 IP 주소를 구하기.
	ACE_SOCK_STREAM sock(newConnSock);
	ACE_INET_Addr peer;
	sock.get_remote_addr( peer );

	PAS_INFO4( "Accept %s:%d sock=%d conn[%d] ", peer.get_host_addr(), peer.get_port_number(), newConnSock, numAccepts);

	numAccepts++;
	if (numAccepts > 10000 * 10000 * 10)
		numAccepts = 0;
		
	// L4 Health check ignore	
	Config *conf = Config::instance();
	if( !strcmp(peer.get_host_addr(), conf->network.L4Addr1.toStr()) ||
		!strcmp(peer.get_host_addr(), conf->network.L4Addr2.toStr()) )
	{
		#if 0
		// set SO_LINGER - 2 sec. WIGCS 로부터 받은 팁. 2007.1.12
		linger lingtime;
		lingtime.l_onoff = 1;
		lingtime.l_linger = 2;
		sock.set_option( SOL_SOCKET, SO_LINGER, (void*)&lingtime, sizeof(linger) );	
		#endif

		sock.close_writer();
		sock.close();
				
		PAS_INFO3( "Close L4 %s:%d sock=%d", peer.get_host_addr(), peer.get_port_number(), newConnSock);
		return 0;
	}


	
	ReactorInfo* rInfo = ReactorPool::instance()->workerReactor();
	ClientHandler* pClientEH = new ClientHandler(rInfo);
	pClientEH->init(newConnSock);
	
	// reactor에 새로운 client 등록
	// pClientEH 는 소켓이 종료될때 스스로 자신을 메모리에서 삭제한다.
	rInfo->pReactor->register_handler(pClientEH, READ_MASK);

	
	return 0;
}


/**
idle 이 20분 이상된 사용자는 제거한다.
*/
int	MyAcceptor::svc(void)
{
	Config *conf = Config::instance();	

	PAS_NOTICE("MyAcceptor::svc start");	
	myPort = conf->network.listenPort;
	
	while(runFlag)
	{	
		int ret = waitConnection(100000);
		if (ret < 0) {
			ACE_DEBUG ((LM_DEBUG, "Acceptor: select() failed (%d)\n", ret));
			break;
		}
		else if (ret==0) {
			continue;
		}
		
		handleConnection();		

		#ifdef CHECK_MAX_CLIENTS
			if (Config::instance()->process.checkMaxClients)
			{
				if (ClientHandler::numClientHandlers > (unsigned)Config::instance()->process.maxClients )
				{
					PAS_ERROR2("MyAcceptor:: Too many concurent clients (%d). Limit=(%d) by config settting.",
						ClientHandler::numClientHandlers, Config::instance()->process.maxClients);
					return 0;
				}
			}
		#endif
	}
	PAS_NOTICE("MyAcceptor::svc stop");
	return 0;
}



