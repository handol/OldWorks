//////////////////////////////////////////////////////////////////////
//
// UDRChecker.cpp: implementation of the CUDRChecker class.
//
//////////////////////////////////////////////////////////////////////
#include <sys/types.h>
#include <sys/stat.h>
#include <procfs.h>
#include <dirent.h>
#include <unistd.h>
#include <stdio.h>
#include <iostream.h>
#include <fstream.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <stdlib.h>

#include "UDRChecker.h"

#define UDR_HEADER_SIZE 120
#define UDR_TAIL_SIZE 	120

#define MAX_LINE_BUFFER	8196
#define CLEAR(x) { memset(x, 0x00, sizeof(x)); }

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUDRChecker::CUDRChecker()
{
	memset(m_szGetLastError, 0x00, 256);
	memset(m_szFileName, 0x00, 256);

        m_TotalCount = 0;
        m_SuccessCount = 0;
        m_ErrorCount = 0;
}

CUDRChecker::~CUDRChecker()
{

}

char* CUDRChecker::GetLastError()
{
	return m_szGetLastError;
}

int CUDRChecker::FieldCheck(char *szLine, int sPos, int Size)
{
	char buf[MAX_LINE_BUFFER] = {0};
	strncpy(buf, szLine+sPos, Size);

	if ( buf[0] == '\0' || buf[0] == ' ' )
	{
#ifdef DEBUG
		cout << buf << endl;
#endif
		m_ErrorCount++;
		sprintf(m_szGetLastError, "%s-%s-%d", m_szFileName, "BODY RECORD FIELD ERROR", sPos );	
		return 0;
	} 
		
	return 1;
}

int CUDRChecker::FileSize()
{
	struct stat filestat;
	int ret = stat(m_szFileName, &filestat);

	if ( ret != 0 )
	{
		return -1;
	}
	
	return filestat.st_size;
}

