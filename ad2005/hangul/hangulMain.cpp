#define USE_REAL_ACE

#ifdef USE_REAL_ACE
#include "ace/Log_Msg.h"
#include "ace/streams.h"
#else
#include "Log_Msg.h" // NOT Real ACE. My lib
#endif

#include <stdio.h>
#include <ctype.h>
#include "dbStore.h"

#include "CLog.h"
#include "MemSplit.h"
#include "StrUtil.h"
#include "TimeUtil.h"
#include "FileUtil.h"
#include "Config.h"

#include "HanChar.h"
#include "HDict.h"
#include "HGram.h"
#include "HStem.h"
#include "HProc.h"

#define	PRN	printf

char	*adhome = NULL;
Config	config(50);

void	init_env()
{
	char	logfile[256];
	
	adhome = getenv("ADHOME");
	if (adhome) printf("ADHOME = %s\n", adhome);
	else	printf("ADHOME is NOT set !\n");

	StrUtil::path_merge(logfile, adhome, "log/hdict.txt");

	#ifdef USE_REAL_ACE
	ACE_OSTREAM_TYPE *output = new ofstream(logfile);
	ACE_LOG_MSG->msg_ostream(output, 1);
	ACE_LOG_MSG->set_flags(ACE_Log_Msg::OSTREAM);
	ACE_LOG_MSG->clr_flags (ACE_Log_Msg::STDERR);

	#else
	sprintf(logfile, "log/hdict-%s.txt", TimeUtil::get_yymmdd() );
	
	ACE_LOG_MSG->open(adhome, "log/logfile");
	//ACE_LOG_MSG->overwrite();
	#endif
	config.open_home(adhome, "anydict.cfg");
}

/**
단어, 숙어를 DB  table 에 추가하기 위한 것.
*/

int
main (int argc, char **argv)
{
	int	res=0;
	char fname[256];
	
	/*
	if (argc < 2) {
		printf("option: w for word, i for idiom\n");
		exit(0);
	}
	*/
	init_env();	

	HanChar::prepare();

	//test_nameValPairTable();
	
	/*
	res = dbStore::prepare(
		config.GetStrVal("DBNAME", "ANYDICT"),
		config.GetStrVal("DBID", "inisoft"),
		config.GetStrVal("DBPASS", "gksehf")
		);
	*/
	//res = dbStore::prepare( "ANYDICT2", "inisoft", "gksehf");
	if (res < 0) {
		return -1;
	}

	HDict::load(adhome);
	HGram::load(adhome);

	printf("### %s", argv[0]);
	if (argc > 1) printf("\tOption: %s", argv[1]);
	printf("\n\n");

	printf("# JH2WS_TABSIZE = %d\n", JH2WS_TABSIZE);
	if (argc > 1 && argv[1][0]=='w') {
		HanChar::printCodeTab();
	}
	
	if (argc > 1 && argv[1][0]=='y') {
		HanChar::printSylInfo();
	}

	if (argc > 1 && argv[1][0]=='d') {
		HDict::printList();
	}

	if (argc > 1 && argv[1][0]=='c') {
		HDict::printListConjOnly();
	}

	if (argc > 1 && argv[1][0]=='a') {
		HDict::printListAuxOnly();
	}

	if (argc > 1 && argv[1][0]=='j') {
		HDict::printListJosaOnly();
	}

	if (argc > 1 && argv[1][0]=='e') {
		HDict::printListEomiOnly();
	}
	
	if (argc > 1 && argv[1][0]=='g') {
		HGram::printList();
	}
	
	if (argc > 1 && argv[1][0]=='s') {
		StrStream resStr;
		resStr.init(10*1024);
		HProc::test_stemmer( StrUtil::path_merge(fname, adhome, "dataH/test-howtoberich.txt"),  resStr);
	}

	
	return 0;
		
}

