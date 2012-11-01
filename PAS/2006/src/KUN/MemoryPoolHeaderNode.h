#ifndef __MEMORY_POOL_HEADER_NODE_H__
#define __MEMORY_POOL_HEADER_NODE_H__

#include "Common.h"
#include "Mutex.h"
#include <ace/Message_Block.h>
#include <ace/Null_Mutex.h>
#include <ace/Thread_Mutex.h>
#include <ace/Log_Msg.h>

using namespace std;

typedef std::vector<ACE_Message_Block*> MessageBlockPtrs;

class MemoryPoolHeaderNode
{
public:
	typedef ReadMutex<ACE_RW_Thread_Mutex> ReadLock;
	typedef WriteMutex<ACE_RW_Thread_Mutex> WriteLock;

	MemoryPoolHeaderNode(int maxFreeBlockCount_ = 0);
	~MemoryPoolHeaderNode();

	bool operator < (const MemoryPoolHeaderNode& rhs)
	{
		return blockSize < rhs.blockSize;
	}

	ACE_Message_Block* alloc(int *resultFlag);
	void free(ACE_Message_Block* pMB, int *resultFlag);
	void setMaxFreeBlockCount(int count);
	int getMaxFreeBlockCount();
	void setBlockSize(int size);
	int getBlockSize();
	void clearBlocks();
	int getFreeBlockCount();
	int	getAllocBlockCount();
	// no mutex version of clearBlocks()
	void _clearBlocks();

private:
	int blockSize;
	int maxFreeBlockCount;	///< free block 의 최대 개수
	int allocCount; // 할당된 개수.  
	MessageBlockPtrs ptrs;
	ACE_RW_Thread_Mutex lock;
};

#endif

