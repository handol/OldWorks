/*--------------------------------------------------------
  PROGRAM NAME : mwatchcli.cpp
  DATE         : 2002.04.01
  AUTHOR       : LEE SANG GU
---------------------------------------------------------*/

#include "CommonSocket.h"
#include "mwatchcli.h"
#include <stdlib.h>
//#include <stream.h>
#include <stdio.h>

#include <iostream>

using namespace std;

/*--------------------------------------------------------------------
  ÇÔ ¼ö ¸í : mwatchcli::mwatchcli()
  ±â´É°³¿ä : ¼ÒÄÏÀ» Çì´õÆÄÀÏ¿¡ Á¤ÀÇÇØ ³õÀº È£½ºÆ®ÀÇ ÁÖ¼Ò¿Í Æ÷Æ®¹øÈ£
			 ÃÊ±âÈ­ ½ÃÅ°´Â ÇÔ¼ö¸¦ È£ÃâÇÑ´
  ÀÎ    ¼ö : ¾øÀ
  ¸® ÅÏ °ª : ¾øÀ
--------------------------------------------------------------------*/
mwatchcli::mwatchcli()
{
	char msgWatchAddr[128] = "\0";
	strcpy(msgWatchAddr, MSGWATCH_ADDR);
	Initialize( msgWatchAddr, MSGWATCH_PORT );
}


/*--------------------------------------------------------------------
  ÇÔ ¼ö ¸í : mwatchcli::mwatchcli( char *host )
  ±â´É°³¿ä : È£½ºÆ®ÀÇ Æ÷Æ®¹øÈ£´Â Çì´õÆÄÀÏ¿¡ Á¤ÀÇÇØ³õÀº °ªÀ» °¡Á®¿À°
			 ÁÖ¼Ò´Â µû·Î ÀÔ·Â¹Þ¾Æ ÃÊ±âÈ­ ½ÃÅ°´Â ÇÔ¼ö¸¦ È£ÃâÇÑ´
  ÀÎ    ¼ö : char *host -> È£½ºÆ®ÀÇ ÁÖ¼Ò
  ¸® ÅÏ °ª : ¾øÀ
--------------------------------------------------------------------*/
mwatchcli::mwatchcli( char *host )
{
	Initialize( host, MSGWATCH_PORT );
}


/*-----------------------------------------------------------------------
  ÇÔ ¼ö ¸í : mwatchcli::mwatchcli( char *host, int port )
  ±â´É°³¿ä : È£½ºÆ®ÀÇ ÁÖ¼Ò¿Í Æ÷Æ®¹øÈ£¸¦ ÀÔ·Â¹Þ¾Æ ¼ÒÄÏÀ» ÃÊ±âÈ­½ÃÅ°´Â
			 ÇÔ¼ö¸¦ È£ÃâÇÑ´
  ÀÎ    ¼ö : char *host -> È£½ºÆ®ÀÇ ÁÖ¼Ò,  int port -> È£½ºÆ®ÀÇÆ÷Æ®¹øÈ£
  ¸® ÅÏ °ª : ¾øÀ
-----------------------------------------------------------------------*/
mwatchcli::mwatchcli( char *host, int port )
{
	Initialize( host, port );
}


/*--------------------------------------------------------------------
  ÇÔ ¼ö ¸í : mwatchcli::~mwatchcli()
  ±â´É°³¿ä : ¼ÒÄÏÀ» ´Ý´Â´
  ÀÎ    ¼ö : ¾øÀ
  ¸® ÅÏ °ª : ¾øÀ
--------------------------------------------------------------------*/
mwatchcli::~mwatchcli()
{
	cltSock.Close();
}


/*--------------------------------------------------------------------
  ÇÔ ¼ö ¸í : void mwatchcli::Initialize( char *host, int port )
  ±â´É°³¿ä : È£½ºÆ®ÁÖ¼Ò¿Í Æ÷Æ®¹øÈ£¸¦ ÀÔ·Â¹Þ¾Æ UDP¼ÒÄÏÀ» »ý¼ºÇÏ°
			 ¼ÒÄÏ¿¡ È£½ºÆ®ÁÖ¼Ò¿Í Æ÷Æ®¹øÈ£¸¦ ¹ÙÀÎµå ½ÃÅ²´
  ÀÎ    ¼ö : char *host -> È£½ºÆ®¸í,   int port -> Æ÷Æ®¹øÈ£
  ¸® ÅÏ °ª : ¾øÀ
--------------------------------------------------------------------*/
void mwatchcli::Initialize( char *host, int port )
{
	index = 0;
	cltSock.Create( UDP );
	cltSock.Bind( port, host );

	memset( WatchMsg.ServerID, 0x00, sizeof( WatchMsg.ServerID ) );
	gethostname( WatchMsg.ServerID, sizeof( WatchMsg.ServerID ) );

	cout << " WatchMsg.ServerID => " << WatchMsg.ServerID << endl;
}


