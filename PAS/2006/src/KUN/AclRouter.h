#ifndef __ACL_ROUTER_H__
#define __ACL_ROUTER_H__

/**
@file   AclRouter.h
@author DMS
@brief  magicN DNS 기능 - PAS 자체적으로 정보 관리

-ACL 파일 포맷-

[식별자(4)] [버전(14)]
## End of 헤더
[서버명(5)] [IP(15)] [PORT(5)] [??(1)]
## End of PAS
[Src Addr] [Src Port] [Dest Addr] [Dest Port] [서버명] [CP 가 Proxy 서버인가]
## End of DNS
OV14			<- ACL DNS 버전
[Src Addr] [Src Port] [Dest Addr] [Dest Port] [서버명] [CP 가 Proxy 서버인가]
## End of ACL
ACLX            <- tail...end of file

-기능-
1. 더블샾(##)은 한 필드의 끝을 나타낸다.

2. ACL 파일을 로드하기 전에 버전을 체크한다.

3. DNS 영역에서 서버명이 00000 이 아니거나, 자신의 서버명(예:KUN01)과 다르면 무시한다.

4. 로드가 끝난 후 ACL Version이 변경 되었다면 Monitor 서버에 ACL Info를 송신한다.
-ACL Info : 현재 호스트명, pasgw 포트, ACL DNS 버전

5. 데이터 검색은 ME 와 KUN 용이 있다.
- DNS 검색은 KUN, ME 용
- ACL 검색은 KUN 용

6. 데이터 검색 알고리즘(KUN)
- ACL 을 검색한다.
- DNS 를 검색한다.
- DNS 데이터가 있으면 Dest Addr로 다시 ACL 검색 한다.
- 둘다 없을 경우 URL을 nslookup 하여 IP를 추출한 다음 ACL을 검색한다.
- ACL 데이터가 있으면 HOST ID 를 자신과 비교한다. 같으면 Dest Addr를 리턴하고
  다르면 해당 HOST ID의 IP로 멀티Proxy 처리한다.
- DNS 데이터가 있으면 HOST ID 를 자신과 비교한다. 같으면 Dest Addr를 리턴하고
  다르면 해당 HOST ID의 IP로 멀티Proxy 처리한다.

7. 리턴값의 의미
ACL_NOT_FOUND : 입력한 url 로 DNS 와 ACL 을 검색 하였지만 데이터가 없어서
                url 을 nslookup 하여 ip 를 추출한 다음 ACL 을 검색했지만 역시 없다.

ACL_DENY_ACL  : ACL 에서 데이터를 찾았지만, HOST(KUN01)가 다르기 때문에 Multi Proxy 기능으로 변경
                즉, 9090 포트가 KUN00 이므로 9091 포트 KUN01 로 재접속 하라는 의미

ACL_ALLOW_ACL : ACL 에서 데이터를 찾았고, HOST(KUN01)가 같으므로 접속을 허용한다.
                이 경우는 9091 포트로 Multi Proxy 되어 들어온 유저이거나
                애초에 Multi Proxy 가 필요없는 유저이거나 둘중 한가지 경우이다.

ACL_FIND_DNS  : DNS 에서 데이터를 찾았다.

-Pasgw 에서 AclRouter API 사용법-
1.ME 서버는 searchDNS 만 사용한다.
리턴값은 ACL_NOT_FOUND 혹은 ACL_FIND_DNS 둘중 하나
2.KUN 서버는 searchALL 만 사용한다.
ACL_FIND_DNS, ACL_ALLOW_ACL 의 경우 변환된 url 과 port 를 사용한다.
ACL_NOT_FOUND 의 경우에는 원본 url 과 port 를 사용한다.
ACL_DENY_ACL 의 경우에는 변환된 url 과 port 를 Multiproxy 처리 해야한다.

Multiproxy 란
KUN 에만 해당하는 기능이고, 단말기가 9090 서버에 접속해서 요청을 보냈을 경우
pasgw는 우선 ACL 목록을 검색하게 되고 찾은 아이템이 있다면 Host 명을 비교하게 된다.
하지만 Host 명이 9090 의 것이(KUN00) 아니라면 단말기에 9091(KUN01)로 재접속 하라고
알려주게되고, 단말기는 9091 서버로 재접속 한다.
*/

#include <string>
#include <ace/Task_T.h>
#include <ace/Singleton.h>
#include <sys/socket.h>
#include <iostream>
#include <netinet/in.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <map>

#include "Common.h"
#include "MyFile.h"

using namespace std;


#define	MAXLEN_URL	(255)
#define DNS_URL_SIZE		99
#define HEAD_VERSION_SIZE	14
#define HOST_NAME_SIZE		16

