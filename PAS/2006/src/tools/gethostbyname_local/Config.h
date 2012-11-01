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

#include "Common.h"
#include "LogLevel.h"
#include <ace/Configuration.h> 
#include <ace/Configuration_Import_Export.h> 

enum LogOutputType
{
	LOT_FILE,
	LOT_STDOUT
};

struct NetworkConfig
{
	int listenPort;
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
	bool	StatFilterLog;
	bool	HashKeyLog;
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
	bool	ReuseCpConnection;
	int timeoutCheckInterval;
	int connectionTimeout;
	int receiveTimeout;
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
	mstr_t dnshost;
	int dnsport;
};

struct HotNumberConfig
{
	bool enable;
	url_t ktfSearchUrl;
};

class Config
{
// 멤버함수
public:
	int load(const filename_t& filename);
	static Config* instance();

private:
	ACE_Configuration_Heap* createAndOpenReader(const filename_t& filename);

	int getValue(ACE_Configuration_Heap* pReader, const char* sectionName, const char* valueName, ACE_TString& value);
	int getValue(ACE_Configuration_Heap* pReader, const char* sectionName, const char* valueName, int& value, const int min, const int max);
	int getValue(ACE_Configuration_Heap* pReader, const char* sectionName, const char* valueName, bool& value);

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
};

#endif
