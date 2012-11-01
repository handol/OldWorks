#ifndef __EVENT_INFO_MESSAGE_BLOCK_H__
#define __EVENT_INFO_MESSAGE_BLOCK_H__

/**
@brief Message Queue 관리

ACE Message Queue 를 사용하기 위해서 ACE_Message_Block 을 상속받은 클래스
원래 ACE Message Queue 의 목적은, Block 내부에 데이터를 복사해서 사용해야 하지만
불필요한 복사가 이루어지고, 또 데이터의 크기가 커지면 성능에 악영향을 미치므로
WorkInfo 같은 데이터 구조체를 사용해야 한다.
WorkerID, WorkType 을 정의한 목적은, 여러 핸들러에서 사용되므로
현재 Block 의 목적을 전달하기 위함이다.
*/

#include <ace/Message_Block.h>
#include <ace/Event_Handler.h>

enum WorkerID
{
	WID_PAS,
	WID_CP,
	WID_CLIENT,
	WID_SANTA,
	WID_AUTH
};

enum WorkType
{
	WT_ONRECEIVE,
	WT_ONSENDABLE,
	WT_DELETE,
	WT_TIMEOUT
};

class WorkInfo
{
public:
	WorkerID workerID;
	WorkType workType;
	ACE_Event_Handler* pEventHandler;

	WorkInfo() {}
	WorkInfo(WorkerID iWorkerID, WorkType iWorkType, ACE_Event_Handler* ipEventHandler)
	{
		this->workerID = iWorkerID;
		this->workType = iWorkType;
		this->pEventHandler = ipEventHandler;
	}
};

class WorkInfoMessageBlock :
	public ACE_Message_Block
{
public:
	WorkInfoMessageBlock(WorkerID workerID, WorkType workType, ACE_Event_Handler* pEventHandler);
	virtual ~WorkInfoMessageBlock(void);

	WorkInfo workInfo();
	void workInfo(const WorkInfo& info);
};

#endif
