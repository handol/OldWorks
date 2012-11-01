#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
#include <strings.h>


#include <ace/Log_Msg.h>
#include <ace/Reactor.h>
#include <ace/Select_Reactor.h>
#include <ace/streams.h>
#include <ace/Thread_Manager.h>
#include <ace/Task_T.h> 
#include <ace/Message_Queue_T.h>
#include <ace/TP_Reactor.h>
#include <ace/Dev_Poll_Reactor.h>

#include "Common.h"
#include "PasSignalHandler.h"
#include "PasAcceptor.h"
#include "PasLog.h"
#include "AclRouter.h"
#include "LogLevel.h"
#include "Config.h"
#include "MemoryPoolManager.h"
#include "ReactorPool.h"

#include "UserInfoMng.h"
#include "PhoneTraceMng.h"
#include "StatFilterMng.h"
#include "Util2.h"
#include "SysInfo.h"
#include "PasDataLog.h"
#include "StrSplit.h"
#include "HashKey.h"
#include "CGI.h"
#include "DNSManager.h"

#include "HttpRequest.h"
#include "HttpResponse.h"

#include "AuthAgent.h"

#include "DebugLog.h"

#include "MonitorReporter.h"
#include "WatchReporter.h"
//---------------------------------------------------------------------------

/* 
Sun Compiler 정보 :  

http://predef.sourceforge.net/precomp.html#sec43  -- 요게 진짜.


http://www.cs.wfu.edu/docs/CC.html 
http://predef.sourceforge.net/
*/
/*
GCC predefined macros
http://gcc.gnu.org/onlinedocs/gcc-3.4.0/cpp/Common-Predefined-Macros.html#Common%20Predefined%20Macros
*/
//static const char* version = __VERSION__;
static const char* compile_date = "PAS2006_DATE: " __DATE__ " " __TIME__;

#ifdef __SUNPRO_CC
static const char* compiler = "PAS2006_COMPILER: "  XSTR (__SUNPRO_CC);
#endif

extern char *optarg;
extern int optind, opterr, optopt;

const char* defaultConfigFilename = "9090.cfg";

//---------------------------------------------------------------------------
void initSignal(PasSignalHandler* pSignalHandler);
void initACL();
void stopACL();
int initConfig(const char * confname);
ACE_Configuration_Heap* createAndOpenConfig(const char* filename);
int makeDaemon();
int initLog();
int initMessageBlockManager();
void helpMsg(const char* programName);
void increseFdLimit();
int readMBconfig(void);
void procArguments(int argc, char** argv);

