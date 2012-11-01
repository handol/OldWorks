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
	ACE_TString value;
	if(getValue(pReader, "PROCESS", "ServerID", value) < 0)
		return -1;
	process.serverID = value.c_str();

	if(getValue(pReader, "PROCESS", "CheckMaxClients", process.checkMaxClients) < 0)
		process.checkMaxClients = false;

	if(getValue(pReader, "PROCESS", "MaxClients", process.maxClients, 32, 100*100*100) < 0)
		process.maxClients = 100*100*100;

	
	if(getValue(pReader, "PROCESS", "RecvBufferBytes", process.RecvBufferBytes, 1024, 1024*64) < 0)
		process.RecvBufferBytes = 1024*4;
	
		
	if(getValue(pReader, "PROCESS", "HttpHeaderBufferBytes", process.HttpHeaderBufferBytes, 1024, 1024*10) < 0)
		process.HttpHeaderBufferBytes = 1024*2;
		

	// PROCESS >> UserIdleSec
	if(getValue(pReader, "PROCESS", "UserIdleSec", process.userIdleSec, 60, 60*20) < 0) 
		return -1;

	// PROCESS >> HOTNUMBER
	if(getValue(pReader, "PROCESS", "HotNumber", hotnumber.enable) < 0)
		hotnumber.enable = true;

	// PROCESS >> KtfSearchUrl
	if(getValue(pReader, "PROCESS", "KtfSearchUrl", value) < 0)
		return -1;
	hotnumber.ktfSearchUrl  = value.c_str();

	// PROCESS >> BillInfo
	if(getValue(pReader, "PROCESS", "BillInfo", process.billinfo) < 0)
		process.billinfo = false;

	// PROCESS >> BillInfoTarget
	if(getValue(pReader, "PROCESS", "BillInfoTarget", process.billinfoTarget) < 0)
		process.billinfo = false;

	// PROCESS >> MemConfig FileName
	if(getValue(pReader, "PROCESS", "MemConfig", value) < 0)
		value = "";

	process.memconf = value.c_str();

	// PROCESS >> SysInfoInterval
	if(getValue(pReader, "PROCESS", "SysInfoInterval", process.sysinfointerval, 1, 60) < 0)
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
	if(getValue(pReader, "NETWORK", "ListenPort", network.listenPort, 1024, 0xffff) < 0)
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
	if(getValue(pReader, "CLIENT", "TimeoutCheckInterval", client.timeoutCheckInterval, 5, 60) < 0)
		client.timeoutCheckInterval = 20;
	
	// CLIENT >> ReceiveTimeout
	if(getValue(pReader, "CLIENT", "ReceiveTimeout", client.receiveTimeout, 10, 3600) < 0)
		client.receiveTimeout = 60*10;
	
	return 0;
}

int Config::loadCPSection(ACE_Configuration_Heap* pReader)
{
	ACE_ASSERT(pReader != NULL);

	if(getValue(pReader, "CP", "ReuseCpConnection", cp.ReuseCpConnection) < 0)
		cp.ReuseCpConnection = true;

	// CP >> TimeoutCheckInterval
	if(getValue(pReader, "CP", "TimeoutCheckInterval", cp.timeoutCheckInterval, 1, 5) < 0)
		cp.timeoutCheckInterval = 1;

	// CP >> ConnectionTimeout
	if(getValue(pReader, "CP", "ConnectionTimeout", cp.connectionTimeout, 5, 60) < 0)
		cp.connectionTimeout = 10;

	// CP >> ReceiveTimeout
	if(getValue(pReader, "CP", "ReceiveTimeout", cp.receiveTimeout, 5, 300) < 0)
		cp.receiveTimeout = 20;

	return 0;
}

