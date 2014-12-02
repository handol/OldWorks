
#ifndef LOGGING_HANDLER_H
#define LOGGING_HANDLER_H

#include "ace/INET_Addr.h"

#if !defined (ACE_LACKS_PRAGMA_ONCE)
# pragma once
#endif											  /* ACE_LACKS_PRAGMA_ONCE */

#include "ace/Acceptor.h"
#include "ace/SOCK_Stream.h"
#include "ace/Reactor.h"

#include "StrStream.h"
#include "Docs.h"


class ExtractHandler : public ACE_Svc_Handler <ACE_SOCK_STREAM, ACE_NULL_SYNCH>
{
	public:

		/* The Acceptor<> template will open() us when there is a new client
		  connection.  */
		virtual int open (void *_acceptor);

		/* This is a matter of style & maybe taste.  Instead of putting all
		  of this stuff into a destructor, we put it here and request that
		  everyone call destroy() instead of 'delete'.  */
		virtual void destroy (void);

		/* If somebody doesn't like us, they will close() us.  Actually, if
		  our open() method returns -1, the Acceptor<> will invoke close()
		  on us for cleanup.  */
		virtual int close (u_long flags = 0);

	protected:

		/* Respond to input just like Tutorial 1.  */
		virtual int handle_input (ACE_HANDLE);

		/* When the timer expires, handle_timeout() will be called.  The
		  'arg' is the value passed after 'this' in the schedule_timer()
		  call.  You can pass in anything there that you can cast to a
		  void*.  */
		virtual int handle_timeout (const ACE_Time_Value &tv,
			const void *arg);

		/*
		  Clean ourselves up when handle_input() (or handle_timer()) returns -1
		  */
		virtual int handle_close (ACE_HANDLE,
			ACE_Reactor_Mask);

		

	private:
		/*
		�� handler�� ������ ���� ó�� ����.
		*/
		int process(char *recvdata, int datalen=0);
		int sendRes();
		
		StrStream resStr;
		int	debug;

};

typedef ACE_Acceptor <ExtractHandler, ACE_SOCK_ACCEPTOR> ExtractAcceptor;

#endif											  /* LOGGING_HANDLER_H */
