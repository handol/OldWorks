//CC -o LogmAgent -DDEBUG LogmAgent.cpp TCPSvr.cpp LogPushAgent.cpp FileInfo.cpp HTTPAgent.cpp HTTPParser.cpp TCPSocket.cpp TimerEx.cpp UDRChecker.cpp LOGSearcher.cpp -lsocket -lpthread -lnsl
//CC -compat=4 -o LogmAgent -DDEBUG LogmAgent.cpp TCPSvr.cpp LogPushAgent.cpp FileInfo.cpp HTTPAgent.cpp HTTPParser.cpp TCPSocket.cpp TimerEx.cpp UDRChecker.cpp LOGSearcher.cpp -lsocket -lpthread -lnsl
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <errno.h>
#include <stdlib.h>
#include <fcntl.h>
#include <time.h>
#include <signal.h>
#include <iostream.h>
#include <arpa/inet.h>
#include <assert.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#include <queue>

#include "TCPSvr.h"
#include "LogPushAgent.h"
#include "HTTPParser.h"
#include "LOGSearcher.h"
#include "commondef.h"

pthread_mutex_t g_thread_mutex;
int g_thread_count;

#define HTTP_ERROR_MSG	"-1"

#define HTTP_HEADER_SIZE        256*256
#define HTTP_BODY_SIZE          1024*1024*4
#define HTTP_MAX_SIZE   (HTTP_HEADER_SIZE) + (HTTP_BODY_SIZE)
#define SOCKET_BUFF_SIZE        8196

#define LOG_ERROR_MESSAGE	"<?xml version=\"1.0\" encoding=\"EUC-KR\"?><ERROR><CODE>-1</CODE></ERROR>"

#define LOG_SEARCH_COMMAND      "COMMAND="
#define LOG_SEARCH_STYPE        "STYPE="
#define LOG_SEARCH_FTYPE        "FTYPE="
#define LOG_REQUEST_LIST        "LIST"
#define LOG_REQUEST_VIEW        "VIEW"
#define LOG_SEARCH_SFILENAME	"LIST="
#define LOG_SEARCH_STRING	"STRING="

#define MAX_VIEW_LINE_COUNT	1024

#define MAX_THREAD_COUNT	2

struct stFileList{
	char szFilePath[1024];	
public:
	void init() { memset(this, 0x00, sizeof(stFileList)); }
};

std::queue<int> *g_Queue;

extern "C" {
void* thrAgent(void* param);
}

#ifdef DEBUG
static void DUMP_MEM(unsigned char * mem, int len)
{
    printf("==============[MEMDUMP START]==============================================\n");
    for(int i=0, addr=0; i<len; addr++)
    {
        printf("%04d  | ",addr*16);
        int start = i;
        int j=0;
        for(; j<16&&i<len; j++)
        {
            printf("%02x ", mem[i++]);
        }
        int n = j;
        for(; j<16; j++)
        {
            printf("   ");
        }
        printf(" | ");
        for(j=0; j<n; j++)
        {
            if((char)*(mem+start+j) == '\r') printf(" ");
            else if((char)*(mem+start+j) == '\n') printf(" ");
            else if((char)*(mem+start+j) == '\t') printf(" ");
            else printf("%c", (char)*(mem+start+j));
        }
        printf("\n");
    }
    printf("==============[MEMDUMP END]================================================\n");
}
#endif

void SendErrorPage(int clisocket)
{
	char szResponse[8196] = {0};
	char body[1024] = {0};
	strcpy(body, LOG_ERROR_MESSAGE);
        sprintf(szResponse,
                        "HTTP/1.1 200 OK\r\n"
                        "Server: PAS MONITORING TOOL\r\n"
                        "Date: Tue, 26 Jul 2005 05:00:08 GMT\r\n"
                        "Pragma: no-cache\r\n"
                        "Content-Length: %d\r\n"
                        "Content-Type: text/xml;charset=KS_C_5601-1987\r\n"
                        "Cache-control: no-cache\r\n\r\n%s", strlen(body), body);

	::send(clisocket, szResponse, strlen(szResponse), 0);
	return;	
}

