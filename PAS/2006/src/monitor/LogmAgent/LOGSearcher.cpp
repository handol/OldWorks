//////////////////////////////////////////////////////////////////////
//
// LogSearcher.cpp: implementation of the CLogSearcher class.
//
//////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <strings.h>
#include <iostream.h>
#include <sys/stat.h>
#include <dirent.h>

#include "LOGSearcher.h"
#include "commondef.h"

#define LIST_DATA_SIZE	1024 * 1024 * 4
#define DATA_LINE_COUNT 1024 * 8
#define DATA_BUFFER_SIZE (DATA_LINE_COUNT * (MAX_LOG_BUFFUER_SIZE + 1024))

#define XML_PI	"<xml xmlns:s='uuid:BDC6E3F0-6DA3-11d1-A2A3-00AA00C14882' xmlns:dt='uuid:C2F41010-65B3-11d1-A29F-00AA00C14882' " \
		" xmlns:rs='urn:schemas-microsoft-com:rowset' xmlns:z='#RowsetSchema'>\n" \
		"<s:Schema id='RowsetSchema'>\n" \
		"<s:ElementType name='row' content='eltOnly'>\n" \
		"<s:AttributeType name='NUM' rs:number='1' rs:writeunknown='true'>\n" \
		"<s:datatype dt:type='number' rs:dbtype='numeric' dt:maxLength='19' rs:scale='0' rs:precision='38' " \
		"rs:fixedlength='true' rs:maybenull='false'/>\n" \
		"</s:AttributeType>\n" \
		"<s:AttributeType name='PATH' rs:number='2' rs:writeunknown='true'>\n" \
		"<s:datatype dt:type='string' rs:dbtype='str' dt:maxLength='256' rs:maybenull='false'/>\n" \
		"</s:AttributeType>\n" \
		"<s:AttributeType name='FILENAME' rs:number='3' rs:writeunknown='true'>\n" \
		"<s:datatype dt:type='string' rs:dbtype='str' dt:maxLength='256' rs:maybenull='false'/>\n" \
		"</s:AttributeType>\n" \
		"<s:extends type='rs:rowbase'/>\n" \
		"</s:ElementType>\n" \
		"</s:Schema>\n"



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

extern int errno;

CLogSearcher::CLogSearcher()
{
	m_stLogData = new STLogData[DATA_LINE_COUNT];	
	m_Data = new char[LIST_DATA_SIZE];
	m_LogLine = 0;
	m_DataCount = 0;
}

CLogSearcher::~CLogSearcher()
{
	if ( m_stLogData )
	{
		delete[] m_stLogData;
		m_stLogData = NULL;
	}

	if ( m_Data )
	{
		delete[] m_Data;
		m_Data = NULL;
	}
}

int CLogSearcher::init()
{
	memset(m_Data, 0x00, LIST_DATA_SIZE);	
	m_DataCount = 0;

	for ( int i = 0; i < DATA_LINE_COUNT; i++ )
	{
		m_stLogData[i].init();
	}

	return 1;
}

int CLogSearcher::GetPOpen(const char* szCommand)
{
	FILE *fp = NULL;
	char szLine[MAX_LOG_BUFFUER_SIZE];
	m_LogLine = 0;

#ifdef DEBUG
        	cout << szCommand << endl;
#endif
	if ((fp=popen(szCommand, "rt")) == NULL) 
	{
#ifdef DEBUG
	cerr << "open error" << endl;
#endif
		return 0;
	}


	int iLineCount = 0;

    	while ( fgets(szLine, MAX_LOG_BUFFUER_SIZE-1, fp) && DATA_LINE_COUNT > iLineCount  ) 
	{
#ifdef DEBUG
        	cout << szLine;
#endif
		if ( !strncmp(szLine, "찾지", strlen("찾지")) )
		{
			continue;
		}

		strncpy(m_stLogData[iLineCount].data, szLine, MAX_LOG_BUFFUER_SIZE-1);
		iLineCount++;
    	}

	m_LogLine = iLineCount;
    
	pclose(fp);		
	return 1;
}

int CLogSearcher::GetList(STLogData* &pLogData, int &ListCount)
{
	ListCount = m_LogLine;
	pLogData = m_stLogData; 

	return 1;
}

