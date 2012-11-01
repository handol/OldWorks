/*--------------------------------------------------------
  PROGRAM NAME : CommonSocket.cpp - 소켓라이브러
  DATE         : 2002.04.01
  AUTHOR       : LEE SANG GU
---------------------------------------------------------*/

#include <stdio.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

#include "CommonSocket.h"

extern int      errno;

#define _XDEBUG_

CSocket::CSocket()
{
	m_hSocket = -1;
	m_szNewDomainName[0] = 0x00;
}


CSocket::~CSocket()
{
	Close();
}


/*--------------------------------------------------------
  함 수 명 : BOOL CSocket::Create(int m_nSockType)
  기능개요 : 소켓을 생성한?  인    수 : int m_nSockType -> 소켓타입
  리 턴 값 : FALSE -> 소켓생성 실패, TRUE -> 소켓생성 성?---------------------------------------------------------*/
BOOL CSocket::Create(int m_nSockType)
{
	if(m_nSockType != TCP && m_nSockType != UDP)
		m_nSockType = TCP;

	if((m_hSocket = socket(AF_INET, m_nSockType, 0)) < 0)
		return FALSE;
	else
		return TRUE;
}


/*--------------------------------------------------------
  함 수 명 : void CSocket::Close()
  기능개요 : 소켓을 닫는?  인    수 : 없?  리 턴 값 : 없?---------------------------------------------------------*/
void CSocket::Close()
{
	if (m_hSocket != -1) {
		close(m_hSocket);
		m_hSocket = -1;
	}
}


/*--------------------------------------------------------
  함 수 명 : BOOL CSocket::Bind(short nPort)
  기능개요 : 소켓구조체에 포트번호를 bind시킨?  인    수 : short nPort -> 포트번호
  리 턴 값 : FALSE -> bind 실패,  TRUE -> bind 성?---------------------------------------------------------*/
BOOL CSocket::Bind(short nPort)
{
	memset (&m_soAddr, 0, sizeof(m_soAddr));
	m_soAddr.sin_family         = AF_INET;
	m_soAddr.sin_addr.s_addr    = htonl(INADDR_ANY);
	m_soAddr.sin_port           = htons((u_short)nPort);

	if (bind(m_hSocket, (struct sockaddr *)&m_soAddr, sizeof(m_soAddr)) < 0)
		return FALSE;
	else
		return TRUE;
}


/*----------------------------------------------------------
  함 수 명 : BOOL CSocket::Bind(short nPort, char * szAddr)
  기능개요 : 소켓구조체에 포트번호와 주소를 bind시킨?  인    수 : short nPort -> 포트번호
			 char * szAddr -> 주소
  리 턴 값 : FALSE -> bind 실패,  TRUE -> bind 성?-----------------------------------------------------------*/
BOOL CSocket::Bind(short nPort, char * szAddr)
{
	memset (&m_soAddr, 0, sizeof(m_soAddr));
	m_soAddr.sin_family         = AF_INET;
	m_soAddr.sin_addr.s_addr    = htonl(INADDR_ANY);
	m_soAddr.sin_port           = htons(0);

	if (bind(m_hSocket, (struct sockaddr *)&m_soAddr, sizeof(m_soAddr)) < 0) {
		printf("addr [fail]: %s\n",GetHostAddr(szAddr));
		return FALSE;
	}
	else {
		m_udpAddr.sin_family        = AF_INET;
		m_udpAddr.sin_addr.s_addr   = inet_addr(GetHostAddr(szAddr));
		m_udpAddr.sin_port          = htons((u_short)nPort);

		//printf("szAddr : %s\n",szAddr);
		//printf("addr : %s\n",GetHostAddr(szAddr));
		return TRUE;
	}
}


