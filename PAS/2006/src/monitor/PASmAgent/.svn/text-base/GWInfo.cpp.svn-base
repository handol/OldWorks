//////////////////////////////////////////////////////////////////////
//
// GWInfo.cpp: implementation of the CGWInfo class.
//
//////////////////////////////////////////////////////////////////////
#include <sys/types.h>
#include <procfs.h>
#include <dirent.h>
#include <unistd.h>
#include <stdio.h>
#include <iostream.h>
#include <fstream.h>
#include <string.h>
#include <dirent.h>
#include <sys/errno.h>

#include "GWInfo.h"

#define PROC_DIR_INFO	"/proc/"
#define GWINFO_BUFFER_SIZE	2048

extern int errno;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGWInfo::CGWInfo()
{

}

CGWInfo::~CGWInfo()
{

}



int CGWInfo::GetGWInfo(const char* szProc, int &icpu, int &imemory)
{
	uid_t mypid = ::getuid();
#ifdef DEBUG
        printf("id:%d\n", mypid);
#endif
	icpu = -1;
	imemory = -1;

	int iRet = 0;
      	DIR* dir;
        struct dirent *pdirent;
        dir = opendir(PROC_DIR_INFO);

        if ( dir == NULL )
        {
#ifdef DEBUG
                cout << "dir open error:" << errno << endl;
#endif
                return iRet;
        }

        while((pdirent = readdir(dir)) != NULL)
        {
                if(pdirent->d_name[0] == '.')
		{
                        continue;
		}

                char buf[GWINFO_BUFFER_SIZE] = {0};
                sprintf(buf, "%s%s%s", PROC_DIR_INFO, pdirent->d_name, "/psinfo");

                psinfo_t psinfo;
                ifstream file(buf);

                //chk for error in opening
                char buff[GWINFO_BUFFER_SIZE] = {0};
                file.read(buff, 336);
                memcpy((char*)&psinfo, buff+1, 336);

                if ( psinfo.pr_uid == mypid )
                {
                        float f = psinfo.pr_pctcpu * (100.0 / (float)0x8000);
                        int cpuinfo = f;
#ifdef DEBUG
                        printf("%s\t",  pdirent->d_name);
                        printf("%s\t", psinfo.pr_psargs);
                        printf("%d\t", psinfo.pr_nlwp);
                        printf("%d\t", psinfo.pr_pid);
                        printf("%d\t", psinfo.pr_ppid);
                        printf("%d\t", psinfo.pr_pgid);
                        printf("%d\t", psinfo.pr_sid);
                        printf("%d\n", psinfo.pr_uid);
                        printf("%d\t", cpuinfo);
                        printf("%d\n", psinfo.pr_size/1000);
#endif
#ifdef DEBUG
			cout << psinfo.pr_psargs << ":" << szProc << ";" <<  endl;
#endif
			if ( !strcmp(szProc, psinfo.pr_psargs) )
			{
#ifdef DEBUG
			cout << "Get It+" << endl;
#endif
				icpu = cpuinfo;
				imemory = psinfo.pr_size;
				iRet = 1;
                		file.close();
				break;
			}

                }

                file.close();
        }
	
	closedir(dir);

	return iRet;
}
