#ifndef __COMMON_H__
#define __COMMON_H__

#include <string>
#include <vector>
#include <map>
#include <ace/RW_Thread_Mutex.h>
#include <ace/Log_Msg.h>
#include <ace/Task_T.h>
#include <ace/Event_Handler.h>
#include <ace/Thread_Manager.h>
#include <ace/Message_Queue.h>
#include <ace/Message_Queue_T.h>
#include <ace/Lock_Adapter_T.h>
#include "FastString.hpp"

#include "basicDef.h"

#define	NULLCHAR		'\0'
#define	CR				"\r"
#define	LF				"\n"
#define	CRLF			"\r\n"
#define	LWS				" \t\r\n"
#define	CRLFCRLF		"\r\n\r\n"
#define	SP				" "
#define	COLON			":"

#define MAX_HOST_LEN		128
#define MAX_URL_LEN			1024
#define MAX_LINE_LEN		2048
#define MAX_FILENAME_LEN	1024
#define MAX_SERVERID_LEN	32
#define MAX_PASSWORD_LEN	32
#define MAX_SERVICEID_LEN	32
#define MAX_ID_LENG			32
#define MAX_SHORT_STRING_LEN	32
#define MAX_MEDIUM_STRING_LEN	128
#define MAX_LARGE_STRING_LEN	512
#define MAX_COOKIE_STRING_LEN 2048	///< for me cookie system

/** Transaction Information */
#define	LEN_USER_AGENT (127)
#define	LEN_HOST_NAME (31)
#define	LEN_IPADDR	(23)
#define	LEN_MNC	(3)
#define	LEN_PHONE_NUM	(15)
#define	LEN_MDN	(15)
#define	LEN_PHONE_MIN	(15)
#define	LEN_IMSI	(19)
#define	LEN_CH_INFO	(3)
#define	LEN_BASE_ID	(7) // 기지국 ID로 추정. 필수 로그는 아님.
#define	LEN_MS_MODEL	(15)
#define	LEN_BROWSER	(15)
#define	LEN_CP_NAME	(20)
#define	LEN_SVC_CODE	(20)
#define	LEN_HASH_KEY		(19)
#define	LEN_BILL_INFO		(15)
#define	LEN_COUNTER (12)
#define LEN_CKEY (128)
#define	LEN_KTF_INIT_PATH (32)


#define MaxHashKeySize 20
#define MaxBillInfoKeySize 12

typedef ACE_MT_SYNCH PAS_SYNCH;
typedef ACE_Thread_Mutex PasMutex;
typedef ACE_Null_Mutex NullMutex;
typedef ACE_RW_Thread_Mutex PasRWMutex;
typedef ACE_SYNCH_MUTEX PAS_SYNCH_MUTEX;
typedef ACE_Lock_Adapter<PasMutex> PasLock;

typedef std::vector<ACE_Message_Block*> PasMessageQueue;

typedef unsigned char byte;
typedef std::vector<char> vchar;
typedef std::vector<int> vint;
typedef std::map<int,int> mapint;
typedef std::map<int,int>::iterator iterint;

typedef FastString<64> TinyString;
typedef FastString<256> SmallString;
typedef FastString<1024> MediumString;
typedef FastString<4*1024> BigString;
typedef FastString<64*1024> HugeString;

typedef FastString<MAX_LINE_LEN> line_t;
typedef FastString<MAX_URL_LEN> url_t;
typedef FastString<MAX_HOST_LEN> host_t;
typedef FastString<MAX_SERVERID_LEN> serverid_t;
typedef FastString<MAX_FILENAME_LEN> filename_t;
typedef FastString<MAX_ID_LENG> santaId_t;
typedef FastString<MAX_PASSWORD_LEN> password_t;
typedef FastString<MAX_SERVICEID_LEN> svcID_t;
typedef FastString<MAX_SHORT_STRING_LEN> sstr_t;
typedef FastString<MAX_MEDIUM_STRING_LEN> mstr_t;
typedef FastString<MAX_LARGE_STRING_LEN> lstr_t;
typedef FastString<MAX_COOKIE_STRING_LEN> cookie_t;

typedef FastString<32> MDN_t;
typedef FastString<16> ip_t;

typedef unsigned int intMDN_t;
typedef unsigned int intIP_t;

typedef FastString<1024*2> StrStream_t;

typedef std::vector<url_t> urls_t;
typedef std::vector<cookie_t> cookies_t;

#endif

