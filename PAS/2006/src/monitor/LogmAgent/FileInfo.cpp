//////////////////////////////////////////////////////////////////////
//
// FileInfo.cpp: implementation of the CFileInfo class.
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

#include "commondef.h"
#include "FileInfo.h"

#define ME_TYPE	1
#define KUN_TYPE	2

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFileInfo::CFileInfo()
{
	memset(m_szGetLastError, 0x00, sizeof(m_szGetLastError));
}

CFileInfo::~CFileInfo()
{

}

char* CFileInfo::GetLastError()
{
	return m_szGetLastError;
}

CFileInfo::GetFileInfo(const char* szFilePath, const char* szFileName, int iType,  char* szCreateDate, int &iFileSize, int &iStatus)
{
        int iRet = 0;

        DIR* dir;
        struct dirent *pdirent;

	if ( iType == ME_TYPE )
	{
        	dir = opendir(szFilePath);
	}
	else
	{
		dir = opendir(szFilePath);
	}

        if ( dir == NULL )
        {
#ifdef DEBUG
                cout << "dir open error:" << errno << endl;
#endif
                iStatus = -1;
                sprintf(m_szGetLastError, "%s-%s", szFileName, "DIR_OPEN_ERROR");
                return iRet;
        }

        char filepath[128] = {0};
        struct stat filestat;

        while((pdirent = readdir(dir)) != NULL)
        {
                if(pdirent->d_name[0] == '.')
                {
                        continue;
                }

                if ( !strcmp(pdirent->d_name, szFileName) )
                {
			if ( iType == ME_TYPE )
			{
                        	sprintf(filepath, "%s%s", szFilePath, pdirent->d_name);
			}
			else
			{
				sprintf(filepath, "%s%s", szFilePath, pdirent->d_name);
			}

                        int ret = stat(filepath, &filestat);

			if ( ret == -1 )
			{
				closedir(dir);
				sprintf(m_szGetLastError, "%s-%d", szFileName, errno);
				return ret;
			}

			iRet = 1;
                        break;
                }
        }

	closedir(dir);

	if ( iRet != 1 )
	{
		iStatus = -1;
		sprintf(m_szGetLastError, "%s-%s", szFileName, "File Not Found");
		return iRet;
	}

        struct tm  *tm;
        tm = localtime(&filestat.st_mtime);
        char buf[128] = {0};
        sprintf(buf, "%04d%02d%02d%02d%02d%02d", tm->tm_year+1900, tm->tm_mon+1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);

#ifdef DEBUG
        cerr << filestat.st_size << endl;
        cerr << filestat.st_uid << endl;
        cerr << buf << endl;
#endif

	strcpy(szCreateDate, buf);
	iFileSize = filestat.st_size;
	iStatus = 0;

	return iRet;
}

