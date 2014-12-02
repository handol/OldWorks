
// @dahee
//#define	ACE_USES_OLD_IOSTREAMS

#include "ace/streams.h"
#include "ace/Acceptor.h"
#include "ace/SOCK_Acceptor.h"
#include "ace/Reactor.h"
#include "ace/Signal.h"

#include "HarvestHandler.h"
#include "htmlTagEntity.h"
#include "StrUtil.h"
#include "dbStore.h"
#include "Config.h"
#include "TimeUtil.h"
#include "ctgList.h"
#include "harvestCron.h"

#include "categoryMapper.h"

#include <stdio.h>
#include <stdlib.h>

ACE_Reactor reactor;

static sig_atomic_t finished = 0;
extern "C" void handler (int)
{
	finished = 1;
	reactor.notify(); // notify() 를 하면 handle_events() 함수가 종료.
}


int PORT = 9010;
char	*adhome = NULL;
Config	config(50);

void	init_env()
{
	char	logfile[256];
	char fname[32];

	adhome = getenv("ADHOME");
	sprintf(fname, "log/harvest-%s.txt", TimeUtil::get_yymmdd() );
	StrUtil::path_merge(logfile, adhome, fname);

	// log file  append mode: ofstream::app
	ACE_OSTREAM_TYPE *output = new ofstream(logfile, ofstream::app);
	ACE_LOG_MSG->msg_ostream(output, 1);
	ACE_LOG_MSG->set_flags(ACE_Log_Msg::OSTREAM);
	ACE_LOG_MSG->clr_flags (ACE_Log_Msg::STDERR);

	ACE_DEBUG ((LM_STARTUP, "[%T] -- START UP --\n"));

	if (adhome) ACE_DEBUG ((LM_INFO,	"ADHOME = %s\n", adhome));
	else ACE_DEBUG ((LM_INFO,	"ADHOME is NOT set !\n"));

	config.open_home(adhome, "anydict.cfg");
	PORT = config.GetIntVal("HARVEST_PORT", PORT);
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

class myCron : public ACE_Task<ACE_MT_SYNCH>
{
public:
	virtual int svc (void)
	{
		while(1) {
		harvestCron mycron;
		mycron.setHarvestAddr("127.0.0.1", 9010);
		mycron.loadTable(100);

		/* test 용 */
		// mycron.do_job("http://www.anydict.com/");

		// 120 초 동안 svc() 수행. cronjob 테이블 변경시 반영할 수 있도록 하기 위해.
		mycron.setTimer(120);
		mycron.print();
		mycron.svc();

		#ifdef WIN32
		Sleep(200); // miliseconds
		#else
		usleep(1000*200); // microseconds
		#endif


		}
		return 0;
	}
};


int
main (int, char **)
{
	char	addatapath[256];
	ctgList	url2ctg;
	init_env();

	// Create the acceptor that will listen for client connetions
	HarvestAcceptor peer_acceptor;

	StrUtil::path_merge(addatapath, adhome, "data");
	htmlTagEntity::init(addatapath);
	dbStore::prepare(
		config.GetStrVal("DBNAME", "ANYDICT"),
		config.GetStrVal("DBID", "inisoft"),
		config.GetStrVal("DBPASS", "gksehf"));

	/*
	ctgcnt = url2ctg.load();
	ACE_DEBUG ((LM_INFO,	"Ctg Prefix: %d\n", ctgcnt));
	url2ctg.print();
	*/
	categoryMapper::prepare();

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

	ACE_DEBUG ((LM_SHUTDOWN, "[%T] -- SHUT DOWN --\n"));
	return 0;
}


#if defined (ACE_HAS_EXPLICIT_TEMPLATE_INSTANTIATION)
template class ACE_Acceptor <SearchHandler, ACE_SOCK_ACCEPTOR>;
template class ACE_Svc_Handler<ACE_SOCK_STREAM, ACE_NULL_SYNCH>;
#elif defined (ACE_HAS_TEMPLATE_INSTANTIATION_PRAGMA)
#pragma instantiate ACE_Acceptor <SearchHandler, ACE_SOCK_ACCEPTOR>
#pragma instantiate ACE_Svc_Handler<ACE_SOCK_STREAM, ACE_NULL_SYNCH>
#endif											  /* ACE_HAS_EXPLICIT_TEMPLATE_INSTANTIATION */