/*--------------------------------------------------------------------
  ÇÔ ¼ö ¸í : BOOL mwatchcli::addID( char * szID )
  ±â´É°³¿ä : ¼­ºñ½º¾ÆÀÌµð¿Í ¸Þ½ÃÁö°Ç¼ö¸¦ Ãß°¡ÇÏ´Â ÇÔ¼ö¸¦ È£ÃâÇÑ´
  ÀÎ    ¼ö : char * szID -> ¼­ºñ½º¾ÆÀÌµð
  ¸® ÅÏ °ª : ¾øÀ
--------------------------------------------------------------------*/
BOOL mwatchcli::addID( char * szID )
{
	return addID( szID, 1 );
}


/*----------------------------------------------------------------------
  ÇÔ ¼ö ¸í : BOOL mwatchcli::addID( char * szID, int m_nMsgCnt )
  ±â´É°³¿ä : ¼­ºñ½º¾ÆÀÌµð¸¦ Ãß°¡ÇÏÇÑ´
  ÀÎ    ¼ö : char * szID -> ¼­ºñ½º¾ÆÀÌµð,   int m_nMsgCnt -> ¸Þ½ÃÁö°Ç¼ö
  ¸® ÅÏ °ª : ¾øÀ
-----------------------------------------------------------------------*/
BOOL mwatchcli::addID( char * szID, int m_nMsgCnt )
{
	if ( index < MAX_ID_NUM ) {
		WatchIndex[index] = ( watch_index_t * ) malloc( sizeof(watch_index_t) );

		WatchIndex[index]->cnt = 0;
		// WatchIndex[index]->overflow = m_nMsgCnt;
		WatchIndex[index]->lastsent = time(NULL) - MIN_SEND_TIME;

		if( szID != NULL )
			strcpy( WatchIndex[index]->id, szID );

		index += 1;

		return TRUE;
	}
	return FALSE;
}


/*--------------------------------------------------------------------
  ÇÔ ¼ö ¸í : BOOL mwatchcli::addMsgCnt( char * szID )
  ±â´É°³¿ä : ¼­ºñ½º¾ÆÀÌµð¿Í ¸Þ½ÃÁö°Ç¼ö¸¦ Ãß°¡´Â ÇÔ¼ö¸¦ È£ÃâÇÑ´
  ÀÎ    ¼ö : char * szID -> ¼­ºñ½º¾ÆÀÌµð
  ¸® ÅÏ °ª : ¾øÀ
--------------------------------------------------------------------*/
BOOL mwatchcli::addMsgCnt( char * szID )
{
	return addMsgCnt( szID, 1 );
}


/*---------------------------------------------------------------------
  ÇÔ ¼ö ¸í : BOOL mwatchcli::addMsgCnt( char * szID, int m_nMsgCnt )
  ±â´É°³¿ä : È£½ºÆ®¸í, ¼­ºñ½º¾ÆÀÌµð, ¸Þ½ÃÁö°Ç¼ö¸¦ UDP¼­¹ö·Î Àü¼ÛÇÑ´
  ÀÎ    ¼ö : char * szID -> ¼­ºñ½º¾ÆÀÌµð,  int m_nMsgCnt -> ¸Þ½ÃÁö°Ç¼ö
  ¸® ÅÏ °ª : ¾øÀ
---------------------------------------------------------------------*/
BOOL mwatchcli::addMsgCnt( char * szID, int m_nMsgCnt )
{
	int  m_nCnt;
	int  i;
	char sSendMsg[256];

	for( m_nCnt = 0; m_nCnt < index; m_nCnt++ ) {
		if( !strcmp( WatchIndex[ m_nCnt ]->id, szID ) )
			break;
	}

	if( m_nCnt==index ) {
		addID( szID, m_nMsgCnt );
	}

	WatchIndex[m_nCnt]->cnt += 1;
	time_t thistime = time( NULL );

	if ( ( thistime - WatchIndex[m_nCnt]->lastsent) >= MIN_SEND_TIME ) {
		strcpy(WatchMsg.ServiceID, szID);
		WatchMsg.MessageCount= WatchIndex[m_nCnt]->cnt;

		char szTmpBuf[1024];
		snprintf( szTmpBuf, sizeof(szTmpBuf)-1," SEND [%s : %d]¹øÂ°..",
			szID, WatchIndex[m_nCnt]->cnt );

		memset( sSendMsg, 0x00, sizeof( sSendMsg ) );

		snprintf( sSendMsg, sizeof(szTmpBuf)-1, "%s:%s:%04d", WatchMsg.ServerID
			, WatchMsg.ServiceID,WatchMsg.MessageCount );

		if ( cltSock.Sendto( sSendMsg, sizeof( sSendMsg ) ) < 0 ) {
			return FALSE;
		}
		WatchIndex[m_nCnt]->cnt = 0;
		WatchIndex[m_nCnt]->lastsent = thistime;
		return TRUE;
	}
	else {
		char szTmpBuf[1024];
		sprintf( szTmpBuf, " NOT YET [%s : %d]¹øÂ°.. ",
			szID, WatchIndex[m_nCnt]->cnt );

		return NOT_YET;
	}
}

#ifdef TEST_MAIN
int main(int argc, char *argv[])
{
	mwatchcli Util2::msgwatch;
	char 	Util2::mwatchMsg[256]={0};

	char hostname[64];
	gethostname(hostname, sizeof(hostname)-1);
	sprintf(mwatchMsg, "pasgw_%d(%s)", svrport, hostname);

}
#endif

