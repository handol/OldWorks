#include "MemoryPoolManager.h"

MemoryPoolManager* MemoryPoolManager::instance(void)                  
{                                                             
	return ACE_Singleton<MemoryPoolManager, ACE_SYNCH_MUTEX>::instance();
}                      		

MemoryPoolManager::MemoryPoolManager(void)
{
	totalAllocCount = 0;
	memlog = NULL;

	#ifdef MEM_DEBUG
	memlog = new MyLog();
	memlog->open((char*)"./", (char*)"memlog");
	#endif
}

MemoryPoolManager::~MemoryPoolManager(void)
{
	WriteLock writeLock(_lock);

	_clear();

	if (memlog)
		delete memlog;
}

ACE_Message_Block* MemoryPoolManager::alloc(int size)
{
	ReadLock readLock(_lock);

	MemoryPoolHeader* pHeader = BestFitHeader(size);
	ACE_Message_Block *newblock = NULL;

	totalAllocCount++;
	
	// 관리되는 blockSize가 아니면 new를 통해 생성해서 반환한다.
	if(pHeader == NULL)
	{
		newblock = new ACE_Message_Block(size);
		
		#ifdef MEM_DEBUG
		memlog->logprint(LVL_DEBUG, "alloc %dK [%X] from OS (NO POOL)\n", (size >> 10), newblock);
		#endif
		
	}
	else
	{
		int allocedFromPool = 0;
		newblock = pHeader->alloc(&allocedFromPool);

		#ifdef MEM_DEBUG
		if (allocedFromPool)
			memlog->logprint(LVL_DEBUG, "alloc %dK for %dK [%X] from Pool\n", 
				(pHeader->getBlockSize() >> 10), (size >> 10), newblock);
		else
			memlog->logprint(LVL_DEBUG, "alloc %dK for %dK [%X] from OS\n", 
				(pHeader->getBlockSize() >> 10), (size >> 10), newblock);
		
		memlog->logprint(LVL_DEBUG, "Pool [%dK] - Alloc %d, Free %d\n", 
			(pHeader->getBlockSize() >> 10), pHeader->getAllocBlockCount(), pHeader->getFreeBlockCount());
		#endif
	}

	if(newblock->size() >= 10 * 1024 * 1024)
		PAS_INFO3("MemoryPoolManager::Alloc %d Bytes MB[%X] for %d Bytes", newblock->size(), newblock, size);

	return newblock;
}

void MemoryPoolManager::free(ACE_Message_Block* pMB)
{
	ReadLock readLock(_lock);

	ACE_ASSERT(pMB != NULL);

	if(pMB->size() >= 10 * 1024 * 1024)
		PAS_INFO2("MemoryPoolManager::Free %d Bytes MB[%X]", pMB->size(), pMB);

	totalAllocCount--;
	
	MemoryPoolHeader* pHeader = ExactFitHeader(pMB->size());
	
	// 관리되는 blockSize 가 아니면 그냥 삭제
	if(pHeader == NULL)
	{
		#ifdef MEM_DEBUG
		memlog->logprint(LVL_DEBUG, "free %dK [%X] to OS (NO POOL)\n", pMB->size() >> 10, pMB);
		#endif

		delete pMB;
	}

	// 관리되는 blockSize 이면 free list 에 추가
	else
	{
		int	freeFromPool = 0;
		pHeader->free(pMB, &freeFromPool);

		#ifdef MEM_DEBUG
		if (freeFromPool)
			memlog->logprint(LVL_DEBUG, "free %dK [%X] to Pool\n", 
				(pHeader->getBlockSize() >> 10), pMB);
		else
			memlog->logprint(LVL_DEBUG, "free %dK [%X] to OS\n", 
				(pHeader->getBlockSize() >> 10), pMB);
		
		memlog->logprint(LVL_DEBUG, "Pool [%dK] - Alloc %d, Free %d\n", 
			(pHeader->getBlockSize() >> 10), pHeader->getAllocBlockCount(), pHeader->getFreeBlockCount());
		#endif
	}
}

