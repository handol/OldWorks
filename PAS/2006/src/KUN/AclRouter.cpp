/**
@file AclRouter.cpp
@brief ACL 매니저(파싱, 검색, 리로드)
@author 현근창
*/

#include "AclRouter.h"
#include "StrSplit.h"
#include "PasLog.h"
#include "Config.h"
#include "Util2.h"
#include <errno.h>

#include "DNSManager.h"

extern int errno;

/**
@brief 워커 스레드 함수
*/
int AclRouter::svc()
{
	#ifdef TEST_MAIN_ACL_ROUTER
	const int& ACL_FILE_CHECK_INTERVAL = 3;
	#else
	const int& ACL_FILE_CHECK_INTERVAL = Config::instance()->acl.updateCheckInterval;
	#endif


	mstr_t aclFileName = Config::instance()->acl.fileName;

	while(run)
	{
		struct stat chkmodify;


		if( stat(aclFileName.toStr(), &chkmodify) < 0 )
		{
			PAS_ERROR2( "%s, %s", aclFileName.toStr(), ACE_OS::strerror(errno));
			sleep(ACL_FILE_CHECK_INTERVAL);
			continue;
		}

		if( mTime == chkmodify.st_mtime )
		{
			sleep(ACL_FILE_CHECK_INTERVAL);
			continue;
		}

		mTime = chkmodify.st_mtime;		

		int result = load( aclFileName.toStr() );
		switch( result )
		{
			case ACL_SYNTAX_ERROR :
				PAS_NOTICE1( "%s ACL Syntax Error\n", aclFileName.toStr() );
				break;

			case ACL_FILE_NOT_FOUND :
				PAS_NOTICE1( "%s File not found\n", aclFileName.toStr() );
				break;

			case ACL_VERSION_ERROR :
				PAS_NOTICE1( "%s ACL same version, not reload.", aclFileName.toStr() );
				break;

			case ACL_TAIL_ERROR :
				PAS_NOTICE( "ACL missing end of ACLX\n" );
				break;

			default :
				PAS_INFO( "ACL Load Complete" );
				break;
		}
	}

	PAS_NOTICE( "AclRouter::svc stop" );

	return 0;
}

AclRouter::~AclRouter()
{
	// Host, DNS, ACL 목록을 삭제한다.
	clearHost();
	clearDNS();
	clearACL();
}

/**
@brief pasgw 의 호스트명과 포트 번호를 셋팅
       인스턴스 생성 후 최초 한번만 해주면 된다.
@param <pHostID>호스트명
       <nPasgwPort>해당 호스트의 포트번호
@return 0:성공 -1:ACL 파일 없음(실패)
*/
int AclRouter::initial(const char *pHostID, int nPasgwPort)
{
	STRNCPY( PasID, pHostID, HOST_NAME_SIZE );
	PasgwPort = nPasgwPort;

	mstr_t aclFileName = Config::instance()->acl.fileName;

	// 파일이 존재하는지 체크
	struct stat chk;
	if( stat(aclFileName.toStr(), &chk) < 0 )
	{
		PAS_ERROR1("ACL file not found [%s]", aclFileName.toStr());
		printf("ACL file not found [%s]", aclFileName.toStr());
		return -1;
	}

	return 0;
}

/**
@brief 워커 스레드를 종료시킨다.
*/
void AclRouter::remove_worker_thread(void)
{
	run = 0;
}

/**
@brief ACE_Shared_Object 의 가상함수 재정의
@param <argc>아규먼트 갯수
       <argv>각 아규먼트의 포인터
@return 의미없음
*/
/*
int AclRouter::init(int argc, ACE_TCHAR *argv[])
{
	return 0;
}
*/

AclRouter *AclRouter::instance()
{
	return ACE_Singleton<AclRouter, PAS_SYNCH_MUTEX>::instance();
}

