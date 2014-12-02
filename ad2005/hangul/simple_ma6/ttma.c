/* by S. C. Lee
licese GPL
*/
#include <stdio.h>
#include "ma.h"

enum { PAUSE, ENDING, QUESTION, FEEL };

STR_RESULT_MA result_ma[100];

extern void convert_ks_to_3(int h, int l, unsigned char *des);
extern void init_list_result (int no_of_word);
extern void sentence_ana(int no_of_words);
extern void free_list_result (int no_of_word);

int token_to_johabs(unsigned char *str,int len,unsigned char token_johab[20][3])
{

	int i;
	int index;
	unsigned char *src;
	index=0;
	for(i=0;i<len;)
	{
		while( (int)str[i] < 128 )
		{
			if( str[i]==',')
			{
				i++;
				token_johab[index][1]=0;
				token_johab[index][0]=PAUSE;
				index++;
			}
			else
			{
				i++;
			}
		}
		if( str[i] >=0xb0 && str[i] <=0xc8 && str[i+1] > 128 )
		{
			src=str+i;
			convert_ks_to_3(*src,*(src+1),token_johab[index]);
			index++;
			i+=2;
		}
		else
		{
			i+=1;
		}
	}
	return index;
}


int str_to_token(unsigned char *str,int len,unsigned char token[][100])
{
	int index,pos;
	int i;
	index=0;
	pos=0;
	for(i=0;i<len;i++)
	{
		if(i==0 && str[i]==' ')
		{
			while(str[i+1]==' ') i++;
		}
		if(str[i]==' ')
		{
			token[index][pos]=0;
			while(str[i+1]==' ') i++;
			index++;
			pos=0;
		}
		else
		{
			token[index][pos]=str[i];
			pos++;
		}
	}
	return index+1;
}


void ttma(unsigned char *str,int len)
{
	FILE *fp_target;
	int i,j;
	int position;
	int no_of_tokens;
	unsigned char token[100][100];
	//  unsigned char token[100][100];
	float slope;

	str[len]=0;
	printf("str=%s \n",str);
	no_of_tokens=str_to_token(str,len,token);
	//printf("no of tokens=%d \n",no_of_tokens);
	init_list_result(no_of_tokens);
	for(i=0;i<no_of_tokens;i++)
	{
		if(i==no_of_tokens-1) position=2;
		else if(i==0 ) position=0;
		else position =1;
		if(token[i]!=0)
		{
			token_to_ma(token[i],i,position);
		}
	}

	print_result(no_of_tokens);
	//sentence_ana(no_of_tokens);
	free_list_result(no_of_tokens);

}