//---------------------------------------------------------------------------
int ACE_MAIN(int argc, ACE_TCHAR  *argv[])
{
	// 환경설정 (제일 먼저 초기화 해야함)
	procArguments(argc, argv);

	// 로그
	if(initLog() < 0)
	{
		printf("로그 초기화를 실패했으므로 프로그램을 종료합니다.\n");
		return -1;
	}

	//-----------------------------------------------------------------------
	// 데몬 만들기 
	//-----------------------------------------------------------------------
	// acceptor 가 초기화 된 후 데몬을 만들게 되면, 부모프로세스가 제거되면서
	// listen 소켓도 같이 제거된다. 그러므로 acceptor 를 초기화 하기 전에
	// 데몬을 먼저 만들어야 한다.
	if(Config::instance()->process.daemon == true)
	{
		if(makeDaemon() < 0)
		{
			printf("데몬으로 만들기를 실패했으므로 프로그램을 종료합니다.\n");
			return -1;
		}
	}
	
	PAS_NOTICE1("%s", compile_date);

	// 시그널 핸들러
	PasSignalHandler* pSignalHandler = new PasSignalHandler;
	initSignal(pSignalHandler);

	//------------------
	// 각 모듈의 초기화
	//------------------
	HTTP::Request::HeaderBuffBytes = HTTP::Response::HeaderBuffBytes = Config::instance()->process.HttpHeaderBufferBytes;
	
	increseFdLimit();

	
	// PAS 공식 로그
	char hostName[32];

	gethostname(hostName, sizeof(hostName)-1);
	PasDataLog::setPasAddr(hostName);
	
	// 메모리 매니저 초기화
	//initMessageBlockManager();
	if (readMBconfig() < 0)
	{
		printf("메모리 풀 컨피그 설정값을 확인하세요.\n");
		PAS_ERROR("메모리 풀 컨피그 설정값을 확인하세요.\n");
		return -1;
	}

	const int numWorkerThread = Config::instance()->thread.numWorker;

	// 리액터
	ACE_Reactor* pReactor = ReactorPool::instance()->createMaster();
	
	ReactorPool::instance()->createWorkers(numWorkerThread);

	ACE_Reactor* pGlobalReactor = ACE_Reactor::instance();

	PAS_NOTICE3("REACTOR: Master=%X, Global=%X, Master=%x", 
		pReactor,  pGlobalReactor, ReactorPool::instance()->masterReactor());

	// Listen 포트 설정
	PAS_NOTICE("Listen Socket Activate");
	PasAcceptor* pAcceptor = new PasAcceptor(pReactor);
	
	const int listenPort = Config::instance()->network.listenPort;

	// mIDC 내의 서버를 감시하는 쪽에 mwatch 전달하기 위해 메시지 작성한다.
	Util2::setMwatchMsg(listenPort);

	// acceptor 기동 
	if(pAcceptor->open(listenPort) < 0)
	{
		PAS_ERROR1("Listen 소켓 생성 실패, Port[%d]\n", listenPort);
		PAS_ERROR("프로그램 종료\n");

		printf("Listen 소켓 생성 실패, Port[%d]\n", listenPort);
		printf("프로그램 종료\n");
		return -1;
	}

	// 쓰레드 매니저
	ACE_Thread_Manager* pTManager = ACE_Thread_Manager::instance();	

	// monitor 보고리를 위한 thread 를 생성한다.
	MonitorReporter *monitor = MonitorReporter::instance(pTManager);
	monitor->activate(THR_NEW_LWP | THR_JOINABLE);

	WatchReporter *watch = WatchReporter::instance(pTManager);
	watch->activate(THR_NEW_LWP | THR_JOINABLE);

	// UserInfo 관리를 위한 thread 를 생성한다.
	UserInfoMng *userInfoMng = UserInfoMng::instance(pTManager);
	userInfoMng->activate(THR_NEW_LWP | THR_JOINABLE);
	
	// phone trace 를 위한 thread 를 생성한다.
	PhoneTraceMng *phoneTraceMng = PhoneTraceMng::instance(pTManager);
	phoneTraceMng->setDataFile((char*)"trace.acl");
	phoneTraceMng->activate(THR_NEW_LWP | THR_JOINABLE);

	// 공지 처리 (Stat Filter)  를 위한 thread 를 생성한다.
	StatFilterMng *statFilterMng = StatFilterMng::instance(pTManager);
	statFilterMng->setDataFile((char*)"k_stat.cfg");
	statFilterMng->activate(THR_NEW_LWP | THR_JOINABLE);

	// ACL 초기화
	if(Config::instance()->acl.enable)
		initACL();

	CGI::cgiSetupConstants();
	
	// Create AuthAgent Thread
	AuthAgent *authAgent = AuthAgent::instance(pTManager);
	authAgent->activate(THR_NEW_LWP | THR_JOINABLE);

	//usleep(1000);
	
	// 내부 정보 (sysinfo) 출력를 위한 Thread
	SysInfo  *sysInfo = SysInfo::instance(pTManager);
	sysInfo->activate(THR_NEW_LWP | THR_JOINABLE);

	// hash key 로그 작성을 위한 초기화.
	HashKey::prepare();
	
	// 로그 화일을 먼저 만들어 놓는다.  테스트시에 편하다. (tail -f )
	PasDataLog::instance();
	
	// accept event 핸들러 등록
	pReactor->register_handler(pAcceptor, ACE_Event_Handler::ACCEPT_MASK);

	// 이벤트 디멀티플렉싱
	PAS_NOTICE("Master Reactor Start");
	pReactor->run_event_loop();
	PAS_NOTICE("Master Reactor Stop");

	ReactorPool::instance()->stopWorkers();


	/*--- Reactor 가 종료된 경우 아래 라인으로 진행된다. ---*/

	stopACL(); // ACL 종료
	userInfoMng->stop();
	monitor->stop();
	watch->stop();
	phoneTraceMng->putq(new ACE_Message_Block());
	statFilterMng->putq(new ACE_Message_Block());
	sysInfo->putq(new ACE_Message_Block());
	authAgent->putq(new ACE_Message_Block());

	DNS::Manager::instance()->removeAllQuerier();
	
	// 모든 쓰레드 종료 대기
	PAS_NOTICE("Waiting for all threads to stop");
	pTManager->wait();

	delete phoneTraceMng;
	delete statFilterMng;
	delete sysInfo;
	// 생성한 동적 객체 삭제
	delete pSignalHandler;
	
	PAS_NOTICE("======= PAS GW Stop =======");

	return 0;
}

