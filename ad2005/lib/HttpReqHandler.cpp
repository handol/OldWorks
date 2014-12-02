#include "ace/streams.h"
#include "ace/Acceptor.h"
#include "ace/SOCK_Acceptor.h"
#include "ace/SOCK_Stream.h"
#include "ace/Reactor.h"

#include "HttpReqHandler.h"
#include "CGI.h"


int HttpReqHandler::open (void *_acceptor)
{
	HttpReqAcceptor *acceptor = (HttpReqAcceptor *) _acceptor;
	ACE_INET_Addr addr;

	this->reactor (acceptor->reactor ());
	
	if (this->peer ().get_remote_addr (addr) == -1)
		return -1;

	if (this->reactor ()->register_handler (this,
		ACE_Event_Handler::READ_MASK) == -1)
		ACE_ERROR_RETURN ((LM_ERROR,
			"[%T] can't register with reactor\n"),
			-1);

	else if (this->reactor ()->schedule_timer (this,
		0,
		ACE_Time_Value (2),
		ACE_Time_Value (3)) == -1)
		ACE_ERROR_RETURN ((LM_ERROR,
				"can'[%T] t register with reactor\n"),
				-1);

	ACE_DEBUG ((LM_DEBUG,
		"[%T] connected with %s\n",
		addr.get_host_name ()));

	return 0;
}


/**
delete 대신 destroy() 를 호출. 그냥 스타일이고 취향.
*/
void HttpReqHandler::destroy (void)
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
int HttpReqHandler::close (u_long flags)
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
int HttpReqHandler::handle_input (ACE_HANDLE)
{
	char buf[1024];

	ACE_OS::memset (buf, 0, sizeof (buf));

	switch (this->peer ().recv (buf,
		sizeof buf))
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
				"[%T] read: %s",
				buf));
			process(buf);
	}

	return 0;
}


/* When the timer expires, handle_timeout() will be called.  The
  'arg' is the value passed after 'this' in the schedule_timer()
  call.  You can pass in anything there that you can cast to a
  void*.  */
int HttpReqHandler::handle_timeout (const ACE_Time_Value &tv,
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
int HttpReqHandler::handle_close (ACE_HANDLE,
ACE_Reactor_Mask)
{
	this->destroy ();
	return 0;
}

/**
HTTP request header 를 분석해서
주어진 일을 처리..

GET /search?query=each HTTP/1.1
...

*/
int HttpReqHandler::process(char *recvdata, int datalen)
{
	return 0;
}

int HttpReqHandler::sendRes()
{
	char header[256];

	// HTTP response Header
	ACE_OS::sprintf(header,
		"HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: %d\r\n\r\n",
		resStr.len() );

	this->peer().send(header, strlen(header));

	// HTTP response body
	this->peer().send(resStr.str(), resStr.len() );
	this->peer().close();
	return 0;
}


