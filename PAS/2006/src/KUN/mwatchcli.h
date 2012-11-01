#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <strings.h>
#include <sys/param.h>
#include <sysent.h>
#include <unistd.h>

#include "CommonSocket.h"

//#define DEBUG	                1
#define NOT_FOUN                -1
#define NOT_YET                 -1

#define MSGWATCH_PORT           15469			  // UDP 서버의 포트번호를 설정한�
#define MSGWATCH_ADDR           "msgwatchsvr"	  // UDP 서버명을 설정한�
#define MAX_NAME_LEN            80
#define MAX_DATA_LEN            128
#define MAX_MSG_LEN         (MAX_NAME_LEN + MAX_DATA_LEN)
#define MAX_ID_NUM              1000

#define MIN_SEND_TIME           60

typedef struct
{
	time_t lastsent;
	int    cnt;
	char   id[MAX_NAME_LEN];
} watch_index_t;

typedef struct _msg
{
	char   ServerID[MAX_NAME_LEN];
	char   ServiceID[MAX_NAME_LEN];
	int    MessageCount;
}MSG;

class mwatchcli
{
	int            index;
	watch_index_t *WatchIndex[MAX_ID_NUM];
	CSocket        cltSock;
	MSG            WatchMsg;

	void           Initialize(char *host, int port);

	public:
		mwatchcli();
		mwatchcli(char *host);
		mwatchcli(char *host, int port);
		~mwatchcli();

		BOOL addID(char * szID);

		BOOL addID(char * szID, int m_nMsgCnt);

		BOOL addMsgCnt(char * szID);
		BOOL addMsgCnt(char * szID, int m_nMsgCnt);
};
