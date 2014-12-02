#include "hanja.h"
int index_last=476;


int convert_hanja_to_hangul(int start, int end, int hanja)
{
	int half;
	if(  start==end-1  ) return start;
	else
	{
		half=start+(end-start)/2;
		if( half > index_last)// index_last=476
			return convert_hanja_to_hangul(start,half,hanja);
		else if( hanja < hanja_table[half][1])
			return convert_hanja_to_hangul(start,half,hanja);
		else
			return convert_hanja_to_hangul(half,end,hanja);
	}
}

int get_hangul_code(int start, int end, int hanja)
{
	int index;
	index=convert_hanja_to_hangul(start, end, hanja);
	return hanja_table[index][0];
}