int Config::loadAuthSection(ACE_Configuration_Heap* pReader)
{
	ACE_ASSERT(pReader != NULL);

	// AUTH >> Enable
	if(getValue(pReader, "AUTH", "Enable", auth.enable) < 0)
		return -1;

	// 사용하지 않은면 더이상 config 값을 읽을 필요 없다.
	if(auth.enable == false)
		return 0;

	// AUTH >> Port
	if(getValue(pReader, "AUTH", "Port", auth.port, 1024, 0xffff) < 0)
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

	// AUTH >> Host
	ACE_TString value;
	if(getValue(pReader, "AUTH", "Host", value) < 0)
		return -1;

	auth.host = value.c_str();

	// AUTH >> ErrorCount
	if(getValue(pReader, "AUTH", "ErrorCount", auth.errCount, 1, 10) < 0)
		auth.errCount = 10;

	// AUTH >> RetryDelay
	if(getValue(pReader, "AUTH", "RetryDelay", auth.retryDelay, 60, 3600) < 0)
		auth.retryDelay = 600;

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

	// SANTA >> Port
	if(getValue(pReader, "SANTA", "Port", santa.port, 1024, 0xffff) < 0)
		santa.port = 8082;

	// SANTA >> TimeoutCheckInterval
	if(getValue(pReader, "SANTA", "TimeoutCheckInterval", santa.timeoutCheckInterval, 1, 5) < 0)
		santa.timeoutCheckInterval = 2;

	// SANTA >> ConnectionTimeout
	if(getValue(pReader, "SANTA", "ConnectionTimeout", santa.connectionTimeout, 5, 60) < 0)
		santa.connectionTimeout = 10;

	// SANTA >> ReceiveTimeout
	if(getValue(pReader, "SANTA", "ReceiveTimeout", santa.receiveTimeout, 5, 60) < 0)
		santa.receiveTimeout = 10;

	// SANTA >> Host
	if(getValue(pReader, "SANTA", "Host", value) < 0)
		return -1;

	santa.host = value.c_str();

	return 0;
}

int Config::loadAclSection(ACE_Configuration_Heap* pReader)
{
	ACE_ASSERT(pReader != NULL);

	// ACL >> Enable
	if(getValue(pReader, "ACL", "Enable", acl.enable) < 0)
		acl.enable = false;

	// 사용하지 않은면 더이상 config 값을 읽을 필요 없다.
	if(acl.enable == false)
		return 0;

	// ACL 파일명
	ACE_TString value;
	if(getValue(pReader, "ACL", "FileName", value) < 0)
		value = "pas.acl";

	acl.fileName = value.c_str();

	// ACL 모니터 서버 IP
	if(getValue(pReader, "ACL", "Monitor Server IP", value) < 0)
		value = "192.168.208.98";

	acl.monitorServerIP = value.c_str();

	// ACL 모니터 서버 PORT
	if(getValue(pReader, "ACL", "Monitor Server PORT", acl.monitorServerPORT, 1024, 0xffff) < 0)
		acl.monitorServerPORT = 5015;

	// ACL >> UpdateCheckInterval
	if(getValue(pReader, "ACL", "UpdateCheckInterval", acl.updateCheckInterval, 3, 60) < 0)
		acl.updateCheckInterval = 3;

	// ACL >> MultiProxy
	if(getValue(pReader, "ACL", "MultiProxy", acl.multiproxy) < 0)
		acl.multiproxy = true;

	// ACL >> DNSHost
	if(getValue(pReader, "ACL", "DNSHost", value) < 0)
		value = "221.148.247.32";

	acl.dnshost = value.c_str();

	// ACL >> DNSPort
	if(getValue(pReader, "ACL", "DNSPort", acl.dnsport, 1024, 0xffff) < 0)
		acl.dnsport = 9999;

	return 0;
}

int Config::loadHotNumberSection(ACE_Configuration_Heap* pReader)
{
/*	ACE_ASSERT(pReader != NULL);

	// HOTNUMBER >> Enable
	if(getValue(pReader, "HOTNUMBER", "Enable", hotnumber.enable) < 0)
		return -1;
*/
	return 0;
}