/**
@brief acl 파일을 읽고 지정된 형식에 맞는지 구문검사를 한다.
@return true:정상 false:비정상
*/
bool AclRouter::syntaxCheck(const char* pszFileName)
{
	bool bCorrect = false;

	MyFile fp;

	if (fp.openForRead(pszFileName) < 0)
	{
		PAS_ERROR1("ACL file open error [%s]", pszFileName);
		return bCorrect;
	}

	char buff[512];

	// 헤더 검사
	StrSplit headSpliter( 2, sizeof(buff) );
	while( fp.readLine(buff, sizeof(buff)) >= 0 )
	{
		PAS_TRACE1("ACL HEAD [%s]", buff);
		headSpliter.split( buff );

		// End of List
		if( !strcmp(headSpliter.fldVal(0), "##") )
			break;

		// 정상적인 데이터가 아니면 패스
		if( headSpliter.numFlds() < 2 )
			continue;

		// 식별자의 글자 수가 맞는지 체크
		if( strlen(headSpliter.fldVal(0)) != strlen(ACL_PREFIX) )
		{
			PAS_ERROR1("ACL - ACL_PREFIX [%s]", buff);
			fp.close();
			return bCorrect;
		}

		// 헤더 버전의 글자 수가 맞는지 체크
		if( strlen(headSpliter.fldVal(1)) != HEAD_VERSION_SIZE )
		{
			PAS_ERROR1("ACL - HEAD_VERSION_SIZE [%s]", buff);
			fp.close();
			return bCorrect;
		}
	}

	// 호스트 목록 검사(PAS 영역)
	StrSplit hostSpliter( 4, sizeof(buff) );
	while( fp.readLine(buff, sizeof(buff)) >= 0 )
	{
		PAS_TRACE1("ACL HOST [%s]", buff);
		hostSpliter.split( buff );

		// End of List
		if( !strcmp(hostSpliter.fldVal(0), "##") )
			break;

		// 정상적인 데이터가 아니면 패스
		if( hostSpliter.numFlds() < 4 )
			continue;
	}

	// DNS 목록 검사(DNS 영역)
	StrSplit dnsSpliter( 6, sizeof(buff) );
	while( fp.readLine(buff, sizeof(buff)) >= 0)
	{
		PAS_TRACE1("ACL DNS [%s]", buff);
		dnsSpliter.split( buff );

		// End of List
		if( !strcmp(dnsSpliter.fldVal(0), "##") )
			break;

		// 정상적인 데이터가 아니면 패스
		if( dnsSpliter.numFlds() < 6 )
			continue;
	}

	// ACL 버전 체크(한 라인만 읽는다.)
	int ACLReadOK = 0;
	int oldACL = 0;
	while( fp.readLine(buff, sizeof(buff)) >= 0 )
	{
		PAS_TRACE1("ACL VERSION [%s]", buff);
		// End of List
		if( strncmp(buff, "##", 2) == 0 )
			break;

		// 공백 라인이면 무시
		if( ISSPACE(buff[0]) )
			continue;

		char *pVer = &buff[ACL_HEADER_VERSION_DIGIT_START];

		if( buff[0] == 'N' && buff[1] == 'V' && buff[2] > '0' && buff[2] <= '9' && chkACLHeader(pVer) )
		{
			int NewVer = atoi(pVer);
			if( NewVer < 1 )
				NewVer = ACLVer;

			ACLReadOK = 1;

			break;
		}

		else
		if( buff[0] == 'O' && buff[1] == 'V' && buff[2] > '0' && buff[2] <= '9' )
		{
			int NewVer = atoi(pVer);
			if( NewVer < 1 )
				NewVer = ACLVer;

			else
			if( NewVer != ACLVer || NewVer > ACLVer )
				ACLVer = NewVer;

			else oldACL = 1;

			ACLReadOK = 1;

			break;
		}

		// exception
		else
		{
			PAS_ERROR1("ACL - ACL_HEADER_VERSION_DIGIT_START [%s]", buff);
			fp.close();
			return bCorrect;
		}
	}

	// ACL DNS 영역
	StrSplit ACLSpliter( 6, sizeof(buff) );
	while( fp.readLine(buff, sizeof(buff)) >= 0 )
	{
		PAS_TRACE1("ACL DNS [%s]", buff);
		ACLSpliter.split( buff );

		// End of List
		if( !strcmp(ACLSpliter.fldVal(0), "##") )
			break;

		// 정상적인 데이터가 아니면 패스
		if( ACLSpliter.numFlds() < 6 )
			continue;
	}

	// TAIL
	while(  fp.readLine(buff, sizeof(buff)) >= 0 )
	{
		PAS_TRACE1("ACL TAIL [%s]", buff);
		int numsplit = ACLSpliter.split( buff );
		
		// skip white line
		if( numsplit == 0 )
			continue;

		// find ACL TAIL
		if( strncmp(buff, ACL_TAIL, strlen(ACL_TAIL)) == 0 )
		{
			bCorrect = true;
			break;
		}
	}

	fp.close();
	return bCorrect;
}