#define ACL_PREFIX			"PAS1"
#define ACL_TAIL			"ACLX"
#define ALLTYPE_PASID		"00000"
#define ACL_HEADER_VERSION_DIGIT_START	2
#define ACL_APPLY_HOSTNAME_SIZE			24
#define ACL_MONITOR_SERVER	"221.148.247.32"
#define ACL_MONITOR_PORT	5015

#define ACL_NOT_FOUND		0
#define ACL_INPUT_ERR		-1
#define ACL_DENY_ACL		-2
#define ACL_ALLOW_ACL		-3
#define ACL_FIND_DNS		-4

#define ACL_SYNTAX_ERROR	-1
#define ACL_FILE_NOT_FOUND	-2
#define ACL_VERSION_ERROR	-3
#define ACL_TAIL_ERROR		-4


struct ACL_KEY
{
	string SrcAddr;
	int SrcPort;

	bool operator < (const ACL_KEY& ref) const
	{
		if (SrcAddr == ref.SrcAddr)
		{
			return SrcPort < ref.SrcPort;
		}
		else
			return SrcAddr < ref.SrcAddr;
	}
};

struct ACL_DATA
{
	ACL_DATA()
	{
		memset( SrcAddr, 0x00, sizeof(SrcAddr) );
		memset( SrcPort, 0x00, sizeof(SrcPort) );
		memset( DestAddr, 0x00, sizeof(DestAddr) );
		memset( DestPort, 0x00, sizeof(DestPort) );
		memset( Owner, 0x00, sizeof(Owner) );
		OnOff = 0;
	};

	char SrcAddr[DNS_URL_SIZE+1];
	char SrcPort[6];
	char DestAddr[DNS_URL_SIZE+1];
	char DestPort[6];
	char Owner[6];
	char OnOff;
};

struct ACL_HOST
{
	ACL_HOST()
	{
		memset( Name, 0x00, sizeof(Name) );
		memset( Addr, 0x00, sizeof(Addr) );
		memset( Port, 0x00, sizeof(Port) );
		memset( Opt, 0x00, sizeof(Opt) );
	};

	char Name[HOST_NAME_SIZE+1];
	char Addr[DNS_URL_SIZE+1];
	char Port[6];
	char Opt[2];
};

struct ACL_RECORD_INFO
{
	char szHostName[ACL_APPLY_HOSTNAME_SIZE];
	unsigned int nPort;
	unsigned int nVersion;
};

typedef map<ACL_KEY, ACL_DATA*>						mapACLDATA;
typedef map<ACL_KEY, ACL_DATA*>::iterator			iterACLDATA;
typedef map<string, ACL_HOST*>						mapACLHOST;
typedef map<string, ACL_HOST*>::iterator			iterACLHOST;

class AclRouter : public ACE_Task<PAS_SYNCH>
{
private:
	char headVersion[HEAD_VERSION_SIZE+1];		/// ACL 파일 버전
	char PasID[HOST_NAME_SIZE+1];				/// KUN00, KUN01 등...
	int ACLVer;									/// ACL 버전(예:OV14)
	int mTime;									/// ACL 파일의 마지막 변경 시간
	int PasgwPort;								/// PAS 서버 포트
	int run;									/// Worker thread 제어용 플랙
	mapACLDATA mapDNS;							/// DNS 목록 (KUN, ME 용)
	mapACLDATA mapACL;							/// ACL 목록 (KUN 용)
	mapACLHOST mapHost;							/// PAS 호스트 목록
	ACE_RW_Thread_Mutex rwMutex;				/// 동기화 RW Mutex 객체

private:
	bool setHead(MyFile &fp);
	bool setHost(char *pBuff);
	bool setDNS(char *pBuff);
	bool setACL(char *pBuff);
	void clearHost();
	void clearDNS();
	void clearACL();

protected:
	inline int chkACLHeader(const char* pszHeader);
	bool syntaxCheck(const char* pszFileName);
	//virtual int init (int argc, ACE_TCHAR *argv[]);

public:
	AclRouter(ACE_Thread_Manager* threadManager=0)
		: ACE_Task<PAS_SYNCH>(threadManager, NULL)
	{
		headVersion[0] = 0;
		PasID[0] = 0;
		ACLVer = 0;
		mTime = 0;
		PasgwPort = 9090;
		run = 1;

		STRNCPY( PasID, "KUN00", HOST_NAME_SIZE );
	}

	~AclRouter();

	virtual int svc();

	int initial(const char *pHostID, int nPasgwPort);
	void remove_worker_thread(void);
	int load(const char *pFileName);
	int	searchDNS(const char *orgHost, const int orgPort, char *destHost, int destHostSize, int &destPort);
	int	searchALL(const char *orgHost, const int orgPort, char *destHost, int destHostSize, int &destPort);

	static AclRouter *instance();
	void getHost(char *host, int hostsize, int &port, const char *hostname);

	void test();
};

#endif
