
#ifndef _POOL_MNG_TMPL_H
#define _POOL_MNG_TMPL_H

/**
@file PoolMng.h

����ũ�� (construct �ÿ� ũ�� ����)�� array�� ����� Ư�� ����Ÿ Pool ���� template.
free����Ÿ ����Ʈ�� active ����Ÿ ����Ʈ ����.
�Ҵ�Ǵ� ����Ÿ��  free����Ʈ�� ���ŵǴ� ���ÿ� active ����Ʈ�� ����.

����� ����� ������ free (giveback)�� �� �ִ� ���ǿ� ������ �ִ�.
giveback()�� a_begin(), a_next() ���� �������� ȣ�� �ؾ� �Ѵ�. �׿� ���� ����  �߻�.
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

		ListType * freeIterator; // ������� �ʴ�  item �鿡 ���� free Iterator.
		ListType * activeIterator; // ��� ���� item �鿡 ����  active Iterator
		ListType * activePrev;
		int maxSize;
		int freeCnt;

	public:
		PoolMng() { memPool = NULL; freeCnt = 0; }
		~PoolMng() { destroy(); }

		int	create(int _maxsize);
		void	destroy();
		
		ItemType *borrow(); // free���� �ϳ��� �Ҵ� �޴´�.
		int	giveback(); // ���� iterator ���� active item �� ��ȯ�Ѵ�.

		int count() // ���� Ȱ��ȭ���� item ���� 
		{
			return maxSize-freeCnt;
		}
		int available() // ���� free( ��Ȱ��) item ���� 
		{
			return freeCnt;
		}

		// free ����Ʈ iterator ����
		void	f_begin() { freeIterator = freeHead; }
		int	f_NOTend() {return (freeIterator!=NULL); }
		void	f_next() {if (freeIterator) freeIterator = freeIterator->next; }

		ItemType *f_curr() {
			if (freeIterator) return &(freeIterator->item);
			return NULL;
		}

		// active ����Ʈ iterator ����
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
memory pool �� cell �� ����ڿ��� ����.
free list �� �� �� cell�� ����.
*/
template <class ItemType>
ItemType* PoolMng<ItemType>::borrow( )
{
	ListType *allocNode;
	if (freeCnt==0) return NULL;
	freeCnt--;
	// free ����Ʈ���� ����
	allocNode = freeHead;
	freeHead = freeHead->next;

	// active  ����Ʈ�� �߰�
	allocNode->next = activeHead;
	activeHead = allocNode;
	return &(allocNode->item);
}


/**
memory pool �� cell �� ��ȯ.
free list �� �� �տ� �ִ´�.
a_begin(), a_next() ���� �������� ȣ�� �ؾ� �Ѵ�. �׿� ���� ����  �߻�.
*/
template <class ItemType>
int PoolMng<ItemType>::giveback()
{

	if (activeIterator==NULL) return -1;

	// active  ����Ʈ���� ����
	if (activePrev) {
		activePrev->next = activeIterator->next;
	}
	else { // �� �տ��� ������ ��� 
		activeHead = activeHead->next;
	}
	
	// free ����Ʈ�� �߰� 	
	activeIterator->next = freeHead;
	freeHead = activeIterator;

	// ���� active list�� ���� iterate ���̴�. a_next() ����� �����ϱ� ���ؼ� �Ʒ� ������ �ʿ��ϴ�.
	activeIterator = activePrev;
	
	freeCnt ++;
	return 0;
}


#endif
