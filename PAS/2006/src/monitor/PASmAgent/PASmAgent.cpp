//CC -o PASmAgent -DDEBUG PASmAgent.cpp HTTPAgent.cpp TCPSocket.cpp HTTPParser.cpp DNSQuery.cpp TimerEx.cpp SYSMemInfo.cpp 
//SYSCPUInfo.cpp GWInfo.cpp FileInfo.cpp  -lsocket -lnsl -lkstat

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

#include <fstream.h>
#include <queue>

#include "HTTPAgent.h"
#include "DNSQuery.h"
#include "TimerEx.h"
#include "SYSMemInfo.h"
#include "SYSCPUInfo.h"
#include "GWInfo.h"
#include "FileInfo.h"

#define DOMAIN_INFO_IP		"61.74.64.96"
#define DOMAIN_INFO_PORT	8000
#define RUNNING_TIME		20
#define POST	

#define ME_GW_1		"pasgw1a"
#define ME_GW_2		"pasgw1a 8080"
#define KUN_GW_1	"pasgw1k pasgw_kun_9090.cfg"
#define KUN_GW_2	"pasgw1k pasgw_kun_9091.cfg"

#define ME_GW_TYPE	'M'
#define KUN_GW_TYPE	'K'

#define BUFF_SIZE		1024
#define MAX_RECORD_COUNT	1
#define RECORD_FILE "./PAS_MON.REC"
#define RECORD_TEMP_FILE "./PAS_MON.REC.TMP"

#define INTERVAL_MIN	5

char szhostname[128];

int GWInfoSend(const char* gwname, char Type, int iSEQ);
int FileInfoSend(const char* filename, int Type, char filetype);
void ErrorHandler(const char* szRecord);
int WriteFile();
void ReSend();

CDNSQuery dns;

std::queue<char*> *g_Queue;

