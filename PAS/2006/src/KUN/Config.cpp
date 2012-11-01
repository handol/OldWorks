#include "Common.h"
#include "Config.h"
#include <ace/Singleton.h>

Config* Config::instance()
{
	return ACE_Singleton<Config, PAS_SYNCH_MUTEX>::instance();
}

int Config::getValue(ACE_Configuration_Heap* pReader, const char* sectionName, const char* valueName, ACE_TString& value)
{
	ACE_ASSERT(pReader != NULL);

	ACE_Configuration_Section_Key section;
	int ret = pReader->open_section(pReader->root_section(), sectionName, 0, section);
	if(ret < 0)
	{
		printf("Config >> %s 섹션 열기 실패\n", sectionName);
		return -1;
	}

	// CLIENT >> ReceiveTimeout
	ret = pReader->get_string_value(section, valueName, value);
	if(ret == -1)
	{
		printf("Config >> %s >> %s 읽기 실패\n", sectionName, valueName);
		return -1;
	}

	if(value.length() == 0)
	{
		printf("Config >> %s >> %s가 설정되어 있지 않습니다.\n", sectionName, valueName);
		return -1;
	}

	return 0;
}

int Config::getValue(ACE_Configuration_Heap* pReader, const char* sectionName, const char* valueName, int& value, const int min, const int max)
{
	ACE_ASSERT(pReader != NULL);

	ACE_Configuration_Section_Key section;
	int ret = pReader->open_section(pReader->root_section(), sectionName, 0, section);
	if(ret < 0)
	{
		printf("Config >> %s 섹션 열기 실패\n", sectionName);
		return -1;
	}

	// CLIENT >> ReceiveTimeout
	ACE_TString valueString;
	ret = pReader->get_string_value(section, valueName, valueString);
	if(ret == -1)
	{
		printf("Config >> %s >> %s 읽기 실패\n", sectionName, valueName);
		return -1;
	}

	if(valueString.length() == 0)
	{
		printf("%s >> %s가 설정되어 있지 않습니다.\n", sectionName, valueName);
		return -1;
	}

	value = atoi(valueString.c_str());

	bool inRange = (min <= value && value <= max);
	
	if(inRange)
	{
		return 0;
	}
	else
	{
		printf("Config >> %s >> %s 의 범위는 %d~%d 입니다.\n", sectionName, valueName, min, max);
		return -1;
	}
}

int Config::getValue(ACE_Configuration_Heap* pReader, const char* sectionName, const char* valueName, bool& value)
{
	ACE_ASSERT(pReader != NULL);

	ACE_Configuration_Section_Key section;
	int ret = pReader->open_section(pReader->root_section(), sectionName, 0, section);
	if(ret < 0)
	{
		printf("Config >> %s 섹션 열기 실패\n", sectionName);
		return -1;
	}

	// CLIENT >> ReceiveTimeout
	ACE_TString valueString;
	ret = pReader->get_string_value(section, valueName, valueString);
	if(ret == -1)
	{
		printf("Config >> %s >> %s 읽기 실패\n", sectionName, valueName);
		return -1;
	}

	if(valueString.length() ==0)
	{
		printf("%s >> %s가 설정되어 있지 않습니다.\n", sectionName, valueName);
		return -1;
	}

	if(valueString == "yes" || valueString == "true" || valueString == "1")
	{
		value = true;
	}
	else if(valueString == "no" || valueString == "false" || valueString == "0")
	{
		value = false;
	}
	else
	{
		printf("Config >> %s >> %s 는 [yes 또는 no] 여야 합니다.\n", sectionName, valueName);
		return -1;
	}

	return 0;
}

