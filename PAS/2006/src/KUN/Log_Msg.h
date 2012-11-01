/**
Minic ACE files.

KTF-DMS-INISOFT mobile agent 2004
@author handol@gmail.com 한대희 011-430-0258
@date 2004.11.30
*/

#ifndef Log_Msg_H
#define	Log_Msg_H

/**
@file Log_Msg.h

@brief ACE framework의 Log 기능 구현

== 사용법 ==
 1.  
     * Log_Msg::logprint() 을 직접 호출하지 않고 MACRO (ACE_DEBUG, ACE_LOG_MSG, ACE_HEX_DUMP) 를 이용한다.
     * 기본적으로 라인앞에 thread ID, 시간 정보 등이 출력된다.
     * overwrite() 를 호출하면 기존 로그를 지운다.
     * 라인앞에 thread ID, 시간 정보를 출력을 disable 할려면 set_head() 호출.
     
 2. main.cpp 에서 사용 실례

 main() {
	home = getenv("MASERVER_HOME");

	ACE_LOG_MSG->open(home, "msger.log"); // msger.log 라는 이름뒤에 MMDD(월일)이 붙은 이름으로 화일 생성.

	ACE_LOG_MSG->overwrite(); // 기존 화일 엎어쓴다.
	
	ACE_DEBUG ((LM_INFO, "\n==== %s BEGIN ====\n", argv[0]));
	ACE_HEX_DUMP((LM_DEBUG, recvBuf, 64));
 }
 
 
 3. main.cpp 에서 사용법 설명

   * step 1: 로그 화일을 open한다.  home 디렉토리를 지정할 수 있다.
		 home 디렉토리가 주어지면, home/log/xxxx.0316 이라는 식으로 생성된다.
		 
   * step 2: ACE_DEBUG(( )); ACE_HEX_DUMP(( )); 등을 그냥 사용하면 된다.

   
   * 라인앞에 thread ID, 시간 정보를 출력을 disable 할려면 set_head() 호출.
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
	static	int mkdir_ifnot(char *fname);
	
	static void logprint(int pri, const char *fmt, ...);
	static void hexdump(int pri, char *src, int size);

	static	void	overwrite();
	static	int	close(void);
  
private:
	static Log_Msg * _instance;
	static	int	priority;
	static	int	mask;
	static	int	timeprint;
	static	int	thridprint;
	static	int	tostd; // bool: stderr, stdout 등으로 출력.
		
	static	char fName[256];
	static	FILE *logfd;
	static	int old_day;

	static	struct tm t;

	static	void	logopen(void);
	static	void	_goodhex(byte *ptr, int size, int maxsize);
		
};
#endif

