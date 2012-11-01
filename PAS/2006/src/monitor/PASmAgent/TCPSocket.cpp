// TCPSocket.cpp: implementation of the CTCPSocket class.
//
//////////////////////////////////////////////////////////////////////

#include "TCPSocket.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define SOCKET_BUFFER_SIZE 1024 * 8

#define INADDR_NONE -1

#include <iostream.h>

#define SOCKET_INIT_VALUE	-1
#define SOCKET_CREATE_ERROR	-1
#define SOCKET_CONNECT_ERROR	-2

#define K_E_SIGNATURE_ERROR	-1
#define K_E_SELECT_TIMEOUT	-9

//#define DEBUG

#ifdef DEBUG
static void DUMP_MEM(unsigned char * mem, int len)
{
    printf("==============[MEMDUMP START]==============================================\n");
    for(int i=0, addr=0; i<len; addr++)
    {
        printf("%04d  | ",addr*16);
        int start = i;
        int j=0;
        for(; j<16&&i<len; j++)
        {
            printf("%02x ", mem[i++]);
        }
        int n = j;
        for(; j<16; j++)
        {
            printf("   ");
        }
        printf(" | ");
        for(j=0; j<n; j++)
        {
            if((char)*(mem+start+j) == '\r') printf(" ");
            else if((char)*(mem+start+j) == '\n') printf(" ");
            else if((char)*(mem+start+j) == '\t') printf(" ");
            else printf("%c", (char)*(mem+start+j));
        }
        printf("\n");
    }
    printf("==============[MEMDUMP END]================================================\n");
}
#endif

CTCPSocket::CTCPSocket()
{
	m_s = SOCKET_INIT_VALUE;
}

CTCPSocket::~CTCPSocket()
{
	if ( m_s > 0 )
	{
		close(m_s);
	}

	m_s = SOCKET_INIT_VALUE;
}

int CTCPSocket::Connect(const char *szHostName, unsigned int Port)
{
	int ret;

	struct sockaddr_in ServerAddr;
	struct hostent    *host;

	memset(&ServerAddr, 0x00, sizeof(sockaddr_in));

	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_addr.s_addr = inet_addr(szHostName);
	ServerAddr.sin_port = htons(Port);
/*
	if (ServerAddr.sin_addr.s_addr == INADDR_NONE)
	{
		host = gethostbyname(szHostName);
        	
		if ( host == NULL )
		{	
			close(m_s);
			m_s = SOCKET_INIT_VALUE;
			return SOCKET_INIT_VALUE;
		}
		
		memcpy((char*)&ServerAddr.sin_addr, host->h_addr_list[0],  host->h_length);
        }

*/
	linger lingtime;	
	lingtime.l_onoff = 1;	
	lingtime.l_linger = 3;	
	setsockopt(m_s, SOL_SOCKET, SO_LINGER, (void*)&lingtime, sizeof(linger));	 

	if ( (ret = connect(m_s, (struct sockaddr*)&ServerAddr, sizeof(sockaddr_in))) < 0 )
	{
		return SOCKET_CONNECT_ERROR; //--Connection Fail
	}

    return ret;
}

int CTCPSocket::init()
{
	m_s = socket(AF_INET, SOCK_STREAM, 0);

#ifdef DEBUG
	//cerr << "TCP Socket Open : " << m_s << endl; 
#endif 

	if ( m_s < 0 )
	{
		return SOCKET_CREATE_ERROR;
	}

	return m_s;
}

int CTCPSocket::Send(char *buff, int buffsize)
{
	if ( buff == NULL || buffsize < 1 )
	{
		return K_E_SIGNATURE_ERROR;
	}
	
	int iRet = send(m_s, buff, buffsize, 0); 


#ifdef DEBUG
	cerr << "AGENT Send Request Size : " << buffsize << endl;
	cerr << "AGENT Send iRet : " << iRet << " socket : " << m_s << endl;

	if ( iRet > 0 )
		DUMP_MEM((unsigned char*)buff, iRet);
#endif
	return iRet;
}

int CTCPSocket::Send(char *buff, int buffsize, int isec, int iusec)
{
    if ( buff == NULL || buffsize < 1 )
    {
        return K_E_SIGNATURE_ERROR;
    }

    struct timeval timeout; 

    timeout.tv_sec = isec;  
    timeout.tv_usec = iusec;    

    fd_set  fds;    
    FD_ZERO(&fds);  
    FD_SET(m_s, &fds);  

    int Ret = select( m_s + 1, NULL, &fds, NULL, &timeout);

    if ( Ret <= 0 ) // || FD_ISSET(m_s + 1, &fds) == 0 ) 
    {   
        return K_E_SELECT_TIMEOUT;  
    }   
   
    return send(m_s, buff, buffsize, 0);
}

int CTCPSocket::Recv(char *buff, int buffsize)
{
	if ( buff == NULL || buffsize < 1 )
	{
		return K_E_SIGNATURE_ERROR;
	}
	
	return recv(m_s, buff, buffsize, 0);
}

int CTCPSocket::Recv(char *buff, int buffsize, int isec, int iusec)
{
	if ( buff == NULL || buffsize < 1 || isec < 0 || iusec < 0 )
	{
		return K_E_SIGNATURE_ERROR;
	}
	
	struct timeval timeout;

	timeout.tv_sec = isec;
	timeout.tv_usec = iusec;

	fd_set	fds;
	FD_ZERO(&fds);
	FD_SET(m_s, &fds);

	int Ret = select( m_s + 1, &fds, NULL, NULL, &timeout);
#ifdef DEBUG
	cerr << "AGENT Recv select : " << Ret << " socket : " << m_s << endl;
#endif
	if ( Ret <= 0 ) // || FD_ISSET(m_s + 1, &fds) == 0 )
	{
		return K_E_SELECT_TIMEOUT;
	}

	int iRet = recv(m_s, buff, buffsize, 0);

#ifdef DEBUG	
	cerr << "AGENT Recv iRet : " << iRet << " socket : " << m_s << endl;
	if ( iRet > 0 )
		DUMP_MEM((unsigned char*)buff, iRet);
#endif
	return iRet;
}

void CTCPSocket::Close()
{
    if ( m_s > 0 )
    {
        close(m_s);
    }

#ifdef DEBUG
    //cerr << "TCP Socket Close : " << m_s << endl;
#endif

    m_s = SOCKET_INIT_VALUE;
    
}