int Config::load(const filename_t& filename)
{
	// config 값 읽기
	ACE_Configuration_Heap* pReader = createAndOpenReader(filename);
	if(pReader == NULL)
	{
		printf("Config >> 생성/열기 실패\n");
		return -1;
	}

	int result = 0;

	if(loadProcessSection(pReader) < 0)
		result = -1;

	if(loadThreadSection(pReader) < 0)
		result = -1;

	if(loadNetworkSection(pReader) < 0)
		result = -1;

	if(loadLogSection(pReader) < 0)
		result = -1;

	if(loadClientSection(pReader) < 0)
		result = -1;

	if(loadCPSection(pReader) < 0)
		result = -1;

	if(loadAuthSection(pReader) < 0)
		result = -1;

	if(loadSantaSection(pReader) < 0)
		result = -1;

	if(loadAclSection(pReader) < 0)
		result = -1;

	if(loadDNSSection(pReader) < 0)
		result = -1;

	if(loadSisorSection(pReader) < 0)
		result = -1;

	if(loadGuideSection(pReader) < 0)
		result = -1;

	if(loadServiceSection(pReader) < 0)
		result = -1;

	if(loadSessionInfoUpdateSection(pReader) < 0)
		result = -1;

	if(loadSpeedupTagSection(pReader) < 0)
		result = -1;

//	if(loadHotNumberSection(pReader) < 0)
//		result = -1;

	// config reader 삭제
	delete pReader;

	return result;
}


ACE_Configuration_Heap* Config::createAndOpenReader(const filename_t& filename)
{
	ACE_Configuration_Heap* pConfig = new ACE_Configuration_Heap;
	
	int ret = pConfig->open();
	if(ret < 0)
	{
		printf("Config 초기화 실패");
		return NULL;
	}

	ACE_Ini_ImpExp importer(*pConfig);
	ret = importer.import_config(filename);
	
	// importer 는 ret < 0 으로 테스트해서는 안된다.
	// 정상처리일 경우에도 0보다 작은 값을 리턴하는 듯 하다.
	if(ret == -1)
	{
		printf("%s Import Fail", filename.toStr());
		return NULL;
	}

	return pConfig;
}

int Config::loadProcessSection(ACE_Configuration_Heap* pReader)
{
	ACE_ASSERT(pReader != NULL);

	// PROCESS >> Daemon
	if(getValue(pReader, "PROCESS", "Daemon", process.daemon) < 0)
		process.daemon = true;

	// PROCESS >> ServerID
	if(getValue(pReader, "PROCESS", "ServerID", process.serverID) < 0)
		return -1;

	// server id 에 ME 나 KUN 이 없으면 에러
	// 다른 설정들이 server id 에 의존적인 것들이 있기 때문에 server id 가 정확해야 한다.
	if(process.serverID.incaseFind("ME") < 0 && process.serverID.incaseFind("KUN") < 0)
		return -1;

	if(getValue(pReader, "PROCESS", "CheckMaxClients", process.checkMaxClients) < 0)
		process.checkMaxClients = false;

	if(getValue(pReader, "PROCESS", "MaxClients", process.maxClients, 32, 100*100*100) < 0)
		process.maxClients = 100*100*100;

	
	if(getValue(pReader, "PROCESS", "RecvBufferBytes", process.RecvBufferBytes, 1024, 1024*64) < 0)
		process.RecvBufferBytes = 1024*4;
	
		
	if(getValue(pReader, "PROCESS", "HttpHeaderBufferBytes", process.HttpHeaderBufferBytes, 1024, 1024*10) < 0)
		process.HttpHeaderBufferBytes = 1024*2;
		

	// PROCESS >> UserIdleSec
	if(getValue(pReader, "PROCESS", "UserIdleSec", process.userIdleSec, 1, 60*20) < 0) 
		process.userIdleSec = 1200;

	// PROCESS >> HOTNUMBER
	if(getValue(pReader, "PROCESS", "HotNumber", hotnumber.enable) < 0)
		hotnumber.enable = true;

	// PROCESS >> KtfSearchUrl
	if(getValue(pReader, "PROCESS", "KtfSearchUrl", hotnumber.ktfSearchUrl) < 0)
		return -1;

	// PROCESS >> BillInfo
	if(getValue(pReader, "PROCESS", "BillInfo", process.billinfo) < 0)
		process.billinfo = false;

	// PROCESS >> BillInfoTarget
	if(getValue(pReader, "PROCESS", "BillInfoTarget", process.billinfoTarget) < 0)
		process.billinfo = false;

	// PROCESS >> MemConfig FileName
	if(getValue(pReader, "PROCESS", "MemConfig", process.memconf) < 0)
		process.memconf = "";

	// PROCESS >> SysInfoInterval
	if(getValue(pReader, "PROCESS", "SysInfoInterval", process.sysinfointerval, 5, 600) < 0)
		process.sysinfointerval = 30;

	// PROCESS >> StatFilterInterval
	if(getValue(pReader,"PROCESS", "StatFilterInterval", process.statfilterinterval, 5, 60) < 0)
		process.statfilterinterval = 5;

	// PROCESS >> PhoneTraceInterval
	if(getValue(pReader, "PROCESS", "PhoneTraceInterval", process.phonetraceinterval, 5, 60) < 0)
		process.phonetraceinterval = 5;

	// PROCESS >> PhoneTraceInterval
	if(getValue(pReader, "PROCESS", "StatFilterLog", process.StatFilterLog) < 0)
		process.StatFilterLog = false;

	if(getValue(pReader, "PROCESS", "HashKeyLog", process.HashKeyLog) < 0)
		process.HashKeyLog = true;

	if(getValue(pReader, "PROCESS", "pasmonAddr", process.pasmonAddr) < 0)
		return -1;

	if(getValue(pReader, "PROCESS", "pasmonPort", process.pasmonPort, 0, 0xffff) < 0)
		return -1;

	if(getValue(pReader, "PROCESS", "pasmonPeriod", process.pasmonPeriod, 10, 60) < 0)
		return -1;

	if(getValue(pReader, "PROCESS", "watchsvrAddr", process.watchsvrAddr) < 0)
		return -1;

	if(getValue(pReader, "PROCESS", "watchsvrPort", process.watchsvrPort, 0, 0xffff) < 0)
		return -1;

	if(getValue(pReader, "PROCESS", "watchsvrPeriod", process.watchsvrPeriod, 10, 120) < 0)
		return -1;

	// PROCESS >> BrowserTypeCheck
	if(getValue(pReader, "PROCESS", "BrowserTypeCheck", process.browserTypeCheck) < 0)
		process.browserTypeCheck = true;
	
	return 0;
}