int main(int argc, char *argv[])
{
	char url[8196] = {0};
#ifdef DEBUG
	strcpy(szhostname, "pasgw1");
#else
	//strcpy(szhostname, "pasgw1");
	gethostname(szhostname, 128);
#endif
	g_Queue = new std::queue<char*>();

	CTimerEx timer;
	timer.SetTimer(RUNNING_TIME);

	dns.lGetIP((char*)DOMAIN_INFO_IP);

	struct timeval st;
	char stime[32] = {0};

	int ichkCount = 0;

	while(1)
	{
		ReSend();

		//Timer ±âµ¿
                timer.cron_sync_sec();
                timer.cron_sleep();

		int cpuusage = 0;
		int memusage = 0;
		int freememory = 0;
		int freeswap = 0;
		int swapusage = 0;

		int reservedswap = 0;
		int availswap = 0;
		int phycalmem = 0;

		int iRet = 0;

		CSYSMemInfo sysMEMInfo;
		iRet = sysMEMInfo.GetSysMemoryInfo(phycalmem, freememory, reservedswap, swapusage, availswap, freeswap);
	
		if ( iRet != 1 )
		{
			continue;
		}
		
		memusage = phycalmem - freememory;

		int idle = 0;
		int user = 0;
		int kernel = 0;
		int wait = 0;

		CSYSCPUInfo CPUInfo;
		iRet = CPUInfo.GetCPUInfo(idle, user, kernel, wait);

		if ( iRet != 1 )
		{
			continue;
		}

		cpuusage = 100 - idle;

		CHTTPAgent http;
		http.init();
		
		gettimeofday(&st, 0);
		cftime(stime, (char*)"%Y%m%d%H%M%S", &st.tv_sec);

#ifndef POST
		//System Info GET	
		sprintf(url, "GET /app/MON_PAS_CONN.ASP.asp?"
			"SDATE=%s&SERVERNAME=%s&STYPE=S&CPUUSAGE=%d&MEMUSAGE=%d&"
			"FREEMEMORY=%d&FREESWAP=%d&SWAPUSAGE=%d&STATDATE=%s"
			" HTTP/1.0\r\nHost: %s:%d\r\nAccept: */*\r\n\r\n", 
			stime, szhostname, cpuusage, memusage, freememory, freeswap, swapusage, stime, 
				DOMAIN_INFO_IP, DOMAIN_INFO_PORT);
#else
		//System Info POST	

		char body[1024] = {0};
		sprintf(body, "SDATE=%s&SERVERNAME=%s&STYPE=S&CPUUSAGE=%d&MEMUSAGE=%d&"
				"FREEMEMORY=%d&FREESWAP=%d&SWAPUSAGE=%d&STATDATE=%s",
				stime, szhostname, cpuusage, memusage, freememory, freeswap, swapusage, stime );

		sprintf(url, "POST /app/MON_PAS_CONN.asp HTTP/1.0\r\n"
				"Host: %s:%d\r\nAccept: */*\r\nContent-Length: %d\r\n"
				"Content-Type: application/x-www-form-urlencoded\r\n\r\n%s", 
				DOMAIN_INFO_IP, DOMAIN_INFO_PORT, strlen(body), body);
#endif

#ifdef DEBUG
		cout << url << endl;
#endif
		http.SetRequest((unsigned char*)url, strlen(url));
		http.Send(dns.GetIP(), DOMAIN_INFO_PORT);
		int status = http.GetStatus();
#ifdef DEBUG
		cout << status << endl;
#endif

		if ( status != 200 )
		{
			ErrorHandler(body);
		}
		
		GWInfoSend(ME_GW_1, ME_GW_TYPE, 0);
		GWInfoSend(KUN_GW_1, KUN_GW_TYPE, 0);

		//gettimeofday(&st, 0);
		cftime(stime, (char*)"%Y%m%d%H%M%S", &st.tv_sec);

		///--- file info every 10min
		int min_time = 0;
		char min_buf[32] = {0};
		cftime(min_buf, (char*)"%M", &st.tv_sec);
		min_time = atoi(min_buf);

		if ( INTERVAL_MIN != (min_time % 10) ) 
		{
			continue;
		}
		
		//File Info Send
		char szlogfilename[256] = {0};

		//ME
		cftime(szlogfilename, (char*)"passtat.%m%d.log", &st.tv_sec);
		FileInfoSend(szlogfilename, 1, 'S');

		cftime(szlogfilename, (char*)"pas.%m%d.log", &st.tv_sec);
		FileInfoSend(szlogfilename, 1, 'C');

		cftime(szlogfilename, (char*)"auth.%m%d.log", &st.tv_sec);
		FileInfoSend(szlogfilename, 1, 'A');

		cftime(szlogfilename, (char*)"santa.%Y%m%d.log", &st.tv_sec);
		FileInfoSend(szlogfilename, 1, 'T');

		cftime(szlogfilename, (char*)"speedup.%Y%m%d.log", &st.tv_sec);
		FileInfoSend(szlogfilename, 1, 'P');

		cftime(szlogfilename, (char*)"hashkey.%Y%m%d.log", &st.tv_sec);
		FileInfoSend(szlogfilename, 1, 'H');

		sprintf(szlogfilename, "sockerr.log");
		FileInfoSend(szlogfilename, 1, 'E');

		//KUN
                cftime(szlogfilename, (char*)"k_passtat.%m%d.log", &st.tv_sec);
                FileInfoSend(szlogfilename, 2, 'S');

                cftime(szlogfilename, (char*)"k_pas.%m%d.log", &st.tv_sec);    
                FileInfoSend(szlogfilename, 2, 'C');

                cftime(szlogfilename, (char*)"k_auth.%m%d.log", &st.tv_sec);
                FileInfoSend(szlogfilename, 2, 'A');

                cftime(szlogfilename, (char*)"k_santa.%Y%m%d.log", &st.tv_sec);
                FileInfoSend(szlogfilename, 2, 'T');

                cftime(szlogfilename, (char*)"k_hashkey.%Y%m%d.log", &st.tv_sec);
                FileInfoSend(szlogfilename, 2, 'H');

                sprintf(szlogfilename, "sockerr.log");
                FileInfoSend(szlogfilename, 2, 'E');

		
		ichkCount++;
#ifdef DEBUG
		cerr << "CHKCOUNT:" << ichkCount << endl;
#endif
	}	

	if ( g_Queue )
	{
		while (!g_Queue->empty())
    		{
        		char *p = g_Queue->front();
       			g_Queue->pop();
			delete[] p;
		}

		delete g_Queue;
		g_Queue = NULL;
	}

	return 0;
}