/*----------------------------------------------------------------------
  함 수 명 : BOOL CSocket::Listen(int nConBacklog)
  기능개요 : 서버가 클라이언트로부터 들어오는 접속 요청을 받아들일것?			 선언하고 얼마나 많은 요청(nConBacklog )을 받아들일것인가를
			 지정한?  인    수 : int nConBacklog -> 클라인언트로 부터의 요청수
  리 턴 값 : FALSE -> listen 실패,  TRUE -> listen 성?----------------------------------------------------------------------*/
BOOL CSocket::Listen(int nConBacklog)
{
	if (listen (m_hSocket, nConBacklog) < 0)
		return FALSE;
	else
		return TRUE;
}


/*---------------------------------------------------------------------
  함 수 명 : int CSocket::Read(char *buffptr, int readlen)
  기능개요 : 소켓의 데이터를 읽는?  인    수 : char *buffptr  ->
			 int readlen    ->
  리 턴 값 : nread  ->
			 readlen - nleft  ->
---------------------------------------------------------------------*/
int CSocket::Read(char *buffptr, int readlen)
{
	int  nleft, nread;

	nleft = readlen;
	while (nleft > 0) {
		nread = read(m_hSocket, buffptr, nleft);
		if (nread < 0)
			return nread;
		else if (nread == 0)
			break;

		nleft -= nread;
		buffptr += nread;
	}

	return (readlen - nleft);
}


/*---------------------------------------------------------------------
  함 수 명 : int CSocket::Read(char *buffptr)
  기능개요 : 소켓의 데이터를 읽는?  인    수 : char *buffptr
  리 턴 값 : nread
---------------------------------------------------------------------*/
int CSocket::Read(char *buffptr)
{
	int  nleft, nread;

	while(1) {
		nread = read(m_hSocket, buffptr, 1024);
		if(nread > 0) break;
	}
	return nread;
}


/*---------------------------------------------------------------------
  함 수 명 : int CSocket::ReadLine(char *buffptr)
  기능개요 :
  인    수 : char *buffptr
  리 턴 값 : nread
			 nleft
---------------------------------------------------------------------*/
int CSocket::ReadLine(char *buffptr)
{
	int  nread = 0, nleft = 0;
	char szReadBuf[10];

	while(1) {
		nread = read(m_hSocket, szReadBuf,1);
		if(szReadBuf[0] == '\n') {
			*buffptr++ = szReadBuf[0];
			return nread;
		}
		else *buffptr++ = szReadBuf[0];

		if(nread < 0)
			return nread;

		nleft++;
	}
	return nleft;
}


/*---------------------------------------------------------------------
  함 수 명 : int CSocket::Write(char *buffptr, int writelen)
  기능개요 : 소켓에 데이터를 쓴?  인    수 : char *buffptr
			 int writelen
  리 턴 값 : 없?---------------------------------------------------------------------*/
int CSocket::Write(char *buffptr, int writelen)
{
	int  nleft, nwritten;

	nleft = writelen;

	while (nleft > 0) {
		nwritten = write(m_hSocket, buffptr, nleft);
		if (nwritten <= 0)
			return nwritten;

		nleft -= nwritten;
		buffptr += nwritten;
	}

	return (writelen - nleft);
}


/*---------------------------------------------------------------------
  함 수 명 : int CSocket::Sendto(char *buffptr, int writelen)
  기능개요 : 상대방의 주소로 데이터를 전송한?  인    수 : char *buffptr
			 int writelen
  리 턴 값 : nwritten
			 writelen - nleft
---------------------------------------------------------------------*/
int CSocket::Sendto(char *buffptr, int writelen)
{
	int addrlen;
	int  nleft, nwritten;

	nleft = writelen;

	addrlen = sizeof(m_udpAddr);
	while (nleft > 0) {
		nwritten = sendto(m_hSocket, buffptr, nleft,0,(struct sockaddr *)&m_udpAddr, addrlen);
		if (nwritten <= 0)
			return nwritten;

		nleft -= nwritten;
		buffptr += nwritten;
	}

	return (writelen - nleft);
}