//---------------------------------------------------------------------------
void procArguments(int argc, char** argv)
{
	const char* configFilename = NULL;

	int listenPort = -1;
	TinyString logLevel;

	bool debugMode = false;
	
	int c;
	while ((c = getopt(argc, argv, ":hdl:p:")) != -1) 
	{
		switch(c) 
		{
		case 'd':
			debugMode = true;
			break;

		case 'p':
			listenPort = atoi(optarg);
			break;

		case 'l':
			logLevel = optarg;
			break;

		case ':':
			printf("-%c is need more value\n", optopt);
			helpMsg(argv[0]);
			exit(0);
			break;

		case '?':
			if(optopt == '-')
			{
				if(strcmp(argv[optind-1], "--help") == 0)
				{
					helpMsg(argv[0]);
					exit(0);
				}
			}
			else
			{
				printf("Unknown arg %c\n", optopt);
				helpMsg(argv[0]);
				exit(0);
			}

		case 'h':
			helpMsg(argv[0]);
			exit(0);

		default:
			helpMsg(argv[0]);
			exit(0);
		}
	}

	if(optind < argc) 
	{
		configFilename = argv[optind];
		optind++;
	}

	if(configFilename == NULL)
		configFilename = defaultConfigFilename;

	if(initConfig(configFilename) < 0)
	{
		printf("컨피그 파일의 존재 및 설정값을 다시 확인해 주세요.\n");
		exit(0);
	}

	if(debugMode)
	{
		Config::instance()->process.daemon = false;
		Config::instance()->log.outputType = LOT_STDOUT;
		Config::instance()->log.level = LLT_MEDIUM;
	}

	if(!logLevel.isEmpty())
	{
		if(logLevel == "HIGH" || logLevel=="4")
		{
			Config::instance()->log.level = LLT_HIGH;
		}
		else if(logLevel == "MEDIUM" || logLevel=="3")
		{
			Config::instance()->log.level = LLT_MEDIUM;
		}
		else if(logLevel == "LOW" || logLevel=="2")
		{
			Config::instance()->log.level = LLT_LOW;
		}
		else if(logLevel == "VERYLOW" || logLevel == "1")
		{
			Config::instance()->log.level = LLT_VERYLOW;
		}
	}

	if(listenPort > 0)
		Config::instance()->network.listenPort = listenPort;
}

//---------------------------------------------------------------------------
void helpMsg(const char* programName)
{
	printf("Usage : %s [-p listen_port] [-d] [-l log_level] [config]\n", programName);
	printf("            -d : Debug mode (No Daemon with MEDIUM log level)\n");
	printf("            -l : Log level [HIGH | MEDIUM | LOW | VERYLOW | 4 | 3 | 2 | 1]\n");
	printf("        config : Config file. default : %s\n", defaultConfigFilename);
}

