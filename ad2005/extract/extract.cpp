#include "StrUtil.h"
#include "EDict.h"
#include "TextProc.h"
#include "FileUtil.h"

#include "DbConn.h"
#include "DbQuery.h"

#include "dbStore.h"

#include "Docs.h"

#include "Search.h"
#include "StrStream.h"

#include "HDict.h"
#include "HGram.h"

#include "TimeUtil.h"
#include "Config.h"
#include "ace/streams.h"
#include "ace/Log_Msg.h"

#include <stdio.h>

char	*adhome = NULL;


int	test_DB() {
	int	ret;
	DbConn	dbConn	;
	DbQuery	query;

	if ( (ret = dbConn.connect("ANYDICT", "inisoft", "gksehf")) < 0) {
		printf("DB login failed (%d)\n", ret);
		printf("Error: %d, %s\n", dbConn.errorcode(), dbConn.errormesg() );
		return 0;
	}

	query.exec(dbConn, "insert into test values ('jeein', '30', 99)");
	printf("insert res = %d\n", query.count() );
	query.exec(dbConn, "select  * from test");
	query.info();
	printf("Rows = %d\n", query.count() );

	for(int i=0; i<2; i++) {
		if (query.fetch() == 0) break;
		printf("%s\n", query.colStrVal(0) );
		printf("%s\n", query.colStrVal(1) );
		printf("%s\n", query.colStrVal(2) );
		printf("%d\n", query.colIntVal(1) );
		printf("%d\n", query.colIntVal(2) );
	}

	query.cleanup();
	dbConn.disconnect();
	return 0;
}

void	test_EDict()
{
	char word[64]={0};
	wordType *w = 0;
	byte	PS=0;
	StrStream st;

	adhome = getenv("ADHOME");
	dbStore::prepare("ad2004", "root", "wpxk00");

	EDict::prepare(adhome); 
	
	while(1) {
		printf("\nEnter a word : ");
		if (fgets(word, sizeof(word) -1, stdin) == NULL) break;
		if (word[0] == '.') break;
		word[strlen(word)-1] = 0;
		if (strchr(word, ' ')) {
			printf("Idiom --\n");
		// 여러 단어로 이루어진 경우
			int res = EDict::testIdiom(word, st);
			if (res < 0)
				printf("idiom not found\n");
		}
		else {
			printf("Word --\n");
		// 한 단어로 이루어진 경우
			w = EDict::stemWord(word, 0, &PS);
			if (w) EDict::printWord(w);
			else printf("Unknown word\n");
		}
	}
}



/**
return 1: if processed and updated 
return 0: 일부 오류 발생.
*/
int	test_Extract(char *fname)
{
	int	alloc_size=0;
	int	load_size=0;
	docType *docinfo = new docType();
	char	fullname[256];

	TextProc	extract;
	StrStream resStr;
	resStr.init(1024);

	adhome = getenv("ADHOME");
	dbStore::prepare("anydict", "root", "wpxk00");

	EDict::prepare(adhome);

	HDict::prepare(adhome);

	memset(docinfo, 0, sizeof(docType));
	
	StrUtil::path_merge(fullname, adhome, fname);
	docinfo->fpath = fullname;
	

	if (docinfo->processed != 0) {
		resStr.add("$SKIP$ %d , processed (%d)\n", docinfo->doc_id, docinfo->processed);
		resStr.print();
		return 0;
	}
	
	if (docinfo->size <= 0) docinfo->size = FileUtil::get_file_size(docinfo->fpath);
	if (docinfo->size <= 0) {
		resStr.add("# file open error: %s\n", docinfo->fpath);
		resStr.print();
		return 0;
	}
	
	alloc_size = ALIGN_HALF_K ( docinfo->size );

	resStr.add("# %d file bytes, %d mem bytes, %s\n", docinfo->size, alloc_size, docinfo->fpath);

	docinfo->text = new char[ alloc_size ];
	
	load_size = FileUtil::load_file_2_given_mem(docinfo->text, docinfo->fpath, docinfo->size);
	/* 실행결과 :  FileUtil::get_file_size() 에서 얻은 값과 load한 값이 다름. 그 차이는 라인수와 일치.  */
	resStr.add("fsize in DB = %d, loaded size = %d\n", docinfo->size, load_size);
	
	TextProc::prepare();

	extract.setDebug(0);
	docinfo->doc_id = 1; // extract.txt_analyze() 에서 doc_id > 0 인 경우에만 처리하므로.
	extract.txt_analyze(docinfo);
	extract.get_result(resStr);
	resStr.add("\n");
	
	delete [] docinfo->text;
	docinfo->text = 0;
	resStr.print();
	
	return 1;
}



int test_Search()
{
	adhome = getenv("ADHOME");
	dbStore::prepare("ad2004", "root", "wpxk00");

	StrStream resStr;

	//test_EDict();
	Search::prepare(adhome);
	Search::search("community", resStr);
	resStr.print();
	return 0;
}

void help()
{
	printf("usage: option\n");
	printf("\tdb\n");
	printf("\tedict\n");
	printf("\textract\n");
	printf("\tsearch\n");
}

Config	config(50);

void	init_env()
{
	char	logfile[256];
	char fname[32];

	adhome = getenv("ADHOME");
	sprintf(fname, "log/test-%s.txt", TimeUtil::get_yymmdd() );
	StrUtil::path_merge(logfile, adhome, fname);

	ACE_OSTREAM_TYPE *output = new ofstream(logfile, ofstream::app);
	ACE_LOG_MSG->msg_ostream(output, 1);
	ACE_LOG_MSG->set_flags(ACE_Log_Msg::OSTREAM);
	ACE_LOG_MSG->clr_flags (ACE_Log_Msg::STDERR);

	ACE_DEBUG ((LM_DEBUG, "[%T] -- START UP --\n"));
	
	if (adhome) ACE_DEBUG ((LM_INFO,	"ADHOME = %s\n", adhome));
	else ACE_DEBUG ((LM_INFO,	"ADHOME is NOT set !\n"));
	
	config.open_home(adhome, "anydict.cfg");
	
	
}


int main(int argc, char *argv[])
{

	init_env();
	
	if (argc > 1) {
		if (strcmp(argv[1], "db")==0)
		{
			test_DB();
		}
		else if (strcmp(argv[1], "edict")==0)
		{
			test_EDict();
		}
		else if (strcmp(argv[1], "extract")==0)
		{
			if (argc == 3)
				test_Extract(argv[2]);
			else
				test_Extract("txt/www_whitehouse_gov-_news_releases_2005_04_20050421-4.txt");
				
		}
		else if (strcmp(argv[1], "search")==0)
		{
			test_Search();
		}
		else {
			help();
		}
		
	}
	else {
		help();
	}

	
	dbStore::finish();
	return 0;
}