void* thrAgent(void* param)
{
	//int clisock = (int)param;
   int clisock = 0;
   int queuesize = 0;

   int iRet = 1;

   int nByte = 0;
   int nSize = 0;

   while(1) 
   {

	int  status = pthread_mutex_lock(&g_thread_mutex);
        if ( status == 0  )
        {
		queuesize = g_Queue->size();
		if ( queuesize > 0 )
		{
			clisock = g_Queue->front();
			g_Queue->pop();
		}
        	pthread_mutex_unlock(&g_thread_mutex);
        }

	if ( queuesize == 0 )
	{
		sleep(1);
		continue;
	}	

	CHTTPParser parser;
        parser.init();

        while ( HTTP_MAX_SIZE > nByte + SOCKET_BUFF_SIZE)
        {
                int ret;
                char buff[SOCKET_BUFF_SIZE] = {0};
                ret = recv(clisock, buff, SOCKET_BUFF_SIZE, 0);

#ifdef DEBUG
        cerr << "Recv:" << ret << endl;
#endif
                if ( ret < 0 )
                {
#ifdef DEBUG
        cerr << "Recv Error...-" << ret << endl;
#endif
			iRet = -1;
          		break; 
                }

#ifdef DEBUG
        cerr << "Recv Data..." << endl <<  buff << endl;
#endif
                parser.SetAddData(buff, ret);
		nByte += iRet;

                if ( parser.GetBodySize() >= parser.GetContentLength() )
                {
#ifdef DEBUG
        cerr << "Total Recv:" << nByte << endl;
        cerr << "GetHeaderSize:" << parser.GetHeaderSize() << endl;
        cerr << "GetBodySize:" << parser.GetBodySize() << endl;
        cerr << "GetContentLength:" << parser.GetContentLength() << endl;
#endif
                        break;
                }
        }

	char szCommand[8196] = {0};
	char buf_command[128] = {0};
	char buf_ftype[128] = {0};
	char buf_stype[128] = {0};
	char buf_string[128] = {0};
	char *p1;

	std::string buf_filename;

	if ( (p1 = strstr((char*)parser.GetBodyData(), LOG_SEARCH_COMMAND)) != NULL )
	{
		strncpy(buf_command, p1 + strlen(LOG_SEARCH_COMMAND), 4);
	}
	else
	{
		SendErrorPage(clisock);
		::close(clisock);
		continue;
		//return NULL;
	}

        if ( (p1 = strstr((char*)parser.GetBodyData(), LOG_SEARCH_FTYPE)) != NULL )
        {
        	strncpy(buf_ftype, p1 + strlen(LOG_SEARCH_FTYPE), 1);
        }
        else
        {
        	SendErrorPage(clisock);
		::close(clisock);
		continue;
		//return NULL;
        }

        if ( (p1 = strstr((char*)parser.GetBodyData(), LOG_SEARCH_STYPE)) != NULL )
        {
                strncpy(buf_stype, p1 + strlen(LOG_SEARCH_STYPE), 1);
        }
        else
        {
                SendErrorPage(clisock);
                ::close(clisock);
		continue;
                //return NULL;
        }

        if ( (p1 = strstr((char*)parser.GetBodyData(), LOG_SEARCH_SFILENAME)) != NULL )
        {
                buf_filename  =  p1 + strlen(LOG_SEARCH_SFILENAME);
        }

        if ( (p1 = strstr((char*)parser.GetBodyData(), LOG_SEARCH_STRING)) != NULL )
        {
		char *p2 = strstr(p1+1, "&");
		if ( p2 != NULL )
		{
                	strncpy(buf_string, p1 + strlen(LOG_SEARCH_STRING), p2-p1-strlen(LOG_SEARCH_STRING));
		}
        }


#ifdef DEBUG
	cerr << "command:" << buf_command << endl;
	cerr << "buf_ftype:" << buf_ftype << endl;
	cerr << "buf_stype:" << buf_stype << endl;
	cerr << "buf_filename:" << buf_filename << endl;
	cerr << "buf_string:" << buf_string << endl;
#endif
	if ( !strcmp(buf_command, LOG_REQUEST_LIST) )
	{
		
		if ( (p1 = strstr((char*)parser.GetBodyData(), "FTYPE=")) != NULL )
		{
			strncpy(buf_ftype, p1 + strlen("FTYPE="), 1); 
		}
		else
		{
			SendErrorPage(clisock);
		}

	
	        CLogSearcher log;
	        log.init();
		log.GetFileListQuery2(atoi(buf_stype), buf_ftype[0]);
	
	        int iSize = 0;
	
#ifdef DEBUG
                cerr << log.GetList() << endl;
#endif
	
	        char *szResponse = new char[HTTP_BODY_SIZE];
		memset(szResponse, 0x00, HTTP_BODY_SIZE);
	
	        sprintf(szResponse,
	                        "HTTP/1.1 200 OK\r\n"
	                        "Server: PAS MONITORING TOOL\r\n"
	                        "Date: Tue, 26 Jul 2005 05:00:08 GMT\r\n"
	                        "Pragma: no-cache\r\n"
	                        "Content-Length: %d\r\n"
	                        "Content-Type: text/xml\r\n"
	                        "Cache-control: no-cache\r\n\r\n%s", strlen(log.GetList()), log.GetList());
#ifdef DEBUG
	        cerr << szResponse << endl;
	        DUMP_MEM((unsigned char*)szResponse, strlen(szResponse));
#endif
		int totByte = strlen(szResponse);
		int tot = totByte;
		int nByte = 0;

		while ( nByte < tot ) 
		{
	        	int ret = ::send(clisock, szResponse+nByte, totByte, 0);
#ifdef DEBUG
	        	cerr << "send:" << ret << endl;
#endif
			if ( ret < 0 )
			{
				break;
			}	
		
			nByte += ret;
			totByte -= ret;
		}
		
		if ( szResponse )
		{
			delete[] szResponse;
			szResponse = NULL;
		}
	        ::close(clisock);

	}
	else if ( !strcmp(buf_command, LOG_REQUEST_VIEW) )
	{
		//惑技焊扁 贸府 何盒

		int iListCount = 0;
		stFileList *pFileList = new stFileList[1024];
	
		int iPos = 0;
		int nPos = 0;

		if ( buf_filename[0] == '^' )
		{
			SendErrorPage(clisock);
			::close(clisock);
			continue;
		}

		while ( (iPos = buf_filename.find("^", nPos)) != -1 && iListCount < 1024 )
		{
			pFileList[iListCount].init();
		
			char buf[256] = {0};
			strncpy(buf, buf_filename.data()+nPos, iPos - nPos);
#ifdef DEBUG
			cerr << buf << endl;
#endif
			if ( strlen(buf) < 10 || buf[0] == '^' )
			{
				nPos = iPos+strlen("^");
				continue;
			}

			strncpy(pFileList[iListCount].szFilePath, buf_filename.data()+nPos, iPos - nPos);
 
			nPos = iPos+strlen("^");
			iListCount++;
		}

		if ( iListCount == 0 )
		{
			continue;	//Top
		}
#ifdef DEBUG
		for ( int ij = 0; ij < iListCount; ij++ )
		{
			cerr << ij << ":" <<  pFileList[ij].szFilePath << endl;
		}	
#endif
                if ( (p1 = strstr((char*)parser.GetBodyData(), "FTYPE=")) != NULL )
                {
                        strncpy(buf_ftype, p1 + strlen("FTYPE="), 1);
                }
                else
                {
                        SendErrorPage(clisock);
                }
	
                CLogSearcher log;
                log.init();

                char *body = new char[HTTP_BODY_SIZE];
		memset(body, 0x00, HTTP_BODY_SIZE);

                strcpy(body, "");

                if ( buf_ftype[0] == FILE_TYPE_UDR )
                {
			int iTotViewSize = 0;
			for ( int k = 0; k < iListCount; k++ ) 
			{
                               	sprintf(szCommand, "zcat  %s | grep \"%s\"", pFileList[k].szFilePath, buf_string);
#ifdef DEBUG
				cerr << szCommand << endl;
#endif
                		STLogData* pLogData = NULL;
               	 		int iSize = 0;

                		log.GetPOpen(szCommand);
               			log.GetList(pLogData, iSize);
#ifdef DEBUG
                		for ( int i = 0; i < iSize; i++)
                		{
                        		//cerr << pLogData[i].data << endl;
                		}
#endif
                		for ( int j = 0; j < iSize; j++ )
                		{
					if ( iTotViewSize > MAX_VIEW_LINE_COUNT	)
					{
                                                continue;
					}
                        		strcat(body, pLogData[j].data);
					iTotViewSize++;
                		}

			 	if ( iTotViewSize > MAX_VIEW_LINE_COUNT )
                                {
                                 	break;
                               	} 
			}
#ifdef DEBUG
		//cerr << body << endl;
#endif
                }
		else if ( buf_ftype[0] == FILE_TYPE_SRCUDR )
		{
                        int iTotViewSize = 0;
                        for ( int k = 0; k < iListCount; k++ )
                        {
                                sprintf(szCommand, "zcat  %s | grep \"%s\"",  pFileList[k].szFilePath, buf_string);
#ifdef DEBUG
                                cerr << szCommand << endl;
#endif
                                STLogData* pLogData = NULL;
                                int iSize = 0;

                                log.GetPOpen(szCommand);
                                log.GetList(pLogData, iSize);
#ifdef DEBUG
                                for ( int i = 0; i < iSize; i++)
                                {
                                        //cerr << pLogData[i].data << endl;
                                }
#endif
                                for ( int j = 0; j < iSize; j++ )
                                {
                                        if ( iTotViewSize > MAX_VIEW_LINE_COUNT )
                                        {
                                                continue;
                                        }

                                        strcat(body, pLogData[j].data);
                                        iTotViewSize++;
                                }

                                if ( iTotViewSize > MAX_VIEW_LINE_COUNT ) 
                                {        
                                        break;        
                                } 
                        }
		}
		else if ( buf_ftype[0] == FILE_TYPE_STAT )
		{
                        int iTotViewSize = 0;
                        for ( int k = 0; k < iListCount; k++ )
                        {
                                sprintf(szCommand, "zcat  %s | grep \"%s\"",  pFileList[k].szFilePath, buf_string);

#ifdef DEBUG
                                cerr << szCommand << endl;
#endif
                                STLogData* pLogData = NULL;
                                int iSize = 0;

                                log.GetPOpen(szCommand);
                                log.GetList(pLogData, iSize);
#ifdef DEBUG
                                for ( int i = 0; i < iSize; i++)
                                {
                                        //cerr << pLogData[i].data << endl;
                                }
#endif
                                for ( int j = 0; j < iSize; j++ )
                                {
                                        if ( iTotViewSize > MAX_VIEW_LINE_COUNT )
                                        {
                                                continue;
                                        }

                                        strcat(body, pLogData[j].data);
                                        iTotViewSize++;
                                }

                                if ( iTotViewSize > MAX_VIEW_LINE_COUNT ) 
                                {        
                                        break;        
                                } 
                        }
		}
                else
                {
			if ( pFileList ) { delete[] pFileList; pFileList = NULL; }
                        SendErrorPage(clisock);
                        ::close(clisock);
                        //return NULL;
                }

                char *szResponse = new char[HTTP_BODY_SIZE];
		memset(szResponse, 0x00, HTTP_BODY_SIZE);

                sprintf(szResponse,
                                "HTTP/1.1 200 OK\r\n"
                                "Server: PAS MONITORING TOOL\r\n"
                                "Date: Tue, 26 Jul 2005 05:00:08 GMT\r\n"
                                "Pragma: no-cache\r\n"
                                "Content-Length: %d\r\n"
                                "Content-Type: text/html;charset=KS_C_5601-1987\r\n"
                                "Cache-control: no-cache\r\n\r\n%s", strlen(body), body);
		if ( body )
		{
			delete[] body;
			body = NULL;	
		}
#ifdef DEBUG
                //cerr << szResponse << endl;
                //DUMP_MEM((unsigned char*)szResponse, strlen(szResponse));
#endif
                int totByte = strlen(szResponse);
                int tot = totByte;
                int nByte = 0;

                while ( nByte < tot )
                {
                        int ret = ::send(clisock, szResponse+nByte, totByte, 0);
#ifdef DEBUG
                        cerr << "send:" << ret << endl;
#endif
                        if ( ret < 0 )
                        {
                                break;
                        }

                        nByte += ret;
                        totByte -= ret;
                }

                ::close(clisock);

		if ( szResponse ) { delete[] szResponse; szResponse = NULL; }
		if ( pFileList ) { delete[] pFileList; pFileList = NULL; }
			
	}
	else
	{
		SendErrorPage(clisock);
		::close(clisock);
		continue;
		//return NULL;
	}
    } //Never Rearch


	return NULL;
}

