//////////////////////////////////////////////////////////////////////
// TCPSocket.h: interface for the CTCPSocket class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(TCPSOCKET_H__E2868F80_A6B7_4C31_A3A8_E8797612F8B2__INCLUDED_)
#define TCPSOCKET_H__E2868F80_A6B7_4C31_A3A8_E8797612F8B2__INCLUDED_

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <errno.h>
#include <stdlib.h>
#include <fcntl.h>
#include <time.h>
#include <signal.h>
#include <iostream.h>
#include <arpa/inet.h>
#include <assert.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h> 


#define SERVER_NAME_SIZE 128

class CTCPSocket  
{
public:
	void Close();
	int Recv(char* buff, int buffsize);
	int Recv(char *buff, int buffsize, int isec, int iusec);
	int Send(char* buff, int buffsize);
	int CTCPSocket::Send(char *buff, int buffsize, int isec, int iusec);
	int init();
	int Connect(const char* szHostName, unsigned int Port);
	CTCPSocket();
	virtual ~CTCPSocket();

private:
	char m_ServerAddr[SERVER_NAME_SIZE];
	unsigned int m_ServerPort;
	int m_s;
};

#endif // !defined(TCPSOCKET_H__E2868F80_A6B7_4C31_A3A8_E8797612F8B2__INCLUDED_)