int Config::loadThreadSection(ACE_Configuration_Heap* pReader)
{
	ACE_ASSERT(pReader != NULL);

	// THREAD >> NumOfWorker
	if(getValue(pReader, "THREAD", "NumOfWorker", thread.numWorker, 1, 255) < 0)
		thread.numWorker = 1;

	return 0;
}


int Config::loadNetworkSection(ACE_Configuration_Heap* pReader)
{
	ACE_ASSERT(pReader != NULL);

	// NETWORK >> ListenPort
	if(getValue(pReader, "NETWORK", "ListenPort", network.listenPort, 0, 0xffff) < 0)
		return -1;

	// NETWORK >> L4Addr1
	ACE_TString value;
	if(getValue(pReader, "NETWORK", "L4Addr1", network.L4Addr1) < 0)
		return -1;

	// NETWORK >> L4Addr2
	if(getValue(pReader, "NETWORK", "L4Addr2", network.L4Addr2) < 0)
		return -1;

	// NETWORK >> KUN Host
	if(getValue(pReader, "NETWORK", "KunHost", network.kunHost) < 0)
		return -1;

	// NETWORK >> Using Streaming
	if(getValue(pReader, "NETWORK", "Streaming", network.streaming) < 0)
		return -1;

	// NETWORK >> Streaming minimum size
	if(getValue(pReader, "NETWORK", "StreamingMinSize", network.streamingMinSize, 1, 0x7FFFFFFF) < 0)
		return -1;

	return 0;
}

int Config::loadLogSection(ACE_Configuration_Heap* pReader)
{
	ACE_ASSERT(pReader != NULL);

	// LOG >> Level
	ACE_TString value;
	if(getValue(pReader, "LOG", "Level", value) < 0)
		return -1;

	if(value == "HIGH" || value=="4")
	{
		log.level = LLT_HIGH;
	}
	else if(value == "MEDIUM" || value=="3")
	{
		log.level = LLT_MEDIUM;
	}
	else if(value == "LOW" || value=="2")
	{
		log.level = LLT_LOW;
	}
	else if(value == "VERYLOW" || value == "1")
	{
		log.level = LLT_VERYLOW;
	}
	else
	{
		printf("Config >> LOG >> Level = [HIGH or MEDIUM or LOW or VERYLOW]\n");
		return -1;
	}

	// LOG >> Output
	if(getValue(pReader, "LOG", "Output", value) < 0)
		return -1;

	if(value == "FILE")
	{
		log.outputType = LOT_FILE;
	}
	else if(value == "STDOUT")
	{
		log.outputType = LOT_STDOUT;
	}
	else
	{
		printf("Config >> LOG >> Output = [FILE or STDOUT]\n");
	}

	// 로그기록 방식이 파일 일때
	if(log.outputType == LOT_FILE)
	{
		// LOG >> Filename
		if(getValue(pReader, "LOG", "Filename", value) < 0)
			return -1;

		log.filename = value.c_str();
	}

	return 0;
}

