#include "ace/streams.h"
#include "ace/Acceptor.h"
#include "ace/SOCK_Acceptor.h"
#include "ace/SOCK_Stream.h"
#include "ace/Reactor.h"

#include "MorphHandler.h"
#include "Spliter.h"
#include "CGI.h"
#include "StrUtil.h"
#include "MemSplit.h"
#include "CLog.h"

#include "HDict.h"
#include "HGram.h"
#include "HStem.h"
#include "HProc.h"

int MorphHandler::open (void *_acceptor)
{
	SearchAcceptor *acceptor = (SearchAcceptor *) _acceptor;
	ACE_INET_Addr addr;

	adhome = getenv("ADHOME");
	
	this->reactor (acceptor->reactor ());
	
	if (this->peer ().get_remote_addr (addr) == -1)
		return -1;

	if (this->reactor ()->register_handler (this,
		ACE_Event_Handler::READ_MASK) == -1)
		ACE_ERROR_RETURN ((LM_ERROR,
			"[%T] can't register with reactor\n"),
			-1);

	ACE_DEBUG ((LM_DEBUG,
		"[%T] client = %s:%d\n",
		addr.get_host_addr (), addr.get_port_number() ));

	return 0;
}


/**
delete 대신 destroy() 를 호출. 그냥 스타일이고 취향.
*/
void MorphHandler::destroy (void)
{
	/* Remove ourselves from the reactor */
	this->reactor ()->remove_handler
		(this,
		ACE_Event_Handler::READ_MASK | ACE_Event_Handler::DONT_CALL);

	/* Cancel that timer we scheduled in open() */
	this->reactor ()->cancel_timer (this);
	
	/* Shut down the connection to the client.  */
	this->peer ().close ();

	/* Free our memory.  */
	// @dahee  -- 괜찮은가 ?   delete해도.
	delete this;
}


/**
cleanup을 위해 외부에서 호출한다. 강제적 종료와 유사한 것 같다.ㅏ
Accept가 open() 을 호출한 후 오류 발생이면 close()를 호출한다.

If somebody doesn't like us, they will close() us.  Actually, if
  our open() method returns -1, the Acceptor<> will invoke close()
  on us for cleanup.  */
int MorphHandler::close (u_long flags)
{
/* The ACE_Svc_Handler baseclass requires the <flags> parameter.
 */
	ACE_UNUSED_ARG (flags);

/*
  Clean up and go away.
  */
	this->destroy ();
	return 0;
}


/* Respond to input just like Tutorial 1.  */
int MorphHandler::handle_input (ACE_HANDLE)
{
	char buf[1024];
	int	bytes=0;
	
	ACE_OS::memset (buf, 0, sizeof (buf));
	bytes = this->peer ().recv (buf, sizeof(buf) -1);
	
	switch (bytes)
	{
		case -1:
			ACE_ERROR_RETURN ((LM_ERROR,
				"[%T] %p bad read\n",
				"svc handler"),
				-1);
		case 0:
			ACE_ERROR_RETURN ((LM_ERROR,
				"[%T] recv() error (fd = %d)\n",
				this->get_handle ()),
				-1);
		default:
			buf[bytes] = 0;
			ACE_DEBUG ((LM_DEBUG,
				"[%T] read %d bytes [%d]\n",
				bytes, this->get_handle ()));
				
			process(buf);
	}

	return 0;
}


/* When the timer expires, handle_timeout() will be called.  The
  'arg' is the value passed after 'this' in the schedule_timer()
  call.  You can pass in anything there that you can cast to a
  void*.  */
int MorphHandler::handle_timeout (const ACE_Time_Value &tv,
const void *arg)
{
	ACE_UNUSED_ARG(tv);
	ACE_UNUSED_ARG(arg);
	ACE_DEBUG ((LM_DEBUG,
		"[%T] handling timeout from this = %u\n",
		this));
	return 0;
}


/*
  Clean ourselves up when handle_input() (or handle_timer()) returns -1
  */
int MorphHandler::handle_close (ACE_HANDLE,
ACE_Reactor_Mask)
{
	this->destroy ();
	return 0;
}


int MorphHandler::process(char *recvdata, int datalen)
{
	CGI cgi;
	char *query=0;
	char fname[256];
	resStr.init (10*1024);
	
	cgi.parse(recvdata);

	resStr.add("METHOD = %s\n", cgi.method() );
	query = cgi.getVal("query");
	
	resStr.add("QUERY = %s\n", query );
	
	//cgi.print( resStr );	
	ACE_DEBUG ((LM_INFO,"%s %s\n", cgi.method(), query ));
	
	if (strcmp(cgi.method(), "stem")==0) {
		HProc::stem(query, resStr);
	}
	else if (strcmp(cgi.method(), "dict")==0) {
		HProc::dict(query, resStr);
	}
	else if (strcmp(cgi.method(), "all")==0) {
		HProc::test_stemmer(StrUtil::path_merge(fname, adhome, "dataH/test-howtoberich.txt"), resStr);
	}
	
	
	
	ACE_DEBUG ((LM_DEBUG,"%s\n", resStr.str() ));
	
	sendRes();
	return 0;
}




int MorphHandler::sendRes()
{
	char header[256];

	// HTTP response Header
	ACE_OS::sprintf(header, "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n");
	ACE_OS::sprintf(header,
		"HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: %d\r\n\r\n",
		resStr.len() );

	this->peer().send(header, strlen(header));

	// HTTP response body
	this->peer().send(resStr.str(), resStr.len() );
	this->peer().close();
	return 0;
}


