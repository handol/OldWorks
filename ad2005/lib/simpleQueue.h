
#ifndef _QueueTmpl_h
#define _QueueTmpl_h

/**
C++ 참고 좋은 사이트

http://www.glenmccl.com/tutor.htm 
http://www.glenmccl.com/tmpl_cmp.htm - Template관련 
http://www.glenmccl.com/virt_cmp.htm - virtual function

본 코드의 원조 :
http://www.cs.fiu.edu/~weiss/dsaa_c2e/queue.c

http://www.cs.fiu.edu/~weiss/dsaa_c2e/files.html
http://www.cs.fiu.edu/~weiss/dsaa_c++/code/
1. Source Code for Data Structures and Algorithm Analysis in C (Second Edition)
2. Source Code for Data Structures and Algorithm Analysis in C++ (Second Edition) 

*/
template <class ElementType>
class simpleQueue
{

	public:
		simpleQueue() {};
		~simpleQueue() {};

		int IsEmpty(  ) {   return this->_count == 0; }
		int IsFull(  ) {  return this->_count >= this->maxsize; }
		int	available() { return maxsize - _count; }
		int	count() { return _count; }

		int create( int MaxElements );
		void destroy(  );
		void init(  );
		
		ElementType *enqueue(ElementType *X);
		ElementType *gethead(  );
		ElementType *dequeue(  );

	private:
		int maxsize;
		int front;
		int rear;
		int _count;
		ElementType *Array;

		
		inline int inc(int Value);

};


#include <stdlib.h>

template <class ElementType>
int   simpleQueue<ElementType>::create( int MaxElements )
{
	this->Array = new ElementType[MaxElements];

	if( this->Array == NULL )
	{
		return 0;
	}
	this->maxsize = MaxElements;
	init();
	return this->maxsize ;
}


template <class ElementType>
void  simpleQueue<ElementType>::init(  )
{
	this->_count = 0;
	this->front = 1;
	this->rear = 0;
}


template <class ElementType>
void   simpleQueue<ElementType>::destroy(  )
{
	if( this->Array != NULL )
	{
		delete [] this->Array; 
		this->Array = NULL;
	}
}

template <class ElementType>
inline int  simpleQueue<ElementType>::inc( int Value)
{
	if( ++Value == this->maxsize )
		Value = 0;
	return Value;
}

template <class ElementType>
ElementType * simpleQueue<ElementType>::enqueue( ElementType *X )
{
	if( IsFull( ) )
	{
		return 0;
	}

	else
	{
		ElementType *to_insert=0;
		this->_count++;
		this->rear = inc( this->rear );
		to_insert = &this->Array[ this->rear];
		if(X) memcpy(to_insert, X, sizeof(ElementType));
		return to_insert;
	}
}

template <class ElementType>
ElementType * simpleQueue<ElementType>::gethead(  )
{
	if( !IsEmpty(  ) )
		return  & (this->Array[ this->front ]);

	return NULL;								  /* Return value used to avoid warning */
}


template <class ElementType>
ElementType * simpleQueue<ElementType>::dequeue(  )
{
	ElementType *X = 0;

	if( IsEmpty(  ) )
	{
		return NULL;
	}
	else
	{
		this->_count--;
		X = &(this->Array[ this->front ]);
		this->front = inc( this->front);
		return X;
	}

}
#endif											  /* _QueueTmpl_h */
