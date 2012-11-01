#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "StrSplit.h"

void    StrSplit::alloc(int numOfFlds, int lengOfFld)
{
	if (numOfFlds > MAX_SPLIT_FLDS) numOfFlds = MAX_SPLIT_FLDS;

	maxNumFlds = numOfFlds;
	maxLengFlds = lengOfFld;
	#ifdef USE_NEW_OP
	memory = new char [numOfFlds * (lengOfFld+1) ];
	#else
	memory = (char*) calloc(sizeof(char) , numOfFlds * (lengOfFld+1) );
	#endif

	if (fldVals && memory) {
		for(int i=0; i<maxNumFlds; i++) {
			fldVals[i] = memory + i*(lengOfFld+1);
			fldVals[i][0] = 0;
		}
	}
}


void StrSplit::clear()
{
	#ifdef USE_NEW_OP
	if (memory) delete [] memory;
	#else
	if (memory) free((void*)memory);
	#endif

	memset( fldLengs, 0x00, sizeof(int)*MAX_SPLIT_FLDS );
	memset( fldVals, 0x00, sizeof(char*)*MAX_SPLIT_FLDS );
	memory = 0;
}


/**
주어진 line (string) 을 공백 기준으로 나누어, string array 에 저장한다.
*/
int StrSplit::split(char *src)
{
	int i, n;
	char *ptr;

	for (i=0; i<maxNumFlds && *src; i++) {
		/* 공백 skip */
		while(ISSPACE(*src)) src++;
		if (*src == 0 ) break;

		/* 공백 아닌 문자 복사 */
		for(n=0, ptr=fldVals[i]; n < maxLengFlds && *src && !ISSPACE(*src); n++) {
			*ptr++ = *src++;
		}
		*ptr = 0;

		fldLengs[i] = n;

		/* 공백 아닌 문자열의 길이가 maxLengFlds 이상인 경우, 뒷부분은 skip */
		while(*src && !ISSPACE(*src))
			src++;

	}

	/* split 된 결과 string의 개수 */
	numSplit = i;
	return (numSplit);
}


/**
주어진 line (string) 을 주어진 문자 (@param ch) 기준으로 나누어, string array 에 저장한다.
*/
int StrSplit::split(char *src, char ch)
{
	int i, n;
	char *ptr;

	for (i=0; i<maxNumFlds && *src; i++) {
		for(n=0, ptr=fldVals[i]; n < maxLengFlds && *src && *src != ch; n++) {
			*ptr++ = *src++;
		}
		*ptr = 0;

		fldLengs[i] = n;

		if (*src==ch) src++;
	}

	/* split 된 결과 string의 개수 */
	numSplit = i;
	return (numSplit);
}


/**
주어진 line (string) 을 단어(alphabet으로만 구성) 기준으로 나누어, string array 에 저장한다.
*/
int StrSplit::splitWords(char *src)
{
	int i, n;
	char *ptr;

	for (i=0; i<maxNumFlds && *src; i++) {
		/* 공백 skip */
		while(*src && !isalpha(*src)) src++;
		if (*src == 0 ) break;

		/* 공백 아닌 문자 복사 */
		for(n=0, ptr=fldVals[i]; n < maxLengFlds &&  isalpha(*src); n++) {
			*ptr++ = *src++;
		}
		*ptr = 0;

		fldLengs[i] = n;

		/* 공백 아닌 문자열의 길이가 maxLengFlds 이상인 경우, 뒷부분은 skip */
		while(isalpha(*src))
			src++;

	}

	/* split 된 결과 string의 개수 */
	numSplit = i;
	return (numSplit);
}


/**
주어진 문자열이 split 결과 array에 존재하는지 여부 return.
@return boolean
*/
int StrSplit::has(char *str)
{
	if (fldVals==0) return 0;
	for(int i=0; i<numSplit; i++)
		if (strcmp(str, fldVals[i])==0) return 1;
	return 0;
}


void StrSplit::print(char *msg)
{
	if (msg) printf("%s", msg);
	printf("StrSplit: numSplit=%d\n", numSplit);
	if (fldVals)
		for(int i=0; i<numSplit; i++)
			printf(" [%d] %3d - %08X - %s\n", i, fldLengs[i], fldVals[i], fldVals[i]);
}
/*
#ifdef TEST_MAIN
int main(int argc, char *argv[])
{
	StrSplit spliter(10, 64);

	spliter.split("hello world? be happy ^^");

	spliter.print();

	printf("%s %d\n", spliter.fldVal(0), spliter.fldLen(0));
	if (spliter.has("happy"))
		printf("I am happy !\n");
}

#endif*/