/*---------------------------------------------------------------------
  함 수 명 : int CSocket::Receive(char *buffptr, int readlen)
  기능개요 : 전송된 데이터를 받아 읽는?  인    수 : char *buffptr
			 int readlen
  리 턴 값 : -1
			 -2
			 nBytes
---------------------------------------------------------------------*/
int CSocket::Receive(char *buffptr, int readlen)
{
	int     nRecvSize, nBytes;
	char    *pBuf;

	if (Read ((char *)&nRecvSize, 4) != 4)
		return -1;

	nBytes = ntohl(nRecvSize);
	pBuf = new char[nBytes];

	nRecvSize = Read (pBuf, nBytes);
	if (nRecvSize <= 0)
	{
		delete pBuf;
		return -2;
	}

	if (nRecvSize >= readlen)
		nBytes = readlen - 1;
	else
		nBytes = nRecvSize;

	memcpy (buffptr, pBuf, nBytes);
	buffptr[nBytes] = 0x00;

	delete[] pBuf;

	return nBytes;
}


/*---------------------------------------------------------------------
  함 수 명 : int CSocket::Send(char *buffptr, int wrtlen)
  기능개요 :
  인    수 : char *buffptr
			 int wrtlen
  리 턴 값 : nBytes
---------------------------------------------------------------------*/
int CSocket::Send(char *buffptr, int wrtlen)
{
	int     nSendSize, nBytes;

	if (wrtlen == 0)
		wrtlen = strlen(buffptr);

	nSendSize = htonl(wrtlen);
	Write ((char *)&nSendSize, 4);

	nBytes = Write (buffptr, wrtlen);

	return nBytes;
}


/*---------------------------------------------------------------------
  함 수 명 : BOOL CSocket::Connect(short nHostPort, char* pHostAddr)
  기능개요 : 목적지 IP주소가 지정되어 있는 sockaddr 구조체를 인?			 받아 sockfd를 통해 접속 요청을 해준?  인    수 : short nHostPort -> 호스트의 포트번호
			 char* pHostAdd  -> 호스트의 주소
  리 턴 값 : FALSE
			 TRUE
---------------------------------------------------------------------*/
BOOL CSocket::Connect(short nHostPort, char* pHostAddr)
{
	char        szHostName[256];
	int         nLen;

	szHostName[0] = 0x00;
	if (pHostAddr == NULL) {
		gethostname (szHostName, sizeof(szHostName));
	}
	else {
		if (strlen(pHostAddr) >= 256) {
			memset(szHostName, 0x00, 256);
			memcpy(szHostName, pHostAddr, 255);
		}
		else
			strcpy (szHostName, pHostAddr);
	}

	m_szNewDomainName[0] = 0x00;
	memset (&m_soAddr, 0, sizeof(m_soAddr));

	m_soAddr.sin_family         = AF_INET;
	m_soAddr.sin_addr.s_addr    = inet_addr(szHostName);

	//if (m_soAddr.sin_addr.s_addr == INADDR_NONE)
	if (m_soAddr.sin_addr.s_addr == -1) {
		LPHOSTENT lphost;
		lphost = gethostbyname(szHostName);
		if (lphost == NULL)
			return FALSE;

		m_soAddr.sin_addr.s_addr = ((LPIN_ADDR)lphost->h_addr)->s_addr;

		#ifdef  _XDEBUG_
		printf ("[ Try Connecting... ] [%s]\n", lphost->h_name);
		#endif
		if (strcmp(szHostName, lphost->h_name) != 0)
			strcpy(m_szNewDomainName, lphost->h_name);
	}
	m_soAddr.sin_port           = htons((u_short)nHostPort);

	if (connect (m_hSocket, (struct sockaddr *)&m_soAddr, sizeof(m_soAddr)) < 0) {
		printf ("[ Connect Error. ] [%s][%d]-[%d]\n",
			inet_ntoa(m_soAddr.sin_addr), nHostPort, errno);
		return FALSE;
	}

	#ifdef  _XDEBUG_
	printf ("[ Connect OK. ] [%s][%d]\n", inet_ntoa(m_soAddr.sin_addr), nHostPort);
	#endif

	return TRUE;
}