int Config::loadClientSection(ACE_Configuration_Heap* pReader)
{
	ACE_ASSERT(pReader != NULL);

	// CLIENT >> TimeoutCheckInterval
	if(getValue(pReader, "CLIENT", "TimeoutCheckInterval", client.timeoutCheckInterval, 1, 60) < 0)
		client.timeoutCheckInterval = 20;
	
	// CLIENT >> ReceiveTimeout
	if(getValue(pReader, "CLIENT", "ReceiveTimeout", client.receiveTimeout, 1, 3600) < 0)
		client.receiveTimeout = 60*10;
	
	return 0;
}

int Config::loadCPSection(ACE_Configuration_Heap* pReader)
{
	ACE_ASSERT(pReader != NULL);

	// 2006-12-01 현근창
	// ReuseCpConnection 버그로 인하여 ReuseCpConnection 기능 제거
	if(getValue(pReader, "CP", "ReuseCpConnection", cp.ReuseCpConnection) < 0)
		cp.ReuseCpConnection = false;

	// CP >> TimeoutCheckInterval
	if(getValue(pReader, "CP", "TimeoutCheckInterval", cp.timeoutCheckInterval, 1, 5) < 0)
		cp.timeoutCheckInterval = 1;

	// CP >> ConnectionTimeout
	if(getValue(pReader, "CP", "ConnectionTimeout", cp.connectionTimeout, 5, 60) < 0)
		cp.connectionTimeout = 10;

	// CP >> ReceiveTimeout
	if(getValue(pReader, "CP", "ReceiveTimeout", cp.receiveTimeout, 5, 300) < 0)
		cp.receiveTimeout = 20;

	cp.fakeCpPort = 0;
	
	#ifdef TEST_REAL_DUMP_DATA
	// 상용 PAS 에서 덤프한 데이타에 기반한 검증시에 필요.
	if(getValue(pReader, "CP", "FakeCpPort", cp.fakeCpPort, 1000, 65000) < 0)
		cp.fakeCpPort = 0;
	#endif
	
	return 0;
}

int Config::loadAuthSection(ACE_Configuration_Heap* pReader)
{
	ACE_ASSERT(pReader != NULL);

	// AUTH >> Enable
	if(getValue(pReader, "AUTH", "Enable", auth.enable) < 0)
		auth.enable = true;

	// AUTH >> Host
	ACE_TString value;
	if(getValue(pReader, "AUTH", "Host", value) < 0)
		value = "210.123.89.91";	// Auth L4 IP

	auth.host = value.c_str();

	// AUTH >> Port
	if(getValue(pReader, "AUTH", "Port", auth.port, 0, 0xffff) < 0)
		auth.port = 5001;

	// AUTH >> TimeoutCheckInterval
	if(getValue(pReader, "AUTH", "TimeoutCheckInterval", auth.timeoutCheckInterval, 1, 5) < 0)
		auth.timeoutCheckInterval = 1;

	// AUTH >> ConnectionTimeout
	if(getValue(pReader, "AUTH", "ConnectionTimeout", auth.connectionTimeout, 1, 60) < 0)
		auth.connectionTimeout = 10;

	// AUTH >> ReceiveTimeout
	if(getValue(pReader, "AUTH", "ReceiveTimeout", auth.receiveTimeout, 1, 60) < 0)
		auth.receiveTimeout = 10;

	// AUTH >> ErrorCount
	if(getValue(pReader, "AUTH", "ErrorCount", auth.errCount, 1, 10) < 0)
		auth.errCount = 10;

	// AUTH >> RetryDelay
	if(getValue(pReader, "AUTH", "RetryDelay", auth.retryDelay, 10, 600) < 0)
		auth.retryDelay = 30;

	// AUTH >> Authentication
	if(getValue(pReader, "AUTH", "Authentication", auth.authentication, 60, 3600) < 0)
		auth.authentication = 600;

	return 0;
}
int Config::loadSantaSection(ACE_Configuration_Heap* pReader)
{
	ACE_ASSERT(pReader != NULL);

	// SANTA >> Enable
	if(getValue(pReader, "SANTA", "Enable", santa.enable) < 0)
		santa.enable = true;

	// 사용하지 않은면 더이상 config 값을 읽을 필요 없다.
	if(santa.enable == false)
		return 0;

	// SANTA >> ID
	ACE_TString value;
	if(getValue(pReader, "SANTA", "ID", value) < 0)
		value = "pas";

	santa.ID = value.c_str();

	// SANTA >> PASSWORD
	if(getValue(pReader, "SANTA", "Password", value) < 0)
		value = "pas";

	santa.Password = value.c_str();

	// SANTA >> Service ID
	if(getValue(pReader, "SANTA", "ServiceID", value) < 0)
		value = "mapint1";

	santa.svcID = value.c_str();

	// SANTA >> Host
	if(getValue(pReader, "SANTA", "Host", value) < 0)
		value = "220.73.145.36";

	santa.host = value.c_str();

	// SANTA >> Port
	if(getValue(pReader, "SANTA", "Port", santa.port, 0, 0xffff) < 0)
		santa.port = 30001;

	// SANTA >> TimeoutCheckInterval
	if(getValue(pReader, "SANTA", "TimeoutCheckInterval", santa.timeoutCheckInterval, 1, 5) < 0)
		santa.timeoutCheckInterval = 2;

	// SANTA >> ConnectionTimeout
	if(getValue(pReader, "SANTA", "ConnectionTimeout", santa.connectionTimeout, 5, 60) < 0)
		santa.connectionTimeout = 10;

	// SANTA >> ReceiveTimeout
	if(getValue(pReader, "SANTA", "ReceiveTimeout", santa.receiveTimeout, 5, 60) < 0)
		santa.receiveTimeout = 10;

	return 0;
}

