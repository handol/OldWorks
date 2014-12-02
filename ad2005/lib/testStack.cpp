#include <stdio.h>
#include "simpleStack.h"

int	main()
{
	simpleStack<int> A(10);
	int	i;

	for(i=0; i<15; i++) {
		A.push(i*2);
		printf("size=%d, top=%d\n", A.count(), A.top() );
	}
	for(i=0; i<10; i++)
		A.pop();	
}
