#ifndef MEMSPLIT_H
#define MEMSPLIT_H

#include "simpleAlloc.h"

#ifndef NULL
#define NULL  (0)
#endif

/**
@file MemSplit.h 주어진 line (string) 을 공백 기준으로 나누어, string array 에 저장한다. string array의 메모리는 동적으로 할당한다.
*/


class MemSplit {
public:
	MemSplit() { init(); }
	MemSplit(int maxarray, int maxstrlen) {
		init();
		isDynamicMem =  1;
		alloc(maxarray, maxstrlen);
	}

	MemSplit(int maxarray, int maxstrlen, simpleAlloc &mem) {
		init();
		alloc(maxarray, maxstrlen, mem);
	}

	~MemSplit() {
		free();
	}

	void	init() {
		isDynamicMem = 0;
		maxArray = maxLen = numSplit = 0;
		Array = 0;
		Length = 0;
	}
	
	void alloc(int maxarray, int maxstrlen);
	void alloc(int maxarray, int maxstrlen, simpleAlloc &mem);
	void free();
	void	reuse();
	
	int	split(char *src);
	int	split(char *src, char ch);
	int	split_trim(char *src, char ch);
	int	splitWords(char *src);
	int	has(char *str);
	int	find(char *str);

	int	join(char *buf, int max, int fromidx, int toidx, int ch=' ');
	
	char **argv() { return Array; }
	int argc() { return numSplit; }

	char *str(int index) { return Array[index]; }
	int	len(int index) { return Length[index]; }
	int	intval(int index);
	int	size() { return numSplit; }
	int	num() { return numSplit; }
	void print(char *msg=NULL);
	
private:
	int	isDynamicMem;
	int	maxArray;
	int	maxLen;
	int	numSplit; /* split 된 결과 string의 개수 */
	char **Array;
	char	*memory;
	int	*Length;
};

#endif