int Config::loadAclSection(ACE_Configuration_Heap* pReader)
{
	ACE_ASSERT(pReader != NULL);

	// ACL >> Enable
	if(getValue(pReader, "ACL", "Enable", acl.enable) < 0)
		acl.enable = true;

	// 사용하지 않은면 더이상 config 값을 읽을 필요 없다.
	if(acl.enable == false)
		return 0;

	// ACL 파일명
	if(getValue(pReader, "ACL", "FileName", acl.fileName) < 0)
		acl.fileName = "../pas.acl";

	// ACL 모니터 서버 IP
	if(getValue(pReader, "ACL", "Monitor Server IP", acl.monitorServerIP) < 0)
		acl.monitorServerIP = "192.168.208.98";

	// ACL 모니터 서버 PORT
	if(getValue(pReader, "ACL", "Monitor Server PORT", acl.monitorServerPORT, 0, 0xffff) < 0)
		acl.monitorServerPORT = 5015;

	// ACL >> UpdateCheckInterval
	if(getValue(pReader, "ACL", "UpdateCheckInterval", acl.updateCheckInterval, 3, 60) < 0)
		acl.updateCheckInterval = 3;

	// ACL >> MultiProxy
	if(getValue(pReader, "ACL", "MultiProxy", acl.multiproxy) < 0)
		acl.multiproxy = true;

	return 0;
}

int Config::loadDNSSection(ACE_Configuration_Heap* pReader)
{
	ACE_ASSERT(pReader != NULL);

	// DNS >> Timeout
	if(getValue(pReader, "DNS", "Timeout", dns.queryTimeoutSec, 1, 60) < 0)
		dns.queryTimeoutSec = 3;

	// DNS >> Max Number of Query Thread
	if(getValue(pReader, "DNS", "MaxNumOfQuerier", dns.maxNumOfQueriers, 1, 256) < 0)
		dns.maxNumOfQueriers = 32;
		
	// DNS >> Cache Enable
	if(getValue(pReader, "DNS", "CacheEnable", dns.cacheEnable) < 0)
		dns.cacheEnable = true;

	// DNS >> Cache Data Expired Time (sec)
	if(getValue(pReader, "DNS", "CacheTimeoutSec", dns.cacheTimeoutSec, 1, 99999999) < 0)
		dns.cacheTimeoutSec = 3600; // 1 hour

	return 0;
}

