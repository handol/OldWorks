#include "ace/streams.h"
#include "ace/Acceptor.h"
#include "ace/SOCK_Acceptor.h"
#include "ace/SOCK_Stream.h"
#include "ace/Reactor.h"

#include "ExtractHandler.h"
#include "Extracter.h"
#include "Spliter.h"
#include "CGI.h"
#include "FileUtil.h"
#include "StrUtil.h"

#include "EDict.h"
#include "Docs.h"
#include "TextProc.h"
#include "dbStore.h"

int ExtractHandler::open (void *_acceptor)
{
	ExtractAcceptor *acceptor = (ExtractAcceptor *) _acceptor;
	ACE_INET_Addr addr;

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
void ExtractHandler::destroy (void)
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
int ExtractHandler::close (u_long flags)
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
int ExtractHandler::handle_input (ACE_HANDLE)
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
				
			process(buf, bytes);
			
	}

	return 0;
}


/* When the timer expires, handle_timeout() will be called.  The
  'arg' is the value passed after 'this' in the schedule_timer()
  call.  You can pass in anything there that you can cast to a
  void*.  */
int ExtractHandler::handle_timeout (const ACE_Time_Value &tv,
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
int ExtractHandler::handle_close (ACE_HANDLE,
ACE_Reactor_Mask)
{
	this->destroy ();
	return 0;
}

/**
extract를 수행하는 메인 함수.
문서가 하나일 때는 extract_by_docid() 이용.
문서가 여러개 일 때는 dbStore::selectDB_doc_id() 로  문서의 ID 목록을 먼저 구한 다음에,
extract_by_docid() 이용. -- extract_range()

extract_by_docid() 는 do_extract() 를 호출.

*/
int ExtractHandler::process(char *recvdata, int datalen)
{
	CGI cgi;
	char	*method=0, *textfname=0;
	int docID, fromID, toID, many;
	resStr.init (10*1024);
	
	cgi.parse(recvdata);

	if ( strcmp(cgi.method(), "extract") != 0) {
		resStr.add("$ERROR$ Unknown request ! (%s) \n\n", cgi.method() );
		sendRes();
		return 0;
	}
	
	debug = cgi.getIntVal("debug");
	docID = cgi.getIntVal("docid");
	if (docID == 0) {
		fromID = cgi.getIntVal("from");
		toID = cgi.getIntVal("to");
		many = cgi.getIntVal("many");
	}

	if (docID) {
		Extracter::extract_by_docid( docID, resStr);
		
	}
	else {
		Extracter::extract_range(fromID, toID, many, resStr);
	}

	sendRes();

	
	return 0;
}



int ExtractHandler::sendRes()
{
	char header[256];

	ACE_DEBUG ((LM_DEBUG,"[%T] %s\n", resStr.str() ));
	
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


