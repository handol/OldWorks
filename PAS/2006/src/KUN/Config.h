#ifndef __CONFIG_FILE__
#define __CONFIG_FILE__

/**
@brief config 파일을 파싱하여 메모리에 저장

config 파일의 구성은 아래와 같다.

[섹션명]
아이템1=1
아이템2="2"
아이템3=on
...

각 섹션에 해당하는 데이터 구조체가 있고 맴버변수에 값이 저장된다.
config 파일에 섹션에 해당하는 아이템이 없다면 실행시에 경고를 보여준다.
아이템이 없을 경우, 기본값으로 대체하지만 그렇지 못한 경우 실행이 중지된다.
*/

#include <ace/Configuration.h> 
#include <ace/Configuration_Import_Export.h> 

#include "Common.h"
#include "LogLevel.h"
#include "FastString.hpp"


enum LogOutputType
{
	LOT_FILE,
	LOT_STDOUT
};

struct NetworkConfig
{
	int listenPort;
	mstr_t L4Addr1;
	mstr_t L4Addr2;
	host_t kunHost;
	bool streaming;
	int streamingMinSize;
};

struct ThreadConfig
{
	int numWorker;
};

struct ProcessConfig
{
	bool daemon;
	bool billinfo;
	bool checkMaxClients;

	int	maxClients; // 동시 접속자 수
	int	RecvBufferBytes;
	int	HttpHeaderBufferBytes;
	//const char	billinfoTarget[LEN_PHONE_NUM+1];
	ACE_TString billinfoTarget;
	filename_t memconf;
	serverid_t serverID;
	int	userIdleSec;
	int sysinfointerval;
	int statfilterinterval;
	int phonetraceinterval;
	bool StatFilterLog;
	bool HashKeyLog;
	ACE_TString pasmonAddr;
	int	pasmonPort;
	int	pasmonPeriod;
	ACE_TString watchsvrAddr;
	int	watchsvrPort;
	int	watchsvrPeriod;

	bool browserTypeCheck;
};

struct LogConfig
{
	LogLevelType level;
	LogOutputType outputType;
	filename_t filename;	
};

struct ClientConfig
{
	int timeoutCheckInterval;
	int receiveTimeout;
};

struct CPConfig
{
	bool ReuseCpConnection;
	int timeoutCheckInterval;
	int connectionTimeout;
	int receiveTimeout;
	int fakeCpPort;
};

struct AuthConfig
{
	bool enable;
	host_t host;
	int port;
	int timeoutCheckInterval;
	int connectionTimeout;
	int receiveTimeout;
	int errCount;
	int retryDelay;
	int authentication;
};

struct SantaConfig
{
	bool enable;
	santaId_t ID;
	password_t Password;
	svcID_t svcID;
	host_t host;
	int port;
	int timeoutCheckInterval;
	int connectionTimeout;
	int receiveTimeout;
};

struct AclConfig
{
	bool enable;
	mstr_t fileName;
	host_t monitorServerIP;
	int monitorServerPORT;
	int updateCheckInterval;
	bool multiproxy;
};

struct HotNumberConfig
{
	bool enable;
	url_t ktfSearchUrl;
};

struct DNSConfig
{
	int queryTimeoutSec;
	int maxNumOfQueriers;

	bool cacheEnable;
	int cacheTimeoutSec;
};

struct SisorConfig
{
	host_t host;
	int port;
	int recvTimeoutSec;
	int connectionRetryIntervalSec;
	int connectionPoolSize;
};

/// 안내페이지
struct GuideConfig
{
	bool enable;
	url_t guideUrlMagicn;
	url_t guideUrlFimm;
	url_t guideUrlMultipack;

	url_t guideResultUrlMagicn;
	url_t guideResultUrlFimm;
	url_t guideResultUrlMultipack;

	int fimmSpecificFirstCounter;
};

// 세션정보 업데이트
struct SessionInfoUpdateConfig
{
	bool enable;
};

// 과금인증 (가입형 정액제 서비스)
struct ServiceConfig
{
	bool enable;
};

// Speed up tag
struct SpeedupTagConfig
{
	bool enable;
	int maxSize;
};

class Config
{
// 멤버함수
public:
	static Config* instance();

	int load(const filename_t& filename);

	TinyString getLogPrefix();

private:
	ACE_Configuration_Heap* createAndOpenReader(const filename_t& filename);

	int getValue(ACE_Configuration_Heap* pReader, const char* sectionName, const char* valueName, ACE_TString& value);
	int getValue(ACE_Configuration_Heap* pReader, const char* sectionName, const char* valueName, int& value, const int min, const int max);
	int getValue(ACE_Configuration_Heap* pReader, const char* sectionName, const char* valueName, bool& value);

	template<size_t Size> 
	int getValue(ACE_Configuration_Heap* pReader, const char* sectionName, const char* valueName, FastString<Size>& value);

	int loadProcessSection(ACE_Configuration_Heap* pReader);
	int loadThreadSection(ACE_Configuration_Heap* pReader);
	int loadNetworkSection(ACE_Configuration_Heap* pReader);
	int loadLogSection(ACE_Configuration_Heap* pReader);
	int loadClientSection(ACE_Configuration_Heap* pReader);
	int loadCPSection(ACE_Configuration_Heap* pReader);
	int loadAuthSection(ACE_Configuration_Heap* pReader);
	int loadSantaSection(ACE_Configuration_Heap* pReader);
	int loadAclSection(ACE_Configuration_Heap* pReader);
	int loadHotNumberSection(ACE_Configuration_Heap* pReader);
	int loadDNSSection(ACE_Configuration_Heap* pReader);
	int loadSisorSection(ACE_Configuration_Heap* pReader);
	int loadGuideSection(ACE_Configuration_Heap* pReader);
	int loadServiceSection(ACE_Configuration_Heap* pReader);
	int loadSessionInfoUpdateSection(ACE_Configuration_Heap* pReader);
	int loadSpeedupTagSection(ACE_Configuration_Heap* pReader);
	

// 멤버변수
public:
	ProcessConfig process;
	NetworkConfig network;
	ThreadConfig thread;
	LogConfig log;
	ClientConfig client;
	CPConfig cp;
	AuthConfig auth;
	SantaConfig santa;
	AclConfig acl;
	HotNumberConfig hotnumber;
	DNSConfig dns;
	SisorConfig sisor;
	GuideConfig guide;
	ServiceConfig service;
	SessionInfoUpdateConfig sessionInfoUpdate;
	SpeedupTagConfig speedupTag;
};

template<size_t Size>
int Config::getValue( ACE_Configuration_Heap* pReader, const char* sectionName, const char* valueName, FastString<Size>& value )
{
	ACE_TString tString;
	if(getValue(pReader, sectionName, valueName, tString) < 0)
		return -1;

	value = tString.c_str();
	return 0;
}


#endif
