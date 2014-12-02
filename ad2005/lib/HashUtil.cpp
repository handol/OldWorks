#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include "HashUtil.h"

#define SEED    (1159241)

int HashUtil::isprime(int n)
{
	int max=0;
	int i=2;
	int isPrime=0;

	max = (int)sqrt((float)n);
	do
	{
		isPrime = (n % i++ != 0);
	} while (isPrime && i<=max);
	return isPrime;
}


int HashUtil::getprime(int start)
{
	if (start < 3) start = 3;
	if ((start&1) == 0) start++;

	while (isprime(start)==0)
		start+=2;
	return start;
}


uint4 HashUtil::hashfunc(char * s)
{
	uint4 h = SEED;
	while (*s)
		h = (h << 5) + h + (unsigned char)*s++;
	return h;
}


uint4 HashUtil::hashfunc_nocase(char * s)
{
	uint4 h = SEED;
	while (*s)
		h = (h << 5) + h + (unsigned char)toupper(*s++);
	return h;
}

