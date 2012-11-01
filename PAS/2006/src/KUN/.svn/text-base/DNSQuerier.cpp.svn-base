#include "DNSQuerier.h"
#include "PasLog.h"
#include "MemoryPoolManager.h"
#include "Util2.h"
#include "Mutex.h"

using namespace DNS;

Querier::Querier()
{
	setIdle();
}

Querier::~Querier()
{
	// 큐에 있는 MessageBlock 제거
	ACE_Message_Block* pMB;
	ACE_Time_Value timeout(0, 0); // non block
	
	while(getq(pMB, &timeout) >= 0)
	{
		MemoryPoolManager::instance()->free(pMB);
	}

	while(responseQueue.dequeue(pMB, &timeout) >= 0)
	{
		MemoryPoolManager::instance()->free(pMB);
	}
}

bool Querier::isIdle()
{
	return state == DQS_IDLE;
}

bool Querier::isTimeout()
{
	return state == DQS_TIMEOUT;
}

ACE_Message_Block* Querier::getResponse(ACE_Time_Value* pTimeout)
{
	ACE_Message_Block* pMB;
	int ret = responseQueue.dequeue(pMB, pTimeout);
	if(ret < 0)
		return NULL;

	return pMB;
}

void Querier::query(const ACE_Message_Block* pMB)
{
	const int destLen = 1024;
	char dotstrIP[destLen];

	PAS_TRACE1("DNS Querier >> Query %s", pMB->rd_ptr());
	bool result = Util2::getHostByName_threadsafe(pMB->rd_ptr(), dotstrIP, destLen);
	
	{
		ReadMutex<ACE_Thread_Mutex> guard(lock);
		
		// Timeout 으로 셋팅되어 있지 않다면, 외부에서 응답을 기다리고 있는 상태라고 할 수 있다.
		if(isTimeout() == false)
		{
			ACE_Message_Block* pDestMB = MemoryPoolManager::instance()->alloc(destLen);
			ACE_ASSERT(pDestMB != NULL);
			ACE_ASSERT(static_cast<int>(pDestMB->size()) >= destLen);

			// 성공일 경우 IP 를 큐에 넣는다.
			if(result == true)
			{
				pDestMB->copy(dotstrIP, strlen(dotstrIP)+1);
				PAS_TRACE2("DNS Querier >> Receive Response, %s => %s", pMB->rd_ptr(), dotstrIP);
				responseQueue.enqueue(pDestMB);
			}

			// 실패일 경우 empty 를 큐에 넣는다.
			else
			{
				responseQueue.enqueue(pDestMB);
				PAS_INFO1("DNS Querier >> DNS Lookup Fail, %s", pMB->rd_ptr());
			}
		}

		// 외부에서 Timeout 으로 셋팅했다면, DNS Query 결과를 응답큐에 넣지 않는다.
		else
		{
			// do nothing
			PAS_INFO1("DNS Querier >> Timeout, %s", pMB->rd_ptr());
		}
	}
}

int Querier::svc(void)
{
	PAS_NOTICE("Start DNS Querier");
	
	ACE_Message_Block* pMB;
	while(getq(pMB) >= 0)
	{
		ACE_ASSERT(pMB != NULL);

		setBusy();
		
		// 종료 시그널
		if(pMB->length() == 0)
		{
			MemoryPoolManager::instance()->free(pMB);
			break;
		}

		query(pMB);

		MemoryPoolManager::instance()->free(pMB);

		setIdle();
	}

	PAS_NOTICE("End DNS Querier");
	return 0;
}

void Querier::setBusy()
{
	state = DQS_BUSY;
}

void Querier::setIdle()
{
	state = DQS_IDLE;
}


void Querier::setTimeout()
{
	WriteMutex<ACE_Thread_Mutex> guard(lock);

	state = DQS_TIMEOUT;

	// DNS Query 결과를 응답큐(responseQueue)에 push하는 사이에 외부에서는 
	// setTimeout() 을 호출한다면, 응답큐에 응답메시지가 들어있게된다.

	ACE_Message_Block* pMB;
	ACE_Time_Value timeout(0, 0);
	// 다음 요청자를 위해 response queue 에 응답이 들어가 있다면 삭제하자.
	while(responseQueue.dequeue(pMB, &timeout) >= 0)
	{
		MemoryPoolManager::instance()->free(pMB);

		// 응답큐에 데이터가 있다는 것은 DNS Query가 완료됐음을 의미한다.
		// 그러므로 상태를 IDLE 로 셋팅한다.
		state = DQS_IDLE;
	}
}


