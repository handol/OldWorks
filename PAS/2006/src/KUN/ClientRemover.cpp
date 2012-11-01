
#include "Common.h"
#include "Mutex.h"
#include "Util.h"
#include "Util2.h"
#include "ClientRemover.h"
#include "ClientHandler.h"
#include "WorkInfo.h"


#include <iostream>
#include <fstream>

ClientRemover *ClientRemover::oneInstance = NULL;

ClientRemover *ClientRemover::instance(ACE_Thread_Manager* threadManager)
{
	if (oneInstance == NULL) 
		oneInstance = new ClientRemover(threadManager);
		
	return oneInstance;

}


ClientRemover::ClientRemover(ACE_Thread_Manager* threadManager)
: ACE_Task<PAS_SYNCH>(threadManager)
{
}

ClientRemover::~ClientRemover(void)
{
}

int	ClientRemover::svc(void)
{
	ACE_Message_Block *mesg = 0;

	PAS_NOTICE("ClientRemover::svc start");
	PAS_INFO1("ClientRemover:: msq Q = %X", msg_queue ());

	WorkInfoMessageBlock *msgBlock = NULL;

	while(1)
	{
		int	now = time(NULL);
		if (getq(mesg) < 0)
		{
			PAS_ERROR("ClientRemover::Get Queue fail");
			return -1;
		}
		else if (mesg->length()==0) // 빈 메시지 수신 --> 종료를 의미.
		{
			break;
		}

		int msgcnt = msg_queue()->message_count();
		PAS_DEBUG1( "ClientRemover:: left jobs (%d)", msgcnt);
		
		WorkInfo workinfo;

		ACE_ASSERT(mesg->length() == sizeof(WorkInfo));
		memcpy((void*)&workinfo, (void*)mesg->rd_ptr(), sizeof(workinfo));
		ClientHandler *pClientHandler = dynamic_cast<ClientHandler*>(workinfo.pEventHandler);

		PAS_DEBUG1( "ClientRemover:: job [%X]", pClientHandler );

		if (pClientHandler==NULL)
		{
			PAS_TRACE1( "ClientRemover:: delete MB= [%X]", mesg );
			delete mesg;
		}
		else if (pClientHandler->isRemovable())
		{
			PAS_DEBUG1( "ClientRemover:: Remove OK [%X] ", pClientHandler );
			PAS_TRACE1( "ClientRemover:: delete MB= [%X]", mesg );
			// Auth, Santa 등이 모두 제거된 경우.

			delete pClientHandler;
			delete mesg;
		}
		else
		{
			// Auth, Santa 등이 남아있어, 다시 remove를 시도해야 하는 경우.
			PAS_TRACE1( "ClientRemover:: insert again MB= [%X]", mesg );
			putq(mesg);
			if (msgcnt==0) // 단 하나만 Q에 남는 경우.
				sleep(1);
		}
		//usleep(1000);
	}

	PAS_NOTICE("ClientRemover::svc stop");
	return 0;
}

int ClientRemover::putWork(PasHandler *pHandler)
{
	WorkInfoMessageBlock* pMB = new WorkInfoMessageBlock(WorkerID(0), WorkType(0), pHandler);
	PAS_TRACE1( "ClientRemover:: putWork MB= [%X]", pMB );
	if(this->putq((ACE_Message_Block*)pMB) < 0)
	{
		PAS_ERROR1("ClientRemover::putWork >> Work 큐에 입력 실패 [%X]", pHandler);
		return -1;
	}

	return 0;
}
