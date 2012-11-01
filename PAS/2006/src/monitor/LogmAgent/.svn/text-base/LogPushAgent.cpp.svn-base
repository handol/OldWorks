//////////////////////////////////////////////////////////////////////
//
// LogPushAgent.cpp: implementation of the CLogPushAgent class.
//
//////////////////////////////////////////////////////////////////////

#include "LogPushAgent.h"
#include <dirent.h>
#include "TimerEx.h"

#include "HTTPAgent.h"
#include "HTTPParser.h"
#include "FileInfo.h"
#include "UDRChecker.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLogPushAgent::CLogPushAgent()
{

}

CLogPushAgent::~CLogPushAgent()
{

}

int CLogPushAgent::SearchFile(const char* path, const char* searchfile, char *filename)
{
        DIR* dir;
        struct dirent *pdirent;
        dir = opendir(path);
	int iRet = 0;
	
	while((pdirent = readdir(dir)) != NULL)
	{
	        if(pdirent->d_name[0] == '.')
	        {
	                continue;
	        }
	
                if (strstr(pdirent->d_name, searchfile) && strstr(pdirent->d_name, ".sent") != NULL)
                {
#ifdef DEBUG
 	               cout << pdirent->d_name << endl;
#endif                
                        strncpy(filename, pdirent->d_name, 64);
			iRet = 1;
                }
        }
	
	closedir(dir);

	return iRet;
}

int CLogPushAgent::StartServer(const char *domain, const unsigned short port, int interval)
{
	pthread_t       thread;
	pthread_attr_t  attr;
	int status = 0;

        strcpy(m_ConnectIP, domain);
        m_ConnectPort = port;
        m_interval = interval;

    	pthread_attr_init(&attr);

    	if ( (status=pthread_create(&thread, &attr, thrAgent, (void*)this), NULL) != 0 ) 
	{
        	exit(0);
    	}

	return 1; /*TRUE*/
}

void* CLogPushAgent::thrAgent(void *param)
{
	CLogPushAgent* pushAgent = (CLogPushAgent*)&param;
	
	CTimerEx timer;
#ifdef DEBUG
	timer.SetTimer(RUNNING_TIME_MIN);	
	//timer.SetTimer(RUNNING_TIME_SEC);	
#else
	timer.SetTimer(RUNNING_TIME_MIN);	
#endif

	while (1)
	{
#ifdef DEBUG
                //timer.cron_sync_sec();
                timer.cron_sync_min();
#else
                timer.cron_sync_min();
#endif
                timer.cron_sleep();

		pushAgent->GetOver10FileCheck(ME_TYPE);
		pushAgent->GetOver10FileCheck(KUN_TYPE);

		pushAgent->GetSRCUDRFileCheck(ME_TYPE);
		pushAgent->GetSRCUDRFileCheck(KUN_TYPE);
		
		pushAgent->GetUDRFileCheck(ME_TYPE);	
		pushAgent->GetUDRFileCheck(KUN_TYPE);	
	}

	return NULL;	//never return
}

