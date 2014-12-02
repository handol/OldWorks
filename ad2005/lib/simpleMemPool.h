
#ifndef simpleMemPool_H
#define simpleMemPool_H

/**
@file simpleMemPool.h
dynamic linked list�� �ƴ϶� ����ũ�� (construct �ÿ� ũ�� ����)�� array�� ����� linked list.

��ü array �߿� ������ ���� free list ������ ���ԵǾ� �ִ�.
������� �����۵��� ����Ʈ�� �� template�� �̿��ڰ� �����Ͽ��� �Ѵ�.
PoolMng template�� �� ��ü�� active ����Ʈ ���� ����� �ִ�.


�� �ڵ��� ������ :
http://ironbark.bendigo.latrobe.edu.au/courses/subjects/DataStructures/mal/lecture090/lecture.html
	- java �ڵ���. array ���. free list ���� ���

http://condor.stcloudstate.edu/~julstrom/cs301/examples/tlist.html
	- C++ Linked List Template Class. C++ template ������ ���� ����

���� : linked list ������ ���� �ɵ����� �ڷ�
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

		ListType * iterator; // ��� ���� item �鿡 ���� iterator. inuse iterator
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
memory pool �� cell �� ����ڿ��� ����.
free list �� �� �� cell�� ����.
*/
template <class ListType>
ListType* simpleMemPool<ListType>::allocmem( )
{
	ListType *item;
	if (freeCnt==0) return NULL;
	freeCnt--;
	// free ����Ʈ���� ����
	item = freeHead;
	freeHead = freeHead->next;

	// inuse ����Ʈ�� �߰�
	item->next = inuseHead;
	inuseHead = item;
	return item;
}


/**
memory pool �� cell �� ��ȯ.
free list �� �� �տ� �ִ´�.
*/
template <class ListType>
int simpleMemPool<ListType>::freemem( ListType *item )
{
	if (item==NULL) return -1;
	if (freeCnt >= maxSize) return -1;

	// free ����Ʈ�� �߰� 
	item->next = freeHead;
	freeHead = item;

	// inuse ����Ʈ���� ����
	
	freeCnt ++;
	return 0;
}

#endif
