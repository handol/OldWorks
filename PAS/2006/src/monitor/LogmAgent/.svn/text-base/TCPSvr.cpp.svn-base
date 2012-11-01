//////////////////////////////////////////////////////////////////////
//
// TCPSvr.cpp: implementation of the CTCPSvr class.
//
//////////////////////////////////////////////////////////////////////

#include "TCPSvr.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTCPSvr::CTCPSvr()
{
	m_errno = 0;
}

CTCPSvr::~CTCPSvr()
{
	close();
}

int CTCPSvr::open(unsigned short nPort)
{
	struct sockaddr_in name;

	int flag = 1;
	/* Create the socket. */
	if((m_sock = socket(PF_INET, SOCK_STREAM, 0)) < 0)
	{
		m_errno = errno;
		return 0;
	}

	if(setsockopt(m_sock, SOL_SOCKET, SO_REUSEADDR, (char *)&flag, sizeof(flag)) < 0)  
	{
		m_errno = errno;
		return 0;
	}
	
	flag = 1;
	if(setsockopt(m_sock, SOL_SOCKET, SO_KEEPALIVE, (char *)&flag, sizeof(flag)) < 0)
	{
		m_errno = errno;
		return 0;
	}

	name.sin_family = AF_INET;
	name.sin_port = htons (nPort);
	name.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind (m_sock, (struct sockaddr *) &name, sizeof (name)) < 0) 
	{
		m_errno = errno;
		close();
		return 0;
	}

	if (listen(m_sock, 1) < 0) 
	{
		m_errno = errno;
		close();
		return 0;
	}

	return 1;
}

void CTCPSvr::close()
{
	(void)::close(m_sock);
	m_sock = -1;
}

int CTCPSvr::accept()
{
	int sock = -1;

    	struct sockaddr_in saddr;
	size_t size = sizeof (saddr);

	sock = ::accept (m_sock, (struct sockaddr *) &saddr, (socklen_t *)&size);

	//char szIP[128] = {0};
	//strncpy(szIP, inet_ntoa(clientname.sin_addr), sizeof(szIP));

	return sock;
}
