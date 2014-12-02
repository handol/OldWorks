
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>

#include "simpleAlloc.h"

#define PRN	printf

/**
@param kilo_bytes memory block 크기 ( kilo byte 단위 )
*/
void simpleAlloc::set(int kilo_bytes)
{
	if (kilo_bytes) blockBaseSize = kilo_bytes << 10;
	numOfBlocks = 0;

	firstBlock = currBlock = NULL;
}

void	simpleAlloc::cleanup()
{
	int	i;
	memBlockSt *blkptr, *nextptr;
	for (i=0, blkptr=firstBlock; blkptr && i<numOfBlocks; i++) {
		if (blkptr->headptr) delete [] blkptr->headptr;
		nextptr = blkptr->next;
		delete blkptr;
		blkptr = nextptr;
	}
}

int	simpleAlloc::new_block()
{
	memBlockSt *newblock;
	newblock = new memBlockSt;
	if (newblock == NULL) return -1;

	numOfBlocks++;
	memset(newblock, 0, sizeof (memBlockSt) );

	if (firstBlock==NULL) {
		firstBlock = currBlock = newblock;
	}
	else {
		currBlock->next = newblock;
		currBlock = newblock;
	}

	currBlock->headptr = new char [blockBaseSize];
	memset(currBlock->headptr, 0, blockBaseSize);
	if (currBlock->headptr != NULL) {
		currBlock->currptr  = (char*) currBlock->headptr;
		currBlock->left = blockBaseSize;
		return 0;
	}
	else {
		return -1;
	}	
}


void *simpleAlloc::alloc(int requested)
{
	char	*allocptr;

	if (requested > blockBaseSize) return NULL;
	
	if (currBlock == NULL || requested > currBlock->left) 
	{
		if (new_block() < 0) return NULL;
	}
	
	allocptr = currBlock->currptr;
	currBlock->currptr += requested;					
	currBlock->left -= requested;					
	return ((void *)allocptr);
}

/**
strdup 와 유사. 주어진 string의 길이만큼 메모리를 새로 할당하고, 복사한다.
*/
#if 0
void *simpleAlloc::alloc(char *str, int len)
{
	char *newstr;
	if (len==0) {
		len = strlen(str);
		newstr = (char*) alloc(len + 1);
		strcpy(newstr, str);
	}
	else {
		newstr = (char*) alloc(len + 1);
		strncpy(newstr, str, len);
		newstr[len] = 0;
	}
	return (void *) newstr;
}
#endif

void *simpleAlloc::copyAlloc(void *src, int len)
{
	void *newcopy=0;
	
	newcopy = (void  *)alloc(len);
	if (newcopy) {
		memcpy(newcopy, src, len);
	}
	return newcopy;
}

/**
strdup 와 유사. 주어진 string의 길이만큼 메모리를 새로 할당하고, 복사한다.
*/
char *simpleAlloc::strAlloc(char *src, int len)
{
	char *newstr=0;
	if (len==0) len = strlen(src);
	newstr = (char *)alloc(len+1);
	if (newstr) {
		strncpy(newstr, src, len);
		newstr[len] = 0;
	}
	return newstr;
}

void simpleAlloc::print()
{
	int	i;
	memBlockSt *blkptr;
	
	PRN("One Block=%dKb,  %d Blocks\n", blockBaseSize >>10, numOfBlocks);
	
	for (i=0, blkptr=firstBlock; blkptr && i<=numOfBlocks; i++, blkptr = blkptr->next) {
		/*
		PRN("[%02d] %X  %4d b\n", 			
			i, 
			(int)blkptr->headptr, 
			(int)blkptr->left );
		*/
		PRN("[%02d] %X  %4d bytes left (%d K)  %d%%\n", 			
			i, 
			(int)blkptr->headptr, 
			(int)blkptr->left,
			(int)blkptr->left >> 10, 
			(int)(blkptr->left*100)/blockBaseSize);
		
	}

}

#ifdef TEST_MAIN

#include <stdlib.h>

main()
{
	char	*ptr;
	int	i;
	simpleAlloc my_mem(30);

	for(i=0; i<1000; i++) {
		#ifdef __WIN32__
		ptr = (char*) my_mem.alloc(500  );
		#else
		ptr = (char*) my_mem.alloc(500+random()%500);
		#endif
		
		if (ptr == NULL) printf("alloc failed\n");
	}
	my_mem.print();
}
#endif




