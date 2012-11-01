#ifndef __DNS_QUERIER_H__
#define __DNS_QUERIER_H__

#include <ace/Task.h>
#include <ace/Message_Queue.h>
#include <ace/Time_Value.h>
#include <ace/Thread_Mutex.h>

#include "MemoryPoolManager.h"

#include "Util2.h"

namespace DNS
{
	class Querier : public ACE_Task<ACE_MT_SYNCH>
	{
	// types
	public:
		enum QuerierState
		{
			DQS_IDLE,
			DQS_BUSY,
			DQS_TIMEOUT
		};

	// operator
	public:
		Querier();
		virtual ~Querier();
		
		virtual int svc(void);
		
		ACE_Message_Block* getResponse(ACE_Time_Value* pTimeout = NULL);
		
		bool isIdle();
		bool isTimeout();
		
		void setTimeout();
		
	private:
		void setIdle();
		void setBusy();
		void query(const ACE_Message_Block* pMB);
		

	// attribute
	private:
		volatile QuerierState state;
		ACE_Message_Queue<ACE_MT_SYNCH> responseQueue;

		ACE_Thread_Mutex lock;
	};
};

#endif




