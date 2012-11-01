//////////////////////////////////////////////////////////////////////
//
// DNSQuery.cpp: implementation of the CDNSQuery class.
//
//////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <errno.h>
#include <stdlib.h>
#include <fcntl.h>
#include <time.h>
#include <signal.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <iostream.h>
#include "DNSQuery.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDNSQuery::CDNSQuery()
{

}

CDNSQuery::~CDNSQuery()
{

}

in_addr_t CDNSQuery::lGetIP(char* sAddr)
{
	char sIP[32];
	unsigned long lAddr;

        memset(m_svrip, 0x00, 128);
	
	if ( (lAddr = inet_addr(sAddr)) == (in_addr_t)-1 ) 
	{
        	memset(sIP, 0x00, sizeof(sIP));

        	if ( bGetHostByName(sAddr, sIP) != 0 ) 
		{
            		return 0;
        	}

                strncpy(m_svrip, sIP, 20);
                m_svrip[15] = '\0'; //safety
        
		return inet_addr(sIP);
        } 
	else 
	{
                strcpy(m_svrip, sAddr);
                m_svrip[15] = '\0'; //safyty
                return lAddr;
        }
}

int CDNSQuery::bGetHostByName(char* sName, char* sIP)
{
	hostent* pHE;
        struct in_addr InAddr;
        char** ppAddrList;

        struct hostent hp_allocated;
        char buf[1024];
        int buf_len = (sizeof buf) - 1;
        int errnop;

        pHE = gethostbyname_r(sName, &hp_allocated, buf, buf_len, &errnop);

    	if ( pHE == NULL ) return 0;
    	if ( pHE->h_addrtype != AF_INET ) return 0;
        for ( ppAddrList = pHE->h_addr_list; *ppAddrList != NULL; ppAddrList++ ) 
	{
                memcpy(&InAddr, *ppAddrList, sizeof(InAddr.s_addr));
                strncpy(sIP, inet_ntoa(InAddr), 19);
#ifdef DEBUG
                cerr << sIP << endl;
#endif
        }

        return 0;
}

char* CDNSQuery::GetIP()
{
	return m_svrip;
}