/**
이 MemoryPoolManager 가 new로 할당 받은 메모리 전체 크기.
모든 메시지 블락의 총합.
2006.09.16 handol
*/
int MemoryPoolManager::getSumMemKiloBytes()
{
	ReadLock readLock(_lock);

	MemoryPoolHeaderList::iterator it = _headers.begin();
	MemoryPoolHeaderList::iterator itE = _headers.end();

	int sum = 0;
	for( ; it != itE; ++it)
	{
		sum += ((*it)->getBlockSize() >> 10) * (*it)->getFreeBlockCount();
	}

	return sum;
}

vint MemoryPoolManager::getBlockSizes()
{
	ReadLock readLock(_lock);

	vint sizes;
	sizes.reserve(_headers.size());	

	MemoryPoolHeaderList::iterator it = _headers.begin();
	MemoryPoolHeaderList::iterator itE = _headers.end();

	for( ; it != itE; ++it)
	{
		sizes.push_back((*it)->getBlockSize());
	}

	return sizes;
}

// No mutex version for clear
void MemoryPoolManager::_clear()
{
	while(!_headers.empty())
	{
		MemoryPoolHeader* pHeader = _headers.back();
		delete pHeader;
		_headers.pop_back();
	}
}

void MemoryPoolManager::setBlockSizes(vint sizes)
{
	WriteLock writeLock(_lock);

	// clear MemoryPoolHeaderList
	_clear();

	// 헤더리스트가 blockSize 를 기준으로 오름차순 정렬이 되도록 
	// blockSize 를 정렬한다.
	std::sort(sizes.begin(), sizes.end());
	
	// create headers
	_headers.reserve(sizes.size());

	vint::iterator it = sizes.begin();
	vint::iterator itE = sizes.end();

	// init. MemoryPoolHeaderList
	for( ; it != itE; ++it)
	{
		MemoryPoolHeader* pHeader = new MemoryPoolHeader;
		pHeader->setBlockSize(*it);
		_headers.push_back(pHeader);
	}
}

void MemoryPoolManager::setBlockSizeAndMax(BlockInfoList blockInfos)
{
	WriteLock writeLock(_lock);

	// clear MemoryPoolHeaderList
	_clear();

	// 헤더리스트가 blockSize 를 기준으로 오름차순 정렬이 되도록 
	// blockInfos 를 정렬한다.
	std::sort(blockInfos.begin(), blockInfos.end());
	
	// create headers
	_headers.reserve(blockInfos.size());

	BlockInfoList::iterator it = blockInfos.begin();
	BlockInfoList::iterator itE = blockInfos.end();

	// init. MemoryPoolHeaderList
	for( ; it != itE; ++it)
	{
		MemoryPoolHeader* pHeader = new MemoryPoolHeader;
		pHeader->setBlockSize(it->blockSize);
		pHeader->setMaxFreeBlockCount(it->maxNum);
		_headers.push_back(pHeader);
	}
}

int MemoryPoolManager::BestFitFreeBlocks(int size)
{
	ReadLock readLock(_lock);

	MemoryPoolHeader* pHeader = BestFitHeader(size);

	if(pHeader == NULL)
		return -1;

	return pHeader->getFreeBlockCount();
}

int MemoryPoolManager::AvailFreeBlocks(int size)
{
	ReadLock readLock(_lock);

	int sum = 0;

	MemoryPoolHeaderList::iterator it = _headers.begin();
	MemoryPoolHeaderList::iterator itE = _headers.end();

	for( ; it != itE; ++it)
	{
		if((*it)->getBlockSize() >= size)
		{
			sum += (int)((*it)->getFreeBlockCount());
		}
	}

	return sum;
}

int MemoryPoolManager::BestFitSize(int size)
{
	ReadLock readLock(_lock);

	MemoryPoolHeader* pHeader = BestFitHeader(size);
	
	if(pHeader == NULL)
		return -1;

	return pHeader->getBlockSize();
}