int CLogSearcher::GetFileListQuery(int iType, char fType)
{
	char filepath[1024] = {0};
	char szcommand[4096] = {0};

	if ( fType == 'R' )
	{
		if ( iType == ME_TYPE )
		{
			sprintf(filepath, "%sN_SWPAS*.sent", ME_UDR_FILE_PATH);
		}
		else
		{
			sprintf(filepath, "%sN_SWKAS*.sent", KUN_UDR_FILE_PATH);
		}
	
	}
	else if ( fType == 'I' )
	{
		if ( iType == ME_TYPE )
		{
			sprintf(filepath, "%sa_n_pasidr*.sent", ME_UDR_FILE_PATH);
		}
		else
		{
			sprintf(filepath, "%sk_n_pasidr*.sent", KUN_UDR_FILE_PATH);
		}
	}
	else if ( fType == 'S' )
	{
		if ( iType == ME_TYPE )
		{
			sprintf(filepath, "%spasstat.*", ME_STAT_FILE_PATH);
		}
		else
		{
			sprintf(filepath, "%sk_passtat.*", KUN_STAT_FILE_PATH);
		}		
	}
	else
	{
		return -1;
	}

	sprintf(szcommand, "ls -Rl %s | grep \"N_SW*\"", filepath);
#ifdef DEBUG
	cerr << szcommand << endl;
#endif 
	GetPOpen(szcommand);
	
	return 1;
}

int CLogSearcher::GetFileContentQuery(int iType, char fType, const char* szfilelist, const char* szSearchString)
{
        char filepath[1024] = {0};
        char szcommand[4096] = {0};

        if ( fType == 'U' )
        {
                if ( iType == ME_TYPE )
                {
                        sprintf(filepath, "%s%s", ME_UDR_FILE_PATH, szfilelist);
                }
                else
                {
                        sprintf(filepath, "%s%s", KUN_UDR_FILE_PATH, szfilelist);
                }

        }
        else if ( fType == 'I' )
        {
                if ( iType == ME_TYPE )
                {
                        sprintf(filepath, "%s%s", ME_UDR_FILE_PATH, szfilelist);
                }
                else
                {
                        sprintf(filepath, "%s%s", KUN_UDR_FILE_PATH, szfilelist);
                }
        }
        else if ( fType == 'S' )
        {
                if ( iType == ME_TYPE )
                {
                        sprintf(filepath, "%s%s", ME_STAT_FILE_PATH, szfilelist);
                }
                else
                {
                        sprintf(filepath, "%s%s", KUN_STAT_FILE_PATH, szfilelist);
                }
        }
        else
        {
                return -1;
        }

        sprintf(szcommand, "ls -Rl %s | grep \"N_SW*\"", filepath);
#ifdef DEBUG
        cerr << szcommand << endl;
#endif
        GetPOpen(szcommand);		
}

int CLogSearcher::GetFileListQuery2(int iType, char fType)
{
	m_DataCount = 0;

	if ( iType == ME_TYPE)
	{
		if ( fType == FILE_TYPE_UDR )
		{
			searchdir(ME_UDR_FILE_PATH, fType);
		}
		else if ( fType == FILE_TYPE_SRCUDR )
		{
			searchdir(ME_UDR_FILE_PATH, fType);
		}
		else if ( fType == FILE_TYPE_STAT )
		{
			searchdir(ME_STAT_FILE_PATH, fType);
		}
		else
		{
			return -1;
		}
	}
	else
	{
                if ( fType == FILE_TYPE_UDR )
                {
                        searchdir(KUN_UDR_FILE_PATH, fType);
                }
                else if ( fType == FILE_TYPE_SRCUDR )
                {
                        searchdir(KUN_UDR_FILE_PATH, fType);
                }
                else if ( fType == FILE_TYPE_STAT )
                {
                        searchdir(KUN_STAT_FILE_PATH, fType);
                }
                else
                {
                        return -1;
                }			
	}

	if ( m_DataCount <= 0 )
	{
		return -1;
	}

	strcpy(m_Data, XML_PI);
	strcat(m_Data, "<rs:data>\n"); 

	for ( int i = 0; i < m_DataCount; i++ )
	{
#ifdef DEBUG
		cout << m_stLogData[i].path << "--->" << m_stLogData[i].data << endl;
#endif

		char buff[1024] = {0};
		sprintf(buff, "<z:row NUM='%d' PATH='%s' FILENAME='%s' />\n", i+1, m_stLogData[i].path, m_stLogData[i].data);
		strcat(m_Data, buff);
	}

	strcat(m_Data, "</rs:data>\n"); 
	strcat(m_Data, "</xml>");


#ifdef DEBUG
		cout << m_Data << endl;
#endif
	return 0;

}

