
#ifndef simpleMemPool_H
#define simpleMemPool_H

/**
@file simpleMemPool.h
dynamic linked list가 아니라 고정크기 (construct 시에 크기 지정)의 array에 기반한 linked list.

전체 array 중에 사용되지 않은 free list 관리가 포함되어 있다.
사용중인 아이템들의 리스트는 이 template의 이용자가 관리하여야 한다.
PoolMng template은 그 자체에 active 리스트 관리 기능이 있다.


본 코드의 원조들 :
http://ironbark.bendigo.latrobe.edu.au/courses/subjects/DataStructures/mal/lecture090/lecture.html
	- java 코드임. array 기반. free list 관리 기능

http://condor.stcloudstate.edu/~julstrom/cs301/examples/tlist.html
	- C++ Linked List Template Class. C++ template 구현을 위해 참조

참고 : linked list 구현에 대한 심도깊은 자료
http://pegasus.rutgers.edu/~elflord/cpp/list_howto/

*/
#ifndef TRUE
#define TRUE	1
#endif

#ifndef FALSE
#define FALSE	0
#endif

template <class ListType>
class simpleMemPool
{
	private:
		ListType * memPool;
		ListType * freeHead;
		ListType * inuseHead;

		ListType * iterator; // 사용 중인 item 들에 대한 iterator. inuse iterator
		int maxSize;
		int freeCnt;

	public:
		simpleMemPool() { memPool = NULL; freeCnt = 0; }
		~simpleMemPool() { destroy(); }

		int	create(int _maxsize);
		void	destroy();
		
		ListType *allocmem();
		int freemem(ListType *);

		int available()
		{
			return freeCnt;
		}

		ListType *get_freelist() {
			return freeHead;
		}
		
		//--- mimic iterator 
		void	begin() { iterator = inuseHead; }
		int	end() {return (iterator==NULL); }
		int	NOTend() {return (iterator!=NULL); }
		void	next() {iterator = iterator->next; }

		ListType *currnode() { 
			return iterator;
		}
};

//#include "Log_Msg.h"

template <class ListType>
int	simpleMemPool<ListType>::create(int _maxsize)
{
	if (memPool) return -1;

	memPool = new ListType[_maxsize];
	if (memPool==NULL) return -1;

	maxSize = _maxsize;
	freeHead = memPool;
	freeCnt = maxSize;
	
	// setup the free list of unused nodes
	for (int i=0; i < maxSize-1 ; i++)
	{
		memPool[i].next = &memPool[i+1];
	}	
	memPool[maxSize-1].next = NULL;			  // terminates freelist
	return maxSize;
}

template <class ListType>
void simpleMemPool<ListType>::destroy()
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
template <class ListType>
ListType* simpleMemPool<ListType>::allocmem( )
{
	ListType *item;
	if (freeCnt==0) return NULL;
	freeCnt--;
	// free 리스트에서 삭제
	item = freeHead;
	freeHead = freeHead->next;

	// inuse 리스트에 추가
	item->next = inuseHead;
	inuseHead = item;
	return item;
}


/**
memory pool 에 cell 을 반환.
free list 의 맨 앞에 넣는다.
*/
template <class ListType>
int simpleMemPool<ListType>::freemem( ListType *item )
{
	if (item==NULL) return -1;
	if (freeCnt >= maxSize) return -1;

	// free 리스트에 추가 
	item->next = freeHead;
	freeHead = item;

	// inuse 리스트에서 삭제
	
	freeCnt ++;
	return 0;
}

#endif