//---------------------------------------------------------------------------
void increseFdLimit()
{
	struct rlimit limit;
	
	getrlimit(RLIMIT_NOFILE, &limit);
	PAS_NOTICE1("LIMIT: Current FD Limit: %d" ,limit.rlim_cur);
	
	limit.rlim_cur = limit.rlim_max;	
	//limit.rlim_cur = 65536;
	setrlimit(RLIMIT_NOFILE, &limit);

	getrlimit(RLIMIT_NOFILE, &limit);
	PAS_NOTICE1("LIMIT: New FD Limit: %d" ,limit.rlim_cur);

	getrlimit(RLIMIT_FSIZE, &limit);
	PAS_NOTICE1("LIMIT: Current FileSize Limit: %d" ,limit.rlim_cur);
	
	limit.rlim_cur = limit.rlim_max;	
	//limit.rlim_cur = (unsigned long)(2 << 30);
	setrlimit(RLIMIT_FSIZE, &limit);
	getrlimit(RLIMIT_FSIZE, &limit);
	PAS_NOTICE1("LIMIT: New FileSize Limit: %d" ,limit.rlim_cur);

	PAS_NOTICE1("FD_SETSIZE[%d]", FD_SETSIZE);
}
//---------------------------------------------------------------------------
void initSignal(PasSignalHandler* pSignalHandler)
{
	// 시그널 핸들러 셋팅
	ACE_Sig_Handler* pSigHandler = new ACE_Sig_Handler;
	
	// 시그널 핸들링
	pSigHandler->register_handler(SIGINT, pSignalHandler);
	pSigHandler->register_handler(SIGTERM, pSignalHandler);
	pSigHandler->register_handler(SIGQUIT, pSignalHandler);
	pSigHandler->register_handler(SIGUSR1, pSignalHandler);
	pSigHandler->register_handler(SIGUSR2, pSignalHandler);

	// 시그널 무시
	pSigHandler->register_handler(SIGPIPE, pSignalHandler);	
	pSigHandler->register_handler(SIGALRM, pSignalHandler);
	pSigHandler->register_handler(SIGHUP, pSignalHandler);	
	pSigHandler->register_handler(SIGCHLD, pSignalHandler);	
}
//---------------------------------------------------------------------------
int initConfig(const char * confname)
{
	Config* pConfig = Config::instance();

	if (pConfig->load(confname) < 0)
		return -1;

	return 0;
}
//---------------------------------------------------------------------------
void initACL()
{
	ACE_ASSERT(Config::instance()->acl.enable);
	const ProcessConfig& procConf = Config::instance()->process;
	const NetworkConfig& netConf = Config::instance()->network;

	AclRouter* pAcl = AclRouter::instance();
	
	if(pAcl->initial(procConf.serverID, netConf.listenPort) < 0)
	{
		PAS_ERROR("AclRouter 초기화 실패");
		ACE_OS::exit();
	}

	pAcl->activate();
}
//---------------------------------------------------------------------------
void stopACL()
{
	AclRouter* pAcl = AclRouter::instance();
	pAcl->remove_worker_thread();
}
//---------------------------------------------------------------------------
int makeDaemon()
{
	int ret = fork();
	if(ret < 0)
	{
		printf("데몬프로세스 생성 실패\n");
		return -1;
	}
	
	// child
	if(ret == 0)
	{
		//pass
	}

	// parent
	else
	{
		// 부모는 종료
		exit(0);
	}

	return 0;
}

//---------------------------------------------------------------------------
int initLog()
{
	openMainLogFile();
	
	PAS_NOTICE("======= PAS GW Start =======");
	
	// 로그 수준 설정
	const LogConfig& logConfig = Config::instance()->log;	
	LogLevel* logLevel = LogLevel::instance();
	logLevel->setLevel(logConfig.level);;
	
	return 0;
}