int main(int argc, char *argv[])
{

	g_Queue = new std::queue<int>();

	pthread_mutex_init(&g_thread_mutex, NULL);

	CLogPushAgent *pushAgent = new CLogPushAgent();
	pushAgent->StartServer(MONITOR_WEB_IP,  MONITOR_WEB_PORT, RUNNING_TIME_MIN);
	
	CTCPSvr tcpsvr;

	if ( tcpsvr.open(SERVER_PORT) != 1 )
	{
		pthread_mutex_destroy(&g_thread_mutex);
		return 0;
	}
        pthread_t       thread;
        pthread_attr_t  attr;
        pthread_attr_init(&attr);

	for ( int i = 0; i < MAX_THREAD_COUNT; i++ )
	{	
       		if ( pthread_create(&thread, &attr, thrAgent, (void*)NULL) != 0 )
		{
			exit(0);
		}
	}

	while (1)
	{
		int clisock = tcpsvr.accept();
#ifdef DEBUG
	cerr << "connect clisock:" << clisock << endl;
#endif
		int  status = pthread_mutex_lock(&g_thread_mutex);
		
		if ( status == 0 )
		{
			if ( g_Queue->size() < MAX_THREAD_COUNT )
			{
				g_Queue->push(clisock);
			}
			else
			{
				::close(clisock);
			}

			pthread_mutex_unlock(&g_thread_mutex);
		}
		else
		{
			::close(clisock);
		}
	}	

#ifdef DEBUG
	cerr << "Queue size:" << g_Queue->size() << endl;
#endif

	pthread_mutex_destroy(&g_thread_mutex);

	return 1;
}