/**
@brief 헤더의 버전만 추출하여 저장한다.
@param <fp>MyFile *fp, 최초 첫 라인에서 버전을 읽고,
       버전이 바뀌었으면 true 를 리턴하여 ACL 파일을 reload 할 수 있게 한다.
@return true : 버전 갱신, false : 이미 로드한 ACL 파일과 같은 버전
*/
bool AclRouter::setHead(MyFile &fp)
{
	char buff[256]="\0";
	bool bRetVal = false;

	// 헤더 파싱
	StrSplit headSpliter( 2, sizeof(buff) );
	while(fp.readLine(buff, sizeof(buff)) >= 0 )
	{
		headSpliter.split( buff );

		// End of List
		if( !strcmp(headSpliter.fldVal(0), "##") )
			break;

		// 정상적인 데이터가 아니면 패스
		if( headSpliter.numFlds() < 2 )
			continue;

		// PAS1
		if( strncmp(headSpliter.fldVal(0), ACL_PREFIX, sizeof(ACL_PREFIX)) != 0 )
			return bRetVal;

		// 헤더 버전이 같은지 비교한다. 버전이 같으면 return
		if( strncmp(headSpliter.fldVal(1), headVersion, HEAD_VERSION_SIZE) == 0 )
			return bRetVal;

		else
		{
			memset( headVersion, 0x00, HEAD_VERSION_SIZE );
			STRNCPY( headVersion, headSpliter.fldVal(1), HEAD_VERSION_SIZE );
			bRetVal = true;
		}
	}

	return bRetVal;
}

/**
@brief PAS 목록에 아이템을 삽입한다.
@param <pBuff>PAS 데이터 예)"KUN00 221.148.247.32 9090 2"
@return true : 성공, false : 실패
*/
bool AclRouter::setHost(char *pBuff)
{
	StrSplit hostSpliter( 4, MAXLEN_URL );
	hostSpliter.split( pBuff );

	// End of List
	if( !strcmp(hostSpliter.fldVal(0), "##") )
		return false;

	// 정상적인 데이터가 아니면 패스
	if( hostSpliter.numFlds() < 4 )
		return true;

	int nFldPos = 0;
	ACL_HOST *host = new ACL_HOST;
	STRNCPY( host->Name, hostSpliter.fldVal(nFldPos++), HOST_NAME_SIZE );
	STRNCPY( host->Addr, hostSpliter.fldVal(nFldPos++), DNS_URL_SIZE );
	STRNCPY( host->Port, hostSpliter.fldVal(nFldPos++), sizeof(host->Port)-1 );
	STRNCPY( host->Opt, hostSpliter.fldVal(nFldPos++), sizeof(host->Opt)-1 );

	mapHost.insert( make_pair(string(host->Name), host) );

	return true;
}

