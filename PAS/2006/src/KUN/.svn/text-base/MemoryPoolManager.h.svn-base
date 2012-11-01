#ifndef __MEMORY_POOL_MANAGER_H__
#define __MEMORY_POOL_MANAGER_H__

/**
@brief 메모리 관리용 클래스

목적
메모리를 재사용 함으로써 빈번히 발생하는 생성/삭제 오버헤드를 줄인다.

구현
최초에 mem.cfg 에 정의된 만큼의 메모리를 생성한다.
메모리를 재사용 중에 더 큰 메모리가 요구되거나 잔여량이 부족하다면
필요한 만큼 메모리(블럭)를 할당한다.
*/


#include "Common.h"
#include "Mutex.h"
#include <ace/Message_Block.h>
#include <ace/Null_Mutex.h>
#include <ace/Thread_Mutex.h>
#include <ace/Log_Msg.h>
#include "MyLog.h"
#include "MemoryPoolHeaderNode.h"
#include "PasLog.h"

using namespace std;

class BlockInfo{
public:
	bool operator < (const BlockInfo& rhs) const
	{
		return this->blockSize < rhs.blockSize;
	}
	
	int	blockSize;
	int	maxNum;
};

typedef vector<BlockInfo> BlockInfoList;
typedef MemoryPoolHeaderNode MemoryPoolHeader;
typedef vector<MemoryPoolHeader *> MemoryPoolHeaderList;


class MemoryPoolManager
{
public:

	
	typedef ReadMutex<ACE_RW_Thread_Mutex> ReadLock;
	typedef WriteMutex<ACE_RW_Thread_Mutex> WriteLock;

	
	MemoryPoolManager();
	virtual ~MemoryPoolManager(void);

	ACE_Message_Block* alloc(int size);
	void free(ACE_Message_Block* mb);

	int getSumMemKiloBytes();
	vint getBlockSizes();
	void setBlockSizes(vint sizes);

	/* setBlockSizes(), getMaxFreeBlockCount() 를 한번에 */
	void setBlockSizeAndMax(BlockInfoList blockInfos);
	
	int BestFitFreeBlocks(int size);
	int AvailFreeBlocks(int size);
	int BestFitSize(int size);
	
	int MinBlockSize();
	int MaxBlockSize();

	int getFreeBlockCount(int blockSize);
	int getAllocBlockCount(int blockSize);

	void setMaxFreeBlockCount(int blockSize, int maxCount);
	int getMaxFreeBlockCount(int blockSize);

	static MemoryPoolManager* instance(void);

	static void test();

protected:
	MemoryPoolHeaderList _headers;
	ACE_RW_Thread_Mutex _lock;

	MemoryPoolHeader* BestFitHeader(int size);
	MemoryPoolHeader* ExactFitHeader(int size);
	void _clear();

private:
	int	totalAllocCount;
	MyLog *memlog;
};

typedef MemoryPoolManager MessageBlockManager;

#endif
