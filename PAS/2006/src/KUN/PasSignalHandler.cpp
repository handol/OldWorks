#include "PasSignalHandler.h"
#include "ace/Reactor.h"
#include "Common.h"
#include "PasLog.h"
#include "LogLevel.h"
#include "SystemStatistic.h"

PasSignalHandler::PasSignalHandler()
{
}

PasSignalHandler::~PasSignalHandler()
{
}


int	PasSignalHandler::handle_signal(int signum, siginfo_t * , ucontext_t * )
{
	switch(signum)
	{
	case SIGINT:
		PAS_NOTICE("Recv SIGINT");
		ACE_Reactor::instance()->end_reactor_event_loop();
		break;

	case SIGTERM:
		PAS_NOTICE("Recv SIGTERM");
		ACE_Reactor::instance()->end_reactor_event_loop();
		break;

	case SIGQUIT:
		PAS_NOTICE("Recv SIGQUIT");
		ACE_Reactor::instance()->end_reactor_event_loop();
		break;

	case SIGUSR1:
		PAS_NOTICE("Recv SIGUSR1 >> Change Log Level");
		changeLogLevel();
		break;

	case SIGUSR2:
		PAS_NOTICE("Recv SIGUSR2 >> Print System Statistic");
		printSystemStatistic();
		break;

	case SIGPIPE:
		PAS_DEBUG0("Recv SIGPIPE");
		break;

	case SIGALRM:
		PAS_INFO("Recv SIGALRM");
		break;

	case SIGHUP:
		PAS_NOTICE("Recv SIGHUP");
		break;

	case SIGCHLD:
		PAS_NOTICE("Recv SIGCHLD");
		break;

	case SIGILL:
		PAS_NOTICE("Recv SIGILL");
		break;

	case SIGSYS:
		PAS_NOTICE("Recv SIGSYS");
		break;

	case SIGTRAP:
		PAS_NOTICE("Recv SIGTRAP");
		break;

	case SIGFPE:
		PAS_NOTICE("Recv SIGFPE");
		break;

	case SIGPWR:
		PAS_NOTICE("Recv SIGPWR");
		break;

	case SIGWINCH:
		PAS_NOTICE("Recv SIGWINCH");
		break;

	case SIGURG:
		PAS_NOTICE("Recv SIGURG");
		break;

	case SIGPOLL:
		PAS_NOTICE("Recv SIGPOLL");
		break;

	case SIGTSTP:
		PAS_NOTICE("Recv SIGSTP");
		break;

	case SIGXCPU:
		PAS_NOTICE("Recv SIGXCPU");
		break;

	case SIGXFSZ:
		PAS_NOTICE("Recv SIGXFSZ");
		break;

	default:
		PAS_NOTICE1("Recv SigNum[%d]", signum);
		break;
	}

	return 0;

}

int PasSignalHandler::changeLogLevel()
{
	LogLevel* pLogLevel = LogLevel::instance();
	switch(pLogLevel->getLevel())
	{
	case LLT_VERYLOW:
		pLogLevel->setLevel(LLT_LOW);
		break;

	case LLT_LOW:
		pLogLevel->setLevel(LLT_MEDIUM);
		break;

	case LLT_MEDIUM:
		pLogLevel->setLevel(LLT_HIGH);
		break;

	case LLT_HIGH:
		pLogLevel->setLevel(LLT_VERYLOW);
		break;

	default:
		pLogLevel->setLevel(LLT_VERYLOW);
		break;
	}

	return 0;
}

int PasSignalHandler::printSystemStatistic()
{
	SystemStatistic* pStat = SystemStatistic::instance();
	pStat->writeLog();

	return 0;
}
