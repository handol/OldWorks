#ifndef SIMPLE_STACK_H
#define SIMPLE_STACK_H

template <class T>
class simpleStack
{
public:
	simpleStack();
	simpleStack(int s) ; 
	~simpleStack() { if (stackPtr) delete [] stackPtr ; }
	int	count();
	T	top();
	void	alloc(int s);
	int push(const T&); 
	T* pop() ;  // pop an element off the stack
	int isEmpty() { return (topptr == -1) ; } 
	int isFull()  { return (topptr >= size - 1) ; } 
private:
	int size ;  // Number of elements on simpleStack
	int topptr ;
	
	T* stackPtr ;  
} ;

//constructor with the default size 10
template <class T>
int	simpleStack<T>::count()
{
	return topptr + 1;
}

template <class T> 
T	simpleStack<T>::top() 
{
	if (topptr >= 0)
	return stackPtr[topptr];
	else 
	return 0;
}

//constructor with the default size 10
template <class T>
void simpleStack<T>::alloc(int s)
{
	size = (s > 0 && s < 1000) ? s : 10 ;  
	topptr = -1 ;  // initialize stack
	stackPtr = new T[size] ; 
}

//constructor with the default size 10
template <class T>
simpleStack<T>::simpleStack()
{
	size = 0 ;  
	topptr = -1 ;  // initialize stack
	//stackPtr = new T[size] ; 
}


//constructor with the default size 10
template <class T>
simpleStack<T>::simpleStack(int s)
{
	size = s > 0 && s < 1000 ? s : 10 ;  
	topptr = -1 ;  // initialize stack
	stackPtr = new T[size] ;
	printf("size=%d, total=%d\n", size, sizeof(stackPtr) );
	 
}

 // push an element onto the simpleStack 
template <class T>
int simpleStack<T>::push(const T& item)
{
	if (!isFull())
	{
		stackPtr[++topptr] = item ;
		return 1 ;  // push successful
	}
	return 0 ;  // push unsuccessful
}

// pop an element off the simpleStack
template <class T> 
T* simpleStack<T>::pop() 
{
	if (!isEmpty())
	{
		return  &(stackPtr[topptr--]) ;
	}
	return NULL ;  // pop unsuccessful
}


#endif
