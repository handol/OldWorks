#ifndef HASH_UTIL_H
#define HASH_UTIL_H

#ifndef uint4
typedef unsigned int  uint4;
#endif

class HashUtil
{
	public:
		static	int isprime(int n);
		static	int getprime(int start);

		static	uint4 hashfunc(char * s);
		static	uint4 hashfunc_nocase(char * s);

};

#endif
