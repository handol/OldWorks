#include "WorkInfo.h"

WorkInfoMessageBlock::WorkInfoMessageBlock(WorkerID workerID, WorkType workType, ACE_Event_Handler* pEventHandler)
{
	WorkInfo info(workerID, workType, pEventHandler);
	size(sizeof(info));

	workInfo(info);
}

WorkInfoMessageBlock::~WorkInfoMessageBlock(void)
{
}

WorkInfo WorkInfoMessageBlock::workInfo()
{
	WorkInfo info;
	ACE_ASSERT(length() == sizeof(info));

	memcpy((void*)&info, (void*)rd_ptr(), sizeof(info));
	
	return info;
}

void WorkInfoMessageBlock::workInfo(const WorkInfo& info)
{
	reset();

	ACE_ASSERT(space() >= sizeof(info));
	
	memcpy((void*)wr_ptr(), (const void*)&info, sizeof(info));
	wr_ptr(sizeof(info));

	ACE_ASSERT(length() == sizeof(info));
}