int CLogPushAgent::GetUDRFileCheck(int igwType)
{
        char url[8196] = {0};

        CHTTPAgent http;

        struct timeval st;
        char stime[32] = {0};

        gettimeofday(&st, 0);
        cftime(stime, (char*)"%Y%m%d%H%M%S", &st.tv_sec);

        char filetime[32] = {0};
        st.tv_sec -= UDR_CHECK_INTERVAL;
        cftime(filetime, (char*)"%Y%m%d%H%M00", &st.tv_sec);
        filetime[11] = '0';

        for ( int i = 0; i < PAS_GW_COUNT; i++ )
        {
                char filename[1024] = {0};
                char filepath[1024] = {0};

                char szErrMsg[1024] = {0};
                char filecreatedate[128] = {0};

                int filesize  = 0;
                int filestatus = 0;
		int formatstatus = 0;

                http.init();
                char stype = ' ';

                if ( igwType == 1 )
                {
                        stype = 'M';
                        sprintf(filepath, "%s", ME_UDR_FILE_PATH);
                }
                else
                {
                        stype = 'K';
                        sprintf(filepath, "%s", KUN_UDR_FILE_PATH);
                }

		CUDRChecker udr;
		filestatus = 0;
		char buf[16] = {0};
		sprintf(buf, "%d", i);
		int iRet = udr.GetUDRFileList(filepath, buf, stype, filetime, filename, filesize, filecreatedate);

#ifdef DEBUG
	cerr << "filepath:" << filepath << "-" << "filetime:" << filetime << "-" << "filename:" << filename << endl;
#endif
                if ( iRet != 1 )
                {
			filestatus = -1;
                        strcpy(szErrMsg, udr.GetLastError());
                }
		else
		{
			char szfullpath[1024] = {0};
			sprintf(szfullpath, "%s%s", filepath, filename);
			iRet = udr.SetFileName(szfullpath);
			int tot = 0, success = 0, fail = 0; 
			iRet = udr.GetValidate(tot, success, fail);

                	if ( iRet != 1 )
                	{
				formatstatus = -2;
                        	strcpy(szErrMsg, udr.GetLastError());
                	}
#ifdef DEBUG
	cerr << szfullpath << endl;
#endif
		}

#ifdef DEBUG
        cerr << iRet << endl;
#endif

		
#ifndef POST
                //System Info GET
                sprintf(url, "GET /app/MON_UDR_INFO.asp?"
                        "SDATE=%s&SERVERNAME=pasgw%d&STYPE=%c&FTYPE=%c&"
                        "FILENAME=%s&FILEDATE=%s&FILESIZE=%d&STATUS=%d&STATUS2=%d&ERRORMSG=%s"
                        " HTTP/1.0\r\nHost: %s:%d\r\nAccept: */*\r\n\r\n",
         stime, (i+1), stype, 'U', filename, filecreatedate, filesize, filestatus, formatstatus, szErrMsg, MONITOR_WEB_IP, MONITOR_WEB_PORT);
#else
                //System Info POST
                char body[1024] = {0};
                sprintf(body, "SDATE=%s&SERVERNAME=pasgw%d&STYPE=%c&FTYPE=%c&"
                      "FILENAME=%s&FILEDATE=%s&FILESIZE=%d&STATUS=%d&STATUS2=%d&ERRORMSG=%s",
                        stime, (i+1), stype, 'U', filename, filecreatedate, filesize, filestatus, formatstatus, szErrMsg);

                sprintf(url, "POST /app/MON_UDR_INFO.asp HTTP/1.0\r\n"
                     "Host: %s:%d\r\nAccept: */*\r\nContent-Length: %d\r\n"
                     "Content-Type: application/x-www-form-urlencoded\r\n\r\n%s",
                      MONITOR_WEB_IP, MONITOR_WEB_PORT, strlen(body), body);
#endif


#ifdef DEBUG
                cerr << url << endl;
#endif
                http.SetRequest((unsigned char*)url, strlen(url));
                http.Send(MONITOR_WEB_IP, MONITOR_WEB_PORT);
                int status = http.GetStatus();

#ifdef DEBUG
                cerr << status << endl;
#endif
        }

	return 1;
}

int CLogPushAgent::GetSRCUDRFileCheck(int igwType)
{
        char url[8196] = {0};

        CHTTPAgent http;

        struct timeval st;
        char stime[32] = {0};

        gettimeofday(&st, 0);
        cftime(stime, (char*)"%Y%m%d%H%M%S", &st.tv_sec);

        char filetime[32] = {0};
	st.tv_sec -= UDR_CHECK_INTERVAL;
        cftime(filetime, (char*)"%Y%m%d%H%M", &st.tv_sec);
	filetime[11] = '0';

        for ( int i = 1; i <= PAS_GW_COUNT; i++ )
        {
                char filename[1024] = {0};
		char srcfilename[1024] = {0};
                char filepath[1024] = {0};
		char buf[128] = {0};

                char szErrMsg[1024] = {0};
                char filecreatedate[128] = {0};

                int filesize  = 0;
                int filestatus = 0;

                http.init();
                char stype = ' ';

		int iRet = 0;

                if ( igwType == 1 )
                {
                        stype = 'M';
                        sprintf(filepath, "%s", ME_UDR_FILE_PATH);
                        sprintf(filename, "a_n_pasidr.%s.log.pasgw%d", filetime, i);
			strcpy(srcfilename, filename);
			iRet = SearchFile(ME_UDR_FILE_PATH, filename, buf);
			strncpy(filename, buf, 64);
                }
                else
                {
                        stype = 'K';
                        sprintf(filepath, "%s", KUN_UDR_FILE_PATH);
                        sprintf(filename, "k_n_pasidr.%s.log.pasgw%d", filetime, i);
			strcpy(srcfilename, filename);
			iRet = SearchFile(KUN_UDR_FILE_PATH, filename, buf);
			strncpy(filename, buf, 64);
                }


#ifdef DEBUG
        cerr << filepath << endl;
        cerr << filename << endl;
        cerr << filepath << "-" << filename << "-" << igwType;
#endif

		if ( iRet ) 
		{
                	CFileInfo fileinfo;
                	int bRet = fileinfo.GetFileInfo(filepath, filename, igwType, filecreatedate, filesize, filestatus);

                	if ( bRet != 1 )
                	{
                        	strcpy(szErrMsg, fileinfo.GetLastError());
                	}
#ifdef DEBUG
        cerr << iRet << endl;
#endif
		}
		else
		{
			filestatus = -1;
			sprintf(szErrMsg, "%s-%s", srcfilename, "FILE_NOT_FOUND");
		}

#ifndef POST
                //System Info GET
                sprintf(url, "GET /app/MON_UDR_INFO.asp?"
                        "SDATE=%s&SERVERNAME=pasgw%d&STYPE=%c&FTYPE=%c&"
                        "FILENAME=%s&FILEDATE=%s&FILESIZE=%d&STATUS=%d&ERRORMSG=%s"
                        " HTTP/1.0\r\nHost: %s:%d\r\nAccept: */*\r\n\r\n",
         stime, i, stype, 'I', filename, filecreatedate, filesize, filestatus, szErrMsg, MONITOR_WEB_IP, MONITOR_WEB_PORT);
#else
                //System Info POST
                char body[1024] = {0};
                sprintf(body, "SDATE=%s&SERVERNAME=pasgw%d&STYPE=%c&FTYPE=%c&"
                      "FILENAME=%s&FILEDATE=%s&FILESIZE=%d&STATUS=%d&ERRORMSG=%s",
                        stime, i, stype, 'I', filename, filecreatedate, filesize, filestatus, szErrMsg);

                sprintf(url, "POST /app/MON_UDR_INFO.asp HTTP/1.0\r\n"
                     "Host: %s:%d\r\nAccept: */*\r\nContent-Length: %d\r\n"
                     "Content-Type: application/x-www-form-urlencoded\r\n\r\n%s",
                      MONITOR_WEB_IP, MONITOR_WEB_PORT, strlen(body), body);
#endif


#ifdef DEBUG
                cerr << url << endl;
#endif
                http.SetRequest((unsigned char*)url, strlen(url));
                http.Send(MONITOR_WEB_IP, MONITOR_WEB_PORT);
                int status = http.GetStatus();

#ifdef DEBUG
                cerr << status << endl;
#endif
        }

        return 1;
}