int CUDRChecker::GetValidate(int &iTot, int &iSuccess, int &iError)
{
	ifstream	in;	

	in.open(m_szFileName, ios::in | ios::nocreate);
	if (!in.rdbuf()->is_open())
	{
		iTot = 0;
		iSuccess = 0;
		iError = 1;
		sprintf(m_szGetLastError, "%s-File Open Error!!!", m_szFileName);
		return -11;
	}

        m_TotalCount = 0;
        m_SuccessCount = 0;
        m_ErrorCount = 0;

	char szLine[MAX_LINE_BUFFER] = {0};
	int iLineCount = 0;//라인 갯수

	char buf[MAX_LINE_BUFFER] = {0};//임시버퍼
	int ibuf = 0;

	int t_LineCount = 0;
	int t_FileSize = 0;

	int iFileSize = FileSize();

	while (in.getline(szLine, MAX_LINE_BUFFER)) 
	{
#ifdef DEBUG		
		cout << iLineCount << ":" << m_ErrorCount << ":" << m_szGetLastError << endl;
#endif
		iLineCount++;

		if ( szLine[0] == 'H' )
		{
			if ( strlen(szLine) + 1 != UDR_HEADER_SIZE )
			{
				m_ErrorCount++;
				sprintf(m_szGetLastError, "%s-%s", m_szFileName, "HEADER SIZE ERROR");
				continue;
			}
		}	
		else if ( szLine[0] == 'R' )
		{
                        if ( FieldCheck(szLine, 1  , 7 ) != 1 ) continue;         //record_sequence
                        if ( FieldCheck(szLine, 8  , 4 ) != 1 ) continue;         //record_length
                        if ( FieldCheck(szLine, 12 , 15) != 1 ) continue;         //G/W IP
                        //if ( FieldCheck(szLine, 27 , 15) != 1 ) continue;         //단말 IP
                        if ( FieldCheck(szLine, 42 , 1 ) != 1 ) continue;         //Service_type
                        if ( FieldCheck(szLine, 43 , 11) != 1 ) continue;         //Charging_MDN
                        if ( FieldCheck(szLine, 54 , 15) != 1 ) continue;         //Charging_MIN(IMSI)
                        if ( FieldCheck(szLine, 69 , 11) != 1 ) continue;         //Calling_MDN
                        if ( FieldCheck(szLine, 80 , 15) != 1 ) continue;         //Calling_MIN(IMSI)
                        if ( FieldCheck(szLine, 95 , 15) != 1 ) continue;         //Call_phone_type
                        if ( FieldCheck(szLine, 110, 15) != 1 ) continue;         //Call_browser_version
                        if ( FieldCheck(szLine, 125, 11) != 1 ) continue;         //Receive_MDN
                        if ( FieldCheck(szLine, 136, 15) != 1 ) continue;         //Receive_MIN(IMSI)
                        if ( FieldCheck(szLine, 151, 15) != 1 ) continue;         //Rcv_ Phone_type
                        if ( FieldCheck(szLine, 166, 15) != 1 ) continue;         //Rcv _browser_version
                        if ( FieldCheck(szLine, 181, 14) != 1 ) continue;         //Call_start_time
                        if ( FieldCheck(szLine, 195, 14) != 1 ) continue;         //Call_Response_time
                        if ( FieldCheck(szLine, 209, 9 ) != 1 ) continue;         //Duration
                        if ( FieldCheck(szLine, 218, 9 ) != 1 ) continue;         //Packet count
                        if ( FieldCheck(szLine, 227, 15) != 1 ) continue;         //Cp_ip
                        if ( FieldCheck(szLine, 242, 6 ) != 1 ) continue;         //Cp_port
                                                                                  
                        //if ( FieldCheck(szLine, 283, 20) != 1 ) continue;         //Cp_name      
                        //if ( FieldCheck(szLine, 303, 20) != 1 ) continue;         //Svc_code     
                        if ( FieldCheck(szLine, 323, 6 ) != 1 ) continue;         //Response_Code
                        if ( FieldCheck(szLine, 329, 8 ) != 1 ) continue;         //url 

			//#2
			CLEAR(buf);
			strncpy(buf, szLine+1, 7);
			ibuf = atoi(buf);

			if ( ibuf != iLineCount - 1 )
			{
				m_ErrorCount++;
                                sprintf(m_szGetLastError, "%s-%d-%d-%s", m_szFileName, ibuf, iLineCount - 1, "BODY RECORD SEQUENCE ERROR" );
                                continue;
			}

			//#3
			CLEAR(buf);
			strncpy(buf, szLine+8, 4); 		
			ibuf = atoi(buf);
			if ( ibuf != strlen(szLine)+2 )
			{
#ifdef DEBUG
				cout << ibuf << ":" << strlen(szLine) + 2 << endl;
#endif
				m_ErrorCount++;
                                sprintf(m_szGetLastError, "%s-%s", m_szFileName, "BODY RECORD SIZE ERROR" );
                                continue;
			}

                        //#7
			CLEAR(buf);
                        strncpy(buf, szLine+43, 11);
                        if ( strncmp(buf, "01", 2) )
                        {
                                m_ErrorCount++;
                                sprintf(m_szGetLastError, "%s-%s", m_szFileName, "BODY RECORD MDN ERROR" );
                                continue;
                        }
/*
                        //#?
			CLEAR(buf);
                        strncpy(buf, szLine+?, ?);
                        ibuf = atoi(buf);
                        if ( )
                        {
                                m_ErrorCount++;
                                sprintf(m_szGetLastError, "%s-%s", m_szFileName, "BODY RECORD ERROR" );
                                continue;
                        }
*/
		}	
		else if ( szLine[0] == 'T' )
		{
			if ( strlen(szLine) + 1 != UDR_TAIL_SIZE )
			{
				m_ErrorCount++;
				sprintf(m_szGetLastError, "%s-%s", m_szFileName, "TAIL SIZE ERROR" );
				continue;
			}
		
			//record count	
			CLEAR(buf);
			strncpy(buf, szLine+1, 7);
			ibuf = atoi(buf);
			t_LineCount = ibuf;

			//file size
			CLEAR(buf);
			strncpy(buf, szLine+22, 10);
			ibuf = atoi(buf);
			t_FileSize = ibuf;
		}
		else
		{
			m_ErrorCount++;	
			sprintf(m_szGetLastError, "%s-%s", m_szFileName, "INVALID RECORD PREFIX" );
		}
	}	

	//라인 카운트 체크
	if ( iLineCount != t_LineCount )
	{
		m_ErrorCount++;
                sprintf(m_szGetLastError, "%s-%s", m_szFileName, "INVALID FILE LINE COUNT");
	}

	//파일 사이즈 체크
	if ( iFileSize != t_FileSize  )
	{
		m_ErrorCount++;
                sprintf(m_szGetLastError, "%s-%s", m_szFileName, "INVALID FILE SIZE");
	}

	iTot = iLineCount;
	iError = m_ErrorCount;
	iSuccess = iTot - iError;

	m_TotalCount = iLineCount;
	m_SuccessCount = iSuccess;	

	if ( m_ErrorCount > 0 )
	{
		return -1;			
	}
	
	return 1;
}