/**
@brief DNS 목록에 아이템을 삽입한다.
@param <pBuff>DNS 데이터 예)"www.click018.co.kr 80 210.123.89.88 80 KUN00 0"
@return true : 성공, false : 실패
*/
bool AclRouter::setDNS(char *pBuff)
{
	StrSplit dnsSpliter( 6, MAXLEN_URL );
	dnsSpliter.split( pBuff );

	// End of List
	if( !strcmp(dnsSpliter.fldVal(0), "##") )
		return false;

	// 정상적인 데이터가 아니면 패스
	if( dnsSpliter.numFlds() < 6 )
		return true;

	// 다른 호스트 전용 DNS는 무시한다.
	if( strncmp(dnsSpliter.fldVal(4), ALLTYPE_PASID, HOST_NAME_SIZE) != 0 )
		if( strncmp(dnsSpliter.fldVal(4), PasID, HOST_NAME_SIZE) != 0 )
			return true;

	int nFldPos = 0;
	ACL_DATA *aclData = NULL;
	aclData = new ACL_DATA;
	STRNCPY( aclData->SrcAddr, dnsSpliter.fldVal(nFldPos++), DNS_URL_SIZE );
	STRNCPY( aclData->SrcPort, dnsSpliter.fldVal(nFldPos++), sizeof(aclData->SrcPort)-1 );
	STRNCPY( aclData->DestAddr, dnsSpliter.fldVal(nFldPos++), DNS_URL_SIZE );
	STRNCPY( aclData->DestPort, dnsSpliter.fldVal(nFldPos++), sizeof(aclData->DestPort)-1 );
	STRNCPY( aclData->Owner, dnsSpliter.fldVal(nFldPos++), sizeof(aclData->Owner)-1 );
	strncpy( &aclData->OnOff, dnsSpliter.fldVal(nFldPos++), sizeof(aclData->OnOff) );	// STRNCPY 사용금지

	ACL_KEY key;
	key.SrcAddr = aclData->SrcAddr;
	key.SrcPort = atoi( aclData->SrcPort );
	mapDNS.insert( make_pair(key, aclData) );

	return true;
}

/**
@brief ACL 목록에 아이템을 삽입한다. ACL 은 KUN 에서만 사용되며 멀티프락시 기능을 지원한다.
@param <pBuff>ACL 데이터 예)"210.123.89.89 8080 210.123.89.89 8080 KUN01 0"
@return true : 성공, false : 실패
*/
bool AclRouter::setACL(char *pBuff)
{
	StrSplit ACLSpliter( 6, MAXLEN_URL );
	ACLSpliter.split( pBuff );

	// End of List
	if( !strcmp(ACLSpliter.fldVal(0), "##") )
		return false;

	// 정상적인 데이터가 아니면 패스
	if( ACLSpliter.numFlds() < 6 )
		return true;

	// Host 정보가 없는 경우 Host가 지워진 경우이므로 무시한다.
	if( mapHost.find(ACLSpliter.fldVal(4)) == mapHost.end() )
		return true;

	int nFldPos = 0;
	ACL_DATA *aclData = NULL;
	aclData = new ACL_DATA;
	STRNCPY( aclData->SrcAddr, ACLSpliter.fldVal(nFldPos++), DNS_URL_SIZE );
	STRNCPY( aclData->SrcPort, ACLSpliter.fldVal(nFldPos++), sizeof(aclData->SrcPort)-1 );
	STRNCPY( aclData->DestAddr, ACLSpliter.fldVal(nFldPos++), DNS_URL_SIZE );
	STRNCPY( aclData->DestPort, ACLSpliter.fldVal(nFldPos++), sizeof(aclData->DestPort)-1 );
	STRNCPY( aclData->Owner, ACLSpliter.fldVal(nFldPos++), sizeof(aclData->Owner)-1 );
	strncpy( &aclData->OnOff, ACLSpliter.fldVal(nFldPos++), sizeof(aclData->OnOff) );

	ACL_KEY key;
	key.SrcAddr = aclData->SrcAddr;
	key.SrcPort = atoi( aclData->SrcPort );
	mapACL.insert( make_pair(key, aclData) );

	return true;
}