int CLogSearcher::searchdir(const char *szPath, char fType)
{
        int iRet = 0;

        DIR* dir;
        struct dirent *pdirent;

        dir = opendir(szPath);

        if ( dir == NULL )
        {
#ifdef DEBUG
                cout << "dir open error:" << errno << endl;
#endif
                return iRet;
        }

        char filepath[128] = {0};
        struct stat64 filestat;

#ifdef DEBUG
        cout << "dir open ok" << endl;
#endif

        while((pdirent = readdir(dir)) != NULL)
        {
                if(pdirent->d_name[0] == '.')
                {
                        continue;
                }

                sprintf(filepath, "%s/%s", szPath, pdirent->d_name);

                int ret = stat64(filepath, &filestat);

                if ( ret == -1 )
                {
#ifdef DEBUG
        cout << "stat error:" << errno << endl;
        cout << filepath << endl;
#endif
                        closedir(dir);
                        return ret;
                }

                if ( S_ISDIR(filestat.st_mode) )
                {
#ifdef DEBUG
                        //cout << filepath << ":" << filestat.st_mode <<  endl;
#endif
			if ( fType != FILE_TYPE_STAT )
			{
                        	this->searchdir(filepath, fType);
			}

                        iRet = 1;
                }
                else
                {
			if ( fType == FILE_TYPE_UDR )
			{
                        	if ( strstr(filepath, ".sent.Z") != NULL && strstr(filepath, "FWEBIDR1") != NULL )
				{
#ifdef DEBUG
                                	cout << filepath << ":" << filestat.st_mode <<  endl;
#endif
					char *p = filepath;
					int count = 0;
					int pos = -1;

					while( *p != NULL )
					{
						if ( *p == '/' )
						{
							pos = count;
						}

						p++;
						count++;	
					}
					m_stLogData[m_DataCount].init();
					strncpy(m_stLogData[m_DataCount].path, filepath, pos+1);
					strncpy(m_stLogData[m_DataCount].data, filepath+pos+1, 128);
					m_DataCount++;
				}
				else
				{
					continue;
				}
			}
			else if ( fType == FILE_TYPE_SRCUDR )
			{
                                if ( strstr(filepath, "pasidr") != NULL && strstr(filepath, ".sent.Z") != NULL )
                                {
#ifdef DEBUG
                                        cout << filepath << ":" << filestat.st_mode <<  endl;
#endif
                                        char *p = filepath;
                                        int count = 0;
                                        int pos = -1;

                                        while( *p != NULL )
                                        {
                                                if ( *p == '/' )
                                                {
                                                        pos = count;
                                                }

                                                p++;
                                                count++;
                                        }
                                        m_stLogData[m_DataCount].init();
                                        strncpy(m_stLogData[m_DataCount].path, filepath, pos+1);
                                        strncpy(m_stLogData[m_DataCount].data, filepath+pos+1, 128);
                                        m_DataCount++;
                                }
                                else
                                {
                                        continue;
                                }

			}
			else if ( fType == FILE_TYPE_STAT )
			{
#ifdef DEBUG
                        //cout << filepath << ":" << filepath <<  endl;
#endif
                                if ( strstr(filepath, "passtat.") != NULL && strstr(filepath, ".Z") != NULL ) 
                                {
#ifdef DEBUG
                                        cout << filepath << ":" << filestat.st_mode <<  endl;
#endif
                                        char *p = filepath;
                                        int count = 0;
                                        int pos = -1;

                                        while( *p != NULL )
                                        {
                                                if ( *p == '/' )
                                                {
                                                        pos = count;
                                                }

                                                p++;
                                                count++;
                                        }
                                        m_stLogData[m_DataCount].init();
                                        strncpy(m_stLogData[m_DataCount].path, filepath, pos+1);
                                        strncpy(m_stLogData[m_DataCount].data, filepath+pos+1, 128);
                                        m_DataCount++;
                                }
                                else
                                {
                                        continue;
                                }

			}
			else
			{
				continue;
			}
                }
        }

        closedir(dir);

        return iRet;

}

char* CLogSearcher::GetList()
{

	return m_Data;
}

/*
int main()
{
	CLogSearcher log;
	log.init();
	log.GetFileListQuery2(2, 'S');
	
	return 0;
}

*/