int CUDRChecker::SetFileName(const char* szFileName)
{
	strncpy(m_szFileName, szFileName, 254);
	return 1;
}

int CUDRChecker::GetUDRFileList(const char* szPath, const char* szGWInfo, char Type, const char* szCurrentDate, char* szFileName, int &filesize, char* filecreatedate)
{
#ifdef DEBUG
	cout << szPath << endl;
#endif
	char szString1[128] = {0};	
	char szString2[128] = {0};	

	if ( Type == 'M' )
	{
		sprintf(szString1, "N_SWPAS%s_FWEBIDR1_ID", szGWInfo);
		sprintf(szString2, "T%s.DAT", szCurrentDate);
	}
	else
	{
		sprintf(szString1, "N_SWKAS%s_FWEBIDR1_ID", szGWInfo);
		sprintf(szString2, "T%s.DAT", szCurrentDate);
	}

	int iRet = 0;
	
        DIR* dir;
        struct dirent *pdirent;
 	dir = opendir(szPath);

        if ( dir == NULL )
        {
#ifdef DEBUG
                cout << "dir open error:" << errno << endl;
#endif
		sprintf(m_szGetLastError, "%s-DIR_OPEN_ERROR", szPath);
                return iRet;
        }

        char filepath[256] = {0};
        struct stat filestat;

        while((pdirent = readdir(dir)) != NULL)
        {	
                if(pdirent->d_name[0] == '.')
                {
                        continue;
                }		
#ifdef DEBUG
		//cout << pdirent->d_name << endl;
#endif
		if ( !strncmp(pdirent->d_name, szString1, strlen(szString1)) )
		{
			if (!strncmp(pdirent->d_name+25, szString2, strlen(szString2)) && strstr(pdirent->d_name, ".sent") != NULL)
			{
				iRet++;
				strncpy(szFileName, pdirent->d_name, 64);
                        
				struct stat filestat;
				char filepath[1024] = {0};
				sprintf(filepath, "%s%s", szPath, pdirent->d_name);
				int ret = stat(filepath, &filestat);

                        	if ( ret == -1 )
                        	{
					filesize = -1;
				}
				else
				{
					filesize = filestat.st_size;
					struct tm  *tm;
        				tm = localtime(&filestat.st_mtime);
					sprintf(filecreatedate, "%04d%02d%02d%02d%02d%02d", 
					tm->tm_year+1900, tm->tm_mon+1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
				}
			}
		}
	}

	if ( iRet == 0 )
	{
		sprintf(m_szGetLastError, "%s*%s-FILE_NOT_FOUND", szString1, szString2);
	}

	closedir(dir);

	return iRet;
}
/*
int main()
{
	int tot, success, error;
	CUDRChecker udr;
	udr.SetFileName("/nfs2/paslog/N_SWPAS0_FWEBIDR1_ID0086_T20040914141000.DAT.sent");
	udr.GetValidate(tot, success, error);
	cout << tot << "-" << success << "-" << error << endl;

	char szFileName[128] = {0};
	int iRet = 0;
	iRet = udr.GetUDRFileList("/nfs2/paslog/", "0", 'M', "20040914141000", szFileName);
	cout << iRet << "-" << szFileName << endl;

	return 0;	
}
*/