/**
@brief ACL 파일을 한줄씩 읽으며 파싱하고 저장한다.
@return 1 : 성공
        0 : 실패(구문검사 오류, 파일없음, 헤더 버전 자리수 오류, ACLX 없음, 등의 이유)
*/
int AclRouter::load(const char *pFileName)
{
	// 구문 검사
	bool syntaxgood = false;
	for( int retry=0; retry < 10; retry++ )
	{
		sleep(1);
		syntaxgood = syntaxCheck( pFileName );

		if( syntaxgood ) 
		{
			PAS_INFO( "ACL syntaxCheck OK");
			break;
		}

		PAS_INFO1( "ACL reload retry [%d]", retry );
	}

	if( syntaxgood == false )
	{
		PAS_ERROR( "ACL syntaxCheck ERROR");
		return ACL_SYNTAX_ERROR;
	}

	MyFile fp;
	if (fp.openForRead(pFileName) < 0)
		return ACL_FILE_NOT_FOUND;

	char buff[MAXLEN_URL];

	// write 락을 건다. 이전에 read 락이 걸려 있으면 풀릴 때 까지 대기한다.
	rwMutex.acquire_write();

	// ACL 헤더를 읽고 버전을 저장한다.
	if( setHead(fp) == false )
	{
		fp.close();
		rwMutex.release();
		return ACL_VERSION_ERROR;
	}

	// 이전 호스트 목록 제거
	clearHost();

	// Pas Host 목록을 갱신한다.
	while(fp.readLine(buff, sizeof(buff)) >= 0)
		if( setHost(buff) == false ) break;

	// 이전 DNS 목록 제거
	clearDNS();

	// DNS 목록 갱신
	while( fp.readLine(buff, sizeof(buff)) >= 0)
		if( setDNS(buff) == false ) break;

	// ACL 버전 체크(한 라인만 읽는다.)
	int ACLReadOK = 0;
	int oldACL = 0;
	int NewVer = 0;
	while( fp.readLine(buff, sizeof(buff)) >= 0)
	{
		// End of List
		if( strncmp(buff, "##", 2) == 0 )
			break;

		// 공백 라인이면 무시
		if( ISSPACE(buff[0]) )
			continue;

		char *pVer = &buff[ACL_HEADER_VERSION_DIGIT_START];

		if( buff[0] == 'N' && buff[1] == 'V' && buff[2] > '0' && buff[2] <= '9' && chkACLHeader(pVer) )
		{
			NewVer = atoi(pVer);
			if( NewVer < 1 )
				NewVer = ACLVer;

			ACLReadOK = 1;

			break;
		}

		else
		if( buff[0] == 'O' && buff[1] == 'V' && buff[2] > '0' && buff[2] <= '9' )
		{
			int NewVer = atoi(pVer);
			if( NewVer < 1 )
				NewVer = ACLVer;

			else
			if( NewVer != ACLVer || NewVer > ACLVer )
				ACLVer = NewVer;

			else oldACL = 1;

			ACLReadOK = 1;

			break;
		}

		// exception
		else
		{
			break;
		}
	}

	// 이전 ACL DNS 목록 제거
	clearACL();

	// ACL 목록 갱신
	while( fp.readLine(buff, sizeof(buff)) >= 0 )
		if( setACL(buff) == false ) break;

	// TAIL
	// 체크 안함

	if( ACLReadOK )
	{
		#ifndef TEST_MAIN_ACL_ROUTER
		// ACL 버전(ex:OV14)이 변경되면 Monitor 서버에 ACL 데이터를 송신한다.
		ACL_RECORD_INFO ACLInfo;
		gethostname( ACLInfo.szHostName, ACL_APPLY_HOSTNAME_SIZE );
		ACLInfo.nPort = PasgwPort;

		if( (ACLVer != NewVer && ACLVer < NewVer) || oldACL == 0 )
		{
			ACLInfo.nVersion = NewVer;
			ACLVer = NewVer;

			int sock = socket( AF_INET, SOCK_STREAM, 0 );

			struct sockaddr_in saddr;

			memset( &saddr, 0x00, sizeof(sockaddr_in) );

			saddr.sin_family = AF_INET;
			saddr.sin_addr.s_addr = inet_addr( Config::instance()->acl.monitorServerIP );
			saddr.sin_port = htons( Config::instance()->acl.monitorServerPORT );

			// 타임아웃 시간 재설정
			linger lingtime;
			lingtime.l_onoff = 1;
			lingtime.l_linger = 3;
			setsockopt(sock, SOL_SOCKET, SO_LINGER, (void*)&lingtime, sizeof(linger));

			if( connect(sock, (struct sockaddr*)&saddr, sizeof(sockaddr_in)) < 0 )
			{
				if( errno == EWOULDBLOCK )
					send( sock, &ACLInfo, sizeof(ACLInfo), 0 );
			}

			else
				send( sock, &ACLInfo, sizeof(ACLInfo), 0 );

			close( sock );
		}
		#endif
	}

	fp.close();
	rwMutex.release();

	return 1;
}

