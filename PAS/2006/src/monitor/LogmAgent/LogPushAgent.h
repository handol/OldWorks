//////////////////////////////////////////////////////////////////////
//
// LogPushAgent.h: interface for the CLogPushAgent class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(LOGPUSHAGENT_H__1F1F52DE_4EAA_4348_AC25_F0D3FE887EBB__INCLUDED_)
#define LOGPUSHAGENT_H__1F1F52DE_4EAA_4348_AC25_F0D3FE887EBB__INCLUDED_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#include "commondef.h"

class CLogPushAgent  
{
private:
	int SearchFile(const char* path, const char* searchfile, char *filename); 
        char m_ConnectIP[32];
        unsigned short m_ConnectPort;
        int m_interval;
public:
	int GetUDRFileCheck(int igwType);		
	int GetSRCUDRFileCheck(int igwType);		
	int GetOver10FileCheck(int igwType);

        int StartServer(const char *domain, const unsigned short port, int interval);
        static void* thrAgent(void *param);

	CLogPushAgent();
	virtual ~CLogPushAgent();
};

#endif // !defined(LOGPUSHAGENT_H__1F1F52DE_4EAA_4348_AC25_F0D3FE887EBB__INCLUDED_)

