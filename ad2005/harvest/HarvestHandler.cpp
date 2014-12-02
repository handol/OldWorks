#include "ace/streams.h"
#include "ace/Acceptor.h"
#include "ace/SOCK_Acceptor.h"
#include "ace/SOCK_Stream.h"
#include "ace/Reactor.h"

#include "HarvestHandler.h"
#include "Spliter.h"
#include "CGI.h"

#include "FileUtil.h"
#include "StrUtil.h"

#include "URLTrav.h"
#include "TravExpr.h"
#include "htmlTagEntity.h"

int HarvestHandler::open (void *_acceptor)
{
	HarvestAcceptor *acceptor = (HarvestAcceptor *) _acceptor;
	ACE_INET_Addr addr;

	this->reactor (acceptor->reactor ());
	
	if (this->peer ().get_remote_addr (addr) == -1)
		return -1;

	if (this->reactor ()->register_handler (this,
		ACE_Event_Handler::READ_MASK) == -1)
		ACE_ERROR_RETURN ((LM_ERROR,
			"[%T] can't register with reactor\n"),
			-1);

	/*
	// ACE 의 timer 기능 사용 방법. 필요 없음.
	else if (this->reactor ()->schedule_timer (this,
		0,
		ACE_Time_Value (2),
		ACE_Time_Value (3)) == -1)
		ACE_ERROR_RETURN ((LM_ERROR,
				"can'[%T] t register with reactor\n"),
				-1);
	*/

	ACE_DEBUG ((LM_DEBUG,
		"[%T] client = %s:%d\n",
		addr.get_host_addr (), addr.get_port_number() ));

	return 0;
}


/**
delete 대신 destroy() 를 호출. 그냥 스타일이고 취향.
*/
void HarvestHandler::destroy (void)
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
int HarvestHandler::close (u_long flags)
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
int HarvestHandler::handle_input (ACE_HANDLE)
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
			//ACE_DEBUG ((LM_DEBUG, "%s", buf));
			
			process(buf);
			
	}

	return 0;
}


/* When the timer expires, handle_timeout() will be called.  The
  'arg' is the value passed after 'this' in the schedule_timer()
  call.  You can pass in anything there that you can cast to a
  void*.  */
int HarvestHandler::handle_timeout (const ACE_Time_Value &tv,
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
int HarvestHandler::handle_close (ACE_HANDLE,
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
int HarvestHandler::process(char *recvdata, int datalen)
{
	CGI cgi;
	char *expr=0;
	int	early_ret=0;
	resStr.init (10*1024);
	
	cgi.parse(recvdata);
	//cgi.print();
	
	debug = cgi.getIntVal("debug");

	expr = cgi.getVal("expr");
	early_ret = cgi.getIntVal("early");

	if (strcmp(cgi.method(), "get") == 0) {
		webfetch(expr);
		sendRes();
		return 0;
	}
	
	if ( strcmp(cgi.method(), "harvest") != 0
		|| expr[0] == 0 ) 
	{
		resStr.add("$ERROR$ Unknown request ! (%s) \n", cgi.method() );
		sendRes();
		return 0;
	}
	

	if (debug) resStr.add("method = %s\n", cgi.method() );
	if (debug) resStr.add("expr = %s\n", expr);
	
	//cgi.print( resStr );	
	
	harvest( expr, early_ret);
	
	return 0;
}


/**
실제로 harvest 수행.
early_flag 가 true 이면 request 내용을 DB 에 저장하고, REQ ID 를 구한 다음 바로 결과를 던져 준다. 
early_flag 가 false 이면 문서 수집을 수행한 후 그 결과를 던져 준다.
*/
int HarvestHandler::harvest(char *expr_str, int early_flag)
{
	URLTrav trav;
	int	reqID=0;
	char *adhome = 0;

	adhome = getenv("ADHOME");	

	trav.setDebug(debug);
	
	reqID = trav.expr(expr_str);
	resStr.add( "$REQID$ %d\n", reqID);
	if (early_flag) sendRes();
	
	trav.traverse( resStr, adhome );
	if (! early_flag) sendRes();
	return 0;
}


int HarvestHandler::sendRes()
{
	char header[256];

	ACE_DEBUG ((LM_DEBUG,"[%T] %s\n", resStr.str() ));
	
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

int HarvestHandler::webfetch(char *url)
{
	useCurl urlfetch;
	char *ptr = strchr(url, ' ');
	char *newurl =0;
	if (ptr) *ptr = 0;
	
	/* web 에서 HTML 문서 가져오기 */
	urlfetch.httpGet( url, 1024*10);

	resStr.add("HTTP RES: size=%d code=%d Content-Type:%s\n", 
		urlfetch.bodysize(), urlfetch.httpResult(), urlfetch.httpType() );

	resStr.add("RES Header\n%s\n", urlfetch.header() );
	newurl = urlfetch.location() ;
	if (newurl) resStr.add("NEW location: %s\n", newurl );
	
	ACE_DEBUG((LM_DEBUG,"[%T] HTTP RES: size=%d code=%d Content-Type:%s\n", 
		urlfetch.bodysize(), urlfetch.httpResult(), urlfetch.httpType() ));
	return 0;
}