/**
@brief ME 에서 사용하는 ACL 검색 루틴
@return ACL_NOT_FOUND	url 길이가 너무 길거나 찾을 수 없다.
        ACL_FIND_DNS	DNS 목록에서 url 에 대한 데이터 발견
*/
int	AclRouter::searchDNS(const char *orgHost, const int orgPort, char *destHost, int destHostSize, int &destPort)
{
	if( strlen(orgHost) > 63+4 )
		return ACL_NOT_FOUND;

	ACL_KEY key;
	key.SrcAddr = orgHost;
	key.SrcPort = orgPort;

	// read 락을 건다. 이미 write 락이 걸려있다면 풀릴 때 까지 대기한다.
	rwMutex.acquire_read();

	iterACLDATA iter = mapDNS.find( key );
	if( iter == mapDNS.end() )
	{
		rwMutex.release();
		return ACL_NOT_FOUND;
	}

	ACL_DATA *pData = iter->second;

	// Proxy Flag 이 0 이면 사용하지 않는다는 뜻
/*	if( pData->OnOff == '0' )
	{
		rwMutex.release();
		return ACL_NOT_FOUND;
	}
*/
	STRNCPY( destHost, pData->DestAddr, destHostSize );
	destPort = atoi( pData->DestPort );

	rwMutex.release();

	return ACL_FIND_DNS;
}

/**
@brief KUN 에서 사용하는 ACL 검색 루틴
@return ACL_NOT_FOUND	url 길이가 너무 길거나 찾을 수 없다.
        ACL_DENY_ACL	Multiproxy
        ACL_ALLOW_ACL	인증완료
        ACL_FIND_DNS	DNS 목록에서 url 에 대한 데이터 발견
*/
int	AclRouter::searchALL(const char *orgHost, const int orgPort, char *destHost, int destHostSize, int &destPort)
{
	if( strlen(orgHost) > 63+4 )
		return ACL_NOT_FOUND;

	ACL_KEY key;
	key.SrcAddr = orgHost;
	key.SrcPort = orgPort;

	// read 락을 건다. 이미 write 락이 걸려있다면 풀릴 때 까지 대기한다.
	rwMutex.acquire_read();

	iterACLDATA iterACL = mapACL.find( key );
	iterACLDATA iterDNS;

	// ACL 목록에서 찾는다.
	if( iterACL == mapACL.end() )
	{
		// DNS 목록에서 찾는다.
		iterDNS = mapDNS.find( key );
		if( iterDNS == mapDNS.end() )
		{
			// 어디에도 없다면, orgurl을 ip로 변환하여 ACL 에서 검색
			if( (int) inet_addr(orgHost) == -1 )
			{
				char destIP[16] = "\0";
				bool isgethostname = false;

				//isgethostname = Util2::getHostByName_threadsafe( orgHost, destIP, sizeof(destIP) );
				isgethostname = DNS::Manager::instance()->getHostByName(orgHost, destIP, sizeof(destIP));
				
				if( isgethostname )
				{
					key.SrcAddr = destIP;
					key.SrcPort = orgPort;

					// ACL 목록에서 찾는다.
					iterACL = mapACL.find( key );
				}
			}
		}

		// DNS 목록에서 찾았으면 Dest Addr 로 다시 ACL 에서 찾는다.
		else
		{
			ACL_DATA *pDNS = iterDNS->second;
			key.SrcAddr = pDNS->DestAddr;
			key.SrcPort = atoi( pDNS->DestPort );

			// ACL 목록에서 찾는다.
			iterACL = mapACL.find( key );
		}
	}

	// ACL 목록에서 찾았다.
	if( iterACL != mapACL.end() )
	{
		ACL_DATA *pACL = iterACL->second;

/*		if( pACL->OnOff == '0' )
		{
			rwMutex.release();
			return ACL_NOT_FOUND;
		}
*/
		// Pas ID가 다르면 Host 목록에서 해당 Pas의 url을 기록한다.
		if( strcmp(pACL->Owner, PasID) != 0 )
		{
			// 멀티 Proxy
			iterACLHOST iterHOST = mapHost.find( pACL->Owner );
			if( iterHOST != mapHost.end() )
			{
				ACL_HOST *pHost = iterHOST->second;
				STRNCPY( destHost, pHost->Addr, destHostSize );
				destPort = atoi( pHost->Port );
			}

			rwMutex.release();

			return ACL_DENY_ACL;
		}

		// 인증승인
		else
		{
			STRNCPY( destHost, pACL->DestAddr, destHostSize );
			destPort = atoi( pACL->DestPort );

			rwMutex.release();

			return ACL_ALLOW_ACL;
		}
	}

	// 등록된 DNS 처리
	else
	if( iterDNS != mapDNS.end() )
	{
		ACL_DATA *pDNS = iterDNS->second;

/*		if( pDNS->OnOff == '0' )
		{
			rwMutex.release();
			return ACL_NOT_FOUND;
		}
*/
		STRNCPY( destHost, pDNS->DestAddr, destHostSize );
		destPort = atoi( pDNS->DestPort );

		rwMutex.release();

		// 2006.09.16 DNS 목록은 MultiProxy 와 전혀 무관하다.
		// MultiProxy는 오직 ACL 목록에서만 가능하다.
		// 그래서 아래의 코드는 주석 처리

		// DNS 목록에서 찾았지만, Host 명이 다르기 때문에 Multiproxy 해야한다.
		// Host 명이 "00000" 이 아니고, 현재 호스트명과 다를 경우 Multiproxy 한다.
//		if( strcmp(pDNS->Owner, PasID) && strcmp(pDNS->Owner, ALLTYPE_PASID) )
//			return ACL_DENY_ACL;

//		else
			return ACL_FIND_DNS;
	}

	rwMutex.release();

	return ACL_NOT_FOUND;
}

