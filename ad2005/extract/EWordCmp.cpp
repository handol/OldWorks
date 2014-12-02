#include "wordType.h"

#include "EWordCmp.h"

int cmpIdioms(const void *Aptr, const void *Bptr)
{
	idiomType *A = *(idiomType **)Aptr;
	idiomType *B = *(idiomType **)Bptr;
	wordType	*aword;
	wordType	*bword;
	int	i, min;
	
	
	min = MIN(A->nums, B->nums);
	aword = (wordType *)A->words;
	bword = (wordType *)B->words;
	
	for(i=0; i<min; i++, aword++, bword++) {
		if (aword < bword) return -1;
		if (aword > bword) return 1;
	}
	if (A->nums < B->nums) return -1;
	if (A->nums > B->nums) return 1;
	return 0;
}


