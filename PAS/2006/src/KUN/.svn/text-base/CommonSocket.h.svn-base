
#ifndef __COMMON_SOCKET_H__
#define __COMMON_SOCKET_H__

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <strings.h>
#include <sys/param.h>
//#include <sysent.h>

#define         NO_DELAY        0
#define         DELAY           1

#ifndef BOOL
#define BOOL    int
#define ERROR   -1
#define FALSE   0
#define TRUE    1
#endif

// Define socket type
#define         TCP             SOCK_STREAM
#define         UDP             SOCK_DGRAM

typedef int                 SOCKET;
typedef struct hostent      HOSTENT;
typedef struct in_addr      IN_ADDR;
typedef struct sockaddr_in  SOCKADDR_IN;
typedef struct sockaddr     SOCKADDR;

typedef HOSTENT*            LPHOSTENT;
typedef IN_ADDR*            LPIN_ADDR;
typedef SOCKADDR_IN*        LPSOCKADDR_IN;
typedef SOCKADDR*           LPSOCKADDR;

class CSocket
{
	protected:
		SOCKET              m_hSocket;
		struct sockaddr_in  m_soAddr;
		struct sockaddr_in  m_udpAddr;
		char                m_szSockAddr[16], m_szPeerAddr[16];

	public:
		char                m_szNewDomainName[128];

		CSocket();
		~CSocket();

		BOOL Create(int);
		void Close();

		BOOL Bind(short nPort);
		BOOL Bind(short nPort, char * szAddr);
		BOOL Listen(int nConnectionBacklog = 5);

		virtual int Read(char *buffptr, int readlen);
		virtual int Read(char *buffptr);
		int ReadLine(char *buffptr);
		virtual int Write(char *buffptr, int writelen);
		virtual int Sendto(char *buffptr, int writelen);

		virtual int Receive(char *buffptr, int readlen);
		virtual int Send(char *buffptr, int wrtlen = 0);

		BOOL Connect(short nHostPort, char* pHostAddr = NULL);
		BOOL Connect(short nHostPort, char* pHostAddr, int nDelay);
		BOOL IsReadReady (int nSeconds = 5);
		BOOL IsReadReady (int nSeconds, int nMillSec);
		BOOL IsWriteReady (int nSeconds, int nMillSec);
		BOOL IsAcceptReady (int nTimer = 5);
		BOOL IsAcceptReady (int nSeconds, int nMillSec);
		BOOL Accept (CSocket& sktConn);

		SOCKET  GetHandle();
		void    SetHandle(int);
		void Attach(SOCKET sockfd, struct sockaddr_in *psa);

		BOOL GetPeerName(char* rPeerAddress, int& rPeerPort);
		BOOL GetSockName(char* rSocketAddress, int& rSocketPort);
		char * GetHostAddr(char * szAddr);

		char* GetPeerName(void);
		char* GetSockName(void);

		void SetLinger(int on, int tm);
};
#endif
