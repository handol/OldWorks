#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "MemSplit.h"
#include "simpleAlloc.h"

#include <ctype.h>

#ifndef ISSPACE
#define	ISSPACE(X)	((X)==' ' || (X)=='\t' || (X)=='\n' || (X)=='\r')
#endif

void	MemSplit::alloc(int maxarray, int maxstrlen)
{
	maxArray = maxarray;
	maxLen = maxstrlen;
	Array = new char*[maxArray];
	Length = new int [maxArray];
	memory = new char [maxArray * (maxLen+1) ];
	reuse();
}

void	MemSplit::alloc(int maxarray, int maxstrlen, simpleAlloc &mem)
{
	maxArray = maxarray;
	maxLen = maxstrlen;
	Array = (char**)mem.alloc(maxArray * sizeof(char*) );
	Length = (int *)mem.alloc(maxArray * sizeof(int) );
	memory = (char *)mem.alloc(maxArray * (maxLen+1) );
	reuse();
}

void MemSplit::reuse()
{
	//printf("memory=%s\n", memory);
	//printf("Array=%X Length=%X memory=%X maxLen=%d \n", Array, Length, memory, maxLen);
	if (Array && memory) {
		for(int i=0; i<maxArray; i++) {
			Array[i] = memory + i*(maxLen+1);
			//printf("MemSplit:: Array[%d] = %X\n", i, Array[i] );
		}
	}
	memset(Length, 0, sizeof(int)*maxArray);
	memset(memory, 0, maxArray * (maxLen+1));
}

void MemSplit::free()
{
	if (isDynamicMem)  {
		if (Length) {
			delete [] Length;
		}
		if (Array) {
			delete [] Array;
		}
		if (memory) {
			delete [] memory;
		}
	}
}

/**
line2args() 와 유사. 주어진 line (string) 을 공백 기준으로 나누어, string array 에 저장한다.
*/
int MemSplit::split(char *src)
{
	int i, n;
	char *ptr;
	
	for (i=0; i<maxArray && *src; i++) {
	    /* 공백 skip */
	    while(ISSPACE(*src)) src++;
	    if (*src == 0 ) break;

	    /* 공백 아닌 문자 복사 */
	    for(n=0, ptr=Array[i]; n < maxLen && *src && !ISSPACE(*src); n++)  
	    { 
	    	*ptr++ = *src++; 
	    }
		*ptr = 0;
		
		Length[i] = n;

	    /* 공백 아닌 문자열의 길이가 maxLen 이상인 경우, 뒷부분은 skip */
	    while(*src && !ISSPACE(*src)) 
	    	src++;
	    
	}

	/* split 된 결과 string의 개수 */
	numSplit = i;
	return (numSplit);	        
}



/**
line2args() 와 유사. 주어진 line (string) 을 공백 기준으로 나누어, string array 에 저장한다.
*/
int MemSplit::split(char *src, char ch)
{
	int i, n;
	char *ptr;
	
	for (i=0; i<maxArray && *src; i++) {
	    for(n=0, ptr=Array[i]; n < maxLen && *src && *src != ch; n++)  
	    { 
	    	*ptr++ = *src++; 
	    }
		*ptr = 0;
		
		Length[i] = n;

		if (*src==ch) src++;	    
	}

	/* split 된 결과 string의 개수 */
	numSplit = i;
	return (numSplit);	        
}

/**
line2args() 와 유사. 주어진 line (string) 을 공백 기준으로 나누어, string array 에 저장한다.
*/
int MemSplit::split_trim(char *src, char ch)
{
	int i, n, j;
	char *ptr;
	
	for (i=0; i<maxArray && *src; i++) {
		/* 공백 skip */
	    while(ISSPACE(*src)) src++;
	    if (*src == 0 ) break;
	    
	    for(n=0, ptr=Array[i]; n < maxLen && *src && *src != ch; n++)  
	    { 
	    	*ptr++ = *src++; 
	    }
		
		for(j=n-1; j>=0; j--) {
			if ( ISSPACE (*(ptr-1)) )  { 
				ptr--; n--; 
			}
			else break;
		}
		 *ptr = 0;
		 
		Length[i] = n;

		if (*src==ch) src++;	    
	}

	/* split 된 결과 string의 개수 */
	numSplit = i;
	return (numSplit);	        
}


/**
line2args() 와 유사. 주어진 line (string) 을 공백 기준으로 나누어, string array 에 저장한다.
*/
int MemSplit::splitWords(char *src)
{
	int i, n;
	char *ptr;
	
	for (i=0; i<maxArray && *src; i++) {
	    /* 공백 skip */
	    while(*src && !isalpha(*src)) src++;
	    if (*src == 0 ) break;

	    /* 공백 아닌 문자 복사 */
	    for(n=0, ptr=Array[i]; n < maxLen &&  isalpha(*src); n++)  
	    { 
	    	*ptr++ = *src++; 
	    }
		*ptr = 0;
		
		Length[i] = n;
	    
	    /* 공백 아닌 문자열의 길이가 maxLen 이상인 경우, 뒷부분은 skip */
	    while(isalpha(*src)) 
	    	src++;
	    
	}

	/* split 된 결과 string의 개수 */
	numSplit = i;
	return (numSplit);	        
}

int	MemSplit::intval(int index)
{
	return strtol(Array[index], 0, 10);
}

/**
주어진 문자열이 split 결과 array에 존재하는지 여부 return.
@return boolean
*/
int	MemSplit::has(char *str)
{
	if (Array==NULL) return 0;
	for(int i=0; i<numSplit; i++)
		if (strcmp(str, Array[i])==0) return 1;
	return 0;
}

/**
주어진 문자열이 split 결과 array에 어느 위치에 존재하는지 index 값 구함.
@return index
*/
int	MemSplit::find(char *str)
{
	if (Array==NULL) return 0;
	for(int i=0; i<numSplit; i++)
		if (strcmp(str, Array[i])==0) return i;
	return -1;
}

/**
split 되어진 결과 string array 를 다시 하나의 string으로 만든다.
주어진 ch 를 각 문자열 사이에 집어 넣는다.
*/
int	MemSplit::join(char *buf, int max, int fromidx, int toidx, int ch)
{
	int	n=0;
	int	i;
	int	maxidx = toidx;
	if (maxidx >= numSplit) maxidx = numSplit-1;
	for(i=fromidx; i<maxidx; i++) {
		if (i != fromidx) {
			*buf++ = ' ';
			n++;
		}
		
		n += Length[i];
		if (n > max) break;
		strcpy(buf, Array[i]);
		buf += Length[i];
		
	}
	*buf = 0;
	return n;
}

void MemSplit::print(char *msg)
{
	if (msg) printf("%s", msg);
	printf("MemSplit: numSplit=%d\n", numSplit);
	if (Array)
	for(int i=0; i<numSplit; i++)
		printf("\t[%d] %s\n", i, Array[i]);
}
