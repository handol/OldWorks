
#ifndef _sortedListTmpl_h
#define _sortedListTmpl_h

/**
����ũ�� (construct �ÿ� ũ�� ����)�� array�� ����� linked list.
insertion sort ��� ����. �Ʒ��� ���� ����Ʈ�� sorting ����� ����. 

���� :
http://condor.stcloudstate.edu/~julstrom/cs301/examples/tlist.html

http://ironbark.bendigo.latrobe.edu.au/courses/subjects/DataStructures/mal/lecture090/lecture.html
	- java �ڵ���. array ���. free list ���� ���

http://condor.stcloudstate.edu/~julstrom/cs301/examples/tlist.html
	- C++ Linked List Template Class. C++ template ������ ���� ����

���� : linked list ������ ���� �ɵ����� �ڷ�
http://pegasus.rutgers.edu/~elflord/cpp/list_howto/

*/

#include "simpleMemPool.h"
#include <string.h>
#include <stdio.h>

#ifndef TRUE
#define TRUE	1
#endif

#ifndef FALSE
#define FALSE	0
#endif


template <class ListItem>
class sortedList
{
	typedef struct Node
	{
		ListItem data;
		struct Node *next;
	} sortedListNode;

	private:
		simpleMemPool<sortedListNode> listArray;
		sortedListNode *headNode;
		int maxsize;
		int counter;

		//-- iterator
		sortedListNode *iterator;
	public:
		sortedList(int _maxsize) {
			maxsize=_maxsize; 
			counter=0; 
			headNode=NULL;
			iterator=NULL;
			listArray.create(_maxsize); 
		}
		
		~sortedList() { 
			listArray.destroy(); 
		}

		ListItem *search(ListItem *item);
		ListItem *insert(ListItem *item);
		int	remove(ListItem *item);
		void	print();
		int	count()	{ return counter; }
		int	isfull() { return (counter >= maxsize); }
		int	available() { return listArray.available(); }

		ListItem *get_freenode() { 
			if  (listArray.get_freelist()) return  &(listArray.get_freelist()->data);
			else return NULL;
		}
		//--- mimic iterator 
		void	begin() { iterator = headNode; }
		int	end() {return (iterator==NULL); }
		int	NOTend() {return (iterator!=NULL); }
		void	next() {iterator = iterator->next; }
		ListItem *curr() { 
			if (iterator) return &iterator->data;
			else return NULL; 
		}
};

/**
@return NULL : ���н�.
*/
template <class ListItem>
ListItem *sortedList<ListItem>::search(ListItem *item)
{
	sortedListNode	*ptr, *prev;
	int	cmp;

	for (ptr = headNode, prev=NULL; ptr; prev = ptr, ptr = ptr->next) {		
		cmp = item->compare( &(ptr->data) );
		if (cmp == 0) return &(ptr->data);
		if (cmp < 0) break;		
	}
	return NULL;
}

/**
@return -1: ���н�. ���� item ���� �Ǵ� �޸� ���� ��.
*/
template <class ListItem>
ListItem *sortedList<ListItem>::insert(ListItem *item)
{
	sortedListNode	*ptr, *prev, *newone;
	int	cmp;

	newone = listArray.allocmem();
	if (newone==NULL) {
		return NULL;
	}
	newone->next = NULL;
	
	for (ptr = headNode, prev=NULL; ptr; prev = ptr, ptr = ptr->next) {		
		cmp = item->compare( &(ptr->data) );
		if (cmp == 0) return NULL; // same item exist
		if (cmp < 0) break;		
	}


	memcpy(&(newone->data), item, sizeof(ListItem));

	if (ptr==headNode) {
		/* insert at the head position */
		newone->next = headNode;
		headNode = newone;	
	}
	else if (ptr==NULL) {
		/* insert at the tail position */
		prev->next = newone;
	}
	else {
		/* insert at the middle */
		prev->next = newone;
		newone->next =  ptr;		
	}
	counter++;
	return &(newone->data);
}

/**
@return -1: ���н�. ���� item search ����  �Ǵ� �޸� ���� ��.
*/
template <class ListItem>
int	sortedList<ListItem>::remove(ListItem *item)
{
	sortedListNode	*ptr, *prev;
	int	cmp=-1;
	
	for (ptr = headNode, prev=NULL; ptr; prev = ptr, ptr = ptr->next) {		
		cmp = item->compare( &(ptr->data) );
		if (cmp == 0) break; // same item exist
		if (cmp < 0) break;		
	}

	if (cmp != 0 || ptr==NULL) { // not found
		return -1;
	}

	if (ptr==headNode) {
		/* delete at the head position */
		headNode = headNode->next;
	}
	else {
		/* delete in the middle */
		prev->next = ptr->next;
	}
	ptr->next = NULL;
	listArray.freemem(ptr);
	
	counter--;
	return 0;
	
}

template <class ListItem>
void	sortedList<ListItem>::print()
{
	sortedListNode	*ptr;	
	for (ptr = headNode; ptr; ptr = ptr->next) {		
		ptr->data.print();
	}
}

#endif