/*----------------------------------------------------------------------------
  함 수 명 : BOOL CSocket::Connect(short nHostPort, char* pHostAddr, int flag)
  기능개요 : 목적지 IP주소가 지정되어 있는 sockaddr 구조체를 인?			 받아 sockfd를 통해 접속 요청을 해준?  인    수 : short nHostPort -> 호스트의 포트번호
			 char* pHostAdd  -> 호스트의 주소
			 int flag
  리 턴 값 : FALSE
			 TRUE
			 bRet
----------------------------------------------------------------------------*/
BOOL CSocket::Connect(short nHostPort, char* pHostAddr, int flag)
{
	BOOL        bRet = FALSE;
	char        szHostName[256];
	HOSTENT     hstEnt;
	char        szTempBuf[128];
	int         nErrRet;

	szHostName[0] = 0x00;
	if (pHostAddr == NULL)
		gethostname (szHostName, sizeof(szHostName));
	else
		strcpy (szHostName, pHostAddr);

	m_szNewDomainName[0] = 0x00;
	memset (&m_soAddr, 0, sizeof(m_soAddr));

	m_soAddr.sin_family         = AF_INET;
	m_soAddr.sin_addr.s_addr    = inet_addr(szHostName);

	if (m_soAddr.sin_addr.s_addr == -1) {
		LPHOSTENT   lphost;

		memset (&hstEnt, 0x00, sizeof(hstEnt));
		lphost = gethostbyname_r(szHostName, &hstEnt, szTempBuf, sizeof(szTempBuf), &nErrRet);
		if (lphost == NULL)
			return FALSE;

		m_soAddr.sin_addr.s_addr = ((LPIN_ADDR)lphost->h_addr)->s_addr;

		if (strchr(szHostName, '.') == NULL) {
			if (strcmp(szHostName, lphost->h_name) != 0)
				strcpy(m_szNewDomainName, lphost->h_name);
		}
	}
	m_soAddr.sin_port           = htons((u_short)nHostPort);

	//	if(flag == NO_DELAY) fcntl(m_hSocket, F_SETFL, O_NDELAY);
	if (connect (m_hSocket, (struct sockaddr *)&m_soAddr, sizeof(m_soAddr)) < 0) {
		printf ("[ Connect Error. ] [%s][%d]-[%d]\n",
			inet_ntoa(m_soAddr.sin_addr), nHostPort, errno);
		return FALSE;
	}
	else bRet = TRUE;

	#ifdef  _XDEBUG_
	printf ("[ Connect OK. ] [%s][%d]\n", inet_ntoa(m_soAddr.sin_addr), nHostPort);
	#endif

	return bRet;
}


/*---------------------------------------------------------------------
  함 수 명 : BOOL CSocket::IsReadReady (int nSeconds)
  기능개요 :
  인    수 : int nSeconds
  리 턴 값 : FALSE
			 bRet
----------------------------------------------------------------------*/
BOOL CSocket::IsReadReady (int nSeconds)
{
	BOOL                bRet = FALSE;
	SOCKET              socMax = 0;
	fd_set              fdsRead;
	struct timeval      timeout;

	if (m_hSocket == -1)
		return FALSE;

	if (socMax < m_hSocket)
		socMax = m_hSocket;

	FD_ZERO (&fdsRead);
	FD_SET (m_hSocket, &fdsRead);

	timeout.tv_sec  = nSeconds;
	timeout.tv_usec = 0;

	if (select (socMax+1, &fdsRead, NULL, NULL, &timeout) > 0) {
		if (FD_ISSET (m_hSocket, &fdsRead)) {
			bRet = TRUE;
		}
	}

	FD_CLR (m_hSocket, &fdsRead);

	return bRet;
}