/**
@brief ACL 버전의 무결성을 검사한다.
@return 0 : 버전의 자리수가 다르거나 숫자가 아니다
        1 : 정상
*/
inline int AclRouter::chkACLHeader(const char* pszHeader)
{
	long size = strlen(pszHeader);

	if ( size < ACL_HEADER_VERSION_DIGIT_START )
		return 0;

	for ( int i = ACL_HEADER_VERSION_DIGIT_START; i < size; i++ )
		if ( pszHeader[i] < '0' || pszHeader[i] > '9' )
			return 0;

	return 1;
}

/**
@brief
ACL 에서 읽은 KUN 서버 목록 중에 해당하는 서버의 ip 와 port 를 가져온다.
*/
void AclRouter::getHost(char *host, int hostsize, int &port, const char *hostname)
{
	iterACLHOST iterHOST = mapHost.find( hostname );
	if( iterHOST == mapHost.end() )
	{
		// 혹시 못 찾는 경우가 생기면 대표 서버 IP를 셋팅
		strcmp( host, "128.134.98.22" );
		port = 9090;
	}

	else
	{
		ACL_HOST *pHost = iterHOST->second;
		STRNCPY( host, pHost->Addr, hostsize );
		port = atoi( pHost->Port );
	}
}

/**
@brief 호스트 목록 제거(delete)
*/
void AclRouter::clearHost()
{
	iterACLHOST iterhost = mapHost.begin();
	for( ; iterhost != mapHost.end(); iterhost++ )
		delete iterhost->second;

	mapHost.clear();
}

/**
@brief DNS 목록 제거(delete)
*/
void AclRouter::clearDNS()
{
	iterACLDATA iterdns = mapDNS.begin();
	for( ; iterdns != mapDNS.end(); iterdns++ )
		delete iterdns->second;

	mapDNS.clear();
}

/**
@brief ACL 목록 제거(delete)
*/
void AclRouter::clearACL()
{
	iterACLDATA iteracl = mapACL.begin();
	for( ; iteracl != mapACL.end(); iteracl++ )
		delete iteracl->second;

	mapACL.clear();
}

