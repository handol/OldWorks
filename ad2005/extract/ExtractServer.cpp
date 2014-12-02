
// @dahee
//#define	ACE_USES_OLD_IOSTREAMS

#include "ace/streams.h"
#include "ace/Acceptor.h"
#include "ace/SOCK_Acceptor.h"
#include "ace/Reactor.h"
#include "ace/Signal.h"

#include "ExtractHandler.h"
#include "StrUtil.h"
#include "TimeUtil.h"
#include "Config.h"

#include "EDict.h"
#include "HDict.h"
#include "HGram.h"
#include "dbStore.h"
#include "TextProc.h"


#include "Extracter.h"
#include "extractCron.h"

ACE_Reactor reactor;

static sig_atomic_t finished = 0;
extern "C" void handler (int)
{
	finished = 1;
	reactor.notify();
}


int PORT = 9020;

char	*adhome = NULL;
Config	config(50);

void	init_env()
{
	char	logfile[256];
	char fname[32];

	adhome = getenv("ADHOME");
	sprintf(fname, "log/extract-%s.txt", TimeUtil::get_yymmdd() );
	StrUtil::path_merge(logfile, adhome, fname);

	ACE_OSTREAM_TYPE *output = new ofstream(logfile, ofstream::app);
	ACE_LOG_MSG->msg_ostream(output, 1);
	ACE_LOG_MSG->set_flags(ACE_Log_Msg::OSTREAM);
	ACE_LOG_MSG->clr_flags (ACE_Log_Msg::STDERR);

	ACE_DEBUG ((LM_DEBUG, "[%T] -- START UP --\n"));
	
	if (adhome) ACE_DEBUG ((LM_INFO,	"ADHOME = %s\n", adhome));
	else ACE_DEBUG ((LM_INFO,	"ADHOME is NOT set !\n"));
	
	config.open_home(adhome, "anydict.cfg");
	PORT = config.GetIntVal("EXTRACT_PORT", PORT);
	ACE_DEBUG ((LM_INFO,	"SERVER PORT = %d\n", PORT));
	
	if (strcasecmp(config.GetStrVal("DEBUG"), "YES")==0)
	{
		ACE_LOG_MSG->priority_mask( LM_DEBUG | LM_INFO | LM_NOTICE | LM_ALERT | LM_WARNING | LM_ERROR, ACE_Log_Msg::PROCESS);
	}
	else
	{
		ACE_LOG_MSG->priority_mask( LM_INFO | LM_NOTICE | LM_ALERT | LM_WARNING | LM_ERROR, ACE_Log_Msg::PROCESS);
	}
}

void testWordSearch(char *word)
{
	wordType *w = 0;
	byte	PS=0;
	StrStream st;
	
	printf("testWordSearch: %s\n", word);
	w = EDict::stemWord(word, 0, &PS);
	if (w) EDict::printWord(w);
	else printf("Unknown word\n");
}

void testIdiomSearch(char *idiom)
{
	StrStream st;
	EDict::testIdiom(idiom, st);
	st.print();
	
}

void	unitTest()
{
	/*
	testWordSearch("take");
	testWordSearch("takes");
	testWordSearch("took");

	testIdiomSearch("bring back");
	testIdiomSearch("take in");
	testIdiomSearch("takes in");
	*/
	
	//Extracter::test_ExtractOne("She call off the meeting.");
	
	//Extracter::test_ExtractOne("나는 학교에 가다 .");

	char engTest[1000] = \
"President | Vice President | First Lady | Mrs. Cheney | News & Policies \n"
"History & Tours | Kids | Your Government | Appointments | Jobs | Contact | Text only \n"
"Accessibility | Search | Privacy Policy | Help | Site Map \n" ;

	Extracter::test_ExtractOne(engTest);
}


class myCron : public ACE_Task<ACE_MT_SYNCH>
{
public:
	virtual int svc (void)
	{
		extractCron mycron;
		mycron.setHarvestAddr("127.0.0.1", 8010);
		mycron.loadTable(100);
		mycron.print();
		mycron.svc();
		return 0;
	}
};


int
main (int, char **)
{
	// Create the acceptor that will listen for client connetions
	ExtractAcceptor peer_acceptor;
	
	init_env();
	
	dbStore::prepare(
		config.GetStrVal("DBNAME", "ANYDICT"),
		config.GetStrVal("DBID", "inisoft"),
		config.GetStrVal("DBPASS", "gksehf"));

	EDict::prepare(adhome);

	TextProc::prepare();
	
	HDict::prepare(adhome);

	//unitTest();

	if (peer_acceptor.open (ACE_INET_Addr (PORT),
		&reactor) == -1)
		ACE_ERROR_RETURN ((LM_ERROR,
			"%p\n",
			"open"),
			-1);

	ACE_Sig_Action sa ((ACE_SignalHandler) handler, SIGINT);

	myCron mycron;
	mycron.activate();

	// Perform  service until the signal handler receives SIGINT.
	while (!finished)
		reactor.handle_events ();

	// Close the acceptor so that no more clients will be taken in.
	peer_acceptor.close();
	dbStore::finish();

	ACE_DEBUG ((LM_DEBUG,
		"[%T] -- SHUT DOWN --\n"));
	return 0;
}


#if defined (ACE_HAS_EXPLICIT_TEMPLATE_INSTANTIATION)
template class ACE_Acceptor <SearchHandler, ACE_SOCK_ACCEPTOR>;
template class ACE_Svc_Handler<ACE_SOCK_STREAM, ACE_NULL_SYNCH>;
#elif defined (ACE_HAS_TEMPLATE_INSTANTIATION_PRAGMA)
#pragma instantiate ACE_Acceptor <SearchHandler, ACE_SOCK_ACCEPTOR>
#pragma instantiate ACE_Svc_Handler<ACE_SOCK_STREAM, ACE_NULL_SYNCH>
#endif											  /* ACE_HAS_EXPLICIT_TEMPLATE_INSTANTIATION */