/*---------------------------------------------------------------------
  함 수 명 : BOOL CSocket::IsReadReady (int nSeconds, int nMillSec)
  기능개요 :
  인    수 : int nSeconds
			 int nMillSec
  리 턴 값 : FALSE
			 bRet
----------------------------------------------------------------------*/
BOOL CSocket::IsReadReady (int nSeconds, int nMillSec)
{
	BOOL                bRet = FALSE;
	SOCKET              socMax = 0;
	fd_set              fdsRead;
	struct timeval      timeout;

	if (m_hSocket == -1)
		return FALSE;

	if (socMax < m_hSocket)
		socMax = m_hSocket;

	FD_ZERO (&fdsRead);
	FD_SET (m_hSocket, &fdsRead);

	timeout.tv_sec  = nSeconds;
	timeout.tv_usec = nMillSec;

	if (select (socMax+1, &fdsRead, NULL, NULL, &timeout) > 0) {
		if (FD_ISSET (m_hSocket, &fdsRead)) {
			bRet = TRUE;
		}
	}

	FD_CLR (m_hSocket, &fdsRead);

	return bRet;
}


/*---------------------------------------------------------------------
  함 수 명 : BOOL CSocket::IsAcceptReady (int nSeconds)
  기능개요 :
  인    수 : int nSeconds
  리 턴 값 : bRet
----------------------------------------------------------------------*/
BOOL CSocket::IsAcceptReady (int nSeconds)
{
	BOOL                bRet = FALSE;
	SOCKET              socMax = 0;
	fd_set              fdsRead;
	struct timeval      timeout;

	if (socMax < m_hSocket)
		socMax = m_hSocket;

	FD_ZERO (&fdsRead);
	FD_SET (m_hSocket, &fdsRead);

	timeout.tv_sec  = nSeconds;
	timeout.tv_usec = 0;

	if (select (socMax+1, &fdsRead, NULL, NULL, &timeout) > 0) {
		if (FD_ISSET (m_hSocket, &fdsRead)) {
			bRet = TRUE;
		}
	}

	FD_CLR (m_hSocket, &fdsRead);

	return bRet;
}


/*---------------------------------------------------------------------
  함 수 명 : BOOL CSocket::IsAcceptReady (int nSeconds, int nMillSec)
  기능개요 :
  인    수 : int nSeconds
			 int nMillSec
  리 턴 값 : bRet
----------------------------------------------------------------------*/
BOOL CSocket::IsAcceptReady (int nSeconds, int nMillSec)
{
	BOOL                bRet = FALSE;
	SOCKET              socMax = 0;
	fd_set              fdsRead;
	struct timeval      timeout;

	if (socMax < m_hSocket)
		socMax = m_hSocket;

	FD_ZERO (&fdsRead);
	FD_SET (m_hSocket, &fdsRead);

	timeout.tv_sec  = nSeconds;
	timeout.tv_usec = nMillSec;

	if (select (socMax+1, &fdsRead, NULL, NULL, &timeout) > 0) {
		if (FD_ISSET (m_hSocket, &fdsRead)) {
			bRet = TRUE;
		}
	}

	FD_CLR (m_hSocket, &fdsRead);

	return bRet;
}


/*---------------------------------------------------------------------
  함 수 명 : BOOL CSocket::IsWriteReady (int nSeconds, int nMillSec)
  기능개요 :
  인    수 : int nSeconds
			 int nMillSec
  리 턴 값 : FALSE
			 bRet
----------------------------------------------------------------------*/
BOOL CSocket::IsWriteReady (int nSeconds, int nMillSec)
{
	BOOL                bRet = FALSE;
	SOCKET              socMax = 0;
	fd_set              fdsWrite;
	struct timeval      timeout;

	if (m_hSocket == -1)
		return FALSE;

	if (socMax < m_hSocket)
		socMax = m_hSocket;

	FD_ZERO (&fdsWrite);
	FD_SET (m_hSocket, &fdsWrite);

	timeout.tv_sec  = nSeconds;
	timeout.tv_usec = nMillSec;

	if (select (socMax+1, NULL, &fdsWrite, NULL, &timeout) > 0) {
		if (FD_ISSET (m_hSocket, &fdsWrite)) {
			bRet = TRUE;
		}
	}

	FD_CLR (m_hSocket, &fdsWrite);

	return bRet;
}


