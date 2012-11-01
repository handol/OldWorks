//////////////////////////////////////////////////////////////////////
//
// TCPSvr.h: interface for the CTCPSvr class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(TCPSVR_H__1C1BDFA6_20F3_48DE_A955_B959AA228409__INCLUDED_)
#define TCPSVR_H__1C1BDFA6_20F3_48DE_A955_B959AA228409__INCLUDED_

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <inttypes.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>


class CTCPSvr  
{
private:
	int m_sock;
	int m_errno;

public:
	int accept();
	int open(unsigned short nPort);
	int GetLastErrorCode();
	void close();
	CTCPSvr();
	virtual ~CTCPSvr();

};

#endif // !defined(TCPSVR_H__1C1BDFA6_20F3_48DE_A955_B959AA228409__INCLUDED_)