int CLogPushAgent::GetOver10FileCheck(int igwType)
{
        char url[8196] = {0};

        CHTTPAgent http;

        struct timeval st;
        char stime[32] = {0};

        gettimeofday(&st, 0);
        cftime(stime, (char*)"%Y%m%d%H%M%S", &st.tv_sec);

	char filetime[32] = {0};
        cftime(filetime, (char*)"%Y%m%d%H", &st.tv_sec);


	for ( int i = 1; i <= PAS_GW_COUNT; i++ )
	{
		char filename[1024] = {0};
		char filepath[1024] = {0};

        	char szErrMsg[1024] = {0};
        	char filecreatedate[128] = {0};

        	int filesize  = 0;
        	int filestatus = 0;

        	http.init();
        	char stype = ' ';

        	if ( igwType == 1 )
        	{
                	stype = 'M';
			sprintf(filepath, "%s", ME_OVER10_FILE_PATH);
			sprintf(filename, "over10.%s.log.pasgw%d", filetime, i);
        	}
        	else
        	{
                	stype = 'K';
			sprintf(filepath, "%s", KUN_OVER10_FILE_PATH);
			sprintf(filename, "k_over10.%s.log.pasgw%d", filetime, i);
        	}
#ifdef DEBUG
	cerr << filepath << endl;
	cerr << filename << endl;
	cerr << filepath << "-" << filename << "-" << igwType;
#endif
        	CFileInfo fileinfo;
        	int iRet = fileinfo.GetFileInfo(filepath, filename, igwType, filecreatedate, filesize, filestatus);

#ifdef DEBUG
	cerr << iRet << endl;
#endif
        	if ( iRet != 1 )
        	{
                	strcpy(szErrMsg, fileinfo.GetLastError());
        	}
#ifndef POST
        	//System Info GET
        	sprintf(url, "GET /app/MON_LOG_INFO.asp?"
                 	"SDATE=%s&SERVERNAME=pasgw%d&STYPE=%c&FTYPE=%c&"
                 	"FILENAME=%s&FILEDATE=%s&FILESIZE=%d&STATUS=%d&ERRORMSG=%s"
                 	" HTTP/1.0\r\nHost: %s:%d\r\nAccept: */*\r\n\r\n",
         stime, i, stype, 'O', filename, filecreatedate, filesize, filestatus, szErrMsg, MONITOR_WEB_IP, MONITOR_WEB_PORT);
#else
        	//System Info POST
        	char body[1024] = {0};
        	sprintf(body, "SDATE=%s&SERVERNAME=pasgw%d&STYPE=%c&FTYPE=%c&"
                      "FILENAME=%s&FILEDATE=%s&FILESIZE=%d&STATUS=%d&ERRORMSG=%s",
                        stime, i, stype, 'O', filename, filecreatedate, filesize, filestatus, szErrMsg);

        	sprintf(url, "POST /app/MON_LOG_INFO.asp HTTP/1.0\r\n"
                     "Host: %s:%d\r\nAccept: */*\r\nContent-Length: %d\r\n"
                     "Content-Type: application/x-www-form-urlencoded\r\n\r\n%s",
                      MONITOR_WEB_IP, MONITOR_WEB_PORT, strlen(body), body);
#endif


#ifdef DEBUG
        	cerr << url << endl;
#endif
        	http.SetRequest((unsigned char*)url, strlen(url));
        	http.Send(MONITOR_WEB_IP, MONITOR_WEB_PORT);
        	int status = http.GetStatus();

#ifdef DEBUG
        	cerr << status << endl;
#endif
	}

	return 1;
}
