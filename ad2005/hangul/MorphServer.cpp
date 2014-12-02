
// @dahee
//#define	ACE_USES_OLD_IOSTREAMS

#include "ace/streams.h"
#include "ace/Acceptor.h"
#include "ace/SOCK_Acceptor.h"
#include "ace/Reactor.h"
#include "ace/Signal.h"

#include "MorphHandler.h"
#include "TimeUtil.h"
#include "StrUtil.h"
#include "Config.h"
#include "HDict.h"
#include "HGram.h"
#include "dbStore.h"

/* One of the new things will be a signal handler so that we can exit
  the application somewhat cleanly.  The 'finished' flag is used
  instead of the previous infninite loop and the 'handler' will set
  that flag in respose to SIGINT (CTRL-C).
  The invocation of ACE_Reactor::notify() will cause the
  handle_events() to return so that we can see the new value of 'finished'.
*/

ACE_Reactor reactor;

static sig_atomic_t finished = 0;
extern "C" void handler (int)
{
	finished = 1;
	reactor.notify();
}


int PORT = 9040;
char	*adhome = NULL;

Config	config(50);

void	init_env()
{
	char	logfile[256];
	char fname[32];

	adhome = getenv("ADHOME");
	sprintf(fname, "log/morph-%s.txt", TimeUtil::get_yymmdd() );
	StrUtil::path_merge(logfile, adhome, fname);

	ACE_OSTREAM_TYPE *output = new ofstream(logfile, ofstream::app);
	ACE_LOG_MSG->msg_ostream(output, 1);
	ACE_LOG_MSG->set_flags(ACE_Log_Msg::OSTREAM);
	ACE_LOG_MSG->clr_flags (ACE_Log_Msg::STDERR);

	ACE_DEBUG ((LM_DEBUG, "[%T] -- START UP --\n"));
	
	if (adhome) ACE_DEBUG ((LM_INFO,	"ADHOME = %s\n", adhome));
	else ACE_DEBUG ((LM_INFO,	"ADHOME is NOT set !\n"));
	
	config.open_home(adhome, "anydict.cfg");
	PORT = config.GetIntVal("MORPH_PORT", PORT);
	ACE_DEBUG ((LM_INFO,	"MORPH PORT = %d\n", PORT));

	dbStore::prepare(
		config.GetStrVal("DBNAME", "ANYDICT"),
		config.GetStrVal("DBID", "inisoft"),
		config.GetStrVal("DBPASS", "gksehf"));
		
	if (strcasecmp(config.GetStrVal("DEBUG"), "YES")==0)
	{
		ACE_LOG_MSG->priority_mask( LM_DEBUG | LM_INFO | LM_NOTICE | LM_ALERT | LM_WARNING | LM_ERROR, ACE_Log_Msg::PROCESS);
	}
	else
	{
		ACE_LOG_MSG->priority_mask( LM_INFO | LM_NOTICE | LM_ALERT | LM_WARNING | LM_ERROR, ACE_Log_Msg::PROCESS);
	}
	
	
}


int main (int, char **)
{

	// Create the acceptor that will listen for client connetions
	SearchAcceptor peer_acceptor;

	init_env();
	
	HDict::prepare(adhome);
	
	if (peer_acceptor.open (ACE_INET_Addr (PORT),
		&reactor) == -1)
		ACE_ERROR_RETURN ((LM_ERROR,
			"%p\n",
			"open"),
			-1);

	ACE_Sig_Action sa ((ACE_SignalHandler) handler, SIGINT);

	// Perform  service until the signal handler receives SIGINT.
	while (!finished)
		reactor.handle_events ();

	// Close the acceptor so that no more clients will be taken in.
	peer_acceptor.close();

	
	ACE_DEBUG ((LM_DEBUG, "[%T] -- SHUT DOWN --\n"));
	return 0;
}


#if defined (ACE_HAS_EXPLICIT_TEMPLATE_INSTANTIATION)
template class ACE_Acceptor <SearchHandler, ACE_SOCK_ACCEPTOR>;
template class ACE_Svc_Handler<ACE_SOCK_STREAM, ACE_NULL_SYNCH>;
#elif defined (ACE_HAS_TEMPLATE_INSTANTIATION_PRAGMA)
#pragma instantiate ACE_Acceptor <SearchHandler, ACE_SOCK_ACCEPTOR>
#pragma instantiate ACE_Svc_Handler<ACE_SOCK_STREAM, ACE_NULL_SYNCH>
#endif											  /* ACE_HAS_EXPLICIT_TEMPLATE_INSTANTIATION */