MemoryPoolHeader* MemoryPoolManager::BestFitHeader(int size)
{
	MemoryPoolHeaderList::iterator it = _headers.begin();
	MemoryPoolHeaderList::iterator itE = _headers.end();

	for( ; it != itE; ++it)
	{
		if((*it)->getBlockSize() >= size)
		{
			return *it;
		}
	}

	return NULL;
}

MemoryPoolHeader* MemoryPoolManager::ExactFitHeader(int size)
{
	MemoryPoolHeaderList::iterator it = _headers.begin();
	MemoryPoolHeaderList::iterator itE = _headers.end();

	for( ; it != itE; ++it)
	{
		if((*it)->getBlockSize() == size)
		{
			return *it;
		}
	}

	return NULL;
}

int MemoryPoolManager::MinBlockSize()
{
	ReadLock readLock(_lock);

	if(_headers.empty())
		return -1;

	return _headers.front()->getBlockSize();
}

int MemoryPoolManager::MaxBlockSize()
{
	ReadLock readLock(_lock);

	if(_headers.empty())
		return -1;

	return _headers.back()->getBlockSize();
}

int MemoryPoolManager::getFreeBlockCount(int blockSize)
{
	ReadLock readLock(_lock);

	MemoryPoolHeader* pHeader = BestFitHeader(blockSize);
	
	if(pHeader == NULL)
		return -1;

	return pHeader->getFreeBlockCount();
}

int MemoryPoolManager::getAllocBlockCount(int blockSize)
{
	ReadLock readLock(_lock);

	MemoryPoolHeader* pHeader = BestFitHeader(blockSize);
	
	if(pHeader == NULL)
		return -1;

	return pHeader->getAllocBlockCount();
}

void MemoryPoolManager::setMaxFreeBlockCount(int blockSize, int maxCount)
{
	WriteLock writeLock(_lock);

	MemoryPoolHeader* pHeader = BestFitHeader(blockSize);
	
	if(pHeader == NULL)
		return;

	pHeader->setMaxFreeBlockCount(maxCount);
}

int MemoryPoolManager::getMaxFreeBlockCount(int blockSize)
{
	ReadLock readLock(_lock);

	MemoryPoolHeader* pHeader = BestFitHeader(blockSize);

	// not exist
	if(pHeader == NULL)
		return -1;

	return pHeader->getMaxFreeBlockCount();
}

