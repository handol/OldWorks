/**
Minic ACE files.

KTF-DMS-INISOFT mobile agent 2004
@author handol@gmail.com �Ѵ��� 011-430-0258
@date 2004.11.30
*/

#ifndef Log_Msg_H
#define	Log_Msg_H

/**
@file Log_Msg.h

ACE framework �� Log ����� �䳻�� ���̴�.

== ���� ==
 1.  
     * Log_Msg::logprint() �� ���� ȣ������ �ʰ� MACRO (ACE_DEBUG, ACE_LOG_MSG, ACE_HEX_DUMP) �� �̿��Ѵ�.
     * �⺻������ ���ξտ� thread ID, �ð� ���� ���� ��µȴ�.
     * overwrite() �� ȣ���ϸ� ���� �α׸� �����.
     * ���ξտ� thread ID, �ð� ������ ����� disable �ҷ��� set_head() ȣ��.
     
 2. main.cpp ���� ��� �Ƿ�

 main() {
	home = getenv("MASERVER_HOME");

	ACE_LOG_MSG->open(home, "msger.log"); // msger.log ��� �̸��ڿ� MMDD(����)�� ���� �̸����� ȭ�� ����.

	ACE_LOG_MSG->overwrite(); // ���� ȭ�� �����.
	
	ACE_DEBUG ((LM_INFO, "\n==== %s BEGIN ====\n", argv[0]));
	ACE_HEX_DUMP((LM_DEBUG, recvBuf, 64));
 }
 
 
 3. main.cpp ���� ���� ����

   * step 1: �α� ȭ���� open�Ѵ�.  home ���丮�� ������ �� �ִ�.
		 home ���丮�� �־�����, home/log/xxxx.0316 �̶�� ������ �����ȴ�.
		 
   * step 2: ACE_DEBUG(( )); ACE_HEX_DUMP(( )); ���� �׳� ����ϸ� �ȴ�.

   
   * ���ξտ� thread ID, �ð� ������ ����� disable �ҷ��� set_head() ȣ��.
   {
        ACE_LOG_MSG->set_head(0);
	ACE_DEBUG(( .... ));
	ACE_LOG_MSG->set_head(1);
 }
   
   
 
*/

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <sys/timeb.h>
#include <time.h>


#ifndef byte
typedef unsigned char byte;
#endif
#ifndef MIN
#define MIN(A, B) ((A)<(B))?(A):(B)
#endif

//#define	LOGPRN(X)	Log_Msg::logprint(LM_DEBUG, (X))
#define	ACE_DEBUG(X)	Log_Msg::logprint X
#define	ACE_HEX_DUMP(X)	Log_Msg::hexdump X
#define	ACE_LOG_MSG	Log_Msg::getInstance()

enum ACE_Log_Priority
{
  // = Note, this first argument *must* start at 1!

  /// Shutdown the logger (decimal 1).
  LM_SHUTDOWN = 01,

  /// Messages indicating function-calling sequence (decimal 2).
  LM_TRACE = 02,

  /// Messages that contain information normally of use only when
  /// debugging a program (decimal 4).
  LM_DEBUG = 04,

  /// Informational messages (decimal 8).
  LM_INFO = 010,

  /// Conditions that are not error conditions, but that may require
  /// special handling (decimal 16).
  LM_NOTICE = 020,

  /// Warning messages (decimal 32).
  LM_WARNING = 040,

    /// Initialize the logger (decimal 64).
  LM_STARTUP = 0100,

  /// Error messages (decimal 128).
  LM_ERROR = 0200,

  /// Critical conditions, such as hard device errors (decimal 256).
  LM_CRITICAL = 0400,

  /// A condition that should be corrected immediately, such as a
  /// corrupted system database (decimal 512).
  LM_ALERT = 01000,

  /// A panic condition.  This is normally broadcast to all users
  /// (decimal 1024).
  LM_EMERGENCY = 02000,

  /// The maximum logging priority.
  LM_MAX = LM_EMERGENCY,

  /// Do not use!!  This enum value ensures that the underlying
  /// integral type for this enum is at least 32 bits.
  LM_ENSURE_32_BITS = 0x7FFFFFFF
};

class Log_Msg {
public:
	static Log_Msg * getInstance();
	
private:  
	Log_Msg() {}
	~Log_Msg() {}

public:
	static int open(char *dir_name, char *module_name);

	static void	set_priority(int pri) {
		priority = pri;
	}
	static void	set_time_print(int on_off) {
		timeprint = on_off;
	}
	static void	set_thrid_print(int on_off) {
		thridprint =on_off;
	}
	
	static void set_head(int on_off) {
		timeprint = on_off;
		thridprint =on_off;
	}

	static void	set_stderr(void);
	static void	set_stdout(void);
	
	static void logprint(int pri, char *fmt, ...);
	static void hexdump(int pri, char *src, int size);

	static	void	overwrite();
	static	int	close(void);
  
private:
	static Log_Msg * _instance;
	static	int	priority;
	static	int	mask;
	static	int	timeprint;
	static	int	thridprint;
	static	int	tostd; // bool: stderr, stdout ������ ���.
		
	static	char fName[256];
	static	FILE *logfd;
	static	int old_day;

	static	struct tm *t;

	static	void	logopen(void);
	static	void	_goodhex(byte *ptr, int size, int maxsize);
		
};
#endif