int GWInfoSend(const char* gwname, char Type, int iSEQ)
{
	char url[8196] = {0};

	CHTTPAgent http;
        http.init();

	struct timeval st;
        char stime[32] = {0};
	
        gettimeofday(&st, 0);
        cftime(stime, (char*)"%Y%m%d%H%M%S", &st.tv_sec);
		
	int cpuinfo = 0;
	int memoryinfo = 0;

	CGWInfo gwinfo;
	gwinfo.GetGWInfo(gwname, cpuinfo, memoryinfo);

#ifndef POST
        //System Info GET
        sprintf(url, "GET /app/MON_PAS_CONN.asp?"
                 "SDATE=%s&SERVERNAME=%s&STYPE=%c&CPUUSAGE=%d&MEMUSAGE=%d&"
                 "FREEMEMORY=0&FREESWAP=0&SWAPUSAGE=0&STATDATE=%s"
                 " HTTP/1.0\r\nHost: %s:%d\r\nAccept: */*\r\n\r\n",
                	stime, szhostname, Type, cpuinfo, memoryinfo/1000, stime, DOMAIN_INFO_IP, DOMAIN_INFO_PORT);
#else
        //System Info POST

        char body[1024] = {0};
        sprintf(body, "SDATE=%s&SERVERNAME=%s&STYPE=%c&CPUUSAGE=%d&MEMUSAGE=%d&"
                      "FREEMEMORY=0&FREESWAP=0&SWAPUSAGE=0&STATDATE=%s",
			stime, szhostname, Type, cpuinfo, memoryinfo/1000, stime );

        sprintf(url, "POST /app/MON_PAS_CONN.asp HTTP/1.0\r\n"
                     "Host: %s:%d\r\nAccept: */*\r\nContent-Length: %d\r\n"
                     "Content-Type: application/x-www-form-urlencoded\r\n\r\n%s",
                      DOMAIN_INFO_IP, DOMAIN_INFO_PORT, strlen(body), body);
#endif

#ifdef DEBUG
        cerr << url << endl;
#endif
        http.SetRequest((unsigned char*)url, strlen(url));
        http.Send(dns.GetIP(), DOMAIN_INFO_PORT);
        int status = http.GetStatus();
#ifdef DEBUG
        cerr << status << endl;
#endif

	if ( status != 200 )
	{
		ErrorHandler(body); 
	}


	return 1;
}