void AclRouter::test()
{
	ACL_KEY key;

	cout << "[Host]" << endl;
	iterACLHOST iterHost = mapHost.begin();
	for( ; iterHost != mapHost.end(); iterHost++ )
	{
		ACL_HOST *pHost = iterHost->second;
		printf( "[%s] [%s] [%s] [%s]\n", pHost->Name, pHost->Addr, pHost->Port, pHost->Opt );

		iterACLHOST iterIsFound = mapHost.find( pHost->Name );
		if( iterIsFound == mapHost.end() )
			printf( "[%s] [%s] [%s] [%s] Not Found!!\n", pHost->Name, pHost->Addr, pHost->Port, pHost->Opt );
	}

	cout << "[DNS]" << endl;
	iterACLDATA iterDNS = mapDNS.begin();
	for( ; iterDNS != mapDNS.end(); iterDNS++ )
	{
		ACL_DATA *pDNS = iterDNS->second;
		printf( "[%30s:%5s] -> [%15s:%5s]\t[%s]\t[%c]\n",
			pDNS->SrcAddr, pDNS->SrcPort, pDNS->DestAddr, pDNS->DestPort, pDNS->Owner, pDNS->OnOff );

		key.SrcAddr = pDNS->SrcAddr;
		key.SrcPort = strtol( pDNS->SrcPort, NULL, 10 );

		iterACLDATA iterIsFound = mapDNS.find( key );
		if( iterIsFound == mapDNS.end() )
			printf( "[%30s:%5s] -> [%15s:%5s]\t[%s]\t[%c] Not Found!!\n", pDNS->SrcAddr, pDNS->SrcPort, pDNS->DestAddr, pDNS->DestPort, pDNS->Owner, pDNS->OnOff );
	}

	cout << "[ACL]" << endl;
	iterACLDATA iterACL = mapACL.begin();
	for( ; iterACL != mapACL.end(); iterACL++ )
	{
		ACL_DATA *pACL = iterACL->second;
		printf( "[%30s:%5s] -> [%15s:%5s]\t[%s]\t[%c]\n",
			pACL->SrcAddr, pACL->SrcPort, pACL->DestAddr, pACL->DestPort, pACL->Owner, pACL->OnOff );

		key.SrcAddr = pACL->SrcAddr;
		key.SrcPort = strtol( pACL->SrcPort, NULL, 10 );

		iterACLDATA iterIsFound = mapACL.find( key );
		if( iterIsFound == mapACL.end() )
			printf( "[%30s:%5s] -> [%15s:%5s]\t[%s]\t[%c] Not Found!!\n", pACL->SrcAddr, pACL->SrcPort, pACL->DestAddr, pACL->DestPort, pACL->Owner, pACL->OnOff );
	}
}

#ifdef TEST_MAIN_ACL_ROUTER
int main( int argc, char* argv[] )
{
	AclRouter *pAcl = NULL;
	pAcl = AclRouter::instance();

	pAcl->init("KUN00", 9090);

	pAcl->activate();

	cout << "Waiting for ACL Load" << endl;

	sleep(4);

	pAcl->test();

	while(1)
	{
		char szURL[512] = "\0";
		char szPort[512] = "\0";

		cout << "Input url : ";
		gets( szURL );
		cout << "Input port: ";
		gets( szPort );

		char desturl[512] = "\0";
		int destport = 0;
		if( pAcl->searchALL(szURL, atoi(szPort), desturl, destport) == 0 )
			cout << "Not found" << endl;

		else
		{
			printf( "Result URL : %s\n", desturl );
			printf( "Result Port: %d\n", destport );
		}
	}

	return 0;
}
#endif

#ifdef TEST_MAIN_GETHOSTBYNAME
int main( int argc, char* argv[] )
{
	if( argc <= 1 )
	{
		printf("Usage : Aclrouter.test [Config.cfg] [URL] [PORT]\n");
		return 0;
	}

	Config* pConfig = Config::instance();

	if (pConfig->load(argv[1]) < 0)
		return -1;

	AclRouter *pAcl = NULL;
	pAcl = AclRouter::instance();

	pAcl->init("KUN00", 9090);

	pAcl->activate();

	cout << "Waiting for ACL Load" << endl;

	sleep(4);

	pAcl->test();

	while(1)
	{
		char startTime[64] = "\0";
		char endTime[64] = "\0";

		Util2::get_curr_time_str( startTime );

		char desturl[512] = "\0";
		int destport = 0;
		if( pAcl->searchALL(argv[2], atoi(argv[3]), desturl, destport) == 0 )
		{
			cout << "Not found" << endl;
			break;
		}

		else
		{
			Util2::get_curr_time_str( endTime );
			printf( "Start[%s] End[%s] URL[%s] Port[%d]\n", startTime, endTime, desturl, destport );
		}

		sleep(1);
	}

	return 0;
}
#endif
