
#ifndef _POOL_MNG_TMPL_H
#define _POOL_MNG_TMPL_H

/**
@file PoolMng.h

고정크기 (construct 시에 크기 지정)의 array에 기반한 특정 데이타 Pool 관리 template.
free데이타 리스트와 active 데이타 리스트 관리.
할당되는 데이타는  free리스트에 제거되는 동시에 active 리스트로 삽입.

사용이 종료된 아이템 free (giveback)할 수 있는 조건에 제약이 있다.
giveback()은 a_begin(), a_next() 루프 내에서만 호출 해야 한다. 그외 경우는 버그  발생.
*/

#ifndef TRUE
#define TRUE	1
#endif

#ifndef FALSE
#define FALSE	0
#endif

template <class ItemType>
class PoolMng
{
	typedef struct _ListType
	{
		ItemType item;
		struct _ListType *next;
	} ListType;
	
	private:
		ListType * memPool;
		ListType * freeHead;
		ListType * activeHead;

		ListType * freeIterator; // 사용하지 않는  item 들에 대한 free Iterator.
		ListType * activeIterator; // 사용 중인 item 들에 대한  active Iterator
		ListType * activePrev;
		int maxSize;
		int freeCnt;

	public:
		PoolMng() { memPool = NULL; freeCnt = 0; }
		~PoolMng() { destroy(); }

		int	create(int _maxsize);
		void	destroy();
		
		ItemType *borrow(); // free에서 하나들 할당 받는다.
		int	giveback(); // 현재 iterator 중인 active item 을 반환한다.

		int count() // 현재 활성화중인 item 개수 
		{
			return maxSize-freeCnt;
		}
		int available() // 현재 free( 비활성) item 개수 
		{
			return freeCnt;
		}

		// free 리스트 iterator 루프
		void	f_begin() { freeIterator = freeHead; }
		int	f_NOTend() {return (freeIterator!=NULL); }
		void	f_next() {if (freeIterator) freeIterator = freeIterator->next; }

		ItemType *f_curr() {
			if (freeIterator) return &(freeIterator->item);
			return NULL;
		}

		// active 리스트 iterator 루프
		void	a_begin() { activeIterator = activeHead; activePrev = NULL; }
		int	a_NOTend() {return (activeIterator!=NULL); }
		void	a_next() {
			if (activeIterator) {
				activePrev = activeIterator;
				activeIterator = activeIterator->next; 
			}
		}
		int	a_istail() {
			return (activeIterator && activeIterator->next==NULL);
		}
		ItemType *a_curr() {
			if (activeIterator) return &(activeIterator->item);
			return NULL;
		}
		
};

template <class ItemType>
int	PoolMng<ItemType>::create(int _maxsize)
{
	if (memPool) return -1;

	memPool = new ListType[_maxsize];
	if (memPool==NULL) return -1;

	maxSize = _maxsize;
	freeHead = memPool;
	freeCnt = maxSize;
	freeIterator = freeHead;
	activeHead = NULL;
	activeIterator = NULL;
	
	// setup the free list of unused nodes
	for (int i=0; i < maxSize-1 ; i++)
	{
		memPool[i].next = &memPool[i+1];
	}	
	memPool[maxSize-1].next = NULL;			  // terminates freelist
	return maxSize;
}

template <class ItemType>
void PoolMng<ItemType>::destroy()
{
	// free
	if (memPool) {
		delete [] memPool;
		memPool = NULL;
	}
}


/**
memory pool 에 cell 을 사용자에게 제공.
free list 의 맨 앞 cell을 제공.
*/
template <class ItemType>
ItemType* PoolMng<ItemType>::borrow( )
{
	ListType *allocNode;
	if (freeCnt==0) return NULL;
	freeCnt--;
	// free 리스트에서 삭제
	allocNode = freeHead;
	freeHead = freeHead->next;

	// active  리스트에 추가
	allocNode->next = activeHead;
	activeHead = allocNode;
	return &(allocNode->item);
}


/**
memory pool 에 cell 을 반환.
free list 의 맨 앞에 넣는다.
a_begin(), a_next() 루프 내에서만 호출 해야 한다. 그외 경우는 버그  발생.
*/
template <class ItemType>
int PoolMng<ItemType>::giveback()
{

	if (activeIterator==NULL) return -1;

	// active  리스트에서 삭제
	if (activePrev) {
		activePrev->next = activeIterator->next;
	}
	else { // 맨 앞에서 빠지는 경우 
		activeHead = activeHead->next;
	}
	
	// free 리스트에 추가 	
	activeIterator->next = freeHead;
	freeHead = activeIterator;

	// 현재 active list에 대해 iterate 중이다. a_next() 제대로 동작하기 위해서 아래 라인이 필요하다.
	activeIterator = activePrev;
	
	freeCnt ++;
	return 0;
}


#endif