//---------------------------------------------------------------------------
int initMessageBlockManager(mapint &sizes)
{
	MessageBlockManager* pMBManager = MessageBlockManager::instance();

	BlockInfoList	blockInfoList;
	blockInfoList.reserve(sizes.size());
	BlockInfo oneblock;

	iterint iter = sizes.begin();
	for( ; iter != sizes.end(); iter++ )
	{
		oneblock.blockSize = iter->first;
		oneblock.maxNum = iter->second;
		blockInfoList.push_back( oneblock);
		PAS_INFO2( "Memory Pool: %6dK  %5d Blocks ",  (oneblock.blockSize >> 10) , oneblock.maxNum);
	}

	PAS_INFO1( "Memory Pool:  %d Pools", blockInfoList.size());
	pMBManager->setBlockSizeAndMax( blockInfoList );
	
	return 0;
}


//---------------------------------------------------------------------------
/**
@brief
메모리 블럭의 크기와 갯수를 읽어 initMessageBlockManager 에 넘겨준다.
*/
int readMBconfig(void)
{
	filename_t fileName(Config::instance()->process.memconf);

	FILE *fp = NULL;
	fp = fopen( fileName, "rt" );
	if( fp == NULL )
	{
		PAS_ERROR1( "Memory Pool Config File:  [%s]  not found", fileName.toStr() );
		return -1;
	}

	// 맵을 사용하는 이유는, 키값에는 블럭사이즈, Value 값에는 최대 사이즈를 넣으면 되고
	// 만일 config 파일에 중복된 값(예:16K, 16K)이 있을 경우 뒷부분은 자동으로 무시되므로
	// map 을 사용하는 것이 가장 적당하다고 판단된다.
	mapint mapSizes;
	int	lineCount = 0;
	int	maxBlockSize =0;
	while( !feof(fp) )
	{
		char line[1024] = "\0";
		char *pResult = fgets( line, sizeof(line), fp );
		if( pResult == NULL )
			break;
		lineCount++;
		
		StrSplit MBSpliter( 2, sizeof(line) );
		MBSpliter.split( line );

		// skip comment
		if( MBSpliter.fldVal(0)[0] == '#' )
			continue;

		// 정상적인 데이터가 아니면 패스
		if( MBSpliter.numFlds() != 2 )
		{
			PAS_ERROR2( "Memory Pool Config File: Error in line [%d], file[%s]", lineCount, fileName.toStr() );
			return -1;
		}

		char *pLeftItem = MBSpliter.fldVal( 0 );
		char *pRightItem = MBSpliter.fldVal( 1 );
		int nLeftLength = strlen( pLeftItem );

		int nBlockSize = 0;
		int nMaxCount = strtol( pRightItem, NULL, 10 );

		if( pLeftItem[nLeftLength-1] == 'K' || pLeftItem[nLeftLength-1] == 'k' )
		{
			pLeftItem[nLeftLength-1] = '\0';
			nBlockSize = strtol( pLeftItem, NULL, 10 );
			nBlockSize = nBlockSize * 1024;
		}

		else if( pLeftItem[nLeftLength-1] == 'M' || pLeftItem[nLeftLength-1] == 'm' )
		{
			pLeftItem[nLeftLength-1] = '\0';
			nBlockSize = strtol( pLeftItem, NULL, 10 );
			nBlockSize = nBlockSize * 1024 * 1024;
		}

		// 단위값이 잘 못 입력된 경우 무시한다.
		else
		{
			PAS_ERROR2( "Memory Pool Config File: Error in line [%d], file[%s]", lineCount, fileName.toStr());
			return -1;
		}

		if (nBlockSize < maxBlockSize)
		{
			PAS_ERROR2( "Memory Pool Config File: Block Size must be bigger than the previous size. Error in line [%d], file[%s]", lineCount, fileName.toStr());
			return -1;
			
		}

		maxBlockSize = nBlockSize;
		
		mapSizes.insert( make_pair(nBlockSize, nMaxCount) );
		
	}

	fclose( fp );

	initMessageBlockManager( mapSizes );

	return 0;
}