int FileInfoSend(const char* filename, int Type, char filetype)
{
        char url[8196] = {0};

        CHTTPAgent http;
        http.init();

        struct timeval st;
        char stime[32] = {0};

        gettimeofday(&st, 0);
        cftime(stime, (char*)"%Y%m%d%H%M%S", &st.tv_sec);

	char szErrMsg[1024] = {0};
	char filecreatedate[128] = {0};
        int filesize  = 0;
        int filestatus = 0;

	char stype = ' ';
	if ( Type == 1 )
	{
		stype = 'M';
	}
	else
	{
		stype = 'K';
	}

        CFileInfo fileinfo;
        int iRet = fileinfo.GetFileInfo(filename, Type, filecreatedate, filesize, filestatus);
		
	if ( iRet != 1 )
	{
		strcpy(szErrMsg, fileinfo.GetLastError()); 
	}

#ifndef POST
        //System Info GET
        sprintf(url, "GET /app/MON_LOG_INFO.asp?"
                 "SDATE=%s&SERVERNAME=%s&STYPE=%c&FTYPE=%c&"
                 "FILENAME=%s&FILEDATE=%s&FILESIZE=%d&STATUS=%d&ERRORMSG=%s",
                 " HTTP/1.0\r\nHost: %s:%d\r\nAccept: */*\r\n\r\n",
	 stime, szhostname, stype, filetype, filename, filecreatedate, filesize, filestatus, szErrMsg, DOMAIN_INFO_IP, DOMAIN_INFO_PORT); 
#else
        //System Info POST
        char body[1024] = {0};
        sprintf(body, "SDATE=%s&SERVERNAME=%s&STYPE=%c&FTYPE=%c&"
                      "FILENAME=%s&FILEDATE=%s&FILESIZE=%d&STATUS=%d&ERRORMSG=%s",
                        stime, szhostname, stype, filetype, filename, filecreatedate, filesize, filestatus, szErrMsg);

        sprintf(url, "POST /app/MON_LOG_INFO.asp HTTP/1.0\r\n"
                     "Host: %s:%d\r\nAccept: */*\r\nContent-Length: %d\r\n"
                     "Content-Type: application/x-www-form-urlencoded\r\n\r\n%s",
                      DOMAIN_INFO_IP, DOMAIN_INFO_PORT, strlen(body), body);
#endif

#ifdef DEBUG
      	cerr << url << endl;
#endif
      	http.SetRequest((unsigned char*)url, strlen(url));
	http.Send(dns.GetIP(), DOMAIN_INFO_PORT);
   	int status = http.GetStatus();
#ifdef DEBUG
     	cerr << status << endl;
#endif

	return 1;
}

void ErrorHandler(const char* szRecord)
{
	char *p = new char[BUFF_SIZE];
	memset(p, 0x00, BUFF_SIZE);

	strncpy(p, szRecord, BUFF_SIZE-1);
	g_Queue->push(p);
#ifdef DEBUG
	cerr << g_Queue->size() << endl;
#endif
	if ( g_Queue->size() >= MAX_RECORD_COUNT )
	{
		WriteFile();
	}

	return;
}

int WriteFile()
{
	ofstream file;
	file.open(RECORD_FILE, ios::app);
	if ( file.is_open() == 0 )
	{
		return 0;
	}

	while (!g_Queue->empty())
	{	
        	char *p = g_Queue->front();
		file << p << "\n";
		delete[] p;
               	g_Queue->pop();
	}
	
	file.close();

	return 1;
}

void ReSend()
{
	ifstream file;
	file.open(RECORD_FILE, ios::in);
	if ( !file.rdbuf()->is_open())
	{
		return;
	}

	ofstream ofile;
	ofile.open(RECORD_TEMP_FILE, ios::app);
	if ( ofile.is_open() == 0 )
	{
		return;
	}	

	CHTTPAgent http;

	int bflag = 0;
	char szLine[1024] = {0};
	while (file.getline(szLine, 1024))
        {
        	http.init();
		char url[1024] = {0};
                sprintf(url, "POST /app/MON_PAS_CONN.asp HTTP/1.0\r\n"
                                "Host: %s:%d\r\nAccept: */*\r\nContent-Length: %d\r\n"
                                "Content-Type: application/x-www-form-urlencoded\r\n\r\n%s",
                                DOMAIN_INFO_IP, DOMAIN_INFO_PORT, strlen(szLine), szLine);

#ifdef DEBUG
                cout << url << endl;
#endif
                http.SetRequest((unsigned char*)url, strlen(url));
                http.Send(dns.GetIP(), DOMAIN_INFO_PORT);
                int status = http.GetStatus();			
		
		if ( status != 200 )
		{
			ofile << szLine << "\n";	
			bflag = 1;
		}
	}


	ofile.close();
	file.close();

	remove(RECORD_FILE);

	if ( bflag == 1 )
	{
		rename(RECORD_TEMP_FILE, RECORD_FILE);
	}
	else
	{
		remove(RECORD_TEMP_FILE);
	}
	
	return;
}