/*---------------------------------------------------------------------
  함 수 명 : BOOL CSocket::Accept (CSocket& sktConn)
  기능개요 : 해당하는 sockfd에 대해 클라이언트로부터 접속 요청?			 기다리다가 요청을 실제 적으로 받아들인?  인    수 : CSocket& sktConn
  리 턴 값 : FALSE
			 TRUE
----------------------------------------------------------------------*/
BOOL CSocket::Accept (CSocket& sktConn)
{
	int                 nSoaddrLen;
	SOCKET              socSub;
	struct sockaddr_in  soaddrSub;

	nSoaddrLen = sizeof (soaddrSub);

	socSub = accept (m_hSocket, (struct sockaddr *)&soaddrSub, &nSoaddrLen);
	if(socSub < 0) {
		perror("Accept failed");
		return FALSE;
	}

	sktConn.Attach (socSub, &soaddrSub);

	return TRUE;
}


/*---------------------------------------------------------------------
  함 수 명 : SOCKET CSocket::GetHandle()
  기능개요 :
  인    수 : 없?  리 턴 값 : m_hSocket
----------------------------------------------------------------------*/
SOCKET CSocket::GetHandle()
{
	return m_hSocket;
}


/*---------------------------------------------------------------------
  함 수 명 : void CSocket::SetHandle(int NewSock)
  기능개요 :
  인    수 : int NewSock
  리 턴 값 : 없?----------------------------------------------------------------------*/
void CSocket::SetHandle(int NewSock)
{
	m_hSocket = NewSock;
	return;
}


/*------------------------------------------------------------------------
  함 수 명 : void CSocket::Attach(SOCKET sockfd, struct sockaddr_in *psa)
  기능개요 :
  인    수 : SOCKET sockfd
			 struct sockaddr_in *psa
  리 턴 값 : 없?-------------------------------------------------------------------------*/
void CSocket::Attach(SOCKET sockfd, struct sockaddr_in *psa)
{
	m_hSocket = sockfd;
	memcpy (&m_soAddr, psa, sizeof(struct sockaddr_in));
}


/*------------------------------------------------------------------------
  함 수 명 : BOOL CSocket::GetPeerName(char* rPeerAddress, int& rPeerPort)
  기능개요 : 접속된 상대의 호스트 이름을 얻어온?  인    수 : char* rPeerAddress
			 int& rPeerPort
  리 턴 값 : bResult
-------------------------------------------------------------------------*/
BOOL CSocket::GetPeerName(char* rPeerAddress, int& rPeerPort)
{
	SOCKADDR_IN     sockAddr;
	int             nSockAddrLen;
	BOOL            bResult = FALSE;

	memset(&sockAddr, 0, sizeof(sockAddr));

	nSockAddrLen = sizeof(sockAddr);
	if (getpeername(m_hSocket, (SOCKADDR*)&sockAddr, &nSockAddrLen) == 0) {
		rPeerPort = ntohs(sockAddr.sin_port);
		strcpy (rPeerAddress, inet_ntoa(sockAddr.sin_addr));
	}

	return bResult;
}


/*------------------------------------------------------------------------
  함 수 명 : char* CSocket::GetPeerName(void)
  기능개요 : 접속된 상대의 호스트 이름을 얻어온?  인    수 : 없?  리 턴 값 :  pPeerAddr
-------------------------------------------------------------------------*/
char* CSocket::GetPeerName(void)
{
	SOCKADDR_IN     sockAddr;
	int             nSockAddrLen;
	char*           pPeerAddr = NULL;

	memset(&sockAddr, 0, sizeof(sockAddr));

	nSockAddrLen = sizeof(sockAddr);
	if (getpeername(m_hSocket, (SOCKADDR*)&sockAddr, &nSockAddrLen) == 0) {
		strcpy (m_szPeerAddr, inet_ntoa(sockAddr.sin_addr));
		pPeerAddr = m_szPeerAddr;
	}

	return pPeerAddr;
}