void MemoryPoolManager::test()
{
	ACE_Message_Block* pMB1; 
	ACE_Message_Block* pMB2;
	ACE_Message_Block* pMB3;
	ACE_Message_Block* pMB4;

	BlockInfoList bInfos;
	BlockInfo bInfo;

	for(int i = 1; i <= 4; ++i)
	{
		bInfo.blockSize = i*1024;
		bInfo.maxNum = 3;
		bInfos.push_back(bInfo);
	}

	MemoryPoolManager man;
	man.setBlockSizeAndMax(bInfos);
	ACE_ASSERT(man.getAllocBlockCount(1024) == 0);
	ACE_ASSERT(man.getFreeBlockCount(1024) == 0);

	pMB1 = man.alloc(1);
	ACE_ASSERT(pMB1->size() == 1024);
	ACE_ASSERT(man.getAllocBlockCount(1024) == 1);
	ACE_ASSERT(man.getFreeBlockCount(1024) == 0);

	pMB2 = man.alloc(512);
	ACE_ASSERT(pMB1->size() == 1024);
	ACE_ASSERT(man.getAllocBlockCount(1024) == 2);
	ACE_ASSERT(man.getFreeBlockCount(1024) == 0);

	pMB3 = man.alloc(1023);
	ACE_ASSERT(pMB1->size() == 1024);
	ACE_ASSERT(man.getAllocBlockCount(1024) == 3);
	ACE_ASSERT(man.getFreeBlockCount(1024) == 0);

	pMB4 = man.alloc(1024);
	ACE_ASSERT(pMB1->size() == 1024);
	ACE_ASSERT(man.getAllocBlockCount(1024) == 4);
	ACE_ASSERT(man.getFreeBlockCount(1024) == 0);

	man.free(pMB1);
	ACE_ASSERT(man.getAllocBlockCount(1024) == 3);
	ACE_ASSERT(man.getFreeBlockCount(1024) == 1);

	man.free(pMB2);
	ACE_ASSERT(man.getAllocBlockCount(1024) == 2);
	ACE_ASSERT(man.getFreeBlockCount(1024) == 2);

	man.free(pMB3);
	ACE_ASSERT(man.getAllocBlockCount(1024) == 1);
	ACE_ASSERT(man.getFreeBlockCount(1024) == 3);

	man.free(pMB4);
	ACE_ASSERT(man.getAllocBlockCount(1024) == 0);
	ACE_ASSERT(man.getFreeBlockCount(1024) == 3);

	pMB1 = man.alloc(1024);
	ACE_ASSERT(pMB1->size() == 1024);
	ACE_ASSERT(man.getAllocBlockCount(1024) == 1);
	ACE_ASSERT(man.getFreeBlockCount(1024) == 2);

	pMB2 = man.alloc(1025);
	ACE_ASSERT(pMB2->size() == 2048);
	ACE_ASSERT(man.getAllocBlockCount(1024) == 1);
	ACE_ASSERT(man.getFreeBlockCount(1024) == 2);
	ACE_ASSERT(man.getAllocBlockCount(2048) == 1);
	ACE_ASSERT(man.getFreeBlockCount(2048) == 0);

	pMB3 = man.alloc(2047);
	ACE_ASSERT(pMB3->size() == 2048);
	ACE_ASSERT(man.getAllocBlockCount(1024) == 1);
	ACE_ASSERT(man.getFreeBlockCount(1024) == 2);
	ACE_ASSERT(man.getAllocBlockCount(2048) == 2);
	ACE_ASSERT(man.getFreeBlockCount(2048) == 0);

	pMB4 = man.alloc(2048);
	ACE_ASSERT(pMB3->size() == 2048);
	ACE_ASSERT(man.getAllocBlockCount(1024) == 1);
	ACE_ASSERT(man.getFreeBlockCount(1024) == 2);
	ACE_ASSERT(man.getAllocBlockCount(2048) == 3);
	ACE_ASSERT(man.getFreeBlockCount(2048) == 0);

	man.free(pMB1);
	ACE_ASSERT(man.getAllocBlockCount(1024) == 0);
	ACE_ASSERT(man.getFreeBlockCount(1024) == 3);
	ACE_ASSERT(man.getAllocBlockCount(2048) == 3);
	ACE_ASSERT(man.getFreeBlockCount(2048) == 0);

	man.free(pMB2);
	ACE_ASSERT(man.getAllocBlockCount(1024) == 0);
	ACE_ASSERT(man.getFreeBlockCount(1024) == 3);
	ACE_ASSERT(man.getAllocBlockCount(2048) == 2);
	ACE_ASSERT(man.getFreeBlockCount(2048) == 1);

	man.free(pMB3);
	ACE_ASSERT(man.getAllocBlockCount(1024) == 0);
	ACE_ASSERT(man.getFreeBlockCount(1024) == 3);
	ACE_ASSERT(man.getAllocBlockCount(2048) == 1);
	ACE_ASSERT(man.getFreeBlockCount(2048) == 2);

	man.free(pMB4);
	ACE_ASSERT(man.getAllocBlockCount(1024) == 0);
	ACE_ASSERT(man.getFreeBlockCount(1024) == 3);
	ACE_ASSERT(man.getAllocBlockCount(2048) == 0);
	ACE_ASSERT(man.getFreeBlockCount(2048) == 3);
}