int Config::loadSisorSection(ACE_Configuration_Heap* pReader)
{
	ACE_ASSERT(pReader != NULL);

	// SISOR >> RecvTimeout
	if(getValue(pReader, "SISOR", "RecvTimeoutSec", sisor.recvTimeoutSec, 1, 60) < 0)
		dns.queryTimeoutSec = 2;

	// SISOR >> Host
	if(getValue(pReader, "SISOR", "Host", sisor.host) < 0)
		return -1;
	
	// SISOR >> Port
	if(getValue(pReader, "SISOR", "Port", sisor.port, 0, 0xffff) < 0)
		return -1;

	// SISOR >> Connection retry interval (Second)
	if(getValue(pReader, "SISOR", "ConnectionRetryInterval", sisor.connectionRetryIntervalSec, 1, 999) < 0)
		sisor.connectionRetryIntervalSec = 3;

	// SISOR >> Connection pool size
	if(getValue(pReader, "SISOR", "ConnectionPoolSize", sisor.connectionPoolSize, 0, 100) < 0)
		sisor.connectionPoolSize = 0;

	return 0;
}

int Config::loadGuideSection(ACE_Configuration_Heap* pReader)
{
	ACE_ASSERT(pReader != NULL);

	// GUIDE >> Enable
	if(getValue(pReader, "GUIDE", "Enable", guide.enable) < 0)
		guide.enable = true;

	// GUIDE >> GuideUrlMagicn
	if(getValue(pReader, "GUIDE", "GuideUrlMagicn", guide.guideUrlMagicn) < 0)
		return -1;

	// GUIDE >> GuideUrlFimm
	if(getValue(pReader, "GUIDE", "GuideUrlFimm", guide.guideUrlFimm) < 0)
		return -1;
	
	// GUIDE >> GuideUrlMultipack
	if(getValue(pReader, "GUIDE", "GuideUrlMultipack", guide.guideUrlMultipack) < 0)
		return -1;

	// GUIDE >> GuideResultUrlMagicn
	if(getValue(pReader, "GUIDE", "GuideResultUrlMagicn", guide.guideResultUrlMagicn) < 0)
		return -1;

	// GUIDE >> GuideResultUrlFimm
	if(getValue(pReader, "GUIDE", "GuideResultUrlFimm", guide.guideResultUrlFimm) < 0)
		return -1;
	
	// GUIDE >> GuideResultUrlMultipack
	if(getValue(pReader, "GUIDE", "GuideResultUrlMultipack", guide.guideResultUrlMultipack) < 0)
		return -1;

	// GUIDE >> FimmSpecificFirstCounter
	if(getValue(pReader, "GUIDE", "FimmSpecificFirstCounter", guide.fimmSpecificFirstCounter, 1, 9999) < 0)
		return -1;

	return 0;
}

int Config::loadServiceSection(ACE_Configuration_Heap* pReader)
{
	ACE_ASSERT(pReader != NULL);

	// SERVICE >> Enable
	if(getValue(pReader, "SERVICE", "Enable", service.enable) < 0)
		service.enable = true;

	return 0;
}

int Config::loadSessionInfoUpdateSection(ACE_Configuration_Heap* pReader)
{
	ACE_ASSERT(pReader != NULL);

	// SESSION_INFO_UPDATE >> Enable
	if(getValue(pReader, "SESSION_INFO_UPDATE", "Enable", sessionInfoUpdate.enable) < 0)
		sessionInfoUpdate.enable = true;

	return 0;
}

int Config::loadSpeedupTagSection(ACE_Configuration_Heap* pReader)
{
	ACE_ASSERT(pReader != NULL);

	// SPEEDUPTAG >> Enable
	if(getValue(pReader, "SPEEDUPTAG", "Enable", speedupTag.enable) < 0)
		speedupTag.enable = true;

	// SPEEDUPTAG >> MaxSize
	if(getValue(pReader, "SPEEDUPTAG", "MaxSize", speedupTag.maxSize, 0, 0x7FffFFff) < 0)
		speedupTag.maxSize = 50000;

	return 0;
}

int Config::loadHotNumberSection(ACE_Configuration_Heap* pReader)
{
	ACE_ASSERT(pReader != NULL);
/*
	// HOTNUMBER >> Enable
	if(getValue(pReader, "HOTNUMBER", "Enable", hotnumber.enable) < 0)
		return -1;
*/
	return 0;
}

TinyString Config::getLogPrefix()
{
	// KUN
	if(process.serverID.incaseFind("KUN") >= 0)
	{
		return TinyString("k_");
	}

	// ME
	return TinyString("");
}