/*----------------------------------------------------------------------------
  함 수 명 : BOOL CSocket::GetSockName(char* rSocketAddress, int& rSocketPort)
  기능개요 : 지정된 소켓에 대한 로컬 이름(인터페이스 어드레스?			 포트번호)을 얻어낸?  인    수 : 없?  리 턴 값 : bResult
-----------------------------------------------------------------------------*/
BOOL CSocket::GetSockName(char* rSocketAddress, int& rSocketPort)
{
	SOCKADDR_IN     sockAddr;
	int             nSockAddrLen;
	BOOL            bResult = FALSE;

	memset(&sockAddr, 0, sizeof(sockAddr));
	nSockAddrLen = sizeof(sockAddr);
	if (getsockname(m_hSocket, (SOCKADDR*)&sockAddr, &nSockAddrLen) == 0) {
		rSocketPort = ntohs(sockAddr.sin_port);
		strcpy (rSocketAddress, inet_ntoa(sockAddr.sin_addr));
	}

	return bResult;
}


/*----------------------------------------------------------------------------
  함 수 명 : char* CSocket::GetSockName(void)
  기능개요 : 지정된 소켓에 대한 로컬 이름(인터페이스 어드레스?			 포트번호)을 얻어낸?  인    수 : 없?  리 턴 값 : pSockAddr
-----------------------------------------------------------------------------*/
char* CSocket::GetSockName(void)
{
	SOCKADDR_IN     sockAddr;
	int             nSockAddrLen;
	char*           pSockAddr = NULL;

	memset(&sockAddr, 0, sizeof(sockAddr));

	nSockAddrLen = sizeof(sockAddr);
	if (getpeername(m_hSocket, (SOCKADDR*)&sockAddr, &nSockAddrLen) == 0) {
		strcpy (m_szSockAddr, inet_ntoa(sockAddr.sin_addr));
		pSockAddr = m_szSockAddr;
	}

	return pSockAddr;
}


/*----------------------------------------------------------------------------
  함 수 명 : void CSocket::SetLinger(int on, int tm)
  기능개요 :
  인    수 : int on
			 int tm
  리 턴 값 : 없?-----------------------------------------------------------------------------*/
void CSocket::SetLinger(int on, int tm)
{
	struct linger   lng;

	lng.l_onoff = 1;
	lng.l_linger = 0;

	setsockopt (m_hSocket, SOL_SOCKET, SO_LINGER, (char *) &lng, sizeof(lng));
}


/*----------------------------------------------------------------------------
  함 수 명 : char * CSocket::GetHostAddr(char * szAddr)
  기능개요 :
  인    수 : char * szAddr
  리 턴 값 : szHostName
-----------------------------------------------------------------------------*/
char * CSocket::GetHostAddr(char * szAddr)
{
	static char szHostName[256];
	struct hostent      *lphost;
	struct sockaddr_in  m_Addr;

	if(szAddr == NULL) gethostname(szHostName , sizeof(szHostName));
	else strcpy(szHostName, szAddr);

	m_Addr.sin_addr.s_addr = inet_addr(szHostName);

	if(m_Addr.sin_addr.s_addr == -1) {
		lphost = gethostbyname(szHostName);
		if(lphost == NULL)
			return szHostName;
		else {
			m_Addr.sin_addr.s_addr = *(unsigned long *)lphost->h_addr_list[0];
			strcpy(szHostName, inet_ntoa(m_Addr.sin_addr));
			strcpy(szAddr, szHostName);
			return szHostName;
		}
	}
	strcpy(szAddr, szHostName);
	return szHostName;
}
